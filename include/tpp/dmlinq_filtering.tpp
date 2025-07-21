#ifndef __DMLINQ_FILTERING_TPP_INCLUDE__
#define __DMLINQ_FILTERING_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
template<typename TFunc>
DmLinq<T>& DmLinq<T>::where(TFunc predicate) {
    m_filters.push_back(predicate);
    return *this;
}

}

#endif