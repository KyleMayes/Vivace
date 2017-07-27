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

#include <vivace/meta.hpp>

using namespace vce;

struct FieldA { int integer; };
struct FieldB { static int integer; };

VCE_HAS_FIELD(HasInteger, integer);
VCE_HAS_FIELD(HasReal, real);

static_assert(HasIntegerV<FieldA>);
static_assert(HasIntegerV<FieldA, int>);
static_assert(!HasIntegerV<FieldA, float>);
static_assert(!HasRealV<FieldA>);

static_assert(HasIntegerV<FieldB>);
static_assert(HasIntegerV<FieldB, int>);
static_assert(!HasIntegerV<FieldB, float>);
static_assert(!HasRealV<FieldB>);
