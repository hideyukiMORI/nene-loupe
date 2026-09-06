#include "CaptureExclusionStyle.hpp"

#include <utility>

namespace neneloupe
{
DWORD CaptureExclusionStyle::affinity(CaptureExclusion exclusion)
{
    switch (exclusion)
    {
    case CaptureExclusion::enabled:
        return WDA_EXCLUDEFROMCAPTURE;
    case CaptureExclusion::disabled:
        return WDA_NONE;
    }
    std::unreachable();
}

bool CaptureExclusionStyle::excludes(CaptureExclusion exclusion)
{
    switch (exclusion)
    {
    case CaptureExclusion::enabled:
        return true;
    case CaptureExclusion::disabled:
        return false;
    }
    std::unreachable();
}
} // namespace neneloupe
