#ifndef __DMLINQ_QUANTIFIERS_TPP_INCLUDE__
#define __DMLINQ_QUANTIFIERS_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

//--- any ---
template<typename T>
bool DmLinq<T>::any() {
    return !execute().empty();
}
template<typename T>
template<typename TFunc>
bool DmLinq<T>::any(TFunc predicate) {
    auto source = execute();
    return std::any_of(source.begin(), source.end(), predicate);
}

//--- all ---
template<typename T>
template<typename TFunc>
bool DmLinq<T>::all(TFunc predicate) {
    auto source = execute();
    return std::all_of(source.begin(), source.end(), predicate);
}

} // namespace dmlinq
#endif // __DMLINQ_QUANTIFIERS_TPP_INCLUDE__