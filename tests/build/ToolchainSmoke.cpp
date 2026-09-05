#include <array>

int main()
{
    constexpr std::array<int, 3> values{1, 2, 3};
    static_assert(values[0] + values[1] == values[2]);
    return values[2] == 3 ? 0 : 1;
}
