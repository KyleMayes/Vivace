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
class TakeWhile : public Iterator<T, TakeWhile<T, I, F>> {
    I source;
    F f;
    bool done;

protected:
    Bounds bounds_impl() const {
        if (done) {
            return {0, 0};
        } else {
            return {0, source.bounds().upper};
        }
    }

    Option<T> next_impl() {
        if (!done) {
            for (auto item : source) {
                if (f(item)) {
                    return {std::move(item)};
                } else {
                    done = true;
                    return {};
                }
            }
        }
        return {};
    }

public:
    TakeWhile(I source, F f) : source{std::move(source)}, f{std::move(f)}, done{false} { }
};
