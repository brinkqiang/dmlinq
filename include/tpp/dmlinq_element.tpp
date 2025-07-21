#ifndef __DMLINQ_ELEMENT_TPP_INCLUDE__
#define __DMLINQ_ELEMENT_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

//--- first ---
template<typename T>
T DmLinq<T>::first() {
    auto source = execute();
    if (source.empty()) {
        throw std::runtime_error("Sequence contains no elements.");
    }
    return source.front();
}
template<typename T>
template<typename TFunc>
T DmLinq<T>::first(TFunc predicate) {
    return where(predicate).first();
}

//--- firstOrDefault ---
template<typename T>
std::optional<T> DmLinq<T>::firstOrDefault() {
    auto source = execute();
    if (source.empty()) {
        return std::nullopt;
    }
    return source.front();
}
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::firstOrDefault(TFunc predicate) {
    return where(predicate).firstOrDefault();
}

//--- last ---
template<typename T>
T DmLinq<T>::last() {
    auto source = execute();
    if (source.empty()) {
        throw std::runtime_error("Sequence contains no elements.");
    }
    return source.back();
}
template<typename T>
template<typename TFunc>
T DmLinq<T>::last(TFunc predicate) {
    return where(predicate).last();
}

//--- lastOrDefault ---
template<typename T>
std::optional<T> DmLinq<T>::lastOrDefault() {
    auto source = execute();
    if (source.empty()) {
        return std::nullopt;
    }
    return source.back();
}
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::lastOrDefault(TFunc predicate) {
    return where(predicate).lastOrDefault();
}

//--- single ---
template<typename T>
T DmLinq<T>::single() {
    auto source = execute();
    if (source.size() != 1) {
        throw std::runtime_error(source.empty() ? "Sequence contains no elements." : "Sequence contains more than one element.");
    }
    return source.front();
}
template<typename T>
template<typename TFunc>
T DmLinq<T>::single(TFunc predicate) {
    return where(predicate).single();
}

//--- singleOrDefault ---
template<typename T>
std::optional<T> DmLinq<T>::singleOrDefault() {
    auto source = execute();
    if (source.size() > 1) {
        throw std::runtime_error("Sequence contains more than one element.");
    }
    if (source.empty()) {
        return std::nullopt;
    }
    return source.front();
}
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::singleOrDefault(TFunc predicate) {
    return where(predicate).singleOrDefault();
}

} // namespace dmlinq

#endif // __DMLINQ_ELEMENT_TPP_INCLUDE__