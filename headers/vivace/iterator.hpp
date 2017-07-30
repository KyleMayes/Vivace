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

#ifndef VCE_ITERATOR_HPP
#define VCE_ITERATOR_HPP

#include <vivace/math.hpp>

#include <algorithm>
#include <iterator>

namespace vce {

/// A pair of bounds on the size of an iterator.
struct Bounds {
    /// The lower bound on the size of the iterator.
    size_t lower;
    /// The upper bound on the size of the iterator, if known.
    Option<size_t> upper;

    /// Constructs a pair of bounds where only the lower bound is known.
    Bounds(size_t lower);
    /// Constructs a pair of bounds where both the lower and upper bounds are known.
    Bounds(size_t lower, size_t upper);
    /// Constructs a pair of bounds where the lower bound is known and the upper bound may be known.
    Bounds(size_t lower, Option<size_t> upper);
};

bool operator==(Bounds left, Bounds right);
bool operator!=(Bounds left, Bounds right);

std::ostream& operator<<(std::ostream& stream, Bounds bounds);

template <class T, class I>
class Iterator;

namespace detail {
    template <class T, class I>
    class IteratorRef : public Iterator<T, IteratorRef<T, I>> {
        Ref<I> source;

    protected:
        Bounds bounds_impl() const {
            return source.get().bounds();
        }

        size_t size_impl() const {
            return source.get().size();
        }

        Option<T> next_impl() {
            return source.get().next();
        }

        Option<T> next_back_impl() {
            return source.get().next_back();
        }

    public:
        IteratorRef(Ref<I> source) : source{source} { }
    };

    #include <vivace/iterator/chain.hpp>
    #include <vivace/iterator/enumerate.hpp>
    #include <vivace/iterator/filter.hpp>
    #include <vivace/iterator/filter_map.hpp>
    #include <vivace/iterator/map.hpp>
    #include <vivace/iterator/reverse.hpp>
    #include <vivace/iterator/skip.hpp>
    #include <vivace/iterator/skip_while.hpp>
    #include <vivace/iterator/take.hpp>
    #include <vivace/iterator/take_while.hpp>
    #include <vivace/iterator/zip.hpp>

    VCE_HAS_MEMBER_FUNCTION(HasReserve, reserve);
    VCE_HAS_MEMBER_FUNCTION(HasPushBack, push_back);

    template <class I, class C>
    void reserve(I& iterator, C& collection) {
        if constexpr (HasReserve<C, void(size_t)>::value) {
            if constexpr (I::HAS_SIZE) {
                collection.reserve(iterator.size());
            } else {
                collection.reserve(iterator.bounds.lower);
            }
        }
    }

    template <class C, class T>
    void add(C& collection, T value) {
        if constexpr (HasPushBack<C, void(T)>::value) {
            collection.push_back(std::move(value));
        } else {
            collection.insert(std::move(value));
        }
    }
}

/// An iterator.
template <class T, class I>
class Iterator {
    template <class F>
    using map_t = decltype(std::invoke(std::declval<F>(), std::declval<T>()));

    struct Crtp : private I {
        #define VCE_HAS(C, U, FUNCTION) decltype((std::declval<const U&>().*&C::FUNCTION)(), true)

        template <class C, class U>
        constexpr static auto has_size(int) -> VCE_HAS(C, U, size_impl) {
            return true;
        }

        template <class C, class U>
        constexpr static auto has_size(bool) -> bool {
            return false;
        }

        template <class C, class U>
        constexpr static auto has_next_back(int) -> VCE_HAS(C, U, next_back_impl) {
            return true;
        }

        template <class C, class U>
        constexpr static auto has_next_back(bool) -> bool {
            return false;
        }

        #undef VCE_HAS

        static Bounds bounds(const I& iterator) {
            Bounds (I::*function)() const = &Crtp::bounds_impl;
            return (iterator.*function)();
        }

        static size_t size(const I& iterator) {
            size_t (I::*function)() const = &Crtp::size_impl;
            return (iterator.*function)();
        }

        static Option<T> next(I& iterator) {
            Option<T> (I::*function)() = &Crtp::next_impl;
            return (iterator.*function)();
        }

        static Option<T> next_back(I& iterator) {
            Option<T> (I::*function)() = &Crtp::next_back_impl;
            return (iterator.*function)();
        }
    };

    /// An end range-based for loop iterator.
    class End { };

    /// A start range-based for loop iterator.
    class Begin {
        Iterator<T, I>* iterator;
        Option<T> item;

    public:
        Begin(Iterator<T, I>* iterator) : iterator{iterator}, item{iterator->next()} { }

        T operator*() {
            return item.unwrap();
        }

        void operator++() {
            item = iterator->next();
        }

        bool operator!=(End) {
            return item.is_some();
        }
    };

public:
    /// The type of items emitted by this iterator.
    using item_t = T;

    /// Whether this iterator knows its exact size.
    static constexpr bool HAS_SIZE = Crtp::template has_size<Crtp, I>(0);
    /// Whether this iterator supports emitting items from the back.
    static constexpr bool HAS_NEXT_BACK = Crtp::template has_next_back<Crtp, I>(0);

    /// Returns a pair of bounds on the size of this iterator.
    Bounds bounds() const {
        return Crtp::bounds(static_cast<const I&>(*this));
    }

    /// Returns the number of items remaining in this iterator.
    size_t size() const {
        return Crtp::size(static_cast<const I&>(*this));
    }

    /// Returns the next item in this iterator.
    Option<T> next() {
        return Crtp::next(static_cast<I&>(*this));
    }

    /// Returns the next item at the end of this iterator.
    Option<T> next_back() {
        return Crtp::next_back(static_cast<I&>(*this));
    }

    /// Returns a start range-based for loop iterator.
    auto begin() {
        return Begin{this};
    }

    /// Returns an end range-based for loop iterator.
    auto end() {
        return End{};
    }

    /// Returns a non-owning reference to this iterator.
    detail::IteratorRef<T, I> as_ref() {
        return {std::ref(static_cast<I&>(*this))};
    }

    /// Returns an iterator that first emits the items in this iterator and then emits the items in
    /// the supplied iterator.
    template <class R>
    detail::Chain<T, I, R> chain(R iterator) {
        return {static_cast<I&&>(*this), std::move(iterator)};
    }

    /// Returns an iterator that emits the items in this iterator and their position as pairs.
    detail::Enumerate<std::pair<size_t, T>, I> enumerate() {
        return {static_cast<I&&>(*this)};
    }

    /// Returns an iterator that emits the items in this iterator that satisfy the supplied
    /// predicate.
    template <class F>
    detail::Filter<T, I, F> filter(F f) {
        return {static_cast<I&&>(*this), std::move(f)};
    }

    /// Returns an iterator that emits the items in this iterator that are mapped to non-empty
    /// options by the supplied function.
    template <class F>
    detail::FilterMap<typename map_t<F>::some_t, I, F> filter_map(F f) {
        return {static_cast<I&&>(*this), std::move(f)};
    }

    /// Returns an iterator that maps the items emitted by this iterator using the supplied
    /// function.
    template <class F>
    detail::Map<map_t<F>, I, F> map(F f) {
        return {static_cast<I&&>(*this), std::move(f)};
    }

    /// Returns an iterator that emits the items in this iterator in reverse.
    detail::Reverse<T, I> reverse() {
        return {static_cast<I&&>(*this)};
    }

    /// Returns an iterator that skips the supplied number of items in this iterator before
    /// emitting the remainder of the items.
    detail::Skip<T, I> skip(size_t n) {
        return {static_cast<I&&>(*this), n};
    }

    /// Returns an iterator that skips the items in this iterator that satisfy the supplied
    /// predicate before emitting the remainder of the items.
    template <class F>
    detail::SkipWhile<T, I, F> skip_while(F f) {
        return {static_cast<I&&>(*this), std::move(f)};
    }

    /// Returns an iterator that emits at most the supplied number of items in this iterator.
    detail::Take<T, I> take(size_t n) {
        return {static_cast<I&&>(*this), n};
    }

    /// Returns an iterator that emits the items in this iterator only as long as they all satisfy
    /// the supplied predicate.
    template <class F>
    detail::TakeWhile<T, I, F> take_while(F f) {
        return {static_cast<I&&>(*this), std::move(f)};
    }

    /// Returns an iterator that emits the items in this iterator and the supplied iterator together
    /// as pairs.
    template <class R>
    detail::Zip<std::pair<T, typename R::item_t>, I, R> zip(R iterator) {
        return {static_cast<I&&>(*this), std::move(iterator)};
    }

    /// Consumes this iterator and returns the number of items consumed.
    size_t count() {
        return fold(0, [](auto a, auto) { return a + 1; });
    }

    /// Consumes this iterator and returns the last item consumed, if any.
    Option<T> last() {
        Option<T> last;
        for (auto item : *this) {
            last = Option<T>{std::move(item)};
        }
        return last;
    }

    /// Consumes the supplied number of items and returns the last item consumed, if any.
    Option<T> nth(size_t n) {
        for (size_t i = 0; i < n; ++i) {
            if (next().is_none()) {
                return {};
            }
        }
        return next();
    }

    /// Consumes this iterator and returns the consumed items in a container.
    template <class C = std::vector<T>>
    C collect() {
        C collection;
        detail::reserve(*this, collection);
        for (auto item : *this) {
            detail::add(collection, std::move(item));
        }
        return collection;
    }

    /// Consumes this iterator and returns the consumed items partitioned into two containers by the
    /// supplied predicate.
    template <class C = std::vector<T>, class F>
    std::pair<C, C> partition(F f) {
        std::pair<C, C> collections;
        for (auto item : *this) {
            if (std::invoke(f, item)) {
                detail::add(collections.first, std::move(item));
            } else {
                detail::add(collections.second, std::move(item));
            }
        }
        return collections;
    }

    /// Consumes this iterator and returns the value accumulated by the supplied function.
    template <class U, class F>
    U fold(U seed, F f) {
        for (auto item : *this) {
            seed = std::invoke(f, std::move(seed), std::move(item));
        }
        return seed;
    }

    /// Consumes this iterator and returns the sum of the consumed items.
    T sum() {
        return fold(static_cast<T>(0), [](auto a, auto i) { return a + i; });
    }

    /// Consumes this iterator and returns the product of the consumed items.
    T product() {
        return fold(static_cast<T>(1), [](auto a, auto i) { return a * i; });
    }

    /// Consumes this iterator until it can return whether all of the consumed items satisfy the
    /// supplied predicate.
    template <class F>
    bool all(F f) {
        for (auto item : *this) {
            if (!std::invoke(f, item)) {
                return false;
            }
        }
        return true;
    }

    /// Consumes this iterator until it can return whether any of the consumed items satisfy the
    /// supplied predicate.
    template <class F>
    bool any(F f) {
        for (auto item : *this) {
            if (std::invoke(f, item)) {
                return true;
            }
        }
        return false;
    }

    /// Consumes this iterator until the first consumed item which satisfies the supplied predicate
    /// can be returned, if any.
    template <class F>
    Option<T> find(F f) {
        for (auto item : *this) {
            if (std::invoke(f, item)) {
                return {std::move(item)};
            }
        }
        return {};
    }

    /// Consumes this iterator until the position of the first consumed item which satisfies the
    /// supplied predicate can be returned, if any.
    template <class F>
    Option<size_t> position(F f) {
        size_t position = 0;
        for (auto item : *this) {
            if (std::invoke(f, item)) {
                return {position};
            }
            position += 1;
        }
        return {};
    }

    /// Consumes this iterator and returns the first minimal item consumed.
    Option<T> min() {
        return select(std::less{}, [](const auto& i) { return i; });
    }

    /// Consumes this iterator and returns the first minimal item consumed as ordered by the keys
    /// returned by the supplied function.
    template <class F>
    Option<T> min_by_key(F f) {
        return select(std::less{}, f);
    }

    /// Consumes this iterator and returns the last maximal item consumed.
    Option<T> max() {
        return select(std::greater_equal{}, [](const auto& i) { return i; });
    }

    /// Consumes this iterator and returns the last maximal item consumed as ordered by the keys
    /// returned by the supplied function.
    template <class F>
    Option<T> max_by_key(F f) {
        return select(std::greater_equal{}, f);
    }

private:
    template <class C, class F>
    Option<T> select(C comparator, F f) {
        auto selection = next();
        if (selection.is_some()) {
            auto&& skey = std::invoke(f, selection.as_ref().unwrap().get());
            for (auto item : *this) {
                auto&& ikey = std::invoke(f, item);
                if (comparator(ikey, skey)) {
                    selection = std::move(item);
                    skey = std::move(ikey);
                }
            }
        }
        return selection;
    }
};

/// An iterator over the items in a container.
template <class T, class I>
class ContainerIterator : public Iterator<T, ContainerIterator<T, I>> {
    using Tag = typename std::iterator_traits<I>::iterator_category;

    static constexpr bool BIDIRECTIONAL = std::is_same_v<Tag, std::bidirectional_iterator_tag>;
    static constexpr bool RANDOM_ACCESS = std::is_same_v<Tag, std::random_access_iterator_tag>;

    I begin_;
    I end_;

protected:
    Bounds bounds_impl() const {
        if constexpr (RANDOM_ACCESS) {
            auto size = size_impl();
            return {size, size};
        } else {
            return {0};
        }
    }

    size_t size_impl() const {
        static_assert(RANDOM_ACCESS);
        return std::distance(begin_, end_);
    }

    Option<T> next_impl() {
        if (begin_ != end_) {
            return {*begin_++};
        } else {
            return {};
        }
    }

    Option<T> next_back_impl() {
        static_assert(BIDIRECTIONAL || RANDOM_ACCESS);
        if (begin_ != end_) {
            return {*--end_};
        } else {
            return {};
        }
    }

public:
    /// Constructs an iterator over the items in the supplied container.
    ContainerIterator(I begin, I end) : begin_{std::move(begin)}, end_{std::move(end)} { }
};

/// Returns an iterator over the items in the supplied container.
template <class C>
ContainerIterator<Ref<const typename C::value_type>, typename C::const_iterator>
container(const C& container) {
    return {container.begin(), container.end()};
}

/// Returns an iterator over the items in the supplied container.
template <class C>
ContainerIterator<typename C::value_type, std::move_iterator<typename C::iterator>>
container(C&& container) {
    return {std::make_move_iterator(container.begin()), std::make_move_iterator(container.end())};
}

/// An iterator over a half-open range of integers.
template <class T>
class RangeIterator : public Iterator<T, RangeIterator<T>> {
    T begin_;
    T end_;

protected:
    Bounds bounds_impl() const {
        auto size = size_impl();
        return {size, size};
    }

    size_t size_impl() const {
        if (begin_ < end_) {
            return end_ - begin_;
        } else {
            return 0;
        }
    }

    Option<T> next_impl() {
        if (begin_ < end_) {
            auto item = begin_;
            begin_ += 1;
            return {item};
        } else {
            return {};
        }
    }

    Option<T> next_back_impl() {
        if (begin_ < end_) {
            auto item = end_ - 1;
            end_ -= 1;
            return {item};
        } else {
            return {};
        }
    }

public:
    /// Constructs an iterator over the supplied half-open range of integers.
    RangeIterator(T begin, T end) : begin_{begin}, end_{end} { }
};

/// Returns an iterator over the supplied half-open range of integers.
template <class T>
RangeIterator<T> range(T begin, T end) {
    return {begin, end};
}

}

#endif
