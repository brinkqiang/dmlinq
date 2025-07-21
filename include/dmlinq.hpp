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
        DmLinq(std::shared_ptr<void> previous_stage, std::function<std::vector<T>()> execution_logic);

    private:
        friend DmLinq<T> from<T>(const std::vector<T>& source);
        friend DmLinq<T> from<T>(std::vector<T>&& source);
        DmLinq(const std::vector<T>& source);
        DmLinq(std::vector<T>&& source);

        std::shared_ptr<void> m_previous_stage;
        std::function<std::vector<T>()> m_execution_logic;

        std::vector<T> execute() const;

    public:
        // --- Overloads for Element, Aggregation, and Quantifier methods are corrected ---

        // Filtering
        template<typename TFunc> [[nodiscard]] DmLinq<T>& where(TFunc predicate);

        // Sorting
        template <typename TFunc> [[nodiscard]] DmLinq<T>& orderBy(TFunc key_selector, SortDirection direction = SortDirection::ASC);
        template <typename TFunc> [[nodiscard]] DmLinq<T>& orderByDescending(TFunc key_selector);
        template <typename TFunc> [[nodiscard]] DmLinq<T>& thenBy(TFunc key_selector, SortDirection direction = SortDirection::ASC);
        template <typename TFunc> [[nodiscard]] DmLinq<T>& thenByDescending(TFunc key_selector);

        // Projection
        template <typename TFunc> [[nodiscard]] auto select(TFunc selector) -> DmLinq<std::invoke_result_t<TFunc, const T&>>;
        template <typename TFunc> [[nodiscard]] auto selectMany(TFunc selector) -> DmLinq<typename std::invoke_result_t<TFunc, const T&>::value_type>;

        // Partitioning
        [[nodiscard]] DmLinq<T>& take(size_t count);
        [[nodiscard]] DmLinq<T>& skip(size_t count);

        // Element Operations (Overloads corrected)
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

        // Aggregation (Overloads corrected)
        size_t count();
        template<typename TFunc> size_t count(TFunc predicate);
        template<typename TFunc> auto sum(TFunc selector) -> std::invoke_result_t<TFunc, const T&>;
        auto sum() -> T;
        template<typename TFunc> double average(TFunc selector);
        double average();
        T max();
        T min();

        // Quantifiers (Overloads corrected)
        bool any();
        template<typename TFunc> bool any(TFunc predicate);
        template<typename TFunc> bool all(TFunc predicate);

        // Conversion
        std::vector<T> toVector();
        std::set<T> toSet();
        template <typename TFunc> auto toMap(TFunc key_selector) -> std::map<std::invoke_result_t<TFunc, const T&>, T>;
        template <typename TKeyFunc, typename TValueFunc>
        auto toMap(TKeyFunc key_selector, TValueFunc value_selector) -> std::map<std::invoke_result_t<TKeyFunc, const T&>, std::invoke_result_t<TValueFunc, const T&>>;
    };

    // --- Implementation of constructors and entry points ---
    template<typename T>
    DmLinq<T>::DmLinq(const std::vector<T>& source) : m_previous_stage(nullptr) {
        m_execution_logic = [source]() { return source; };
    }
    template<typename T>
    DmLinq<T>::DmLinq(std::vector<T>&& source) : m_previous_stage(nullptr) {
        m_execution_logic = [source = std::move(source)]() { return source; };
    }
    template<typename T>
    DmLinq<T>::DmLinq(std::shared_ptr<void> previous_stage, std::function<std::vector<T>()> execution_logic)
        : m_previous_stage(previous_stage), m_execution_logic(execution_logic) {
    }
    template<typename T>
    std::vector<T> DmLinq<T>::execute() const {
        return m_execution_logic();
    }
    template <typename T>
    DmLinq<T> from(const std::vector<T>& source) {
        return DmLinq<T>(source);
    }
    template <typename T>
    DmLinq<T> from(std::vector<T>&& source) {
        return DmLinq<T>(std::move(source));
    }

} // namespace dmlinq

// Includes for implementation files
#include "tpp/dmlinq_filtering.tpp"
#include "tpp/dmlinq_sorting.tpp"
#include "tpp/dmlinq_projection.tpp"
#include "tpp/dmlinq_partitioning.tpp"
#include "tpp/dmlinq_element.tpp"
#include "tpp/dmlinq_aggregation.tpp"
#include "tpp/dmlinq_quantifiers.tpp"
#include "tpp/dmlinq_conversion.tpp"

#endif // __DMLINQ_HPP_INCLUDE__