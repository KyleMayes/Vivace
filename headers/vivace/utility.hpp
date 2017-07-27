// Copyright 2017 Kyle Mayes
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef VCE_UTILITY_HPP
#define VCE_UTILITY_HPP

#include <ostream>

#include <vivace/meta.hpp>

namespace vce {

/// An ordering of two values.
enum class Ordering : int {
    Less = -1,
    Greater = 1,
    Equal = 0,
};

std::ostream& operator<<(std::ostream& stream, Ordering ordering);

namespace detail { VCE_HAS_MEMBER_FUNCTION(HasCompare, compare); }

/// Returns the ordering of the two supplied values.
template <class T, class U>
Ordering compare(const T& left, const U& right) {
    if constexpr (detail::HasCompareV<const T&, Ordering(const U&)>) {
        return left.compare(right);
    } else if (left < right) {
        return Ordering::Less;
    } else if (right < left) {
        return Ordering::Greater;
    } else {
        return Ordering::Equal;
    }
}

/// The unit type.
struct Unit { };

bool operator==(Unit, Unit);
bool operator!=(Unit, Unit);
bool operator<(Unit, Unit);
bool operator>(Unit, Unit);
bool operator<=(Unit, Unit);
bool operator>=(Unit, Unit);

std::ostream& operator<<(std::ostream& stream, Unit);

/// The only value of the unit type.
constexpr static Unit UNIT{};

}

#endif
