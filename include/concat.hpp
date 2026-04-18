#pragma once

#include "view.hpp"
#include <type_traits>
#include <utility>


template<range R1, range R2>
requires std::same_as<typename R1::value_type, typename R2::value_type>
class concat_view {
    private:
        using view1_t = std::conditional_t<is_view<R1>, R1, view_of<R1>>;
        using view2_t = std::conditional_t<is_view<R2>, R2, view_of<R2>>;
        
        view1_t range1;
        view2_t range2;
    public:
        class concat_iterator;
        struct sentinel;

        using value_type = typename R1::value_type;
        using is_view_ = void;

        concat_view(const R1& range1, const R2& range2) : range1(range1), range2(range2) {};

        concat_iterator begin() const{
            return concat_iterator(range1, range2);
        }
        sentinel end() const {
            return sentinel(range2);
        }
};

template<range R1, range R2>
requires std::same_as<typename R1::value_type, typename R2::value_type>
struct concat_view<R1, R2>::sentinel{
        using view2_t = std::conditional_t<is_view<R2>, R2, view_of<R2>>;

        using sen2 = decltype(std::declval<view2_t>().end());

        sen2 end2;
    public:
        sentinel(view2_t range2) : end2(range2.end()) {};
        sentinel() = default;
};

template<range R1, range R2>
requires std::same_as<typename R1::value_type, typename R2::value_type>
class concat_view<R1, R2>::concat_iterator{
    private:
        using view1_t = std::conditional_t<is_view<R1>, R1, view_of<R1>>;
        using view2_t = std::conditional_t<is_view<R2>, R2, view_of<R2>>;

        using iter1 = decltype(std::declval<view1_t>().begin());
        using iter2 = decltype(std::declval<view2_t>().begin());
        using sen1 = decltype(std::declval<view1_t>().end());
        using sen2 = decltype(std::declval<view2_t>().end());

        iter1 it1;
        iter2 it2;
        sen1 end1;
        sen2 end2;
        bool in_first;
    public:
        concat_iterator() = default;

        using reference = typename iter1::reference;
        using difference_type = ptrdiff_t;
        using value_type = typename R1::value_type;
        using iterator_category = std::forward_iterator_tag;

        concat_iterator(view1_t range1, view2_t range2) : 
            it1(range1.begin()), it2(range2.begin()), end1(range1.end()), end2(range2.end()), in_first(true) {
                if(it1 == end1) in_first = false;
            };

        concat_iterator& operator++() {
            if(in_first){
                ++it1;
                if(it1 == end1) in_first = false;
                
                return *this;
            }
            ++it2;
            return *this;
        }
        concat_iterator operator++(int) {
            concat_iterator tmp = *this;
            ++*this;          
            return tmp;
        }

        reference operator*() const {
            return in_first ? *it1 : *it2;
        }

        friend bool operator==(const concat_iterator& it, const sentinel& sen){
            return !it.in_first && it.it2 == sen.end2;
        }


        bool operator==(const concat_iterator& other) const {
            if(in_first != other.in_first) return false;
            if(in_first) return it1 == other.it1;
            return it2 == other.it2;
        }
};

template<range R1, range R2>
requires std::same_as<typename R1::value_type, typename R2::value_type>
concat_view<R1, R2> operator+(const R1& range1, const R2& range2){
    return concat_view(range1, range2);
}
