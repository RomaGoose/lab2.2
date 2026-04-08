#include "linked_list.hpp"
#include <cstddef>
#include <initializer_list>
#include <utility>

template<typename  T>
class list_sequence {
    private:
        linked_list<T> items_;
    public:
        
        template<bool is_const>
        class base_iterator;
        struct sentinel;

        using iterator = base_iterator<false>;
        using const_iterator = base_iterator<true>;
        

        list_sequence(std::initializer_list<T> items = {}) : items_(items) {};

        ~list_sequence() = default;

        const T& operator[](size_t index) const {
            auto it = items_.begin();
            for(size_t i = 0; i < index; ++i){
                ++it;
            }
            return *it;
        }
        
        T& operator[](size_t index) {
            auto it = items_.begin();
            for(size_t i = 0; i < index; ++i){
                ++it;
            }
            return *it;
        }

        const T& at(size_t index) const {
            return items_.at(index);
        }
        
        T& at(size_t index) {
            return items_.at(index);
        }
        
        const T& first() const {
            return items_.first();
        }
        
        T& first() {
            return items_.first();
        }
        
        const T& last() const {
            return items_.last();
        }
        
        T& last() {
            return items_.last();
        }
        
        size_t size() const noexcept {
            return items_.size();
        }

        list_sequence<T>& append(const T& item) {
            items_.append(item);
            return *this;
        }
        
        list_sequence<T>& prepend(const T& item) {
            items_.prepend(item);
            return *this;
        }
        
        list_sequence<T>& insert_at(size_t index, const T& item) {
            items_.insert_at(index, item);
            return *this;
        }
        list_sequence<T>& append(T&& item) {
            items_.append(std::forward<T>(item));
            return *this;
        }
        list_sequence<T>& prepend(T&& item) {
            items_.prepend(std::forward<T>(item));
            return *this;
        }
        list_sequence<T>& insert_at(size_t index, T&& item) {
            items_.insert_at(index, std::forward<T>(item));
            return *this;
        }

        void clear() { items_.clear(); }


        iterator begin() {
            return iterator(items_.begin());
        }
        const_iterator begin() const {
            return const_iterator(items_.begin());
        }
        sentinel end() const {
            return sentinel();
        }
        const_iterator cbegin() const {
            return const_iterator(items_.cbegin());
        }
        sentinel cend() const {
            return sentinel();
        }
};



template<class T>
struct list_sequence<T>::sentinel {
    linked_list<T>::sentinel sen_inner;
    public:
        sentinel() : sen_inner() {};
};


template<class T>
template<bool is_const>
class list_sequence<T>::base_iterator{
    private: 
        linked_list<T>::template base_iterator<is_const> it_inner;
    public:
        using ref = std::conditional_t<is_const, const T&, T&>;

        base_iterator(auto it) : it_inner(it) {};

        base_iterator& operator++(){
            ++it_inner;
            return *this;
        };
        ref operator*(){
            return *it_inner;
        }
        friend bool operator!=(const base_iterator<is_const>& it, sentinel s){
            return it.it_inner != s.sen_inner;
        };
};