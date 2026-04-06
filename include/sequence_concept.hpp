#include <concepts>
#include <cstddef>
#include <iterator>
#include <utility>
template<class Seq, class T>
concept Sequence = requires (Seq s, const Seq& cs, size_t i, size_t end, size_t start, T item) {
    { s.at(i) } -> std::same_as<T&>;
    { s.first(i) } -> std::same_as<T&>;
    { s.last(i) } -> std::same_as<T&>;
    { s.append(item) };
    { s.prepend(item) };
    { s.insert_at(i, item) };
    { s.append(std::move(item)) };
    { s.prepend(std::move(item)) };
    { s.insert_at(i, std::move(item)) };
    { s.begin() } -> std::forward_iterator;
    { s.end() } -> std::sentinel_for<decltype(s.begin())>;
    
    { cs.at(i) } -> std::same_as<const T&>;
    { cs.first(i) } -> std::same_as<const T&>;
    { cs.last(i) } -> std::same_as<const T&>;
    { cs.cbegin() } -> std::forward_iterator;
    { cs.cend() } -> std::sentinel_for<decltype(s.begin())>;
    
    { cs.size() } -> std::same_as<size_t>;
    { cs.get_subsequence(start, end) } -> std::same_as<Seq>;
};