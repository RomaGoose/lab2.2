#pragma once

#include "view.hpp"
#include <iterator>
#include <type_traits>
#include <utility>

template<view V, typename Func>
class map_view{
    private: 
        V view;
        Func func;
    public:
        map_view(V view, Func func) : view(view), func(func) {};


        using is_view_ = void;
        using value_type = std::invoke_result_t<Func, typename V::value_type>;

        struct sentinel {
            using sen = decltype(std::declval<V>().end());

            sen sen_inner;

            sentinel() = default;
            sentinel(sen sen_inner) : sen_inner(sen_inner) {};
        };

        class map_iterator {
            private:
                using iter = decltype(std::declval<V>().begin());

                iter it_inner;
                Func func;
            public:
                map_iterator() = default;

                map_iterator(iter it_inner, Func func) : it_inner(it_inner), func(func) {}; 

                using difference_type = ptrdiff_t;        
                using value_type = std::invoke_result_t<Func, typename V::value_type>;
                using reference = value_type;
                using iterator_category = std::forward_iterator_tag;

                map_iterator& operator++() {
                    ++it_inner;
                    return *this;
                }
                map_iterator operator++(int){
                    auto tmp = *this;
                    ++(*this);
                    return tmp;
                }
                reference operator*() const {
                    return func(*it_inner);
                }

                friend bool operator==(const map_iterator& it, const sentinel& sen) {
                    return it.it_inner == sen.sen_inner;
                }
                bool operator==(const map_iterator& other) const {
                    return it_inner == other.it_inner;
                }
        };

        map_iterator begin() {
            return map_iterator(view.begin(), func);
        }

        sentinel end() {
            return sentinel(view.end());
        }

        bool is_empty() {
            return begin() == end();
        }
};

template<typename Func>
auto map(Func func) {
    return 
        [func]
        <view V>(V view) 
        { return map_view(view, (func)); };
}


