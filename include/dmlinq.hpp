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
#include <type_traits> // Required for C++17 type traits

namespace dmlinq {

    // Forward declaration
    template <typename T>
    class DmLinq;

    // Entry point for starting a LINQ query
    template <typename T>
    [[nodiscard]] DmLinq<T> from(const std::vector<T>& source);
    template <typename T>
    [[nodiscard]] DmLinq<T> from(std::vector<T>&& source);


    // Enum for sorting direction
    enum class SortDirection {
        ASC,
        DESC
    };

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

    // ===================================================================================
    // === Inlined Implementations (replaces all .tpp files) =============================
    // ===================================================================================

    // --- Constructors and Entry Points ---
    template<typename T>
    DmLinq<T>::DmLinq(const std::vector<T>& source) {
        m_source_provider = [source]() { return source; };
    }
    template<typename T>
    DmLinq<T>::DmLinq(std::shared_ptr<void> previous_stage, std::function<std::vector<T>()> source_provider)
        : m_previous_stage(previous_stage), m_source_provider(source_provider) {
    }

    template <typename T>
    DmLinq<T> from(const std::vector<T>& source) {
        return DmLinq<T>(source);
    }
    template <typename T>
    DmLinq<T> from(std::vector<T>&& source) {
        // To keep it simple, we copy. A move-based optimization is possible but more complex.
        return DmLinq<T>(source);
    }

    // --- dmlinq_execution ---
    template<typename T>
    std::vector<T> DmLinq<T>::execute() const {
        std::vector<T> results = m_source_provider();
        if (!m_filters.empty()) {
            results.erase(std::remove_if(results.begin(), results.end(),
                [this](const T& item) {
                    for (const auto& filter : m_filters) {
                        if (!filter(item)) return true; // true means remove
                    }
                    return false;
                }), results.end());
        }
        if (m_sorter) {
            std::stable_sort(results.begin(), results.end(), m_sorter);
        }
        if (m_skip_count > 0) {
            if (m_skip_count >= results.size()) {
                results.clear();
            }
            else {
                results.erase(results.begin(), results.begin() + m_skip_count);
            }
        }
        if (m_take_count.has_value()) {
            if (*m_take_count < results.size()) {
                results.resize(*m_take_count);
            }
        }
        return results;
    }

    // --- dmlinq_filtering ---
    template <typename T>
    template<typename TFunc>
    DmLinq<T>& DmLinq<T>::where(TFunc predicate) {
        m_filters.push_back(predicate);
        return *this;
    }

    // --- dmlinq_sorting ---
    template <typename T>
    template <typename TFunc>
    DmLinq<T>& DmLinq<T>::orderBy(TFunc key_selector, SortDirection direction) {
        using TKey = std::invoke_result_t<TFunc, const T&>;
        m_sorter = [key_selector, direction](const T& a, const T& b) {
            TKey keyA = key_selector(a);
            TKey keyB = key_selector(b);
            return (direction == SortDirection::ASC) ? (keyA < keyB) : (keyB < keyA);
            };
        return *this;
    }
    template <typename T>
    template <typename TFunc>
    DmLinq<T>& DmLinq<T>::orderByDescending(TFunc key_selector) { return orderBy(key_selector, SortDirection::DESC); }
    template <typename T>
    template <typename TFunc>
    DmLinq<T>& DmLinq<T>::thenBy(TFunc key_selector, SortDirection direction) {
        if (!m_sorter) { return orderBy(key_selector, direction); }
        using TKey = std::invoke_result_t<TFunc, const T&>;
        auto previous_sorter = m_sorter;
        m_sorter = [previous_sorter, key_selector, direction](const T& a, const T& b) {
            if (previous_sorter(a, b)) return true;
            if (previous_sorter(b, a)) return false;
            TKey keyA = key_selector(a);
            TKey keyB = key_selector(b);
            return (direction == SortDirection::ASC) ? (keyA < keyB) : (keyB < keyA);
            };
        return *this;
    }
    template <typename T>
    template <typename TFunc>
    DmLinq<T>& DmLinq<T>::thenByDescending(TFunc key_selector) { return thenBy(key_selector, SortDirection::DESC); }

    // --- dmlinq_projection ---
    template <typename T>
    template <typename TFunc>
    auto DmLinq<T>::select(TFunc selector) -> DmLinq<std::invoke_result_t<TFunc, const T&>> {
        using TResult = std::invoke_result_t<TFunc, const T&>;
        auto self = std::make_shared<DmLinq<T>>(*this);
        auto new_source_provider = [self, selector]() {
            auto source = self->execute();
            std::vector<TResult> result;
            result.reserve(source.size());
            for (const auto& item : source) { result.push_back(selector(item)); }
            return result;
            };
        return DmLinq<TResult>(self, new_source_provider);
    }
    template <typename T>
    template <typename TFunc>
    auto DmLinq<T>::selectMany(TFunc selector) -> DmLinq<typename std::invoke_result_t<TFunc, const T&>::value_type> {
        using TResultVector = std::invoke_result_t<TFunc, const T&>;
        using TResult = typename TResultVector::value_type;
        auto self = std::make_shared<DmLinq<T>>(*this);
        auto new_source_provider = [self, selector]() {
            auto source = self->execute();
            std::vector<TResult> result;
            for (const auto& item : source) {
                auto sub_sequence = selector(item);
                result.insert(result.end(), sub_sequence.begin(), sub_sequence.end());
            }
            return result;
            };
        return DmLinq<TResult>(self, new_source_provider);
    }

    // --- dmlinq_partitioning ---
    template <typename T>
    DmLinq<T>& DmLinq<T>::take(size_t count) { m_take_count = count; return *this; }
    template <typename T>
    DmLinq<T>& DmLinq<T>::skip(size_t count) { m_skip_count = count; return *this; }

    // --- dmlinq_element ---
    template<typename T> T DmLinq<T>::first() { auto r = execute(); if (r.empty()) throw std::runtime_error("Sequence contains no elements."); return r.front(); }
    template<typename T> template<typename TFunc> T DmLinq<T>::first(TFunc predicate) { return this->where(predicate).first(); }
    template<typename T> std::optional<T> DmLinq<T>::firstOrDefault() { auto r = execute(); return r.empty() ? std::nullopt : std::optional<T>(r.front()); }
    template<typename T> template<typename TFunc> std::optional<T> DmLinq<T>::firstOrDefault(TFunc predicate) { return this->where(predicate).firstOrDefault(); }
    template<typename T> T DmLinq<T>::last() { auto r = execute(); if (r.empty()) throw std::runtime_error("Empty sequence"); return r.back(); }
    template<typename T> template<typename TFunc> T DmLinq<T>::last(TFunc predicate) { return this->where(predicate).last(); }
    template<typename T> std::optional<T> DmLinq<T>::lastOrDefault() { auto r = execute(); return r.empty() ? std::nullopt : std::optional<T>(r.back()); }
    template<typename T> template<typename TFunc> std::optional<T> DmLinq<T>::lastOrDefault(TFunc predicate) { return this->where(predicate).lastOrDefault(); }
    template<typename T> T DmLinq<T>::single() { auto r = execute(); if (r.size() != 1) throw std::runtime_error("Sequence does not contain exactly one element."); return r.front(); }
    template<typename T> template<typename TFunc> T DmLinq<T>::single(TFunc predicate) { return this->where(predicate).single(); }
    template<typename T> std::optional<T> DmLinq<T>::singleOrDefault() { auto r = execute(); if (r.size() > 1) throw std::runtime_error("Sequence contains more than one element."); return r.empty() ? std::nullopt : std::optional<T>(r.front()); }
    template<typename T> template<typename TFunc> std::optional<T> DmLinq<T>::singleOrDefault(TFunc predicate) { return this->where(predicate).singleOrDefault(); }

    // --- dmlinq_aggregation ---
    template<typename T> size_t DmLinq<T>::count() { return execute().size(); }
    template<typename T> template<typename TFunc> size_t DmLinq<T>::count(TFunc predicate) { return this->where(predicate).count(); }
    template<typename T> template<typename TFunc> auto DmLinq<T>::sum(TFunc selector) -> std::invoke_result_t<TFunc, const T&> {
        using TResult = std::invoke_result_t<TFunc, const T&>;
        if constexpr (!std::is_arithmetic_v<TResult>) { static_assert(std::is_arithmetic_v<TResult>, "sum() selector must project to an arithmetic type."); }
        auto source = execute(); TResult total{}; for (const auto& item : source) { total += selector(item); } return total;
    }
    template<typename T> auto DmLinq<T>::sum() -> T {
        if constexpr (!std::is_arithmetic_v<T>) { static_assert(std::is_arithmetic_v<T>, "sum() requires an arithmetic type."); }
        auto source = execute(); return std::accumulate(source.begin(), source.end(), T{});
    }
    template<typename T> template<typename TFunc> double DmLinq<T>::average(TFunc selector) {
        auto source = execute(); if (source.empty()) return 0.0;
        double total_sum = 0.0; using TResult = std::invoke_result_t<TFunc, const T&>;
        if constexpr (!std::is_arithmetic_v<TResult>) { static_assert(std::is_arithmetic_v<TResult>, "average() selector must project to an arithmetic type."); }
        for (const auto& item : source) { total_sum += static_cast<double>(selector(item)); }
        return total_sum / source.size();
    }
    template<typename T> double DmLinq<T>::average() {
        if constexpr (!std::is_arithmetic_v<T>) { static_assert(std::is_arithmetic_v<T>, "average() requires an arithmetic type."); }
        auto source = execute(); if (source.empty()) return 0.0;
        double total_sum = 0.0; for (const auto& item : source) { total_sum += static_cast<double>(item); }
        return total_sum / source.size();
    }
    template<typename T> T DmLinq<T>::max() { auto r = execute(); if (r.empty()) throw std::runtime_error("Empty sequence"); return *std::max_element(r.begin(), r.end()); }
    template<typename T> T DmLinq<T>::min() { auto r = execute(); if (r.empty()) throw std::runtime_error("Empty sequence"); return *std::min_element(r.begin(), r.end()); }

    // --- dmlinq_quantifiers ---
    template<typename T> bool DmLinq<T>::any() { return !execute().empty(); }
    template<typename T> template<typename TFunc> bool DmLinq<T>::any(TFunc predicate) { auto r = execute(); return std::any_of(r.begin(), r.end(), predicate); }
    template<typename T> template<typename TFunc> bool DmLinq<T>::all(TFunc predicate) { auto r = execute(); return std::all_of(r.begin(), r.end(), predicate); }

    // --- dmlinq_conversion ---
    template <typename T> std::vector<T> DmLinq<T>::toVector() { return execute(); }
    template <typename T> std::set<T> DmLinq<T>::toSet() { auto r = execute(); return std::set<T>(r.begin(), r.end()); }
    template <typename T> template <typename TFunc> auto DmLinq<T>::toMap(TFunc key_selector) -> std::map<std::invoke_result_t<TFunc, const T&>, T> {
        using TKey = std::invoke_result_t<TFunc, const T&>; auto source = execute(); std::map<TKey, T> result; for (const auto& item : source) { result.emplace(key_selector(item), item); } return result;
    }
    template <typename T> template <typename TKeyFunc, typename TValueFunc> auto DmLinq<T>::toMap(TKeyFunc key_selector, TValueFunc value_selector) -> std::map<std::invoke_result_t<TKeyFunc, const T&>, std::invoke_result_t<TValueFunc, const T&>> {
        using TKey = std::invoke_result_t<TKeyFunc, const T&>; using TValue = std::invoke_result_t<TValueFunc, const T&>; auto source = execute(); std::map<TKey, TValue> result; for (const auto& item : source) { result.emplace(key_selector(item), value_selector(item)); } return result;
    }

} // namespace dmlinq

#endif // __DMLINQ_HPP_INCLUDE__