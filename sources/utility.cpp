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

#include <vivace/utility.hpp>

namespace vce {

std::ostream& operator<<(std::ostream& stream, Ordering ordering) {
    switch (ordering) {
    case Ordering::Less:
        return stream << "Less";
    case Ordering::Greater:
        return stream << "Greater";
    case Ordering::Equal:
        return stream << "Equal";
    default:
        return stream << "?";
    }
}

bool operator==(Unit, Unit) {
    return true;
}

bool operator!=(Unit, Unit) {
    return false;
}

bool operator<(Unit, Unit) {
    return false;
}

bool operator>(Unit, Unit) {
    return false;
}

bool operator<=(Unit, Unit) {
    return true;
}

bool operator>=(Unit, Unit) {
    return true;
}

std::ostream& operator<<(std::ostream& stream, Unit) {
    return stream << "()";
}

}
