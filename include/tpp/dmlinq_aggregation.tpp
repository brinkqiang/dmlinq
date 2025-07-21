#ifndef __DMLINQ_AGGREGATION_TPP_INCLUDE__
#define __DMLINQ_AGGREGATION_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template<typename T>
size_t DmLinq<T>::count() { return execute().size(); }

template<typename T>
template<typename TFunc>
size_t DmLinq<T>::count(TFunc predicate) { return where(predicate).count(); }

template<typename T>
template<typename TFunc>
auto DmLinq<T>::sum(TFunc selector) -> std::invoke_result_t<TFunc, const T&> {
    using TResult = std::invoke_result_t<TFunc, const T&>;
    if constexpr (!std::is_arithmetic_v<TResult>) { static_assert(std::is_arithmetic_v<TResult>, "sum() selector must project to an arithmetic type."); }
    auto source = execute();
    TResult total{};
    for (const auto& item : source) { total += selector(item); }
    return total;
}

template<typename T>
auto DmLinq<T>::sum() -> T {
    if constexpr (!std::is_arithmetic_v<T>) { static_assert(std::is_arithmetic_v<T>, "sum() requires an arithmetic type."); }
    auto source = execute();
    return std::accumulate(source.begin(), source.end(), T{});
}

template<typename T>
template<typename TFunc>
double DmLinq<T>::average(TFunc selector) {
    auto source = execute();
    if (source.empty()) return 0.0;
    return static_cast<double>(sum(selector)) / source.size();
}

template<typename T>
double DmLinq<T>::average() {
    auto source = execute();
    if (source.empty()) return 0.0;
    return static_cast<double>(sum()) / source.size();
}

template<typename T>
T DmLinq<T>::max() { auto r = execute(); if (r.empty()) throw std::runtime_error("Empty sequence"); return *std::max_element(r.begin(), r.end()); }
template<typename T>
T DmLinq<T>::min() { auto r = execute(); if (r.empty()) throw std::runtime_error("Empty sequence"); return *std::min_element(r.begin(), r.end()); }

}

#endif