#include "WindowLayer.hpp"

#include <utility>

namespace neneloupe
{
WindowLayer next_layer(WindowLayer layer)
{
    switch (layer)
    {
    case WindowLayer::topmost:
        return WindowLayer::normal;
    case WindowLayer::normal:
        return WindowLayer::topmost;
    }
    std::unreachable();
}
} // namespace neneloupe
