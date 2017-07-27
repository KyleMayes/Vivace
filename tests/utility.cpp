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

#include <accelerando.hpp>

ACCEL_TESTS

#include <vivace/utility.hpp>

using namespace vce;

template <class T>
struct Celsius {
    T value;

    Celsius(T value) : value{value} { }
};

template <class T, class U>
bool operator<(Celsius<T> left, Celsius<U> right) {
    return left.value < right.value;
}

template <class T>
struct Kelvin {
    T value;

    Kelvin(T value) : value{value} { }

    template <class U>
    Ordering compare(Kelvin<U> other) const {
        return vce::compare(value, other.value);
    }
};

template <class T, class U>
bool operator<(Kelvin<T>, Kelvin<U>) {
    throw std::runtime_error{"unreachable"};
}

TEST(Compare) {
    ASSERT_EQ(compare(4, 17.0f), Ordering::Less);
    ASSERT_EQ(compare(322, 17.0f), Ordering::Greater);
    ASSERT_EQ(compare(17, 17.0f), Ordering::Equal);

    ASSERT_EQ(compare(Celsius{4}, Celsius{17.0f}), Ordering::Less);
    ASSERT_EQ(compare(Celsius{322}, Celsius{17.0f}), Ordering::Greater);
    ASSERT_EQ(compare(Celsius{17}, Celsius{17.0f}), Ordering::Equal);

    ASSERT_EQ(compare(Kelvin{4}, Kelvin{17.0f}), Ordering::Less);
    ASSERT_EQ(compare(Kelvin{322}, Kelvin{17.0f}), Ordering::Greater);
    ASSERT_EQ(compare(Kelvin{17}, Kelvin{17.0f}), Ordering::Equal);
}
