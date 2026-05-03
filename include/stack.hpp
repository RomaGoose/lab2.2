#include "list_sequence.hpp"
#include <initializer_list>

template <class T>
class stack {
    private:
        list_sequence<T> items_; //TODO: choosabel container
    public:
        using value_type = T;

        stack(std::initializer_list<T> items = {}) : items_(items) {}

        template<class U>
        stack& push(U&& item) {
            items_.append(std::forward<U>(item));
            return *this;
        }
        const T& top() const noexcept{
            return items_.last();
        }
        T& top() noexcept{
            return items_.last();
        }
        void pop() noexcept {
            items_.pop_last();
        }

        bool empty() const noexcept {
            return items_.size() == 0;
        };
        size_t size() const noexcept {
            return items_.size();
        };


};