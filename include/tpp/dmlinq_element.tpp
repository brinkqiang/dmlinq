#ifndef __DMLINQ_ELEMENT_TPP_INCLUDE__
#define __DMLINQ_ELEMENT_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template<typename T>
T DmLinq<T>::first() { return execute().front(); }

template<typename T>
template<typename TFunc>
T DmLinq<T>::first(TFunc predicate) {
    where(predicate);
    auto results = execute();
    if (results.empty()) throw std::runtime_error("Sequence contains no matching elements.");
    return results.front();
}

template<typename T>
std::optional<T> DmLinq<T>::firstOrDefault() {
    auto results = execute();
    return results.empty() ? std::nullopt : std::optional<T>(results.front());
}

template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::firstOrDefault(TFunc predicate) {
    where(predicate);
    return firstOrDefault();
}

// last, single, etc. follow the same pattern...
template<typename T>
T DmLinq<T>::last() { auto r = execute(); if(r.empty()) throw std::runtime_error("Empty sequence"); return r.back(); }
template<typename T>
template<typename TFunc>
T DmLinq<T>::last(TFunc predicate) { where(predicate); return last(); }
template<typename T>
std::optional<T> DmLinq<T>::lastOrDefault() { auto r = execute(); return r.empty() ? std::nullopt : std::optional<T>(r.back()); }
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::lastOrDefault(TFunc predicate) { where(predicate); return lastOrDefault(); }

template<typename T>
T DmLinq<T>::single() { auto r = execute(); if(r.size() != 1) throw std::runtime_error("Sequence does not contain exactly one element."); return r.front(); }
template<typename T>
template<typename TFunc>
T DmLinq<T>::single(TFunc predicate) { where(predicate); return single(); }
template<typename T>
std::optional<T> DmLinq<T>::singleOrDefault() { auto r = execute(); if(r.size() > 1) throw std::runtime_error("Sequence contains more than one element."); return r.empty() ? std::nullopt : std::optional<T>(r.front()); }
template<typename T>
template<typename TFunc>
std::optional<T> DmLinq<T>::singleOrDefault(TFunc predicate) { where(predicate); return singleOrDefault(); }

}

#endif