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
class Take : public Iterator<T, Take<T, I>> {
    I source;
    size_t n;

protected:
    Bounds bounds_impl() const {
        auto bounds = source.bounds();
        auto lower = std::min(bounds.lower, n);
        auto upper = bounds.upper.map_or(n, [=](auto u) { return std::min(u, n); });
        return {lower, upper};
    }

    size_t size_impl() const {
        return std::min(source.size(), n);
    }

    Option<T> next_impl() {
        if (n != 0) {
            n -= 1;
            return source.next();
        } else {
            return {};
        }
    }

public:
    Take(I source, size_t n) : source{std::move(source)}, n{n} { }
};
