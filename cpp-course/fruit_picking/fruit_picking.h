#ifndef FRUIT_PICKING_H
#define FRUIT_PICKING_H

#include <algorithm>
#include <compare>
#include <cstddef>
#include <deque>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

enum class Taste { SWEET, SOUR };
enum class Size { LARGE, MEDIUM, SMALL };
enum class Quality { HEALTHY, ROTTEN, WORMY };

class Fruit {
  private:
    Taste taste_;
    Size size_;
    Quality quality_;

  public:
    constexpr explicit Fruit(Taste taste, Size size, Quality quality)
        : taste_(taste), size_(size), quality_(quality) {}

    Fruit(const Fruit &other) = default;
    Fruit(Fruit &&other) noexcept = default;

    constexpr explicit Fruit(const std::tuple<Taste, Size, Quality> &data)
        : taste_(std::get<0>(data)), size_(std::get<1>(data)),
          quality_(std::get<2>(data)) {}

    constexpr explicit operator std::tuple<Taste, Size, Quality>() const {
        return std::make_tuple(taste_, size_, quality_);
    }

    Fruit &operator=(const Fruit &other) = default;
    Fruit &operator=(Fruit &&other) noexcept = default;

    ~Fruit() = default;

    constexpr Taste taste() const { return taste_; }

    constexpr Size size() const { return size_; }

    constexpr Quality quality() const { return quality_; }

    void go_rotten() {
        if (quality_ == Quality::HEALTHY) {
            quality_ = Quality::ROTTEN;
        }
    }

    void become_worm_infested() {
        if (quality_ == Quality::HEALTHY) {
            quality_ = Quality::WORMY;
        }
    }

    constexpr bool operator==(const Fruit &other) const {
        return taste_ == other.taste_ && size_ == other.size_ &&
               quality_ == other.quality_;
    }
};

inline std::string to_string(const Taste taste) {
    switch (taste) {
        case Taste::SWEET:
            return "słodki";
        case Taste::SOUR:
            return "kwaśny";
    }
    std::unreachable();
}

inline std::string to_string(const Size size) {
    switch (size) {
        case Size::LARGE:
            return "duży";
        case Size::MEDIUM:
            return "średni";
        case Size::SMALL:
            return "mały";
    }
    std::unreachable();
}

inline std::string to_string(const Quality quality) {
    switch (quality) {
        case Quality::HEALTHY:
            return "zdrowy";
        case Quality::ROTTEN:
            return "nadgniły";
        case Quality::WORMY:
            return "robaczywy";
    }
    std::unreachable();
}

inline std::ostream &operator<<(std::ostream &os, const Fruit &fruit) {
    os << "[" << to_string(fruit.taste()) << " " << to_string(fruit.size())
       << " " << to_string(fruit.quality()) << "]";
    return os;
}

inline constexpr Fruit YUMMY_ONE{Taste::SWEET, Size::LARGE, Quality::HEALTHY};
inline constexpr Fruit ROTTY_ONE{Taste::SOUR, Size::SMALL, Quality::ROTTEN};

class Picker {
  private:
    std::string name_;
    std::deque<Fruit> fruits_;

    // A list of indices of sweet and healthy fruits in the fruits_,
    // helpful for spreading worm infestation.
    std::deque<std::size_t> sweet_and_healthy_fruits;
    // The offset of all indices in sweet_and_healthy_fruits,
    // needed because indices of fruits_ change when
    // fruits are removed from the front.
    std::size_t sweet_and_healthy_offset = 0;

    // Counts of fruits in the picker for answering
    // count_fruits(), count_taste(), count_size(), count_quality()
    // in constant time.
    struct Counts {
        std::size_t all;
        std::size_t taste[2];
        std::size_t quality[3];
        std::size_t size[3];

        Counts() : all(0), taste{0, 0}, quality{0, 0, 0}, size{0, 0, 0} {}

        void add(const Fruit &fruit) {
            all++;
            taste[static_cast<int>(fruit.taste())]++;
            quality[static_cast<int>(fruit.quality())]++;
            size[static_cast<int>(fruit.size())]++;
        }

        void erase(const Fruit &fruit) {
            all--;
            taste[static_cast<int>(fruit.taste())]--;
            quality[static_cast<int>(fruit.quality())]--;
            size[static_cast<int>(fruit.size())]--;
        }
    } counts_;

    void become_worm_infested_and_update_counts(Fruit &fruit) {
        counts_.erase(fruit);
        fruit.become_worm_infested();
        counts_.add(fruit);
    }

    void go_rotten_and_update_counts(Fruit &fruit) {
        counts_.erase(fruit);
        fruit.go_rotten();
        counts_.add(fruit);
    }

    // It is possible that some fruits that were previously sweet and healthy
    // are no longer so, because a rotten fruit was picked later.
    // This is not a problem, because become_worm_infested() method checks the
    // current quality.
    void spread_worm_infestation() {
        for (auto index_with_offset : sweet_and_healthy_fruits) {
            if (index_with_offset < sweet_and_healthy_offset) {
                continue;
            }
            const std::size_t index =
                index_with_offset - sweet_and_healthy_offset;
            if (index < fruits_.size()) {
                become_worm_infested_and_update_counts(fruits_[index]);
            }
        }
        sweet_and_healthy_fruits.clear();
    }

    // Handles the reactions of other fruits when a new fruit is added.
    void handle_neighbor_reactions(Fruit &new_fruit) {
        if (fruits_.size() <= 1) {
            return;
        }

        Fruit &previous_fruit = fruits_[fruits_.size() - 2];
        if (new_fruit.quality() == Quality::ROTTEN) {
            go_rotten_and_update_counts(previous_fruit);
        }
        else if (new_fruit.quality() == Quality::WORMY) {
            spread_worm_infestation();
        }

        if (previous_fruit.quality() == Quality::ROTTEN) {
            go_rotten_and_update_counts(new_fruit);
        }
    }

    void remember_sweet_and_healthy_if_needed(std::size_t index) {
        const Fruit &fruit = fruits_[index];
        if (fruit.taste() == Taste::SWEET &&
            fruit.quality() == Quality::HEALTHY) {
            sweet_and_healthy_fruits.push_back(index +
                                               sweet_and_healthy_offset);
        }
    }

  public:
    Picker() : name_{"Anonim"}, counts_{} {}
    Picker(const Picker &other) = default;
    Picker(Picker &&other) noexcept = default;

    explicit Picker(std::string_view name) : name_{name}, counts_{} {
        if (name.empty()) {
            name_ = "Anonim";
        }
    }

    Picker &operator=(const Picker &other) = default;
    Picker &operator=(Picker &&other) noexcept = default;
    ~Picker() = default;

    const std::string &get_name() const { return name_; }

    bool operator==(const Picker &other) const {
        return name_ == other.name_ && fruits_ == other.fruits_;
    };

    std::weak_ordering operator<=>(const Picker &other) const {
        auto key = std::make_tuple(
            counts_.quality[static_cast<int>(Quality::HEALTHY)],
            counts_.taste[static_cast<int>(Taste::SWEET)],
            counts_.size[static_cast<int>(Size::LARGE)],
            counts_.size[static_cast<int>(Size::MEDIUM)],
            counts_.size[static_cast<int>(Size::SMALL)], counts_.all);

        auto otherKey = std::make_tuple(
            other.counts_.quality[static_cast<int>(Quality::HEALTHY)],
            other.counts_.taste[static_cast<int>(Taste::SWEET)],
            other.counts_.size[static_cast<int>(Size::LARGE)],
            other.counts_.size[static_cast<int>(Size::MEDIUM)],
            other.counts_.size[static_cast<int>(Size::SMALL)],
            other.counts_.all);

        // Inverse order - the better you are, the lower your index in rankings.
        return otherKey <=> key;
    }

    Picker &operator+=(const Fruit &fruit) {
        fruits_.push_back(fruit);
        Fruit &new_fruit = fruits_.back();
        counts_.add(new_fruit);
        handle_neighbor_reactions(new_fruit);
        remember_sweet_and_healthy_if_needed(fruits_.size() - 1);
        return *this;
    }

    Picker &operator+=(Fruit &&fruit) {
        fruits_.push_back(std::move(fruit));
        Fruit &new_fruit = fruits_.back();
        counts_.add(new_fruit);
        handle_neighbor_reactions(new_fruit);
        remember_sweet_and_healthy_if_needed(fruits_.size() - 1);
        return *this;
    }

    Picker &operator+=(Picker &other) {
        if (&other != this && !other.fruits_.empty()) {
            Fruit &other_front = other.fruits_.front();
            other.counts_.erase(other_front);
            *this += std::move(other_front);

            if (!other.sweet_and_healthy_fruits.empty() &&
                other.sweet_and_healthy_fruits.front() ==
                    other.sweet_and_healthy_offset) {
                other.sweet_and_healthy_fruits.pop_front();
            }

            other.fruits_.pop_front();
            other.sweet_and_healthy_offset++;
        }
        return *this;
    }

    Picker &operator-=(Picker &other) {
        if (&other != this && !fruits_.empty()) {
            Fruit &front_fruit = fruits_.front();

            if (!sweet_and_healthy_fruits.empty() &&
                sweet_and_healthy_fruits.front() == sweet_and_healthy_offset) {
                sweet_and_healthy_fruits.pop_front();
            }
            counts_.erase(front_fruit);
            other += std::move(front_fruit);
            fruits_.pop_front();
            sweet_and_healthy_offset++;
        }
        return *this;
    }

    std::size_t count_fruits() const { return fruits_.size(); }

    std::size_t count_taste(Taste taste) const {
        return counts_.taste[static_cast<int>(taste)];
    }

    std::size_t count_size(Size size) const {
        return counts_.size[static_cast<int>(size)];
    }

    std::size_t count_quality(Quality quality) const {
        return counts_.quality[static_cast<int>(quality)];
    }

    friend std::ostream &operator<<(std::ostream &os, const Picker &picker) {
        os << picker.name_ << ":";
        for (const auto &fruit : picker.fruits_) {
            os << "\n\t" << fruit;
        }
        return os;
    }
};

class Ranking {
  private:
    // RankedPicker represents a picker along the order
    // of its insertion into the ranking.
    struct RankedPicker {
        Picker picker;
        size_t ins_order;

        RankedPicker(const Picker &p, size_t order)
            : picker(p), ins_order(order) {}

        RankedPicker(Picker &&p, size_t order)
            : picker(std::move(p)), ins_order(order) {}

        RankedPicker(const RankedPicker &other)
            : picker(other.picker), ins_order(other.ins_order) {}

        RankedPicker(RankedPicker &&other) noexcept
            : picker(std::move(other.picker)), ins_order(other.ins_order) {}

        RankedPicker &operator=(const RankedPicker &other) {
            if (this == &other)
                return *this;

            picker = other.picker;
            ins_order = other.ins_order;
            return *this;
        }
        RankedPicker &operator=(RankedPicker &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            picker = std::move(other.picker);
            ins_order = other.ins_order;
            return *this;
        }
    };

    std::vector<RankedPicker> pickers_;

    // Stably sorts ranking during initialization - firstly by the picker,
    // secondly by the insertion order.
    void sort_ranking() {
        std::stable_sort(pickers_.begin(), pickers_.end(),
                         [](const RankedPicker &a, const RankedPicker &b) {
                             std::weak_ordering cmp = (a.picker <=> b.picker);
                             if (cmp == std::weak_ordering::equivalent) {
                                 return a.ins_order < b.ins_order;
                             }
                             return cmp < 0;
                         });
    }

    // Merges two rankings keeping it stably sorted.
    std::vector<RankedPicker>
    merge_sorted_pickers(const std::vector<RankedPicker> &lhs,
                         const std::vector<RankedPicker> &rhs) const {
        std::vector<RankedPicker> merged;
        merged.reserve(lhs.size() + rhs.size());

        size_t lhs_it = 0;
        size_t rhs_it = 0;

        while (lhs_it < lhs.size() && rhs_it < rhs.size()) {
            std::weak_ordering cmp = lhs[lhs_it].picker <=> rhs[rhs_it].picker;

            // If pickers are equivalent - prioritize the lhs - such strategy
            // allows to maintain the original order from each ranking.
            if (cmp == std::weak_ordering::less ||
                cmp == std::weak_ordering::equivalent) {
                merged.push_back(lhs[lhs_it++]);
            }
            else {
                merged.push_back(rhs[rhs_it++]);
            }
        }

        // Adds remaining elements from the single non-empty ranking in order.
        while (lhs_it < lhs.size()) {
            merged.push_back(lhs[lhs_it++]);
        }
        while (rhs_it < rhs.size()) {
            merged.push_back(rhs[rhs_it++]);
        }

        return merged;
    }

    std::vector<RankedPicker>
    merge_sorted_pickers(std::vector<RankedPicker> &&lhs,
                         std::vector<RankedPicker> &&rhs) const {
        std::vector<RankedPicker> merged;
        merged.reserve(lhs.size() + rhs.size());

        size_t lhs_it = 0;
        size_t rhs_it = 0;

        while (lhs_it < lhs.size() && rhs_it < rhs.size()) {
            std::weak_ordering cmp = lhs[lhs_it].picker <=> rhs[rhs_it].picker;

            if (cmp == std::weak_ordering::less ||
                cmp == std::weak_ordering::equivalent) {
                merged.push_back(std::move(lhs[lhs_it++]));
            }
            else {
                merged.push_back(std::move(rhs[rhs_it++]));
            }
        }

        while (lhs_it < lhs.size()) {
            merged.push_back(std::move(lhs[lhs_it++]));
        }
        while (rhs_it < rhs.size()) {
            merged.push_back(std::move(rhs[rhs_it++]));
        }

        return merged;
    }

  public:
    Ranking() = default;

    explicit Ranking(std::initializer_list<Picker> pickers_list) {
        size_t i = 0;
        for (const Picker &p : pickers_list) {
            pickers_.push_back({Picker(p), i});
            ++i;
        }
        sort_ranking();
    }

    Ranking(const Ranking &other) = default;
    Ranking(Ranking &&other) noexcept = default;

    Ranking &operator=(const Ranking &other) = default;
    Ranking &operator=(Ranking &&other) noexcept = default;

    ~Ranking() = default;

    Ranking &operator+=(const Picker &picker) {
        // Setting the insertion order number for the picker as ranking's size
        // ensures ins_order is greater than ins_order of all other pickers.
        size_t num = pickers_.size();

        // Inserts the picker right before a strictly greater picker - if there
        //  are any equivalent ones, picker should be the last one of them.
        for (size_t i = 0; i < num; ++i) {
            std::weak_ordering cmp = pickers_[i].picker <=> picker;
            if (cmp == std::weak_ordering::greater) {
                pickers_.insert(pickers_.begin() + i, {picker, num});
                return *this;
            }
        }

        // If all pickers were equivalent or smaller than picker, it should
        // be the last one on the ranking.
        pickers_.push_back({picker, num});
        return *this;
    }

    Ranking &operator-=(const Picker &picker) {
        // Finds a highest picker on the ranking equal to the picker
        // and deletes it from the ranking.
        for (size_t i = 0; i < pickers_.size(); ++i) {
            if (pickers_[i].picker == picker) {
                pickers_.erase(pickers_.begin() + i);
                return *this;
            }
        }

        return *this;
    }

    Ranking &operator+=(const Ranking &other) {
        pickers_ = merge_sorted_pickers(pickers_, other.pickers_);
        return *this;
    }

    Ranking &operator+=(Ranking &&other) {
        if (this == &other) {
            return *this;
        }

        pickers_ = merge_sorted_pickers(std::move(pickers_),
                                        std::move(other.pickers_));
        return *this;
    }

    const Ranking operator+(const Ranking &rhs) const {
        Ranking lhs_copy = *this;
        lhs_copy += rhs;
        return lhs_copy;
    }

    const Picker &operator[](std::size_t index) const {
        if (pickers_.empty()) {
            throw std::out_of_range("Ranking is empty");
        }

        if (index >= pickers_.size()) {
            return pickers_.back().picker;
        }
        return pickers_[index].picker;
    }

    std::size_t count_pickers() const { return pickers_.size(); }

    friend inline std::ostream &operator<<(std::ostream &os,
                                           const Ranking &ranking) {
        for (auto &pick : ranking.pickers_) {
            os << pick.picker << "\n";
        }
        return os;
    }
};

#endif // FRUIT_PICKING_H