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

#include <vivace/math.hpp>

using namespace vce;

#include <cstdint>

TEST(Add) {
    ASSERT_EQ(checked_add<int8_t>(48, 48), (Option<int8_t>{96}));
    ASSERT_EQ(checked_add<int8_t>(-48, -48), (Option<int8_t>{-96}));
    ASSERT_EQ(checked_add<int8_t>(96, 96), (Option<int8_t>{}));
    ASSERT_EQ(checked_add<int8_t>(-96, -96), (Option<int8_t>{}));

    ASSERT_EQ(saturating_add<int8_t>(48, 48), 96);
    ASSERT_EQ(saturating_add<int8_t>(-48, -48), -96);
    ASSERT_EQ(saturating_add<int8_t>(96, 96), 127);
    ASSERT_EQ(saturating_add<int8_t>(-96, -96), -128);

    ASSERT_EQ(saturating_add<uint8_t>(96, 96), 192);
    ASSERT_EQ(saturating_add<uint8_t>(192, 192), 255);
}

TEST(Sub) {
    ASSERT_EQ(checked_sub<int8_t>(48, -48), (Option<int8_t>{96}));
    ASSERT_EQ(checked_sub<int8_t>(-48, 48), (Option<int8_t>{-96}));
    ASSERT_EQ(checked_sub<int8_t>(96, -96), (Option<int8_t>{}));
    ASSERT_EQ(checked_sub<int8_t>(-96, 96), (Option<int8_t>{}));

    ASSERT_EQ(saturating_sub<int8_t>(48, -48), 96);
    ASSERT_EQ(saturating_sub<int8_t>(-48, 48), -96);
    ASSERT_EQ(saturating_sub<int8_t>(96, -96), 127);
    ASSERT_EQ(saturating_sub<int8_t>(-96, 96), -128);

    ASSERT_EQ(saturating_sub<uint8_t>(192, 96), 96);
    ASSERT_EQ(saturating_sub<uint8_t>(96, 192), 0);
}

TEST(Mul) {
    ASSERT_EQ(checked_mul<int8_t>(8, 8), (Option<int8_t>{64}));
    ASSERT_EQ(checked_mul<int8_t>(-8, 8), (Option<int8_t>{-64}));
    ASSERT_EQ(checked_mul<int8_t>(8, -8), (Option<int8_t>{-64}));
    ASSERT_EQ(checked_mul<int8_t>(-8, -8), (Option<int8_t>{64}));
    ASSERT_EQ(checked_mul<int8_t>(16, 16), (Option<int8_t>{}));
    ASSERT_EQ(checked_mul<int8_t>(-16, 16), (Option<int8_t>{}));
    ASSERT_EQ(checked_mul<int8_t>(16, -16), (Option<int8_t>{}));
    ASSERT_EQ(checked_mul<int8_t>(-16, -16), (Option<int8_t>{}));

    ASSERT_EQ(saturating_mul<int8_t>(8, 8), 64);
    ASSERT_EQ(saturating_mul<int8_t>(-8, 8), -64);
    ASSERT_EQ(saturating_mul<int8_t>(8, -8), -64);
    ASSERT_EQ(saturating_mul<int8_t>(-8, -8), 64);
    ASSERT_EQ(saturating_mul<int8_t>(16, 16), 127);
    ASSERT_EQ(saturating_mul<int8_t>(-16, 16), -128);
    ASSERT_EQ(saturating_mul<int8_t>(16, -16), -128);
    ASSERT_EQ(saturating_mul<int8_t>(-16, -16), 127);

    ASSERT_EQ(saturating_mul<uint8_t>(8, 8), 64);
    ASSERT_EQ(saturating_mul<uint8_t>(16, 16), 255);
}

TEST(Div) {
    ASSERT_EQ(checked_div<int8_t>(64, 8), (Option<int8_t>{8}));
    ASSERT_EQ(checked_div<int8_t>(64, 0), (Option<int8_t>{}));
}
