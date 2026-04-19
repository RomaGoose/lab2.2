#include "view.hpp"
#include <concepts>

template<view V, typename Pred>
requires std::predicate<Pred, typename V::value_type> 
class filter_view{
    private: 
        V view;
        Pred pred;
    public:
        filter_view(V view, Pred pred) : view(view), pred(pred) {};


        using is_view_ = void;
        using value_type = typename V::value_type;

        struct sentinel {
            // using sen = decltype(std::declval<V>().end());

            // sen sen_inner;

            // sentinel() = default;
            // sentinel(sen sen_inner) : sen_inner(sen_inner) {};
        };

        class filter_iterator {
            private:
                using iter = decltype(std::declval<V>().begin());
                using sen = decltype(std::declval<V>().end());

                iter it_inner;
                sen sen_inner;
                Pred pred;

                void find_next_valid() {
                    while(it_inner != sen_inner && !pred(*(it_inner)))
                        ++it_inner;
                }

            public:
                filter_iterator() = default;

                filter_iterator(iter it_inner, sen sen_inner, Pred pred) : 
                    it_inner(it_inner), sen_inner(sen_inner), pred(pred) {
                        find_next_valid();
                    }; 

                using difference_type = ptrdiff_t;        
                using value_type = typename V::value_type;
                using reference = value_type;
                using iterator_category = std::forward_iterator_tag;

                filter_iterator& operator++() {
                    ++it_inner;
                    find_next_valid();
                    return *this;
                }
                filter_iterator operator++(int){
                    auto tmp = *this;
                    ++(*this);
                    return tmp;
                }
                reference operator*() const {
                    return *it_inner;
                }

                friend bool operator==(const filter_iterator& it, const sentinel& sen) {
                    return it.it_inner == it.sen_inner;
                }

                bool operator==(const filter_iterator& other) const {
                    return it_inner == other.it_inner;
                }
        };

        filter_iterator begin() {
            return filter_iterator(view.begin(), view.end(), pred);
        }

        sentinel end() {
            return {};
            // return sentinel(view.end());
        }
};

template<typename Pred>
auto filter(Pred pred){
    return 
        [pred]
        <view V>(V view) 
        { return filter_view(view, (pred)); };
}
