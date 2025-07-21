#ifndef __DMLINQ_PARTITIONING_TPP_INCLUDE__
#define __DMLINQ_PARTITIONING_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
DmLinq<T>& DmLinq<T>::take(size_t count) {
    m_take_count = count;
    return *this;
}

template <typename T>
DmLinq<T>& DmLinq<T>::skip(size_t count) {
    m_skip_count = count;
    return *this;
}

}

#endif