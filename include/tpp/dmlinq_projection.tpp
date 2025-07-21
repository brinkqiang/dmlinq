#ifndef __DMLINQ_PROJECTION_TPP_INCLUDE__
#define __DMLINQ_PROJECTION_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
template <typename TFunc>
auto DmLinq<T>::select(TFunc selector) -> DmLinq<std::invoke_result_t<TFunc, const T&>> {
    using TResult = std::invoke_result_t<TFunc, const T&>;
    auto self = std::make_shared<DmLinq<T>>(*this);
    
    auto new_source_provider = [self, selector]() {
        auto source = self->execute();
        std::vector<TResult> result;
        result.reserve(source.size());
        for (const auto& item : source) {
            result.push_back(selector(item));
        }
        return result;
    };
    return DmLinq<TResult>(self, new_source_provider);
}

template <typename T>
template <typename TFunc>
auto DmLinq<T>::selectMany(TFunc selector) -> DmLinq<typename std::invoke_result_t<TFunc, const T&>::value_type> {
    using TResultVector = std::invoke_result_t<TFunc, const T&>;
    using TResult = typename TResultVector::value_type;
    auto self = std::make_shared<DmLinq<T>>(*this);

    auto new_source_provider = [self, selector]() {
        auto source = self->execute();
        std::vector<TResult> result;
        for (const auto& item : source) {
            auto sub_sequence = selector(item);
            result.insert(result.end(), sub_sequence.begin(), sub_sequence.end());
        }
        return result;
    };
    return DmLinq<TResult>(self, new_source_provider);
}

}

#endif