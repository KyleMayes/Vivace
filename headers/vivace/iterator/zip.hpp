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

template <class T, class L, class R>
class Zip : public Iterator<T, Zip<T, L, R>> {
    L left;
    R right;

protected:
    Bounds bounds_impl() const {
        auto lbounds = left.bounds();
        auto rbounds = right.bounds();
        auto lower = std::min(lbounds.lower, rbounds.lower);
        if (lbounds.upper.is_some() && rbounds.upper.is_some()) {
            auto upper = std::min(lbounds.upper.unwrap(), rbounds.upper.unwrap());
            return {lower, upper};
        } else if (lbounds.upper.is_some()) {
            return {lower, lbounds.upper.unwrap()};
        } else if (rbounds.upper.is_some()) {
            return {lower, rbounds.upper.unwrap()};
        } else {
            return {lower};
        }
    }

    size_t size_impl() const {
        return std::min(left.size(), right.size());
    }

    Option<T> next_impl() {
        auto litem = left.next();
        auto ritem = right.next();
        if (litem.is_some() && ritem.is_some()) {
            return {std::make_pair(litem.unwrap(), ritem.unwrap())};
        } else {
            return {};
        }
    }

    Option<T> next_back_impl() {
        auto lsize = left.size();
        auto rsize = right.size();
        if (lsize != rsize) {
            if (lsize < rsize) {
                right.as_ref().rev().nth((rsize - lsize) - 1);
            } else if (rsize < lsize) {
                left.as_ref().rev().nth((lsize - rsize) - 1);
            }
        }

        if (auto litem = left.next_back(); litem.is_some()) {
            return {std::make_pair(litem.unwrap(), right.next_back().unwrap())};
        } else {
            return {};
        }
    }

public:
    Zip(L left, R right) : left{std::move(left)}, right{std::move(right)} { }
};
