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

#ifndef VCE_MATH_HPP
#define VCE_MATH_HPP

#include <vivace/option.hpp>

namespace vce {

/// Returns the sum of the two supplied values unless the sum overflows.
template <class T>
Option<T> checked_add(T left, T right) {
    if (!__builtin_add_overflow(left, right, &left)) {
        return {left};
    } else {
        return {};
    }
}

/// Returns the difference of the two supplied values unless the difference overflows.
template <class T>
Option<T> checked_sub(T left, T right) {
    if (!__builtin_sub_overflow(left, right, &left)) {
        return {left};
    } else {
        return {};
    }
}

/// Returns the product of the two supplied values unless the product overflows.
template <class T>
Option<T> checked_mul(T left, T right) {
    if (!__builtin_mul_overflow(left, right, &left)) {
        return {left};
    } else {
        return {};
    }
}

/// Returns the quotient of the two supplied values unless the denominator is zero.
template <class T>
Option<T> checked_div(T left, T right) {
    if (right != static_cast<T>(0)) {
        return {left / right};
    } else {
        return {};
    }
}

/// Returns the sum of the two supplied values or the closest representable value to the real sum if
/// the sum overflows.
template <class T>
T saturating_add(T left, T right) {
    if (auto sum = checked_add(left, right); sum.is_some()) {
        return sum.unwrap();
    } else if (left < 0) {
        return Limits<T>::min();
    } else {
        return Limits<T>::max();
    }
}

/// Returns the difference of the two supplied values or the closest representable value to the real
/// difference if the difference overflows.
template <class T>
T saturating_sub(T left, T right) {
    if (auto difference = checked_sub(left, right); difference.is_some()) {
        return difference.unwrap();
    } else if (left < 0) {
        return Limits<T>::min();
    } else {
        return Limits<T>::max();
    }
}

/// Returns the product of the two supplied values or the closest representable value to the real
/// product if the product overflows.
template <class T>
T saturating_mul(T left, T right) {
    if (auto product = checked_mul(left, right); product.is_some()) {
        return product.unwrap();
    } else if ((left < 0) != (right < 0)) {
        return Limits<T>::min();
    } else {
        return Limits<T>::max();
    }
}

}

#endif
