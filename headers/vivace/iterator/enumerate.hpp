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
class Enumerate : public Iterator<T, Enumerate<T, I>> {
    I source;
    size_t index;

    template <class S>
    Option<T> impl(Option<S> source, size_t index) {
        return source.map([=](auto i) { return std::make_pair(index, std::move(i)); });
    }

protected:
    Bounds bounds_impl() const {
        return source.bounds();
    }

    size_t size_impl() const {
        return source.size();
    }

    Option<T> next_impl() {
        auto current = index;
        index += 1;
        return impl(source.next(), current);
    }

    Option<T> next_back_impl() {
        auto item = source.next_back();
        return impl(std::move(item), index + source.size());
    }

public:
    Enumerate(I source) : source{std::move(source)}, index{0} { }
};
