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

template <class T, class I, class F>
class Map : public Iterator<T, Map<T, I, F>> {
    I source;
    F f;

protected:
    Bounds bounds_impl() const {
        return source.bounds();
    }

    size_t size_impl() const {
        return source.size();
    }

    Option<T> next_impl() {
        return source.next().map(f);
    }

    Option<T> next_back_impl() {
        return source.next_back().map(f);
    }

public:
    Map(I source, F f) : source{std::move(source)}, f{std::move(f)} { }
};
