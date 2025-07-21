#ifndef __DMLINQ_SORTING_TPP_INCLUDE__
#define __DMLINQ_SORTING_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
template <typename TFunc>
DmLinq<T>& DmLinq<T>::orderBy(TFunc key_selector, SortDirection direction) {
    using TKey = std::invoke_result_t<TFunc, const T&>;
    m_sorter = [key_selector, direction](const T& a, const T& b) {
        TKey keyA = key_selector(a);
        TKey keyB = key_selector(b);
        return (direction == SortDirection::ASC) ? (keyA < keyB) : (keyB < keyA);
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
    if (!m_sorter) { return orderBy(key_selector, direction); }

    using TKey = std::invoke_result_t<TFunc, const T&>;
    auto previous_sorter = m_sorter;
    m_sorter = [previous_sorter, key_selector, direction](const T& a, const T& b) {
        if (previous_sorter(a, b)) return true;
        if (previous_sorter(b, a)) return false;
        TKey keyA = key_selector(a);
        TKey keyB = key_selector(b);
        return (direction == SortDirection::ASC) ? (keyA < keyB) : (keyB < keyA);
    };
    return *this;
}

template <typename T>
template <typename TFunc>
DmLinq<T>& DmLinq<T>::thenByDescending(TFunc key_selector) {
    return thenBy(key_selector, SortDirection::DESC);
}

}

#endif