#ifndef __DMLINQ_AGGREGATION_TPP_INCLUDE__
#define __DMLINQ_AGGREGATION_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

//--- count (Corrected implementation with overloads) ---
template<typename T>
size_t DmLinq<T>::count() {
    return execute().size();
}

template<typename T>
template<typename TFunc>
size_t DmLinq<T>::count(TFunc predicate) {
    this->where(predicate);
    return this->count();
}

//--- sum ---
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

//--- average ---
template<typename T>
template<typename TFunc>
double DmLinq<T>::average(TFunc selector) {
    auto source = execute();
    if (source.empty()) return 0.0;
    // Use double for summation to maintain precision
    double total_sum = 0.0;
    using TResult = std::invoke_result_t<TFunc, const T&>;
    if constexpr (!std::is_arithmetic_v<TResult>) { static_assert(std::is_arithmetic_v<TResult>, "average() selector must project to an arithmetic type."); }
    for(const auto& item : source) {
        total_sum += static_cast<double>(selector(item));
    }
    return total_sum / source.size();
}

template<typename T>
double DmLinq<T>::average() {
     if constexpr (!std::is_arithmetic_v<T>) { static_assert(std::is_arithmetic_v<T>, "average() requires an arithmetic type."); }
    auto source = execute();
    if (source.empty()) return 0.0;
    double total_sum = 0.0;
    for(const auto& item : source) {
        total_sum += static_cast<double>(item);
    }
    return total_sum / source.size();
}

//--- max / min ---
template<typename T>
T DmLinq<T>::max() { auto r = execute(); if (r.empty()) throw std::runtime_error("Empty sequence"); return *std::max_element(r.begin(), r.end()); }
template<typename T>
T DmLinq<T>::min() { auto r = execute(); if (r.empty()) throw std::runtime_error("Empty sequence"); return *std::min_element(r.begin(), r.end()); }

} // namespace dmlinq
#endif // __DMLINQ_AGGREGATION_TPP_INCLUDE__