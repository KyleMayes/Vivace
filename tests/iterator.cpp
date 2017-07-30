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

#include <vivace/iterator.hpp>

using namespace vce;

#include <unordered_map>
#include <vector>

namespace std {

template <class L, class R>
std::ostream& operator<<(std::ostream& stream, const std::pair<L, R>& pair) {
    return stream << "(" << pair.first << ", " << pair.second << ")";
}

}

template <class I>
using item_t = typename std::decay_t<I>::item_t;

using UP = std::unique_ptr<int>;
UP make(int value) { return std::make_unique<int>(value); }

ASSERTION_GROUP_T(next,
ACCEL_GROUP(class I), I&& iterator, Bounds bounds, Option<item_t<I>> item) {
    ASSERT_EQ(iterator.bounds(), bounds);
    ASSERT_EQ(iterator.next(), item);
}

ASSERTION_GROUP_T(next,
ACCEL_GROUP(class I), I&& iterator, size_t size, Option<item_t<I>> item) {
    ASSERT_EQ(iterator.bounds(), (Bounds{size, size}));
    ASSERT_EQ(iterator.size(), size);
    ASSERT_EQ(iterator.next(), item);
}

ASSERTION_GROUP_T(next_back,
ACCEL_GROUP(class I), I&& iterator, Bounds bounds, Option<item_t<I>> item) {
    ASSERT_EQ(iterator.bounds(), bounds);
    ASSERT_EQ(iterator.next_back(), item);
}

ASSERTION_GROUP_T(next_back,
ACCEL_GROUP(class I), I&& iterator, size_t size, Option<item_t<I>> item) {
    ASSERT_EQ(iterator.bounds(), (Bounds{size, size}));
    ASSERT_EQ(iterator.size(), size);
    ASSERT_EQ(iterator.next_back(), item);
}

ASSERTION_GROUP_T(empty,
ACCEL_GROUP(class I), I&& iterator) {
    if constexpr (std::decay_t<I>::HAS_SIZE) {
        ASSERT_GROUP(next, iterator, 0, {});
        ASSERT_GROUP(next, iterator, 0, {});
    } else {
        ASSERT_GROUP(next, iterator, {0, 0}, {});
        ASSERT_GROUP(next, iterator, {0, 0}, {});
    }
}

TEST(Container) {
    std::vector<UP> vector;
    vector.push_back(make(4));
    vector.push_back(make(17));
    vector.push_back(make(322));

    auto iter1 = container(vector);
    ASSERT_EQ(iter1.bounds(), (Bounds{3, 3}));
    ASSERT_EQ(iter1.size(), 3);
    ASSERT_EQ(*iter1.next().unwrap().get(), 4);
    ASSERT_EQ(iter1.bounds(), (Bounds{2, 2}));
    ASSERT_EQ(iter1.size(), 2);
    ASSERT_EQ(*iter1.next().unwrap().get(), 17);
    ASSERT_EQ(iter1.bounds(), (Bounds{1, 1}));
    ASSERT_EQ(iter1.size(), 1);
    ASSERT_EQ(*iter1.next().unwrap().get(), 322);
    ASSERT_EQ(iter1.size(), 0);
    ASSERT_EQ(iter1.bounds(), (Bounds{0, 0}));
    ASSERT_THROW(iter1.next().unwrap());

    auto iter2 = container(vector);
    ASSERT_EQ(iter2.bounds(), (Bounds{3, 3}));
    ASSERT_EQ(iter2.size(), 3);
    ASSERT_EQ(*iter2.next_back().unwrap().get(), 322);
    ASSERT_EQ(iter2.bounds(), (Bounds{2, 2}));
    ASSERT_EQ(iter2.size(), 2);
    ASSERT_EQ(*iter2.next_back().unwrap().get(), 17);
    ASSERT_EQ(iter2.bounds(), (Bounds{1, 1}));
    ASSERT_EQ(iter2.size(), 1);
    ASSERT_EQ(*iter2.next_back().unwrap().get(), 4);
    ASSERT_EQ(iter2.size(), 0);
    ASSERT_EQ(iter2.bounds(), (Bounds{0, 0}));
    ASSERT_THROW(iter2.next_back().unwrap());

    auto iter3 = container(std::move(vector));
    ASSERT_EQ(iter3.bounds(), (Bounds{3, 3}));
    ASSERT_EQ(iter3.size(), 3);
    ASSERT_EQ(*iter3.next().unwrap(), 4);
    ASSERT_EQ(iter3.bounds(), (Bounds{2, 2}));
    ASSERT_EQ(iter3.size(), 2);
    ASSERT_EQ(*iter3.next().unwrap(), 17);
    ASSERT_EQ(iter3.bounds(), (Bounds{1, 1}));
    ASSERT_EQ(iter3.size(), 1);
    ASSERT_EQ(*iter3.next().unwrap(), 322);
    ASSERT_EQ(iter3.size(), 0);
    ASSERT_EQ(iter3.bounds(), (Bounds{0, 0}));
    ASSERT_THROW(iter3.next().unwrap());
}

TEST(Range) {
    ASSERT_GROUP(empty, range(1, 0));
    ASSERT_GROUP(empty, range(1, 1));

    auto iter1 = range(1, 4);
    ASSERT_GROUP(next, iter1, 3, {1});
    ASSERT_GROUP(next, iter1, 2, {2});
    ASSERT_GROUP(next, iter1, 1, {3});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 4);
    ASSERT_GROUP(next_back, iter2, 3, {3});
    ASSERT_GROUP(next_back, iter2, 2, {2});
    ASSERT_GROUP(next_back, iter2, 1, {1});
    ASSERT_GROUP(empty, iter2);
}

TEST(ForEach) {
    std::vector<int> integers;
    for (auto i : range(1, 4)) {
        integers.push_back(i);
    }
    ASSERT_EQ(integers, (std::vector<int>{1, 2, 3}));
}

TEST(Chain) {
    ASSERT_GROUP(empty, range(1, 1).chain(range(1, 1)));

    auto iter1 = range(1, 4).chain(range(1, 1));
    ASSERT_GROUP(next, iter1, 3, {1});
    ASSERT_GROUP(next, iter1, 2, {2});
    ASSERT_GROUP(next, iter1, 1, {3});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 1).chain(range(1, 4));
    ASSERT_GROUP(next, iter2, 3, {1});
    ASSERT_GROUP(next, iter2, 2, {2});
    ASSERT_GROUP(next, iter2, 1, {3});
    ASSERT_GROUP(empty, iter2);

    auto iter3 = range(1, 4).chain(range(1, 4));
    ASSERT_GROUP(next, iter3, 6, {1});
    ASSERT_GROUP(next, iter3, 5, {2});
    ASSERT_GROUP(next, iter3, 4, {3});
    ASSERT_GROUP(next, iter3, 3, {1});
    ASSERT_GROUP(next, iter3, 2, {2});
    ASSERT_GROUP(next, iter3, 1, {3});
    ASSERT_GROUP(empty, iter3);

    auto iter4 = range(1, 4).chain(range(1, 1));
    ASSERT_GROUP(next_back, iter4, 3, {3});
    ASSERT_GROUP(next_back, iter4, 2, {2});
    ASSERT_GROUP(next_back, iter4, 1, {1});
    ASSERT_GROUP(empty, iter4);

    auto iter5 = range(1, 1).chain(range(1, 4));
    ASSERT_GROUP(next_back, iter5, 3, {3});
    ASSERT_GROUP(next_back, iter5, 2, {2});
    ASSERT_GROUP(next_back, iter5, 1, {1});
    ASSERT_GROUP(empty, iter5);

    auto iter6 = range(1, 4).chain(range(1, 4));
    ASSERT_GROUP(next_back, iter6, 6, {3});
    ASSERT_GROUP(next_back, iter6, 5, {2});
    ASSERT_GROUP(next_back, iter6, 4, {1});
    ASSERT_GROUP(next_back, iter6, 3, {3});
    ASSERT_GROUP(next_back, iter6, 2, {2});
    ASSERT_GROUP(next_back, iter6, 1, {1});
    ASSERT_GROUP(empty, iter6);
}

TEST(Enumerate) {
    ASSERT_GROUP(empty, range(1, 1).enumerate());

    auto iter1 = range(1, 4).enumerate();
    ASSERT_GROUP(next, iter1, 3, {{0, 1}});
    ASSERT_GROUP(next, iter1, 2, {{1, 2}});
    ASSERT_GROUP(next, iter1, 1, {{2, 3}});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 4).enumerate();
    ASSERT_GROUP(next_back, iter2, 3, {{2, 3}});
    ASSERT_GROUP(next_back, iter2, 2, {{1, 2}});
    ASSERT_GROUP(next_back, iter2, 1, {{0, 1}});
    ASSERT_GROUP(empty, iter2);
}

TEST(Filter) {
    ASSERT_GROUP(empty, range(1, 1).filter([](auto) { return true; }));

    auto f = [](auto i) { return i % 2 != 0; };

    auto iter1 = range(1, 7).filter(f);
    ASSERT_GROUP(next, iter1, {0, 6}, {1});
    ASSERT_GROUP(next, iter1, {0, 5}, {3});
    ASSERT_GROUP(next, iter1, {0, 3}, {5});
    ASSERT_GROUP(next, iter1, {0, 1}, {});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 7).filter(f);
    ASSERT_GROUP(next_back, iter2, {0, 6}, {5});
    ASSERT_GROUP(next_back, iter2, {0, 4}, {3});
    ASSERT_GROUP(next_back, iter2, {0, 2}, {1});
    ASSERT_GROUP(empty, iter2);
}

TEST(FilterMap) {
    ASSERT_GROUP(empty, range(1, 1).filter_map([](auto i) { return Option<int>{i}; }));

    auto f = [](auto i) { return i % 2 != 0 ? Option<int>{i} : Option<int>{}; };

    auto iter1 = range(1, 7).filter_map(f);
    ASSERT_GROUP(next, iter1, {0, 6}, {1});
    ASSERT_GROUP(next, iter1, {0, 5}, {3});
    ASSERT_GROUP(next, iter1, {0, 3}, {5});
    ASSERT_GROUP(next, iter1, {0, 1}, {});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 7).filter_map(f);
    ASSERT_GROUP(next_back, iter2, {0, 6}, {5});
    ASSERT_GROUP(next_back, iter2, {0, 4}, {3});
    ASSERT_GROUP(next_back, iter2, {0, 2}, {1});
    ASSERT_GROUP(empty, iter2);
}

TEST(Map) {
    ASSERT_GROUP(empty, range(1, 1).map([](auto i) { return std::to_string(i); }));

    auto f = [](auto i) { return std::to_string(i); };

    auto iter1 = range(1, 4).map(f);
    ASSERT_GROUP(next, iter1, 3, {"1"});
    ASSERT_GROUP(next, iter1, 2, {"2"});
    ASSERT_GROUP(next, iter1, 1, {"3"});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 4).map(f);
    ASSERT_GROUP(next_back, iter2, 3, {"3"});
    ASSERT_GROUP(next_back, iter2, 2, {"2"});
    ASSERT_GROUP(next_back, iter2, 1, {"1"});
    ASSERT_GROUP(empty, iter2);
}

TEST(Reverse) {
    ASSERT_GROUP(empty, range(1, 1).reverse());

    auto iter1 = range(1, 4).reverse();
    ASSERT_GROUP(next, iter1, 3, {3});
    ASSERT_GROUP(next, iter1, 2, {2});
    ASSERT_GROUP(next, iter1, 1, {1});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 4).reverse();
    ASSERT_GROUP(next_back, iter2, 3, {1});
    ASSERT_GROUP(next_back, iter2, 2, {2});
    ASSERT_GROUP(next_back, iter2, 1, {3});
    ASSERT_GROUP(empty, iter2);
}

TEST(Skip) {
    ASSERT_GROUP(empty, range(1, 1).skip(3));
    ASSERT_GROUP(empty, range(1, 4).skip(3));

    auto iter1 = range(-2, 4).skip(3);
    ASSERT_GROUP(next, iter1, 3, {1});
    ASSERT_GROUP(next, iter1, 2, {2});
    ASSERT_GROUP(next, iter1, 1, {3});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(-2, 4).skip(3);
    ASSERT_GROUP(next_back, iter2, 3, {3});
    ASSERT_GROUP(next_back, iter2, 2, {2});
    ASSERT_GROUP(next_back, iter2, 1, {1});
    ASSERT_GROUP(empty, iter2);
}

TEST(SkipWhile) {
    ASSERT_GROUP(empty, range(1, 1).skip_while([](auto) { return false; }));

    auto iter1 = range(1, 4).skip_while([](auto) { return true; });
    ASSERT_GROUP(next, iter1, {0, 3}, {});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(-2, 4).skip_while([](auto i) { return i < 1; });
    ASSERT_GROUP(next, iter2, {0, 6}, {1});
    ASSERT_GROUP(next, iter2, {0, 2}, {2});
    ASSERT_GROUP(next, iter2, {0, 1}, {3});
    ASSERT_GROUP(empty, iter2);
}

TEST(Take) {
    ASSERT_GROUP(empty, range(1, 1).take(3));
    ASSERT_GROUP(empty, range(1, 4).take(0));

    auto iter1 = range(1, 7).take(3);
    ASSERT_GROUP(next, iter1, 3, {1});
    ASSERT_GROUP(next, iter1, 2, {2});
    ASSERT_GROUP(next, iter1, 1, {3});
    ASSERT_GROUP(empty, iter1);
}

TEST(TakeWhile) {
    ASSERT_GROUP(empty, range(1, 1).take_while([](auto) { return true; }));

    auto iter1 = range(1, 4).take_while([](auto) { return false; });
    ASSERT_GROUP(next, iter1, {0, 3}, {});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(1, 7).take_while([](auto i) { return i < 4; });
    ASSERT_GROUP(next, iter2, {0, 6}, {1});
    ASSERT_GROUP(next, iter2, {0, 5}, {2});
    ASSERT_GROUP(next, iter2, {0, 4}, {3});
    ASSERT_GROUP(next, iter2, {0, 3}, {});
    ASSERT_GROUP(empty, iter2);
}

TEST(Zip) {
    ASSERT_GROUP(empty, range(1, 1).zip(range(1, 1)));
    ASSERT_GROUP(empty, range(1, 1).zip(range(1, 4)));
    ASSERT_GROUP(empty, range(1, 4).zip(range(1, 1)));

    auto iter1 = range(1, 4).zip(range(4, 10));
    ASSERT_GROUP(next, iter1, 3, {{1, 4}});
    ASSERT_GROUP(next, iter1, 2, {{2, 5}});
    ASSERT_GROUP(next, iter1, 1, {{3, 6}});
    ASSERT_GROUP(empty, iter1);

    auto iter2 = range(4, 10).zip(range(1, 4));
    ASSERT_GROUP(next, iter2, 3, {{4, 1}});
    ASSERT_GROUP(next, iter2, 2, {{5, 2}});
    ASSERT_GROUP(next, iter2, 1, {{6, 3}});
    ASSERT_GROUP(empty, iter2);
}

TEST(Count) {
    ASSERT_EQ(range(1, 1).count(), 0);
    ASSERT_EQ(range(1, 2).count(), 1);
    ASSERT_EQ(range(1, 3).count(), 2);
    ASSERT_EQ(range(1, 4).count(), 3);
}

TEST(Last) {
    ASSERT_EQ(range(1, 1).last(), Option<int>{});
    ASSERT_EQ(range(1, 2).last(), Option<int>{1});
    ASSERT_EQ(range(1, 3).last(), Option<int>{2});
    ASSERT_EQ(range(1, 4).last(), Option<int>{3});
}

TEST(Nth) {
    ASSERT_EQ(range(1, 1).nth(0), Option<int>{});
    ASSERT_EQ(range(1, 2).nth(0), Option<int>{1});
    ASSERT_EQ(range(1, 2).nth(1), Option<int>{});
    ASSERT_EQ(range(1, 3).nth(0), Option<int>{1});
    ASSERT_EQ(range(1, 3).nth(1), Option<int>{2});
    ASSERT_EQ(range(1, 3).nth(2), Option<int>{});
    ASSERT_EQ(range(1, 4).nth(0), Option<int>{1});
    ASSERT_EQ(range(1, 4).nth(1), Option<int>{2});
    ASSERT_EQ(range(1, 4).nth(2), Option<int>{3});
    ASSERT_EQ(range(1, 4).nth(3), Option<int>{});
}

TEST(Collect) {
    ASSERT_EQ(range(1, 1).collect(), (std::vector<int>{}));
    ASSERT_EQ(range(1, 2).collect(), (std::vector<int>{1}));
    ASSERT_EQ(range(1, 3).collect(), (std::vector<int>{1, 2}));
    ASSERT_EQ(range(1, 4).collect(), (std::vector<int>{1, 2, 3}));

    using Map = std::unordered_map<size_t, uint64_t>;
    auto map = range(1, 4).enumerate().collect<Map>();
    ASSERT_EQ(map, (Map{{0, 1}, {1, 2}, {2, 3}}));
}

TEST(Partition) {
    auto [odd, even] = range(1, 7).partition([](auto i) { return i % 2 != 0; });
    ASSERT_EQ(odd, (std::vector<int>{1, 3, 5}));
    ASSERT_EQ(even, (std::vector<int>{2, 4, 6}));

    using Map = std::unordered_map<size_t, uint64_t>;
    auto [left, right] = range(1, 7).enumerate().partition<Map>([](auto p) { return p.first < 3; });
    ASSERT_EQ(left, (Map{{0, 1}, {1, 2}, {2, 3}}));
    ASSERT_EQ(right, (Map{{3, 4}, {4, 5}, {5, 6}}));
}

TEST(Sum) {
    ASSERT_EQ(range(1, 1).sum(), 0);
    ASSERT_EQ(range(1, 7).sum(), 21);
}

TEST(Product) {
    ASSERT_EQ(range(1, 1).product(), 1);
    ASSERT_EQ(range(1, 7).product(), 720);
}

TEST(All) {
    auto f = [](auto i) { return i != 4; };

    ASSERT_TRUE(range(1, 1).all(f));
    ASSERT_TRUE(range(1, 4).all(f));
    ASSERT_FALSE(range(1, 5).all(f));
    ASSERT_FALSE(range(4, 9).all(f));
}

TEST(Any) {
    auto f = [](auto i) { return i == 4; };

    ASSERT_FALSE(range(1, 1).any(f));
    ASSERT_FALSE(range(1, 4).any(f));
    ASSERT_TRUE(range(1, 5).any(f));
    ASSERT_TRUE(range(4, 9).any(f));
}

TEST(Find) {
    auto f = [](auto i) { return i == 4; };

    ASSERT_EQ(range(1, 1).find(f), Option<int>{});
    ASSERT_EQ(range(1, 4).find(f), Option<int>{});
    ASSERT_EQ(range(1, 5).find(f), Option<int>{4});
    ASSERT_EQ(range(4, 9).find(f), Option<int>{4});
}

TEST(Position) {
    auto f = [](auto i) { return i == 4; };

    ASSERT_EQ(range(1, 1).position(f), Option<size_t>{});
    ASSERT_EQ(range(1, 4).position(f), Option<size_t>{});
    ASSERT_EQ(range(1, 5).position(f), Option<size_t>{3});
    ASSERT_EQ(range(4, 9).position(f), Option<size_t>{0});
}

TEST(Min) {
    ASSERT_EQ(range(1, 1).min(), Option<int>{});
    ASSERT_EQ(range(1, 4).min(), Option<int>{1});
    ASSERT_EQ(range(1, 1).min_by_key([](auto i) { return 4 - i; }), Option<int>{});
    ASSERT_EQ(range(1, 4).min_by_key([](auto i) { return 4 - i; }), Option<int>{3});
}

TEST(Max) {
    ASSERT_EQ(range(1, 1).max(), Option<int>{});
    ASSERT_EQ(range(1, 4).max(), Option<int>{3});
    ASSERT_EQ(range(1, 1).max_by_key([](auto i) { return 4 - i; }), Option<int>{});
    ASSERT_EQ(range(1, 4).max_by_key([](auto i) { return 4 - i; }), Option<int>{1});
}
