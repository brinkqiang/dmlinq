#ifndef __DMLINQ_HPP_INCLUDE__
#define __DMLINQ_HPP_INCLUDE__

#include <vector>
#include <functional>
#include <memory>
#include <optional>
#include <numeric>
#include <map>
#include <set>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <type_traits>

namespace dmlinq {

template <typename T>
class DmLinq;

template <typename T>
[[nodiscard]] DmLinq<T> from(const std::vector<T>& source);
template <typename T>
[[nodiscard]] DmLinq<T> from(std::vector<T>&& source);

enum class SortDirection { ASC, DESC };

template <typename T>
class DmLinq {
public:
    // Internal use for chaining
    DmLinq(std::shared_ptr<void> previous_stage, std::function<std::vector<T>()> source_provider);

private:
    friend DmLinq<T> from<T>(const std::vector<T>& source);
    friend DmLinq<T> from<T>(std::vector<T>&& source);
    DmLinq(const std::vector<T>& source);

    // Pipeline components
    std::shared_ptr<void> m_previous_stage;
    std::function<std::vector<T>()> m_source_provider;
    std::vector<std::function<bool(const T&)>> m_filters;
    std::function<bool(const T&, const T&)> m_sorter;
    size_t m_skip_count = 0;
    std::optional<size_t> m_take_count;

    std::vector<T> execute() const;

public:
    template<typename TFunc> [[nodiscard]] DmLinq<T>& where(TFunc predicate);
    template <typename TFunc> [[nodiscard]] DmLinq<T>& orderBy(TFunc key_selector, SortDirection direction = SortDirection::ASC);
    template <typename TFunc> [[nodiscard]] DmLinq<T>& orderByDescending(TFunc key_selector);
    template <typename TFunc> [[nodiscard]] DmLinq<T>& thenBy(TFunc key_selector, SortDirection direction = SortDirection::ASC);
    template <typename TFunc> [[nodiscard]] DmLinq<T>& thenByDescending(TFunc key_selector);
    template <typename TFunc> [[nodiscard]] auto select(TFunc selector) -> DmLinq<std::invoke_result_t<TFunc, const T&>>;
    template <typename TFunc> [[nodiscard]] auto selectMany(TFunc selector) -> DmLinq<typename std::invoke_result_t<TFunc, const T&>::value_type>;
    [[nodiscard]] DmLinq<T>& take(size_t count);
    [[nodiscard]] DmLinq<T>& skip(size_t count);
    T first();
    template<typename TFunc> T first(TFunc predicate);
    std::optional<T> firstOrDefault();
    template<typename TFunc> std::optional<T> firstOrDefault(TFunc predicate);
    T last();
    template<typename TFunc> T last(TFunc predicate);
    std::optional<T> lastOrDefault();
    template<typename TFunc> std::optional<T> lastOrDefault(TFunc predicate);
    T single();
    template<typename TFunc> T single(TFunc predicate);
    std::optional<T> singleOrDefault();
    template<typename TFunc> std::optional<T> singleOrDefault(TFunc predicate);
    size_t count();
    template<typename TFunc> size_t count(TFunc predicate);
    template<typename TFunc> auto sum(TFunc selector) -> std::invoke_result_t<TFunc, const T&>;
    auto sum() -> T;
    template<typename TFunc> double average(TFunc selector);
    double average();
    T max();
    T min();
    bool any();
    template<typename TFunc> bool any(TFunc predicate);
    template<typename TFunc> bool all(TFunc predicate);
    std::vector<T> toVector();
    std::set<T> toSet();
    template <typename TFunc> auto toMap(TFunc key_selector) -> std::map<std::invoke_result_t<TFunc, const T&>, T>;
    template <typename TKeyFunc, typename TValueFunc>
    auto toMap(TKeyFunc key_selector, TValueFunc value_selector) -> std::map<std::invoke_result_t<TKeyFunc, const T&>, std::invoke_result_t<TValueFunc, const T&>>;
};

template<typename T>
DmLinq<T>::DmLinq(const std::vector<T>& source) {
    m_source_provider = [source]() { return source; };
}
template<typename T>
DmLinq<T>::DmLinq(std::shared_ptr<void> previous_stage, std::function<std::vector<T>()> source_provider)
    : m_previous_stage(previous_stage), m_source_provider(source_provider) {}

template <typename T>
DmLinq<T> from(const std::vector<T>& source) {
    return DmLinq<T>(source);
}
template <typename T>
DmLinq<T> from(std::vector<T>&& source) {
    // To keep it simple, we copy. A move-based optimization is possible but more complex.
    return DmLinq<T>(source);
}

}

#include "tpp/dmlinq_execution.tpp"
#include "tpp/dmlinq_filtering.tpp"
#include "tpp/dmlinq_sorting.tpp"
#include "tpp/dmlinq_projection.tpp"
#include "tpp/dmlinq_partitioning.tpp"
#include "tpp/dmlinq_element.tpp"
#include "tpp/dmlinq_aggregation.tpp"
#include "tpp/dmlinq_quantifiers.tpp"
#include "tpp/dmlinq_conversion.tpp"

#endif