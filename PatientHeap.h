// PatientHeap.h - our own binary min-heap (the core structure we write ourselves).
//
// The heap keeps the waiting patients so that the MOST URGENT one is always at index 0.
// "More urgent" means: lower severity first; if equal, earlier arrival; if still equal, lower id.
//
// It also keeps a position index (id -> place in the array), so we can change or remove
// a patient in O(log n) without scanning the whole heap.
//
// Memory: the entries live in std::vector and std::unordered_map, which free their own
// memory when the heap is destroyed, so there is no manual new/delete here.
#ifndef PATIENTHEAP_H
#define PATIENTHEAP_H

#include <cstddef>
#include <unordered_map>
#include <vector>

// What the heap stores for each waiting patient. The full record lives in the hash table.
struct HeapEntry {
    int severity;   // 1 = most urgent ... 5 = least urgent
    int arrival;    // arrival number
    int id;         // patient id (the key)
};

class PatientHeap {
public:
    bool empty() const { return data_.empty(); }
    std::size_t size() const { return data_.size(); }
    bool contains(int id) const { return pos_.count(id) == 1; }

    // Adds an entry. O(log n). Returns false if this id is already in the heap.
    bool push(const HeapEntry& e) {
        if (contains(e.id)) return false;
        data_.push_back(e);
        pos_[e.id] = data_.size() - 1;
        siftUp(data_.size() - 1);
        return true;
    }

    // Looks at the most urgent entry without removing it. O(1). Returns false if empty.
    bool top(HeapEntry& out) const {
        if (data_.empty()) return false;
        out = data_[0];
        return true;
    }

    // Removes the most urgent entry and gives it back in 'out'. O(log n). False if empty.
    bool pop(HeapEntry& out) {
        if (data_.empty()) return false;
        out = data_[0];
        removeAt(0);
        return true;
    }

    // Changes the severity of one patient and puts the heap back in order. O(log n).
    bool updateSeverity(int id, int newSeverity) {
        auto it = pos_.find(id);
        if (it == pos_.end()) return false;
        std::size_t i = it->second;
        data_[i].severity = newSeverity;
        siftUp(i);                 // it may need to go up ...
        siftDown(pos_[id]);        // ... or down (its place may have changed)
        return true;
    }

    // Removes one patient by id. O(log n). False if the id is not in the heap.
    bool remove(int id) {
        auto it = pos_.find(id);
        if (it == pos_.end()) return false;
        removeAt(it->second);
        return true;
    }

    // Used by the tests: is the heap rule true everywhere, and is the position index right?
    bool isValidHeap() const {
        if (pos_.size() != data_.size()) return false;
        for (std::size_t i = 0; i < data_.size(); ++i) {
            auto it = pos_.find(data_[i].id);
            if (it == pos_.end() || it->second != i) return false;
            if (i > 0 && moreUrgent(data_[i], data_[(i - 1) / 2])) return false;  // child beats parent
        }
        return true;
    }

private:
    std::vector<HeapEntry> data_;                 // the tree stored in an array
    std::unordered_map<int, std::size_t> pos_;    // id -> index in data_

    // The ordering rule. A total order, so two different patients are never "equal".
    static bool moreUrgent(const HeapEntry& a, const HeapEntry& b) {
        if (a.severity != b.severity) return a.severity < b.severity;
        if (a.arrival != b.arrival) return a.arrival < b.arrival;
        return a.id < b.id;
    }

    void swapAt(std::size_t i, std::size_t j) {
        HeapEntry tmp = data_[i];
        data_[i] = data_[j];
        data_[j] = tmp;
        pos_[data_[i].id] = i;
        pos_[data_[j].id] = j;
    }

    // Move the entry at i UP while it is more urgent than its parent.
    void siftUp(std::size_t i) {
        while (i > 0) {
            std::size_t parent = (i - 1) / 2;
            if (!moreUrgent(data_[i], data_[parent])) break;
            swapAt(i, parent);
            i = parent;
        }
    }

    // Move the entry at i DOWN while one of its children is more urgent.
    void siftDown(std::size_t i) {
        std::size_t n = data_.size();
        for (;;) {
            std::size_t left = 2 * i + 1, right = 2 * i + 2, best = i;
            if (left < n && moreUrgent(data_[left], data_[best])) best = left;
            if (right < n && moreUrgent(data_[right], data_[best])) best = right;
            if (best == i) break;
            swapAt(i, best);
            i = best;
        }
    }

    // Removes the entry at index i: put the LAST entry in its place, then repair.
    void removeAt(std::size_t i) {
        std::size_t last = data_.size() - 1;
        pos_.erase(data_[i].id);
        if (i == last) {                 // removing the last one: nothing to repair
            data_.pop_back();
            return;
        }
        data_[i] = data_[last];
        data_.pop_back();
        int movedId = data_[i].id;
        pos_[movedId] = i;
        siftUp(i);
        siftDown(pos_[movedId]);
    }
};

#endif
