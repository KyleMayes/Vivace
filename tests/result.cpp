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

#include <vivace/result.hpp>

using namespace vce;

#include <string>

static std::vector<int> ONE_OK{4, 17, 322};
static std::vector<int> TWO_OK{322, 17, 4};
static const char* ONE_ERR = "QlwDn1ND19HhPGrQ2W6Y3Pr7GaULeu8g4xv1tuuU3yeFsaYJyzv3cFVf2fyneBZZ";
static const char* TWO_ERR = "wVXasBGJAPmaa1HErfj91BXuUer9CiFlkw4SD5aiBk3sOMlMg0BKnAmwKnIN2BGa";

using R = Result<std::vector<int>, std::string>;
using UP = std::unique_ptr<int>;
UP make(int value) { return std::make_unique<int>(value); }

TEST(Construction) {
    R a{OK, ONE_OK};
    ASSERT_EQ(a.unwrap(), ONE_OK);

    R b{OK, std::in_place_t{}, ONE_OK.begin(), ONE_OK.end()};
    ASSERT_EQ(b.unwrap(), ONE_OK);

    R c{OK, std::in_place_t{}, {4, 17, 322}};
    ASSERT_EQ(c.unwrap(), ONE_OK);

    R d{ERR, ONE_ERR};
    ASSERT_EQ(d.unwrap_err(), ONE_ERR);

    R e{ERR, std::in_place_t{}, ONE_ERR, 4};
    ASSERT_EQ(e.unwrap_err(), "QlwD");

    R f{ERR, std::in_place_t{}, {'1', '7'}};
    ASSERT_EQ(f.unwrap_err(), "17");

    Result<UP, UP> g{OK, make(322)};
    ASSERT_EQ(*g.unwrap(), 322);

    Result<UP, UP> h{ERR, make(17)};
    ASSERT_EQ(*h.unwrap_err(), 17);
}

TEST(CopyConstruction) {
    R a{OK, ONE_OK};
    R b{a};
    ASSERT_EQ(a.unwrap(), ONE_OK);
    ASSERT_EQ(b.unwrap(), ONE_OK);

    R c{ERR, ONE_ERR};
    R d{c};
    ASSERT_EQ(c.unwrap_err(), ONE_ERR);
    ASSERT_EQ(d.unwrap_err(), ONE_ERR);
}

TEST(CopyAssignment) {
    R a{OK, ONE_OK};
    R b{OK, TWO_OK};
    b = a;
    ASSERT_EQ(a.unwrap(), ONE_OK);
    ASSERT_EQ(b.unwrap(), ONE_OK);

    R c{OK, ONE_OK};
    R d{ERR, TWO_ERR};
    d = c;
    ASSERT_EQ(c.unwrap(), ONE_OK);
    ASSERT_EQ(d.unwrap(), ONE_OK);

    R e{ERR, ONE_ERR};
    R f{OK, TWO_OK};
    f = e;
    ASSERT_EQ(e.unwrap_err(), ONE_ERR);
    ASSERT_EQ(f.unwrap_err(), ONE_ERR);

    R g{ERR, ONE_ERR};
    R h{ERR, TWO_ERR};
    h = g;
    ASSERT_EQ(g.unwrap_err(), ONE_ERR);
    ASSERT_EQ(h.unwrap_err(), ONE_ERR);
}

TEST(MoveConstruction) {
    Result<UP, UP> a{OK, make(322)};
    Result<UP, UP> b{std::move(a)};
    ASSERT_EQ(a.unwrap(), nullptr);
    ASSERT_EQ(*b.unwrap(), 322);

    Result<UP, UP> c{ERR, make(322)};
    Result<UP, UP> d{std::move(c)};
    ASSERT_EQ(c.unwrap_err(), nullptr);
    ASSERT_EQ(*d.unwrap_err(), 322);
}

TEST(MoveAssignment) {
    Result<UP, UP> a{OK, make(322)};
    Result<UP, UP> b{OK, make(17)};
    b = std::move(a);
    ASSERT_EQ(a.unwrap(), nullptr);
    ASSERT_EQ(*b.unwrap(), 322);

    Result<UP, UP> c{OK, make(322)};
    Result<UP, UP> d{ERR, make(17)};
    d = std::move(c);
    ASSERT_EQ(c.unwrap(), nullptr);
    ASSERT_EQ(*d.unwrap(), 322);

    Result<UP, UP> e{ERR, make(322)};
    Result<UP, UP> f{OK, make(17)};
    f = std::move(e);
    ASSERT_EQ(e.unwrap_err(), nullptr);
    ASSERT_EQ(*f.unwrap_err(), 322);

    Result<UP, UP> g{ERR, make(322)};
    Result<UP, UP> h{ERR, make(17)};
    h = std::move(g);
    ASSERT_EQ(g.unwrap_err(), nullptr);
    ASSERT_EQ(*h.unwrap_err(), 322);
}

TEST(AsRef) {
    R a{OK, ONE_OK};
    ASSERT_EQ(a.as_ref().unwrap().get(), ONE_OK);
    ASSERT_EQ(a.unwrap(), ONE_OK);

    R b{ERR, ONE_ERR};
    ASSERT_EQ(b.as_ref().unwrap_err().get(), ONE_ERR);
    ASSERT_EQ(b.unwrap_err(), ONE_ERR);
}

TEST(Unwrap) {
    Result<UP, UP> a{OK, make(322)};
    ASSERT_EQ(*a.unwrap(), 322);
    ASSERT_THROW(a.unwrap_err());

    Result<UP, UP> b{ERR, make(322)};
    ASSERT_EQ(*b.unwrap_err(), 322);
    ASSERT_THROW(b.unwrap());

    Result<UP, UP> c{OK, make(322)};
    ASSERT_EQ(*c.unwrap_or(make(17)), 322);

    Result<UP, UP> d{ERR, make(322)};
    ASSERT_EQ(*d.unwrap_or(make(17)), 17);

    Result<UP, UP> e{OK, make(322)};
    ASSERT_EQ(*e.unwrap_or_else([] { return make(17); }), 322);

    Result<UP, UP> f{ERR, make(322)};
    ASSERT_EQ(*f.unwrap_or_else([] { return make(17); }), 17);
}

TEST(Map) {
    Result<UP, UP> a{OK, make(322)};
    ASSERT_EQ(*a.map([](auto i) { return i; }).unwrap(), 322);
    ASSERT_THROW(a.map_err([](auto i) { return i; }).unwrap_err());

    Result<UP, UP> b{ERR, make(322)};
    ASSERT_EQ(*b.map_err([](auto i) { return i; }).unwrap_err(), 322);
    ASSERT_THROW(b.map([](auto i) { return i; }).unwrap());

    Result<UP, UP> c{OK, make(322)};
    ASSERT_EQ(*c.map_or(make(17), [](auto i) { return i; }), 322);

    Result<UP, UP> d{ERR, make(322)};
    ASSERT_EQ(*d.map_or(make(17), [](auto i) { return i; }), 17);

    Result<UP, UP> e{OK, make(322)};
    ASSERT_EQ(*e.map_or_else([] { return make(17); }, [](auto i) { return i; }), 322);

    Result<UP, UP> f{ERR, make(322)};
    ASSERT_EQ(*f.map_or_else([] { return make(17); }, [](auto i) { return i; }), 17);
}

TEST(AndThen) {
    Result<UP, UP> a{OK, make(322)};
    ASSERT_EQ(*a.and_then([](auto i) { return Result<UP, UP>{OK, std::move(i)}; }).unwrap(), 322);

    Result<UP, UP> b{ERR, make(322)};
    ASSERT_THROW(b.and_then([](auto i) { return Result<UP, UP>{OK, std::move(i)}; }).unwrap());
}

TEST(Compare) {
    using L = Result<int, long>;
    using R = Result<float, double>;

    ASSERT_EQ((L{OK, 322}), (R{OK, 322.0f}));
    ASSERT_EQ((L{ERR, 322}), (R{ERR, 322.0}));

    ASSERT_NE((L{OK, 322}), (R{OK, 17.0f}));
    ASSERT_NE((L{ERR, 322}), (R{ERR, 17.0}));
    ASSERT_NE((L{OK, 322}), (R{ERR, 322.0}));
    ASSERT_NE((L{ERR, 322}), (R{OK, 322.0f}));

    ASSERT_LT((L{OK, 17}), (R{OK, 322.0f}));
    ASSERT_LT((L{OK, 322}), (R{ERR, 17.0}));

    ASSERT_GT((L{OK, 322}), (R{OK, 17.0f}));
    ASSERT_GT((L{ERR, 17}), (R{OK, 322.0f}));

    ASSERT_LE((L{OK, 17}), (R{OK, 322.0f}));
    ASSERT_LE((L{OK, 322}), (R{ERR, 17.0}));
    ASSERT_LE((L{OK, 322}), (R{OK, 322.0f}));
    ASSERT_LE((L{ERR, 322}), (R{ERR, 322.0}));

    ASSERT_GE((L{OK, 322}), (R{OK, 17.0f}));
    ASSERT_GE((L{ERR, 17}), (R{OK, 322.0f}));
    ASSERT_GE((L{OK, 322}), (R{OK, 322.0f}));
    ASSERT_GE((L{ERR, 322}), (R{ERR, 322.0}));
}

TEST(Stream) {
    std::stringstream ss;
    ss << Result<int, int>{OK, 322};
    ASSERT_EQ(ss.str(), "Ok(322)");

    ss = std::stringstream{};
    ss << Result<int, int>{ERR, 322};
    ASSERT_EQ(ss.str(), "Err(322)");
}
