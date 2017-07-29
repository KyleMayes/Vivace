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

#ifndef VCE_OPTION_HPP
#define VCE_OPTION_HPP

#include <vivace/utility.hpp>

namespace vce {

template <class T>
class Option;

namespace detail {
    template <class T, class U>
    static constexpr bool IsOptionConstructibleV =
        std::is_constructible_v<T, U&&> &&
        !std::is_same_v<std::decay_t<U>, std::in_place_t> &&
        !std::is_same_v<std::decay_t<U>, Option<T>>;
}

/// A type that may or may not contain a value.
template <class T>
class Option {
    template <class U>
    friend class Option;

    bool some;
    std::aligned_storage_t<sizeof(T), alignof(T)> value;

public:
    /// The type of values this option may contain.
    using some_t = T;

    /// Whether this type may be safely moved to another location in memory.
    static constexpr bool RELOCATABLE = is_relocatable<T>();

    /// Constructs an empty option.
    constexpr Option() : some{false} { }

    /// Constructs an option containing the supplied value.
    template <class U = T, Sfinae<detail::IsOptionConstructibleV<T, U>> = 0>
    Option(U&& value) : some{true} {
        new(&this->value) T(std::forward<U>(value));
    }

    /// Constructs an option containing a value constructed from the supplied arguments.
    template <class... N>
    Option(std::in_place_t, N&&... arguments) : some{true} {
        new(&value) T(std::forward<N>(arguments)...);
    }

    /// Constructs an option containing a value constructed from the supplied arguments.
    template <class U, class... N>
    Option(std::in_place_t, std::initializer_list<U> list, N&&... arguments) : some{true} {
        new(&value) T(list, std::forward<N>(arguments)...);
    }

    Option(const Option& other) : some{other.some} {
        copy(other);
    }

    template <class U>
    Option(const Option<U>& other) : some{other.some} {
        copy(other);
    }

    Option& operator=(Option& other) {
        return operator=<T>(other);
    }

    template <class U>
    Option& operator=(const Option<U>& other) {
        destroy();
        some = other.some;
        copy(other);
        return *this;
    }

    Option(Option&& other) : some{other.some} {
        move(std::move(other));
    }

    template <class U>
    Option(Option<U>&& other) : some{other.some} {
        move(std::move(other));
    }

    Option& operator=(Option&& other) {
        return operator=<T>(std::move(other));
    }

    template <class U>
    Option& operator=(Option<U>&& other) {
        destroy();
        if (other.some) {
            some = true;
            new(&value) T(other.unsafe_unwrap());
        }
        return *this;
    }

    ~Option() {
        destroy();
    }

    /// Returns whether this option contains a value.
    bool is_some() const {
        return some;
    }

    /// Returns whether this option is empty.
    bool is_none() const {
        return !is_some();
    }

    /// Returns a reference to the value in this option if possible.
    Option<Ref<T>> as_ref() {
        if (some) {
            return {std::ref(unsafe_get())};
        } else {
            return {};
        }
    }

    /// Returns a reference to the value in this option if possible.
    Option<Ref<const T>> as_ref() const {
        if (some) {
            return {std::cref(unsafe_get())};
        } else {
            return {};
        }
    }

    /// Returns the value in this option or throws an exception if this option is empty.
    T unwrap() {
        if (some) {
            return unsafe_unwrap();
        } else {
            throw std::logic_error{"attempted to unwrap the value in an empty option"};
        }
    }

    /// Returns the value in this option or the supplied value if this option is empty.
    T unwrap_or(T value) {
        if (some) {
            return unsafe_unwrap();
        } else {
            return value;
        }
    }

    /// Returns the value in this option or the result of invoking the supplied function if this
    /// option is empty.
    template <class F>
    T unwrap_or_else(F f) {
        if (some) {
            return unsafe_unwrap();
        } else {
            return std::invoke(f);
        }
    }

    /// Returns the result of invoking the supplied function on the value in this option if
    /// possible.
    template <class F>
    auto map(F f) -> Option<decltype(std::invoke(f, unwrap()))> {
        if (some) {
            return {std::invoke(f, unsafe_unwrap())};
        } else {
            return {};
        }
    }

    /// Returns the result of invoking the supplied function on the value in this option or the
    /// supplied value if this option is empty.
    template <class U, class F>
    U map_or(U value, F f) {
        if (some) {
            return std::invoke(f, unsafe_unwrap());
        } else {
            return value;
        }
    }

    /// Returns the result of invoking the second supplied function on the value in this option or
    /// the result of invoking the first supplied function if this option is empty.
    template <class G, class F>
    auto map_or_else(G g, F f)-> decltype(std::invoke(g)) {
        if (some) {
            return std::invoke(f, unsafe_unwrap());
        } else {
            return std::invoke(g);
        }
    }

    /// Returns the result of invoking the supplied function on the value in this option if
    /// possible.
    template <class F>
    auto and_then(F f) -> Option<typename decltype(std::invoke(f, unwrap()))::some_t> {
        if (some) {
            return std::invoke(f, unsafe_unwrap());
        } else {
            return {};
        }
    }

    /// Returns the ordering of this option and the supplied option.
    template <class U>
    Ordering compare(const Option<U>& other) const {
        if (some && other.some) {
            return vce::compare(unsafe_get(), other.unsafe_get());
        } else {
            return vce::compare(some, other.some);
        }
    }

    /// Returns the hash code for this option.
    size_t hash() const {
        if (some) {
            return std::hash<T>{}(unsafe_get());
        } else {
            return 0;
        }
    }

    template <class U>
    friend bool operator==(const Option& left, const Option<U>& right) {
        if (left.some == right.some) {
            if (left.some) {
                return left.unsafe_get() == right.unsafe_get();
            } else {
                return true;
            }
        } else {
            return false;
        }
    }

    template <class U>
    friend bool operator!=(const Option& left, const Option<U>& right) {
        return !operator==(left, right);
    }

    template <class U>
    friend bool operator<(const Option& left, const Option<U>& right) {
        return left.compare(right) == Ordering::Less;
    }

    template <class U>
    friend bool operator>(const Option& left, const Option<U>& right) {
        return left.compare(right) == Ordering::Greater;
    }

    template <class U>
    friend bool operator<=(const Option& left, const Option<U>& right) {
        return !operator>(left, right);
    }

    template <class U>
    friend bool operator>=(const Option& left, const Option<U>& right) {
        return !operator<(left, right);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Option& option) {
        if (option.some) {
            return stream << "Some(" << option.unsafe_get() << ")";
        } else {
            return stream << "None";
        }
    }

private:
    T& unsafe_get() {
        return reinterpret_cast<T&>(value);
    }

    const T& unsafe_get() const {
        return reinterpret_cast<const T&>(value);
    }

    T unsafe_unwrap() {
        some = false;
        return std::move(unsafe_get());
    }

    template <class U>
    void copy(const Option<U>& other) {
        if (some) {
            new(&value) T(other.unsafe_get());
        }
    }

    template <class U>
    void move(Option<U>&& other) {
        if (some) {
            new(&value) T(other.unsafe_unwrap());
        }
    }

    void destroy() {
        if (some) {
            unsafe_unwrap();
        }
    }
};

}

namespace std {

template <class T>
struct hash<vce::Option<T>> {
    size_t operator()(const vce::Option<T>& option) const {
        return option.hash();
    }
};

}

#endif
