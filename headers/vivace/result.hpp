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

#ifndef VCE_RESULT_HPP
#define VCE_RESULT_HPP

#include <vivace/utility.hpp>

namespace vce {

/// A type which helps construct results containing values.
struct Ok { };

/// The only value of the ok type.
static constexpr Ok OK{};

/// A type which helps construct results containing errors.
struct Err { };

/// The only value of the err type.
static constexpr Err ERR{};

/// A type that may contain either a value or an error.
template <class T, class E>
class Result {
    template <class U, class F>
    friend class Result;

    bool ok;
    std::aligned_union_t<0, T, E> either;

public:
    /// The type of values this result may contain.
    using ok_t = T;
    /// The type of errors this result may contain.
    using err_t = E;

    /// Whether this type may be safely moved to another location in memory.
    static constexpr bool RELOCATABLE = is_relocatable<T>() && is_relocatable<E>();

    /// Constructs a result containing the supplied value.
    template <class U>
    Result(Ok, U&& value) : ok{true} {
        new(&either) T(std::forward<U>(value));
    }

    /// Constructs a result containing a value constructed from the supplied arguments.
    template <class... N>
    Result(Ok, std::in_place_t, N&&... arguments) : ok{true} {
        new(&either) T(std::forward<N>(arguments)...);
    }

    /// Constructs a result containing a value constructed from the supplied arguments.
    template <class U, class... N>
    Result(Ok, std::in_place_t, std::initializer_list<U> list, N&&... arguments) : ok{true} {
        new(&either) T(list, std::forward<N>(arguments)...);
    }

    /// Constructs a result containing the supplied error.
    template <class U>
    Result(Err, U&& error) : ok{false} {
        new(&either) E(std::forward<U>(error));
    }

    /// Constructs a result containing an error constructed from the supplied arguments.
    template <class... N>
    Result(Err, std::in_place_t, N&&... arguments) : ok{false} {
        new(&either) E(std::forward<N>(arguments)...);
    }

    /// Constructs a result containing an error constructed from the supplied arguments.
    template <class U, class... N>
    Result(Err, std::in_place_t, std::initializer_list<U> list, N&&... arguments) : ok{false} {
        new(&either) E(list, std::forward<N>(arguments)...);
    }

    Result(const Result& other) : ok{other.ok} {
        copy(other);
    }

    Result& operator=(const Result& other) {
        destroy();
        ok = other.ok;
        copy(other);
        return *this;
    }

    Result(Result&& other) : ok{other.ok} {
        move(std::move(other));
    }

    Result& operator=(Result&& other) {
        destroy();
        ok = other.ok;
        move(std::move(other));
        return *this;
    }

    ~Result() {
        destroy();
    }

    /// Returns whether this result contains a value.
    bool is_ok() const {
        return ok;
    }

    /// Returns whether this result contains an error.
    bool is_err() const {
        return !is_ok();
    }

    /// Returns a reference to the value or error in this result.
    Result<Ref<T>, Ref<E>> as_ref() {
        if (ok) {
            return {OK, std::ref(unsafe_get())};
        } else {
            return {ERR, std::ref(unsafe_get_err())};
        }
    }

    /// Returns a reference to the value or error in this result.
    Result<Ref<const T>, Ref<const E>> as_ref() const {
        if (ok) {
            return {OK, std::cref(unsafe_get())};
        } else {
            return {ERR, std::cref(unsafe_get_err())};
        }
    }

    /// Returns the value in this result or throws an exception if this result contains an error.
    T unwrap() {
        if (ok) {
            return unsafe_unwrap();
        } else {
            throw std::logic_error{"attempted to unwrap the value in a result containing an error"};
        }
    }

    /// Returns the error in this result or throws an exception if this result contains a value.
    E unwrap_err() {
        if (!ok) {
            return unsafe_unwrap_err();
        } else {
            throw std::logic_error{"attempted to unwrap the error in a result containing a value"};
        }
    }

    /// Returns the value in this result or the supplied value if this result contains an error.
    T unwrap_or(T value) {
        if (ok) {
            return unsafe_unwrap();
        } else {
            return value;
        }
    }

    /// Returns the value in this result or the result of invoking the supplied function if this
    /// result contains an error.
    template <class F>
    T unwrap_or_else(F f) {
        if (ok) {
            return unsafe_unwrap();
        } else {
            return std::invoke(f);
        }
    }

    /// Returns the result of invoking the supplied function on the value in this result if
    /// possible.
    template <class F>
    auto map(F f) -> Result<decltype(std::invoke(f, unwrap())), E> {
        if (ok) {
            return {OK, std::invoke(f, unsafe_unwrap())};
        } else {
            return {ERR, unsafe_unwrap_err()};
        }
    }

    /// Returns the result of invoking the supplied function on the error in this result if
    /// possible.
    template <class F>
    auto map_err(F f) -> Result<T, decltype(std::invoke(f, unwrap_err()))> {
        if (!ok) {
            return {ERR, std::invoke(f, unsafe_unwrap_err())};
        } else {
            return {OK, unsafe_unwrap()};
        }
    }

    /// Returns the result of invoking the supplied function on the value in this result or the
    /// supplied value if this result contains an error.
    template <class U, class F>
    U map_or(U value, F f) {
        if (ok) {
            return std::invoke(f, unsafe_unwrap());
        } else {
            return value;
        }
    }

    /// Returns the result of invoking the second supplied function on the value in this result or
    /// the result of invoking the first supplied function if this result contains an error.
    template <class G, class F>
    auto map_or_else(G g, F f)-> decltype(std::invoke(g)) {
        if (ok) {
            return std::invoke(f, unsafe_unwrap());
        } else {
            return std::invoke(g);
        }
    }

    /// Returns the result of invoking the supplied function on the value in this result if
    /// possible.
    template <class F>
    auto and_then(F f) -> Result<typename decltype(std::invoke(f, unwrap()))::ok_t, E> {
        if (ok) {
            return std::invoke(f, unsafe_unwrap());
        } else {
            return {ERR, unsafe_unwrap_err()};
        }
    }

    /// Returns the ordering of this result and the supplied result.
    template <typename U, typename F>
    Ordering compare(const Result<U, F>& other) const {
        if (ok && other.ok) {
            return vce::compare(unsafe_get(), other.unsafe_get());
        } else if (!ok && !other.ok) {
            return vce::compare(unsafe_get_err(), other.unsafe_get_err());
        } else {
            return vce::compare(other.ok, ok);
        }
    }

    /// Returns the hash code for this result.
    size_t hash() const {
        if (ok) {
            return std::hash<T>{}(unsafe_get());
        } else {
            auto code = std::hash<E>{}(unsafe_get_err());
            code ^= 0x9e3779b9 + (code << 6) + (code >> 2);
            return code;
        }
    }

    template <class U, class F>
    friend bool operator==(const Result& left, const Result<U, F>& right) {
        if (left.ok == right.ok) {
            if (left.ok) {
                return left.unsafe_get() == right.unsafe_get();
            } else {
                return left.unsafe_get_err() == right.unsafe_get_err();
            }
        } else {
            return false;
        }
    }

    template <class U, class F>
    friend bool operator!=(const Result& left, const Result<U, F>& right) {
        return !operator==(left, right);
    }

    template <class U, class F>
    friend bool operator<(const Result& left, const Result<U, F>& right) {
        return left.compare(right) == Ordering::Less;
    }

    template <class U, class F>
    friend bool operator>(const Result& left, const Result<U, F>& right) {
        return left.compare(right) == Ordering::Greater;
    }

    template <class U, class F>
    friend bool operator<=(const Result& left, const Result<U, F>& right) {
        return !operator>(left, right);
    }

    template <class U, class F>
    friend bool operator>=(const Result& left, const Result<U, F>& right) {
        return !operator<(left, right);
    }

    friend std::ostream& operator<<(std::ostream& stream, const Result& result) {
        if (result.ok) {
            return stream << "Ok(" << result.unsafe_get() << ")";
        } else {
            return stream << "Err(" << result.unsafe_get_err() << ")";
        }
    }

private:
    T& unsafe_get() {
        return reinterpret_cast<T&>(either);
    }

    const T& unsafe_get() const {
        return reinterpret_cast<const T&>(either);
    }

    E& unsafe_get_err() {
        return reinterpret_cast<E&>(either);
    }

    const E& unsafe_get_err() const {
        return reinterpret_cast<const E&>(either);
    }

    T unsafe_unwrap() {
        return std::move(unsafe_get());
    }

    E unsafe_unwrap_err() {
        return std::move(unsafe_get_err());
    }

    void copy(const Result& other) {
        if (ok) {
            new(&either) T(other.unsafe_get());
        } else {
            new(&either) E(other.unsafe_get_err());
        }
    }

    void move(Result&& other) {
        if (ok) {
            new(&either) T(other.unsafe_unwrap());
        } else {
            new(&either) E(other.unsafe_unwrap_err());
        }
    }

    void destroy() {
        if (ok) {
            unsafe_unwrap();
        } else {
            unsafe_unwrap_err();
        }
    }
};

}

namespace std {

template <class T, class E>
struct hash<vce::Result<T, E>> {
    size_t operator()(const vce::Result<T, E>& result) const {
        return result.hash();
    }
};

}

#endif
