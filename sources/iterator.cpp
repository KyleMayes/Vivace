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

#include <vivace/iterator.hpp>

namespace vce {

Bounds::Bounds(size_t lower) : lower{lower} { }

Bounds::Bounds(size_t lower, size_t upper) : lower{lower}, upper{upper} { }

Bounds::Bounds(size_t lower, Option<size_t> upper) : lower{lower}, upper{upper} { }

std::ostream& operator<<(std::ostream& stream, Bounds bounds) {
    return stream << "(" << bounds.lower << ", " << bounds.upper << ")";
}

bool operator==(Bounds left, Bounds right) {
    return left.lower == right.lower && left.upper == right.upper;
}

bool operator!=(Bounds left, Bounds right) {
    return !operator==(left, right);
}

}
