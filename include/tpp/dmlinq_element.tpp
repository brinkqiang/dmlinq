#ifndef __DMLINQ_ELEMENT_TPP_INCLUDE__
#define __DMLINQ_ELEMENT_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

//--- first ---
template<typename T>
T DmLinq<T>::first() {
    auto results = execute();
    if (results.empty()) throw std::runtime_error("Sequence contains no elements.");
    return results.front();
}
template<typename T>
template<typename TFunc>
T DmLinq<T>::first(TFunc predicate) {
    this->where(predicate);
    return this->first();
}

//--- firstOrDefault ---
template<typename T>
std::optional<T> DmLinq<T>::firstOrDefault() {
    auto results = execute();
    return results.empty() ? std::nullopt : std::optional<T>(results.front());
}
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::firstOrDefault(TFunc predicate) {
    this->where(predicate);
    return this->firstOrDefault();
}

//--- last ---
template<typename T>
T DmLinq<T>::last() {
    auto results = execute();
    if (results.empty()) throw std::runtime_error("Sequence contains no elements.");
    return results.back();
}
template<typename T>
template<typename TFunc>
T DmLinq<T>::last(TFunc predicate) {
    this->where(predicate);
    return this->last();
}

//--- lastOrDefault ---
template<typename T>
std::optional<T> DmLinq<T>::lastOrDefault() {
    auto results = execute();
    return results.empty() ? std::nullopt : std::optional<T>(results.back());
}
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::lastOrDefault(TFunc predicate) {
    this->where(predicate);
    return this->lastOrDefault();
}

//--- single ---
template<typename T>
T DmLinq<T>::single() {
    auto results = execute();
    if (results.size() != 1) throw std::runtime_error("Sequence does not contain exactly one element.");
    return results.front();
}
template<typename T>
template<typename TFunc>
T DmLinq<T>::single(TFunc predicate) {
    this->where(predicate);
    return this->single();
}

//--- singleOrDefault ---
template<typename T>
std::optional<T> DmLinq<T>::singleOrDefault() {
    auto results = execute();
    if (results.size() > 1) throw std::runtime_error("Sequence contains more than one element.");
    return results.empty() ? std::nullopt : std::optional<T>(results.front());
}
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::singleOrDefault(TFunc predicate) {
    this->where(predicate);
    return this->singleOrDefault();
}

} // namespace dmlinq
#endif // __DMLINQ_ELEMENT_TPP_INCLUDE__