"""Exercise the real compiler/lint/format/CMake paths, including restoration."""

import ctypes
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def run(command: list[str], cwd: Path, succeeds: bool, diagnostic: str = "") -> dict:
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace")
    output = result.stdout + result.stderr
    if (result.returncode == 0) != succeeds or (diagnostic and diagnostic not in output):
        raise RuntimeError(f"Unexpected gate proof: {command}\n{output}")
    return {"command": command, "exitCode": result.returncode, "output": output}


def write_files(root: Path, files: dict[str, str]) -> None:
    for name, content in files.items():
        path = root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")


def showincludes_prefix(output: str, header: Path) -> str:
    expected = str(header.resolve()).casefold()
    for line in output.splitlines():
        index = line.casefold().rfind(expected)
        if index > 0:
            return line[:index]
    raise RuntimeError(f"MSVC /showIncludes did not report {header}")


def ninja_dependencies(output: str) -> dict[str, set[str]]:
    dependencies = {}
    current = None
    for line in output.splitlines():
        if ": #deps " in line:
            object_path = line.split(": #deps ", 1)[0]
            current = object_path.replace("\\", "/").rsplit("/", 1)[-1]
            dependencies[current] = set()
        elif current is not None and line.startswith("    "):
            dependency = line.strip().replace("\\", "/").rsplit("/", 1)[-1]
            dependencies[current].add(dependency)
    return dependencies


def file_hash(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def object_evidence(path: Path) -> dict:
    return {"modifiedNs": path.stat().st_mtime_ns, "sha256": file_hash(path)}


def console_output_code_page() -> int:
    return ctypes.windll.kernel32.GetConsoleOutputCP()


def prove_cp932_prefix_mismatch(fixture: Path, prefix: str) -> dict:
    mismatched_prefix = prefix.encode("utf-8").decode("cp932", errors="replace")
    mismatch_source = "actual UTF-8 prefix decoded as CP932"
    if mismatched_prefix == prefix:
        mismatched_prefix = "NeNe mismatched /showIncludes prefix:  "
        mismatch_source = "explicit mismatch because the actual prefix is CP932-invariant"
    build_directory = "build-cp932-mismatch"
    configure = [
        "cmake",
        "-S",
        ".",
        "-B",
        build_directory,
        "-G",
        "Ninja",
        "-DCMAKE_BUILD_TYPE=Debug",
    ]
    run(configure, fixture, True)
    rules_path = fixture / build_directory / "CMakeFiles/rules.ninja"
    rules = rules_path.read_text(encoding="utf-8")
    expected_rule = f"msvc_deps_prefix = {prefix}"
    if rules.count(expected_rule) != 1:
        raise RuntimeError("Could not isolate the Ninja MSVC dependency prefix")
    rules_path.write_text(rules.replace(expected_rule, f"msvc_deps_prefix = {mismatched_prefix}"), encoding="utf-8")
    build_result = run(["cmake", "--build", build_directory, "--verbose"], fixture, True)
    deps_result = run(["ninja", "-C", build_directory, "-t", "deps"], fixture, True)
    dependencies = ninja_dependencies(deps_result["output"])
    if len(dependencies) != 4 or any(dependencies.values()):
        raise RuntimeError(f"The mismatched Ninja prefix unexpectedly retained dependencies: {dependencies}")
    print(f"QLT-007: a mismatched prefix lost the actual MSVC header dependencies ({mismatch_source})")
    return {
        "actualPrefix": prefix,
        "mismatchedPrefix": mismatched_prefix,
        "mismatchSource": mismatch_source,
        "dependencies": {name: sorted(values) for name, values in dependencies.items()},
        "build": build_result,
    }


def prove_incremental_header_dependencies(root: Path) -> dict:
    fixture = root / "header-dependencies"
    fixture.mkdir()
    write_files(
        fixture,
        {
            "CMakeLists.txt": """cmake_minimum_required(VERSION 3.31)
project(HeaderDependencyProbe LANGUAGES CXX)
add_executable(header_dependency_probe
    consumer_a.cpp consumer_b.cpp main.cpp provider.cpp)
""",
            "public.hpp": """#pragma once
using SharedValue = int;
int shared_value(SharedValue value);
""",
            "consumers.hpp": """#pragma once
int consumer_a();
int consumer_b();
""",
            "provider.cpp": """#include \"public.hpp\"
int shared_value(SharedValue value) { return static_cast<int>(value); }
""",
            "consumer_a.cpp": """#include \"consumers.hpp\"
#include \"public.hpp\"
int consumer_a() { return shared_value(17); }
""",
            "consumer_b.cpp": """#include \"consumers.hpp\"
#include \"public.hpp\"
int consumer_b() { return shared_value(17); }
""",
            "main.cpp": """#include \"consumers.hpp\"
int main() { return consumer_a() + consumer_b() == 34 ? 0 : 1; }
""",
        },
    )
    raw = run(
        ["cl", "/nologo", "/showIncludes", "/c", "provider.cpp", "/Foshowincludes.obj"],
        fixture,
        True,
    )
    prefix = showincludes_prefix(raw["output"], fixture / "public.hpp")
    mismatch = prove_cp932_prefix_mismatch(fixture, prefix)
    configure = ["cmake", "-S", ".", "-B", "build", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Debug"]
    build = ["cmake", "--build", "build", "--verbose"]
    run(configure, fixture, True)
    run(build, fixture, True)
    compiler_file = next((fixture / "build/CMakeFiles").glob("*/CMakeCXXCompiler.cmake"))
    compiler_text = compiler_file.read_text(encoding="utf-8")
    rules_text = (fixture / "build/CMakeFiles/rules.ninja").read_text(encoding="utf-8")
    if f'set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "{prefix}")' not in compiler_text:
        raise RuntimeError("CMake /showIncludes prefix differs from the actual MSVC output")
    if f"msvc_deps_prefix = {prefix}" not in rules_text:
        raise RuntimeError("Ninja /showIncludes prefix differs from the actual MSVC output")
    initial_deps_result = run(["ninja", "-C", "build", "-t", "deps"], fixture, True)
    initial_deps = ninja_dependencies(initial_deps_result["output"])
    expected_deps = {
        "consumer_a.cpp.obj": {"consumers.hpp", "public.hpp"},
        "consumer_b.cpp.obj": {"consumers.hpp", "public.hpp"},
        "main.cpp.obj": {"consumers.hpp"},
        "provider.cpp.obj": {"public.hpp"},
    }
    if initial_deps != expected_deps:
        raise RuntimeError(f"Ninja did not retain the expected header dependencies: {initial_deps}")
    object_root = fixture / "build/CMakeFiles/header_dependency_probe.dir"
    before = {path.name: object_evidence(path) for path in object_root.glob("*.obj")}
    if set(before) != set(expected_deps):
        raise RuntimeError(f"Unexpected fixture objects before header change: {sorted(before)}")
    (fixture / "public.hpp").write_text(
        "#pragma once\nusing SharedValue = long long;\nint shared_value(SharedValue value);\n",
        encoding="utf-8",
    )
    incremental = run(build, fixture, True)
    run([str(fixture / "build/header_dependency_probe.exe")], fixture, True)
    after = {path.name: object_evidence(path) for path in object_root.glob("*.obj")}
    if set(after) != set(before):
        raise RuntimeError(f"Unexpected fixture objects after header change: {sorted(after)}")
    rebuilt = sorted(
        name
        for name, evidence in before.items()
        if after[name]["modifiedNs"] != evidence["modifiedNs"]
    )
    unchanged = sorted(
        name
        for name, evidence in before.items()
        if after[name]["modifiedNs"] == evidence["modifiedNs"]
    )
    expected_rebuilt = ["consumer_a.cpp.obj", "consumer_b.cpp.obj", "provider.cpp.obj"]
    if rebuilt != expected_rebuilt or unchanged != ["main.cpp.obj"]:
        raise RuntimeError(f"Unexpected incremental rebuild: rebuilt={rebuilt}, unchanged={unchanged}")
    if any(before[name]["sha256"] == after[name]["sha256"] for name in rebuilt):
        raise RuntimeError("A rebuilt ABI-dependent object retained its pre-change content")
    if before["main.cpp.obj"]["sha256"] != after["main.cpp.obj"]["sha256"]:
        raise RuntimeError("The unrelated object changed without being recompiled")
    post_change_deps = ninja_dependencies(
        run(["ninja", "-C", "build", "-t", "deps"], fixture, True)["output"]
    )
    if post_change_deps != expected_deps:
        raise RuntimeError(f"Ninja lost header dependencies after rebuild: {post_change_deps}")
    code_page = console_output_code_page()
    print(
        "QLT-007 / QLT-011: actual MSVC headers triggered the required incremental rebuild; "
        f"prefix={prefix!r}; consoleOutputCP={code_page}"
    )
    return {
        "rule": "QLT-007 / QLT-011",
        "kind": "incremental-header-dependencies",
        "showIncludes": raw,
        "showIncludesPrefix": prefix,
        "consoleOutputCodePage": code_page,
        "cp932Mismatch": mismatch,
        "cmakePrefixMatched": True,
        "ninjaPrefixMatched": True,
        "dependencies": {name: sorted(values) for name, values in initial_deps.items()},
        "rebuiltObjects": rebuilt,
        "unchangedObjects": unchanged,
        "objectsBefore": before,
        "objectsAfter": after,
        "incremental": incremental,
    }


def main() -> None:
    output_root = (ROOT / "out/proofs").resolve()
    output_root.mkdir(parents=True, exist_ok=True)
    evidence = []
    with tempfile.TemporaryDirectory(prefix="build-", dir=output_root) as temporary:
        root = Path(temporary).resolve()
        if not root.is_relative_to(output_root):
            raise RuntimeError("Proof workspace escaped the intended output directory")
        evidence.append(prove_incremental_header_dependencies(root))
        files = ["CMakeLists.txt", "eng/targets.cmake", "eng/architecture.json", ".clang-tidy", ".clang-format", "tests/build/ToolchainSmoke.cpp"]
        files += [p.relative_to(ROOT).as_posix() for folder in ["src", "tests/unit"] for p in (ROOT / folder).rglob("*") if p.is_file()]
        for path in files:
            destination = root / path
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(ROOT / path, destination)
        source = root / "tests/build/ToolchainSmoke.cpp"
        original = source.read_text(encoding="utf-8")
        configure = ["cmake", "-S", ".", "-B", "build", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Debug"]
        build = ["cmake", "--build", "build", "--target", "toolchain_smoke", "--clean-first"]
        run(configure, root, True)
        run(build, root, True)
        probes = [
            ("QLT-002", "int main() { int unused; return 0; }", "C4101"),
            ("CPP-002", "enum class Mode { rgb, hex };\nint main() { constexpr auto mode = Mode::rgb; switch (mode) { case Mode::rgb: return 0; } return 1; }", "C4062"),
            ("CPP-012", "int select(int a, int b, int c, int d, int e) { return a + b + c + d + e; }\nint main() { return select(0, 0, 0, 0, 0); }", "readability-function-size"),
            ("CPP-003", "struct Sample { int value; int read() const { return value; } };\nint main() { Sample sample{0}; return sample.read(); }", "misc-non-private-member-variables-in-classes"),
        ]
        for rule, text, diagnostic in probes:
            source.write_text(text, encoding="utf-8")
            result = run(build, root, False, diagnostic)
            source.write_text(original, encoding="utf-8")
            restoration = run(build, root, True)
            evidence.append({"rule": rule, "negative": result, "restorationExit": restoration["exitCode"]})
            print(f"{rule}: rejected {diagnostic}; restored build passed")
        source.write_text("int main(){return 0;}\n", encoding="utf-8")
        result = run(["clang-format", "--dry-run", "--Werror", f"--style=file:{root / '.clang-format'}", str(source)], root, False, "clang-format-violations")
        source.write_text(original, encoding="utf-8")
        run(["clang-format", "--dry-run", "--Werror", f"--style=file:{root / '.clang-format'}", str(source)], root, True)
        evidence.append({"rule": "QLT-004", "negative": result, "restorationExit": 0})
        cmake_file = root / "CMakeLists.txt"
        cmake_original = cmake_file.read_text(encoding="utf-8")
        cmake_file.write_text(cmake_original + "\nneneloupe_target(other verification STATIC tests/build/ToolchainSmoke.cpp)\nneneloupe_link(toolchain_smoke other)\n", encoding="utf-8")
        result = run(configure, root, False, "ARC-002")
        cmake_file.write_text(cmake_original, encoding="utf-8")
        run(configure, root, True)
        run(build, root, True)
        evidence.append({"rule": "ARC-002", "negative": result, "restorationExit": 0})
        print("QLT-004 / ARC-002: real tools rejected violations; restoration passed")
        cmake_file.write_text(cmake_original + "\nneneloupe_system_link(neneloupe_core user32)\n", encoding="utf-8")
        result = run(configure, root, False, "ARC-002")
        cmake_file.write_text(cmake_original, encoding="utf-8")
        run(configure, root, True)
        run(build, root, True)
        evidence.append({"rule": "ARC-002", "negative": result, "restorationExit": 0})
        print("ARC-002: core platform library rejected; restoration passed")
    (output_root / "results.json").write_text(json.dumps(evidence, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Gate proofs passed: {len(evidence)} real-tool proofs")


if __name__ == "__main__":
    main()
