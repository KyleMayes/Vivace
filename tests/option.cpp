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

#include <vivace/option.hpp>

using namespace vce;

#include <string>

static const char* ONE = "QlwDn1ND19HhPGrQ2W6Y3Pr7GaULeu8g4xv1tuuU3yeFsaYJyzv3cFVf2fyneBZZ";
static const char* TWO = "wVXasBGJAPmaa1HErfj91BXuUer9CiFlkw4SD5aiBk3sOMlMg0BKnAmwKnIN2BGa";

using UP = std::unique_ptr<int>;
UP make(int value) { return std::make_unique<int>(value); }

TEST(Construction) {
    Option<std::string> a;
    ASSERT_THROW(a.unwrap());

    Option<std::string> b{ONE};
    ASSERT_EQ(b.unwrap(), ONE);
    ASSERT_THROW(b.unwrap());

    Option<std::string> c{std::in_place_t{}, ONE, 4};
    ASSERT_EQ(c.unwrap(), "QlwD");
    ASSERT_THROW(c.unwrap());

    Option<std::string> d{std::in_place_t{}, {'1', '7'}};
    ASSERT_EQ(d.unwrap(), "17");
    ASSERT_THROW(d.unwrap());

    Option<UP> e{make(322)};
    ASSERT_EQ(*e.unwrap(), 322);
    ASSERT_THROW(e.unwrap());
}

TEST(CopyConstruction) {
    Option<std::string> a;
    Option<std::string> b{a};
    ASSERT_THROW(a.unwrap());
    ASSERT_THROW(b.unwrap());

    Option<std::string> c{ONE};
    Option<std::string> d{c};
    ASSERT_EQ(c.unwrap(), ONE);
    ASSERT_EQ(d.unwrap(), ONE);

    Option<const char*> e;
    Option<std::string> f{e};
    ASSERT_THROW(e.unwrap());
    ASSERT_THROW(f.unwrap());

    Option<const char*> g{TWO};
    Option<std::string> h{g};
    ASSERT_EQ(g.unwrap(), TWO);
    ASSERT_EQ(h.unwrap(), TWO);
}

TEST(CopyAssignment) {
    Option<std::string> a;
    Option<std::string> b;
    b = a;
    ASSERT_THROW(a.unwrap());
    ASSERT_THROW(b.unwrap());

    Option<std::string> c{ONE};
    Option<std::string> d;
    d = c;
    ASSERT_EQ(c.unwrap(), ONE);
    ASSERT_EQ(d.unwrap(), ONE);

    Option<std::string> e;
    Option<std::string> f{TWO};
    f = e;
    ASSERT_THROW(e.unwrap());
    ASSERT_THROW(f.unwrap());

    Option<std::string> g{ONE};
    Option<std::string> h{TWO};
    h = g;
    ASSERT_EQ(g.unwrap(), ONE);
    ASSERT_EQ(h.unwrap(), ONE);

    Option<const char*> i;
    Option<std::string> j;
    j = i;
    ASSERT_THROW(i.unwrap());
    ASSERT_THROW(j.unwrap());

    Option<const char*> k{ONE};
    Option<std::string> l;
    l = k;
    ASSERT_EQ(k.unwrap(), ONE);
    ASSERT_EQ(l.unwrap(), ONE);

    Option<const char*> m;
    Option<std::string> n{TWO};
    n = m;
    ASSERT_THROW(m.unwrap());
    ASSERT_THROW(n.unwrap());

    Option<const char*> o{ONE};
    Option<std::string> p{TWO};
    p = o;
    ASSERT_EQ(o.unwrap(), ONE);
    ASSERT_EQ(p.unwrap(), ONE);
}

TEST(MoveConstruction) {
    Option<UP> a;
    Option<UP> b{std::move(a)};
    ASSERT_THROW(a.unwrap());
    ASSERT_THROW(b.unwrap());

    Option<UP> c{make(322)};
    Option<UP> d{std::move(c)};
    ASSERT_THROW(c.unwrap());
    ASSERT_EQ(*d.unwrap(), 322);

    Option<int*> e;
    Option<UP> f{std::move(e)};
    ASSERT_THROW(e.unwrap());
    ASSERT_THROW(f.unwrap());

    Option<int*> g{new int{322}};
    Option<UP> h{std::move(g)};
    ASSERT_THROW(g.unwrap());
    ASSERT_EQ(*h.unwrap(), 322);
}

TEST(MoveAssignment) {
    Option<UP> a;
    Option<UP> b;
    b = std::move(a);
    ASSERT_THROW(a.unwrap());
    ASSERT_THROW(b.unwrap());

    Option<UP> c{make(322)};
    Option<UP> d;
    d = std::move(c);
    ASSERT_THROW(c.unwrap());
    ASSERT_EQ(*d.unwrap(), 322);

    Option<UP> e;
    Option<UP> f{make(17)};
    f = std::move(e);
    ASSERT_THROW(e.unwrap());
    ASSERT_THROW(f.unwrap());

    Option<UP> g{make(322)};
    Option<UP> h{make(17)};
    h = std::move(g);
    ASSERT_THROW(g.unwrap());
    ASSERT_EQ(*h.unwrap(), 322);

    Option<int*> i;
    Option<UP> j;
    j = std::move(i);
    ASSERT_THROW(i.unwrap());
    ASSERT_THROW(j.unwrap());

    Option<int*> k{new int{322}};
    Option<UP> l;
    l = std::move(k);
    ASSERT_THROW(k.unwrap());
    ASSERT_EQ(*l.unwrap(), 322);

    Option<int*> m;
    Option<UP> n{make(17)};
    n = std::move(m);
    ASSERT_THROW(m.unwrap());
    ASSERT_THROW(n.unwrap());

    Option<int*> o{new int{322}};
    Option<UP> p{make(17)};
    p = std::move(o);
    ASSERT_THROW(o.unwrap());
    ASSERT_EQ(*p.unwrap(), 322);
}

TEST(AsRef) {
    Option<UP> a;
    ASSERT_THROW(a.as_ref().unwrap());
    ASSERT_THROW(a.unwrap());

    Option<UP> b{make(322)};
    ASSERT_EQ(*b.as_ref().unwrap().get(), 322);
    ASSERT_EQ(*b.unwrap(), 322);
}

TEST(Unwrap) {
    Option<UP> a{make(322)};
    ASSERT_EQ(*a.unwrap(), 322);
    ASSERT_THROW(a.unwrap());

    Option<UP> b{make(322)};
    ASSERT_EQ(*b.unwrap_or(make(17)), 322);
    ASSERT_EQ(*b.unwrap_or(make(17)), 17);

    Option<UP> c{make(322)};
    ASSERT_EQ(*c.unwrap_or_else([] { return make(17); }), 322);
    ASSERT_EQ(*c.unwrap_or_else([] { return make(17); }), 17);
}

TEST(Map) {
    Option<UP> a{make(322)};
    ASSERT_EQ(*a.map([](auto i) { return i; }).unwrap(), 322);
    ASSERT_THROW(a.map([](auto i) { return i; }).unwrap());

    Option<UP> b{make(322)};
    ASSERT_EQ(*b.map_or(make(17), [](auto i) { return i; }), 322);
    ASSERT_EQ(*b.map_or(make(17), [](auto i) { return i; }), 17);

    Option<UP> c{make(322)};
    ASSERT_EQ(*c.map_or_else([] { return make(17); }, [](auto i) { return i; }), 322);
    ASSERT_EQ(*c.map_or_else([] { return make(17); }, [](auto i) { return i; }), 17);
}

TEST(AndThen) {
    Option<UP> a{make(322)};
    ASSERT_EQ(*a.and_then([](auto i) { return Option<UP>{std::move(i)}; }).unwrap(), 322);
    ASSERT_THROW(a.and_then([](auto i) { return Option<UP>{std::move(i)}; }).unwrap());
}

TEST(Compare) {
    ASSERT_EQ(Option<int>{}, Option<float>{});
    ASSERT_EQ(Option<int>{17}, Option<float>{17.0f});

    ASSERT_NE(Option<int>{}, Option<float>{17.0f});
    ASSERT_NE(Option<int>{17.0f}, Option<float>{});
    ASSERT_NE(Option<int>{322}, Option<float>{17.0f});

    ASSERT_LT(Option<int>{}, Option<float>{17.0f});
    ASSERT_LT(Option<int>{4}, Option<float>{17.0f});

    ASSERT_GT(Option<int>{322}, Option<float>{});
    ASSERT_GT(Option<int>{322}, Option<float>{17.0f});

    ASSERT_LE(Option<int>{}, Option<float>{17.0f});
    ASSERT_LE(Option<int>{4}, Option<float>{17.0f});
    ASSERT_LE(Option<int>{}, Option<float>{});
    ASSERT_LE(Option<int>{17.0f}, Option<float>{17.0f});

    ASSERT_GE(Option<int>{322}, Option<float>{});
    ASSERT_GE(Option<int>{322}, Option<float>{17.0f});
    ASSERT_GE(Option<int>{}, Option<float>{});
    ASSERT_GE(Option<int>{17.0f}, Option<float>{17.0f});
}

TEST(Stream) {
    std::stringstream ss;
    ss << Option<int>{};
    ASSERT_EQ(ss.str(), "None");

    ss = std::stringstream{};
    ss << Option<int>{322};
    ASSERT_EQ(ss.str(), "Some(322)");
}
