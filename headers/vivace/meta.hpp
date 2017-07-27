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

#ifndef VCE_META_HPP
#define VCE_META_HPP

#include <type_traits>

namespace vce {

/// A type which may stand in for any other type in a type trait.
struct Ignore { };

/// Defines a type trait which indicates whether a type has the supplied static or instance field.
#define VCE_HAS_FIELD(TRAIT, FIELD) \
    template <class T, class R = ::vce::Ignore> \
    class TRAIT; \
    template <class T> \
    class TRAIT<T, ::vce::Ignore> { \
    protected: \
        struct No { }; \
        template <class U> \
        static auto test(int) -> decltype(U::FIELD); \
        template <class U> \
        static auto test(bool) -> No; \
        using Result = decltype(test<T>(0)); \
    public: \
        static constexpr bool value = !std::is_same_v<Result, No>; \
    }; \
    template <class T, class R> \
    class TRAIT : private TRAIT<T> { \
        using base = TRAIT<T>; \
    public: \
        static constexpr bool value = std::is_same_v<typename base::Result, R>; \
    }; \
    template <class T, class R = ::vce::Ignore> \
    static constexpr bool TRAIT##V = TRAIT<T, R>::value

/// Defines a type trait which indicates whether the supplied function exists.
#define VCE_HAS_FUNCTION(TRAIT, FUNCTION) \
    template <class F> \
    class TRAIT; \
    template <class... N> \
    class TRAIT<::vce::Ignore(N...)> { \
    protected: \
        struct No { }; \
        template <class... O> \
        static auto test(int) -> decltype(FUNCTION(std::declval<O>()...)); \
        template <class... O> \
        static auto test(bool) -> No; \
        using Result = decltype(test<N...>(0)); \
    public: \
        static constexpr bool value = !std::is_same_v<Result, No>; \
    }; \
    template <class R, class... N> \
    class TRAIT<R(N...)> : private TRAIT<::vce::Ignore(N...)> { \
        using base = TRAIT<::vce::Ignore(N...)>; \
    public: \
        static constexpr bool value = std::is_same_v<typename base::Result, R>; \
    }; \
    template <class F> \
    static constexpr bool TRAIT##V = TRAIT<F>::value

/// Defines a type trait which indicates whether a type has the supplied member function.
#define VCE_HAS_MEMBER_FUNCTION(TRAIT, FUNCTION) \
    template <class T, class F> \
    class TRAIT; \
    template <class T, class... N> \
    class TRAIT<T, ::vce::Ignore(N...)> { \
    protected: \
        struct No { }; \
        template <class U, class... O> \
        static auto test(int) -> decltype(std::declval<U>().FUNCTION(std::declval<O>()...)); \
        template <class U, class... O> \
        static auto test(bool) -> No; \
        using Result = decltype(test<T, N...>(0)); \
    public: \
        static constexpr bool value = !std::is_same_v<Result, No>; \
    }; \
    template <class T, class R, class... N> \
    class TRAIT<T, R(N...)> : private TRAIT<T, ::vce::Ignore(N...)> { \
        using base = TRAIT<T, ::vce::Ignore(N...)>; \
    public: \
        static constexpr bool value = std::is_same_v<typename base::Result, R>; \
    }; \
    template <class T, class F> \
    static constexpr bool TRAIT##V = TRAIT<T, F>::value

}

#endif
