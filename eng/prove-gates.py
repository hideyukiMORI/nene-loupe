"""Exercise the real compiler/lint/format/CMake paths, including restoration."""

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


def main() -> None:
    output_root = (ROOT / "out/proofs").resolve()
    output_root.mkdir(parents=True, exist_ok=True)
    evidence = []
    with tempfile.TemporaryDirectory(prefix="build-", dir=output_root) as temporary:
        root = Path(temporary).resolve()
        if not root.is_relative_to(output_root):
            raise RuntimeError("Proof workspace escaped the intended output directory")
        for path in ["CMakeLists.txt", "eng/targets.cmake", "eng/architecture.json", ".clang-tidy", ".clang-format", "tests/build/ToolchainSmoke.cpp"]:
            destination = root / path
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(ROOT / path, destination)
        source = root / "tests/build/ToolchainSmoke.cpp"
        original = source.read_text(encoding="utf-8")
        configure = ["cmake", "-S", ".", "-B", "build", "-G", "Ninja", "-DCMAKE_BUILD_TYPE=Debug"]
        build = ["cmake", "--build", "build", "--clean-first"]
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
    (output_root / "results.json").write_text(json.dumps(evidence, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Gate proofs passed: {len(evidence)} real-tool negative/restoration pairs")


if __name__ == "__main__":
    main()
