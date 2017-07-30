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

template <class T, class I>
class Skip : public Iterator<T, Skip<T, I>> {
    I source;
    size_t n;

protected:
    Bounds bounds_impl() const {
        auto bounds = source.bounds();
        auto lower = saturating_sub(bounds.lower, n);
        auto upper = bounds.upper.map([=](auto u) { return saturating_sub(u, n); });
        return {lower, upper};
    }

    size_t size_impl() const {
        return saturating_sub(source.size(), n);
    }

    Option<T> next_impl() {
        if (n == 0) {
            return source.next();
        } else {
            auto item = source.nth(n);
            n = 0;
            return item;
        }
    }

    Option<T> next_back_impl() {
        if (size_impl() != 0) {
            return source.next_back();
        } else {
            return {};
        }
    }

public:
    Skip(I source, size_t n) : source{std::move(source)}, n{n} { }
};
