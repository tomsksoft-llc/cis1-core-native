#pragma once

#include <algorithm>

template <class Map>
bool is_maps_equal(Map const &lhs, Map const &rhs)
{
    return lhs.size() == rhs.size()
        && std::equal(lhs.begin(), lhs.end(),
                      rhs.begin());
}
