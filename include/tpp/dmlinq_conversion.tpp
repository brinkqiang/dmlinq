#ifndef __DMLINQ_CONVERSION_TPP_INCLUDE__
#define __DMLINQ_CONVERSION_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
std::vector<T> DmLinq<T>::toVector() { return execute(); }

template <typename T>
std::set<T> DmLinq<T>::toSet() { auto r = execute(); return std::set<T>(r.begin(), r.end()); }

template <typename T>
template <typename TFunc>
auto DmLinq<T>::toMap(TFunc key_selector) -> std::map<std::invoke_result_t<TFunc, const T&>, T> {
    using TKey = std::invoke_result_t<TFunc, const T&>;
    auto source = execute();
    std::map<TKey, T> result;
    for (const auto& item : source) { result.emplace(key_selector(item), item); }
    return result;
}

template <typename T>
template <typename TKeyFunc, typename TValueFunc>
auto DmLinq<T>::toMap(TKeyFunc key_selector, TValueFunc value_selector) -> std::map<std::invoke_result_t<TKeyFunc, const T&>, std::invoke_result_t<TValueFunc, const T&>> {
    using TKey = std::invoke_result_t<TKeyFunc, const T&>;
    using TValue = std::invoke_result_t<TValueFunc, const T&>;
    auto source = execute();
    std::map<TKey, TValue> result;
    for (const auto& item : source) { result.emplace(key_selector(item), value_selector(item)); }
    return result;
}

}

#endif