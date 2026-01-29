#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <algorithm>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>

namespace cxx {

template <typename T, typename P> class playlist {
  private:
    struct PlaylistNode;
    struct UniqueNode;

    // Main playback sequence (doubly linked list).
    using SeqList = std::list<PlaylistNode>;
    using SeqIter = typename SeqList::iterator;
    using ConstSeqIter = typename SeqList::const_iterator;

    // List of iterators pointing to sequence nodes (enables O(1) access).
    using OccList = std::list<SeqIter>;
    using OccIter = typename OccList::iterator;

    // Dictionary mapping tracks to unique shared nodes.
    using Dictionary = std::map<T, std::shared_ptr<UniqueNode>>;
    using MapIter = typename Dictionary::iterator;
    using ConstMapIter = typename Dictionary::const_iterator;

    struct UniqueNode {
        T track;
        OccList occurrences;
        MapIter
            self_in_dict; // Stores iterator to self in map for O(1) removal.

        UniqueNode(const T &t) : track(t) {}
    };

    struct PlaylistNode {
        P params;
        std::shared_ptr<UniqueNode> unique_node;
        OccIter ref_in_unique; // Iterator to occurrence list for O(1) removal.

        PlaylistNode(const P &p, std::shared_ptr<UniqueNode> u, OccIter ref)
            : params(p), unique_node(std::move(u)), ref_in_unique(ref) {}
    };

    struct SharedData {
        SeqList sequence;
        Dictionary dictionary;
    };

    // Invariant: 'data' is never nullptr (guaranteed by constructors).
    std::shared_ptr<SharedData> data;

    // Creates a deep copy (Copy-on-Write). Translates the target iterator if
    // provided. Ensures Strong Exception Guarantee via strict ordering of
    // operations.
    std::pair<std::shared_ptr<SharedData>, SeqIter>
    duplicate_data(ConstSeqIter target = ConstSeqIter()) const {
        auto newData = std::make_shared<SharedData>();
        SeqIter targetInNew = newData->sequence.end();
        bool lookingForTarget = (target != ConstSeqIter());

        for (auto it = data->sequence.begin(); it != data->sequence.end();
             ++it) {
            auto &track = it->unique_node->track;
            std::shared_ptr<UniqueNode> uNode;
            auto dictIt = newData->dictionary.find(track);

            if (dictIt == newData->dictionary.end()) {
                uNode = std::make_shared<UniqueNode>(track);
                auto [insIt, _] = newData->dictionary.insert({track, uNode});
                uNode->self_in_dict = insIt; // Stores the new map iterator.
            } else {
                uNode = dictIt->second;
            }

            // Prepares a slot in the occurrence list.
            uNode->occurrences.emplace_back();
            auto occRef = std::prev(uNode->occurrences.end());

            // Inserts the node into the main sequence.
            newData->sequence.emplace_back(it->params, uNode, occRef);
            auto newSeqIter = std::prev(newData->sequence.end());

            // Links the occurrence node back to the sequence iterator.
            *occRef = newSeqIter;

            // Translates the iterator if it matches the target.
            if (lookingForTarget && it == target) {
                targetInNew = newSeqIter;
                lookingForTarget = false;
            }
        }
        return {newData, targetInNew};
    }

  public:

    class play_iterator {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<T const &, P const &>;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

      private:
        friend class playlist;
        ConstSeqIter it;

      public:
        play_iterator() = default;
        explicit play_iterator(ConstSeqIter i) : it(i) {}

        play_iterator &operator++() {
            ++it;
            return *this;
        }
        play_iterator operator++(int) {
            auto t = *this;
            ++it;
            return t;
        }
        play_iterator &operator--() {
            --it;
            return *this;
        }
        play_iterator operator--(int) {
            auto t = *this;
            --it;
            return t;
        }
        bool operator==(const play_iterator &o) const {
            return it == o.it;
        }
        bool operator!=(const play_iterator &o) const {
            return it != o.it;
        }
    };

    class sorted_iterator {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<T const &, size_t>;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = void;

      private:
        friend class playlist;
        ConstMapIter it;

      public:
        sorted_iterator() = default;
        explicit sorted_iterator(ConstMapIter i) : it(i) {}

        sorted_iterator &operator++() {
            ++it;
            return *this;
        }
        sorted_iterator operator++(int) {
            auto t = *this;
            ++it;
            return t;
        }
        sorted_iterator &operator--() {
            --it;
            return *this;
        }
        sorted_iterator operator--(int) {
            auto t = *this;
            --it;
            return t;
        }
        bool operator==(const sorted_iterator &o) const {
            return it == o.it;
        }
        bool operator!=(const sorted_iterator &o) const {
            return it != o.it;
        }
    };

    playlist() : data(std::make_shared<SharedData>()) {}

    playlist(playlist const &other) : data(other.data) {}

    playlist(playlist &&other) noexcept : data(std::move(other.data)) {
        // Enforces the invariant: 'other' must remain in a valid state.
        other.data = std::make_shared<SharedData>();
    }

    ~playlist() = default;

    playlist &operator=(playlist other) {
        std::swap(data, other.data);
        return *this;
    }

    void push_back(T const &track, P const &params) {
        if (data.use_count() > 1) {
            // Implements CoW: forks data, then modifies the local copy.
            auto [newData, _] = duplicate_data();

            std::shared_ptr<UniqueNode> uNode;
            auto it = newData->dictionary.find(track);

            if (it == newData->dictionary.end()) {
                uNode = std::make_shared<UniqueNode>(track);
                auto [insIt, _] = newData->dictionary.insert({track, uNode});
                uNode->self_in_dict = insIt;
            } else {
                uNode = it->second;
            }

            uNode->occurrences.emplace_back();
            auto occRef = std::prev(uNode->occurrences.end());

            newData->sequence.emplace_back(params, uNode, occRef);
            *occRef = std::prev(newData->sequence.end());

            data = newData; 
        } else {
            // Modifies in-place with rollback support.
            std::shared_ptr<UniqueNode> uNode;
            auto it = data->dictionary.find(track);
            bool insertedNew = false;

            if (it == data->dictionary.end()) {
                uNode = std::make_shared<UniqueNode>(track);
                auto [insIt, _] = data->dictionary.insert({track, uNode});
                uNode->self_in_dict = insIt;
                insertedNew = true;
            } else {
                uNode = it->second;
            }

            uNode->occurrences.emplace_back();
            auto occRef = std::prev(uNode->occurrences.end());

            try {
                data->sequence.emplace_back(params, uNode, occRef);
                *occRef = std::prev(data->sequence.end());
            } catch (...) {
                uNode->occurrences.pop_back();
                if (insertedNew) {
                    data->dictionary.erase(uNode->self_in_dict);
                }
                throw;
            }
        }
    }

    void pop_front() {
        if (data->sequence.empty()) {
            throw std::out_of_range("pop_front: empty");
        }

        if (data.use_count() > 1) {
            auto [newData, _] = duplicate_data();
            data = newData;
        }

        auto &firstNode = data->sequence.front();
        auto trackPtr = firstNode.unique_node;

        // Removes from occurrences list in O(1).
        trackPtr->occurrences.erase(firstNode.ref_in_unique);

        // Removes from dictionary via stored iterator in O(1).
        if (trackPtr->occurrences.empty()) {
            data->dictionary.erase(trackPtr->self_in_dict);
        }

        data->sequence.pop_front();
    }

    void remove(T const &track) {
        // Checks for existence before CoW.
        if (data->dictionary.find(track) == data->dictionary.end()) {
            throw std::invalid_argument("remove: unknown track");
        }

        if (data.use_count() > 1) {
            auto [newData, _] = duplicate_data();
            data = newData;
        }

        auto uIt = data->dictionary.find(track);
        std::shared_ptr<UniqueNode> uNode = uIt->second;

        // Removes all sequence occurrences (O(k)).
        for (auto seqIt : uNode->occurrences) {
            data->sequence.erase(seqIt);
        }

        // Removes from dictionary (O(1) with iterator).
        data->dictionary.erase(uIt);
    }

    void clear() {
        if (!data->sequence.empty()) {
            if (data.use_count() == 1) {
                data->sequence.clear();
                data->dictionary.clear();
            } else {
                data = std::make_shared<SharedData>();
            }
        }
    }

    size_t size() const {
        return data->sequence.size();
    }

    const std::pair<T const &, P const &> front() const {
        if (data->sequence.empty()) {
            throw std::out_of_range("front: empty");
        }
        auto &node = data->sequence.front();
        return {node.unique_node->track, node.params};
    }

    const std::pair<T const &, P const &> play(play_iterator const &it) const {
        auto &node = *it.it;
        return {node.unique_node->track, node.params};
    }

    const std::pair<T const &, size_t> pay(sorted_iterator const &it) const {
        auto &uNode = *(it.it->second);
        return {uNode.track, uNode.occurrences.size()};
    }

    P &params(play_iterator const &it) {
        if (data.use_count() > 1) {
            // Translates iterator to the new data structure.
            auto [newData, newSeqIter] = duplicate_data(it.it);
            data = newData;

            // Updates the iterator given by the user.
            const_cast<play_iterator&>(it).it = newSeqIter;
            return newSeqIter->params;
        }
        return const_cast<P &>(it.it->params);
    }

    const P &params(play_iterator const &it) const {
        return it.it->params;
    }

    play_iterator play_begin() const {
        return play_iterator(data->sequence.begin());
    }
    play_iterator play_end() const {
        return play_iterator(data->sequence.end());
    }
    sorted_iterator sorted_begin() const {
        return sorted_iterator(data->dictionary.cbegin());
    }
    sorted_iterator sorted_end() const {
        return sorted_iterator(data->dictionary.cend());
    }
};

} // namespace cxx

#endif