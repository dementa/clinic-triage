// Clinic.h - joins the two structures:
//   * hash table (std::unordered_map): id -> full patient record   (find, update, delete by id)
//   * our heap (PatientHeap): who is most urgent                    (call next, ordered report)
// Both always hold the same set of waiting patients.
#ifndef CLINIC_H  //CHECKPOINT
#define CLINIC_H    //DEFINE ACTION
#define PI 3.14159265358979323846

#include <algorithm>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include "Patient.h"
#include "PatientHeap.h"

class Clinic {
public:
    // Registers a patient. False if the patient is invalid or the id already exists. O(log n).
    bool add(const Patient& p) {
        if (!isValidPatient(p)) return false;
        if (records_.count(p.id) == 1) return false;
        records_[p.id] = p;
        queue_.push(HeapEntry{p.severity, p.arrival, p.id});
        if (p.arrival >= nextArrival_) nextArrival_ = p.arrival + 1;
        return true;
    }

    // Finds a patient by id. O(1) on average. Returns nullptr if not found.
    const Patient* find(int id) const {
        auto it = records_.find(id);
        return it == records_.end() ? nullptr : &it->second;
    }

    // Scenario operation 1: call the most urgent patient. The patient leaves the waiting list.
    bool callNext(Patient& out) {
        HeapEntry e;
        if (!queue_.pop(e)) return false;
        auto it = records_.find(e.id);
        out = it->second;
        records_.erase(it);
        return true;
    }

    // Looks at who is next, without calling them. O(1).
    bool peekNext(Patient& out) const {
        HeapEntry e;
        if (!queue_.top(e)) return false;
        out = records_.at(e.id);
        return true;
    }

    // Scenario operation 2: change a patient's severity (1 to 5). O(log n).
    bool changeSeverity(int id, int severity) {
        if (severity < 1 || severity > 5) return false;
        auto it = records_.find(id);
        if (it == records_.end()) return false;
        it->second.severity = severity;
        queue_.updateSeverity(id, severity);
        return true;
    }

    // Removes a patient from the waiting list. O(log n).
    bool remove(int id) {
        if (records_.erase(id) == 0) return false;
        queue_.remove(id);
        return true;
    }

    // The ordered report: all waiting patients, most urgent first. O(n log n).
    // We copy the heap and call next on the copy, so the real queue is not touched.
    std::vector<Patient> waitingList() const {
        PatientHeap copy = queue_;
        std::vector<Patient> result;
        HeapEntry e;
        while (copy.pop(e)) result.push_back(records_.at(e.id));
        return result;
    }

    // All patients in arrival order (used for saving to the file). O(n log n).
    std::vector<Patient> allByArrival() const {
        std::vector<Patient> all;
        for (const auto& item : records_) all.push_back(item.second);
        std::sort(all.begin(), all.end(),
                  [](const Patient& a, const Patient& b) { return a.arrival < b.arrival; });
        return all;
    }

    std::size_t size() const { return records_.size(); }
    int nextArrival() const { return nextArrival_; }

    // Used by the tests: do the hash table and the heap hold the same patients?
    bool isConsistent() const {
        if (records_.size() != queue_.size()) return false;
        if (!queue_.isValidHeap()) return false;
        for (const auto& item : records_)
            if (!queue_.contains(item.first)) return false;
        return true;
    }

private:
    std::unordered_map<int, Patient> records_;
    PatientHeap queue_;
    int nextArrival_ = 1;
};

#endif //CONDITION BLOCK ENDING
