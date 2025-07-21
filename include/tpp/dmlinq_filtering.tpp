#ifndef __DMLINQ_FILTERING_TPP_INCLUDE__
#define __DMLINQ_FILTERING_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
template<typename TFunc>
DmLinq<T>& DmLinq<T>::where(TFunc predicate) {
    auto previous_logic = m_execution_logic;
    
    m_execution_logic = [previous_logic, predicate]() {
        auto source = previous_logic();
        std::vector<T> result;
        std::copy_if(source.begin(), source.end(), std::back_inserter(result), predicate);
        return result;
    };
    
    return *this;
}

} // namespace dmlinq

#endif // __DMLINQ_FILTERING_TPP_INCLUDE__