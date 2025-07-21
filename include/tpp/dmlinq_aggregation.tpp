#ifndef __DMLINQ_AGGREGATION_TPP_INCLUDE__
#define __DMLINQ_AGGREGATION_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

//--- count ---
template<typename T>
size_t DmLinq<T>::count() {
    return execute().size();
}
template<typename T>
template<typename TFunc>
size_t DmLinq<T>::count(TFunc predicate) {
    return where(predicate).count();
}

//--- sum ---
template<typename T>
template<typename TFunc>
auto DmLinq<T>::sum(TFunc selector) -> std::invoke_result_t<TFunc, const T&> {
    using TResult = std::invoke_result_t<TFunc, const T&>;
    if constexpr (!std::is_arithmetic_v<TResult>) {
        static_assert(std::is_arithmetic_v<TResult>, "sum() with a selector must project to an arithmetic type.");
    }
    
    auto source = execute();
    TResult total{}; // Zero-initialize
    for (const auto& item : source) {
        total += selector(item);
    }
    return total;
}

template<typename T>
auto DmLinq<T>::sum() -> T {
    if constexpr (!std::is_arithmetic_v<T>) {
        static_assert(std::is_arithmetic_v<T>, "sum() without a selector can only be used on a sequence of arithmetic types.");
    }
    auto source = execute();
    return std::accumulate(source.begin(), source.end(), T{});
}

//--- average ---
template<typename T>
template<typename TFunc>
double DmLinq<T>::average(TFunc selector) {
    using TResult = std::invoke_result_t<TFunc, const T&>;
    if constexpr (!std::is_arithmetic_v<TResult>) {
        static_assert(std::is_arithmetic_v<TResult>, "average() with a selector must project to an arithmetic type.");
    }

    auto source = execute();
    if (source.empty()) {
        return 0.0;
    }
    
    // Use double for summation to maintain precision
    double total_sum = 0.0;
    for(const auto& item : source) {
        total_sum += static_cast<double>(selector(item));
    }
    return total_sum / source.size();
}

template<typename T>
double DmLinq<T>::average() {
     if constexpr (!std::is_arithmetic_v<T>) {
        static_assert(std::is_arithmetic_v<T>, "average() without a selector can only be used on a sequence of arithmetic types.");
    }
    auto source = execute();
    if (source.empty()) {
        return 0.0;
    }
    // Use double for summation to maintain precision
    double total_sum = 0.0;
    for(const auto& item : source) {
        total_sum += static_cast<double>(item);
    }
    return total_sum / source.size();
}

//--- max ---
template<typename T>
T DmLinq<T>::max() {
    auto source = execute();
    if (source.empty()) {
        throw std::runtime_error("Sequence contains no elements to find max value.");
    }
    return *std::max_element(source.begin(), source.end());
}

//--- min ---
template<typename T>
T DmLinq<T>::min() {
    auto source = execute();
    if (source.empty()) {
        throw std::runtime_error("Sequence contains no elements to find min value.");
    }
    return *std::min_element(source.begin(), source.end());
}

} // namespace dmlinq

#endif // __DMLINQ_AGGREGATION_TPP_INCLUDE__