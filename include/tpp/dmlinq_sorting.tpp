#ifndef __DMLINQ_SORTING_TPP_INCLUDE__
#define __DMLINQ_SORTING_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
template <typename TFunc>
DmLinq<T>& DmLinq<T>::orderBy(TFunc key_selector, SortDirection direction) {
    // With C++17, deduce the key type from the return type of the callable
    using TKey = std::invoke_result_t<TFunc, const T&>;
    
    auto previous_logic = m_execution_logic;

    m_execution_logic = [previous_logic, key_selector, direction]() {
        auto source = previous_logic();
        std::stable_sort(source.begin(), source.end(), [key_selector, direction](const T& a, const T& b) {
            TKey keyA = key_selector(a);
            TKey keyB = key_selector(b);
            if (direction == SortDirection::ASC) {
                return keyA < keyB;
            } else {
                return keyB < keyA;
            }
        });
        return source;
    };

    return *this;
}

template <typename T>
template <typename TFunc>
DmLinq<T>& DmLinq<T>::orderByDescending(TFunc key_selector) {
    return orderBy(key_selector, SortDirection::DESC);
}

template <typename T>
template <typename TFunc>
DmLinq<T>& DmLinq<T>::thenBy(TFunc key_selector, SortDirection direction) {
    // Due to std::stable_sort, a subsequent sort correctly acts as a "thenBy".
    return orderBy(key_selector, direction);
}

template <typename T>
template <typename TFunc>
DmLinq<T>& DmLinq<T>::thenByDescending(TFunc key_selector) {
    return thenBy(key_selector, SortDirection::DESC);
}

} // namespace dmlinq

#endif // __DMLINQ_SORTING_TPP_INCLUDE__