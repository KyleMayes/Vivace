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
class Filter : public Iterator<T, Filter<T, I, F>> {
    I source;
    F f;

    template <class S>
    Option<T> impl(S&& source) {
        for (auto item : source) {
            if (std::invoke(f, item)) {
                return {std::move(item)};
            }
        }
        return {};
    }

protected:
    Bounds bounds_impl() const {
        return {0, source.bounds().upper};
    }

    Option<T> next_impl() {
        return impl(source);
    }

    Option<T> next_back_impl() {
        return impl(source.as_ref().reverse());
    }

public:
    Filter(I source, F f) : source{std::move(source)}, f{std::move(f)} { }
};
