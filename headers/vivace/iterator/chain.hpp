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
class Chain : public Iterator<T, Chain<T, L, R>> {
    enum class State { Left, Right, Both };

    L left;
    R right;
    State state;

protected:
    Bounds bounds_impl() const {
        auto lbounds = left.bounds();
        auto rbounds = right.bounds();
        auto lower = saturating_add(lbounds.lower, rbounds.lower);
        if (lbounds.upper.is_some() && rbounds.upper.is_some()) {
            auto upper = checked_add(lbounds.upper.unwrap(), rbounds.upper.unwrap());
            return {lower, upper};
        } else {
            return {lower};
        }
    }

    size_t size_impl() const {
        return left.size() + right.size();
    }

    Option<T> next_impl() {
        switch (state) {
        case State::Left:
            return left.next();
        case State::Right:
            return right.next();
        case State::Both:
            if (auto item = left.next(); item.is_some()) {
                return item;
            } else {
                state = State::Right;
                return right.next();
            }
        default:
            throw std::runtime_error{"unreachable"};
        }
    }

    Option<T> next_back_impl() {
        switch (state) {
        case State::Left:
            return left.next_back();
        case State::Right:
            return right.next_back();
        case State::Both:
            if (auto item = right.next_back(); item.is_some()) {
                return item;
            } else {
                state = State::Left;
                return left.next_back();
            }
        default:
            throw std::runtime_error{"unreachable"};
        }
    }

public:
    Chain(L left, R right) : left{std::move(left)}, right{std::move(right)}, state{State::Both} { }
};
