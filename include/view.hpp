#pragma once

#include <iterator>

template<class Range>
concept range = requires(std::remove_cvref_t<Range> r) {
    typename std::remove_reference_t<Range>::value_type;
    
    { r.begin() } -> std::forward_iterator;
    { r.end() } -> std::sentinel_for<decltype(r.begin())>;
};

template<class View>
concept is_view = requires {
    typename View::is_view_;
};

template<class View>
concept view = range<View> && is_view<View>;


template<class T, class ...Args>
concept all_same_value_types = (std::same_as<typename std::remove_reference_t<T>::value_type, 
                                             typename std::remove_reference_t<Args>::value_type> && ...);

template<range R>
class view_of {
    private:
        using iter_t = decltype(std::declval<const R&>().begin());
        using sent_t = decltype(std::declval<const R&>().end());

        iter_t beg;
        sent_t sen;
    public:
        using is_view_ = void;
        using value_type = R::value_type;

        view_of(const R& range) : beg(range.begin()), sen(range.end()) {};


        struct sentinel{
            sent_t sen_inner;

            sentinel() = default;
            sentinel(sent_t sen_inner) : sen_inner(sen_inner) {};
        };

        class iterator{
                iter_t it_inner;
            public:
                using reference = iter_t::reference;
                using difference_type = ptrdiff_t;
                using value_type = iter_t::value_type;
                using iterator_category = std::forward_iterator_tag;


                iterator() = default;
                iterator(iter_t it_inner) : it_inner(it_inner) {};

                iterator& operator++() { ++it_inner; return *this; }
                iterator operator++(int) { auto tmp = *this; ++it_inner; return tmp; }
                reference operator*() const { return *it_inner; }
                friend bool operator== (const iterator& it, sentinel s) {
                    return it.it_inner == s.sen_inner;
                }
                bool operator==(const iterator& other) const {
                    return it_inner == other.it_inner;
                }
        };

        iterator begin() const {
            return iterator(beg);
        }

        sentinel end() const {
            return sentinel(sen);
        }
};
