#ifndef __DMLINQ_EXECUTION_TPP_INCLUDE__
#define __DMLINQ_EXECUTION_TPP_INCLUDE__

#include "../dmlinq.hpp"

namespace dmlinq {

template<typename T>
std::vector<T> DmLinq<T>::execute() const {
    // 1. Get initial data
    std::vector<T> results = m_source_provider();

    // 2. Apply filters
    if (!m_filters.empty()) {
        results.erase(std::remove_if(results.begin(), results.end(),
            [this](const T& item) {
                for (const auto& filter : m_filters) {
                    if (!filter(item)) return true; // true means remove
                }
                return false;
            }), results.end());
    }

    // 3. Apply sorting
    if (m_sorter) {
        std::stable_sort(results.begin(), results.end(), m_sorter);
    }

    // 4. Apply partitioning (skip)
    if (m_skip_count > 0) {
        if (m_skip_count >= results.size()) {
            results.clear();
        } else {
            results.erase(results.begin(), results.begin() + m_skip_count);
        }
    }

    // 5. Apply partitioning (take)
    if (m_take_count.has_value()) {
        if (*m_take_count < results.size()) {
            results.resize(*m_take_count);
        }
    }
    
    return results;
}

}

#endif