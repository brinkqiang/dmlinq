#ifndef __DMLINQ_PARTITIONING_TPP_INCLUDE__
#define __DMLINQ_PARTITIONING_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template <typename T>
DmLinq<T>& DmLinq<T>::take(size_t count) {
    auto previous_logic = m_execution_logic;

    m_execution_logic = [previous_logic, count]() {
        auto source = previous_logic();
        if (count < source.size()) {
            source.resize(count);
        }
        return source;
    };

    return *this;
}

template <typename T>
DmLinq<T>& DmLinq<T>::skip(size_t count) {
    auto previous_logic = m_execution_logic;

    m_execution_logic = [previous_logic, count]() {
        auto source = previous_logic();
        if (count >= source.size()) {
            return std::vector<T>();
        }
        // Return a new vector created from a slice of the source
        return std::vector<T>(source.begin() + count, source.end());
    };

    return *this;
}

} // namespace dmlinq

#endif // __DMLINQ_PARTITIONING_TPP_INCLUDE__