// tests.cpp - test table: ID, operation, expected, actual, PASS/FAIL (tab separated).
// Run it from the project folder, so that patients.txt can be found.
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "Clinic.h"

static int passed = 0, failed = 0;

static void check(const std::string& id, const std::string& what,
                  const std::string& expected, const std::string& actual) {
    bool ok = (expected == actual);
    std::cout << id << '\t' << what << '\t' << expected << '\t' << actual << '\t'
              << (ok ? "PASS" : "FAIL") << '\n';
    ok ? ++passed : ++failed;
}

static std::string yn(bool b) { return b ? "true" : "false"; }
static HeapEntry E(int id, int sev, int arr) { return HeapEntry{sev, arr, id}; }

static Patient P(int id, int sev, int arr) {
    Patient p;
    p.id = id; p.name = "Test Patient"; p.age = 20;
    p.severity = sev; p.arrival = arr; p.complaint = "cough";
    return p;
}

// Pops everything from a COPY of the heap and returns the ids in the order they came out.
static std::string popOrder(PatientHeap h) {
    std::string s;
    HeapEntry e;
    while (h.pop(e)) s += (s.empty() ? "" : " ") + std::to_string(e.id);
    return s;
}

static std::string idsOf(const std::vector<Patient>& v) {
    std::string s;
    for (const Patient& p : v) s += (s.empty() ? "" : " ") + std::to_string(p.id);
    return s;
}

int main() {
    std::cout << "ID\tOperation\tExpected\tActual\tResult\n";

    // ---------------- Heap ----------------
    {
        PatientHeap h; HeapEntry e;
        check("H-01", "new heap is empty", "true", yn(h.empty()));
        check("H-02", "top of an empty heap", "false", yn(h.top(e)));
        check("H-03", "pop from an empty heap", "false", yn(h.pop(e)));
    }
    {
        PatientHeap h; HeapEntry e;
        h.push(E(10, 3, 1));
        check("H-04", "push one entry: size and top id", "1 / 10",
              std::to_string(h.size()) + " / " + std::to_string(h.top(e) ? e.id : -1));
        h.pop(e);
        check("H-05", "pop the only entry: heap is empty again", "true", yn(h.empty()));
    }
    {
        PatientHeap h;
        h.push(E(10, 3, 1)); h.push(E(11, 1, 2)); h.push(E(12, 2, 3));
        check("H-06", "pop order for severities 3, 1, 2", "11 12 10", popOrder(h));
    }
    {
        PatientHeap h;
        h.push(E(20, 2, 5)); h.push(E(21, 2, 3)); h.push(E(22, 2, 4));
        check("H-07", "equal severity: earlier arrival first", "21 22 20", popOrder(h));
    }
    {
        PatientHeap h;
        bool first = h.push(E(30, 2, 1));
        bool second = h.push(E(30, 1, 2));
        check("H-08", "duplicate id is rejected", "true / false / 1",
              yn(first) + " / " + yn(second) + " / " + std::to_string(h.size()));
    }
    {
        PatientHeap h;
        for (int i = 1; i <= 5; ++i) h.push(E(100 + i, i, i));      // already in order
        check("H-09", "input already sorted (severity 1..5)", "101 102 103 104 105", popOrder(h));
    }
    {
        PatientHeap h;
        for (int i = 5; i >= 1; --i) h.push(E(100 + i, i, 10 - i)); // reversed order
        check("H-10", "input reversed (severity 5..1)", "101 102 103 104 105", popOrder(h));
    }
    {
        PatientHeap h;
        h.push(E(1, 3, 1)); h.push(E(2, 3, 2)); h.push(E(3, 3, 3));
        h.updateSeverity(3, 1);
        check("H-11", "raise urgency: patient 3 to severity 1 comes first", "3 1 2", popOrder(h));
    }
    {
        PatientHeap h;
        h.push(E(1, 1, 1)); h.push(E(2, 2, 2)); h.push(E(3, 3, 3));
        h.updateSeverity(1, 5);
        check("H-12", "lower urgency: top patient moved to severity 5", "2 3 1", popOrder(h));
    }
    {
        PatientHeap h;
        h.push(E(1, 1, 1));
        check("H-13", "update severity of an absent id", "false", yn(h.updateSeverity(99, 2)));
    }
    {
        PatientHeap h;
        h.push(E(1, 1, 1)); h.push(E(2, 2, 2)); h.push(E(3, 3, 3));
        h.remove(1);
        check("H-14", "remove the top patient", "2 3", popOrder(h));
    }
    {
        PatientHeap h;
        h.push(E(1, 1, 1)); h.push(E(2, 2, 2)); h.push(E(3, 3, 3)); h.push(E(4, 4, 4));
        h.remove(2);
        bool ok = h.isValidHeap();
        check("H-15", "remove a middle patient, heap rule still true", "1 3 4 / true",
              popOrder(h) + " / " + yn(ok));
    }
    {
        PatientHeap h;
        h.push(E(1, 1, 1)); h.push(E(2, 2, 2)); h.push(E(3, 3, 3));
        h.remove(3);
        check("H-16", "remove the last patient", "1 2", popOrder(h));
    }
    {
        PatientHeap h;
        h.push(E(1, 1, 1));
        check("H-17", "remove an absent id", "false / 1",
              yn(h.remove(42)) + " / " + std::to_string(h.size()));
    }
    {   // random test, compared with std::sort
        std::mt19937 rng(12345);
        PatientHeap h;
        std::vector<HeapEntry> ref;
        for (int i = 1; i <= 500; ++i) {
            HeapEntry e = E(i, static_cast<int>(rng() % 5) + 1, i);
            h.push(e); ref.push_back(e);
        }
        bool v1 = h.isValidHeap();
        for (int k = 0; k < 100; ++k) {
            int id = static_cast<int>(rng() % 500) + 1;
            if (h.remove(id))
                ref.erase(std::remove_if(ref.begin(), ref.end(),
                          [id](const HeapEntry& x) { return x.id == id; }), ref.end());
        }
        for (int k = 0; k < 100; ++k) {
            int id = static_cast<int>(rng() % 500) + 1;
            int sev = static_cast<int>(rng() % 5) + 1;
            if (h.updateSeverity(id, sev))
                for (HeapEntry& x : ref) if (x.id == id) x.severity = sev;
        }
        bool v2 = h.isValidHeap();
        std::sort(ref.begin(), ref.end(), [](const HeapEntry& a, const HeapEntry& b) {
            if (a.severity != b.severity) return a.severity < b.severity;
            if (a.arrival != b.arrival) return a.arrival < b.arrival;
            return a.id < b.id;
        });
        std::string expected;
        for (const HeapEntry& x : ref) expected += (expected.empty() ? "" : " ") + std::to_string(x.id);
        check("H-18", "500 random pushes: heap rule true", "true", yn(v1));
        check("H-19", "after 100 removals and 100 severity changes: heap rule true", "true", yn(v2));
        check("H-20", "pop order equals std::sort order", "true", yn(popOrder(h) == expected));
    }

    // ---------------- Clinic (hash table + heap together) ----------------
    {
        Clinic c; Patient out;
        bool a1 = c.add(P(1, 3, 1));
        check("C-01", "add a valid patient", "true / 1", yn(a1) + " / " + std::to_string(c.size()));
        bool a2 = c.add(P(1, 2, 2));
        check("C-02", "add the same id again", "false / 1", yn(a2) + " / " + std::to_string(c.size()));
        bool a3 = c.add(P(2, 0, 3));
        bool a4 = c.add(P(3, 6, 4));
        check("C-03", "severity 0 and severity 6 are rejected", "false / false", yn(a3) + " / " + yn(a4));
        Patient old = P(4, 3, 5); old.age = 121;
        Patient noName = P(5, 3, 6); noName.name = "";
        bool a5 = c.add(old);
        bool a6 = c.add(noName);
        check("C-04", "age 121 and an empty name are rejected", "false / false", yn(a5) + " / " + yn(a6));
        const Patient* f = c.find(1);
        check("C-05", "find an existing id, and an absent id", "Test Patient / null",
              std::string(f ? f->name : "null") + " / " + (c.find(999) ? "found" : "null"));
        Clinic empty;
        check("C-06", "call next on an empty clinic", "false", yn(empty.callNext(out)));
        check("C-07", "peek on an empty clinic", "false", yn(empty.peekNext(out)));
    }
    {
        Clinic c; Patient out;
        c.add(P(1, 3, 1)); c.add(P(2, 1, 2)); c.add(P(3, 2, 3));
        bool called = c.callNext(out);
        check("C-08", "call next gives the most urgent and removes it", "true / 2 / 2 / null",
              yn(called) + " / " + std::to_string(out.id) + " / " + std::to_string(c.size()) + " / " +
              (c.find(2) ? "found" : "null"));
        c.peekNext(out);
        check("C-09", "peek does not remove", "3 / 2", std::to_string(out.id) + " / " + std::to_string(c.size()));
    }
    {
        Clinic c; Patient out;
        c.add(P(1, 3, 1)); c.add(P(2, 3, 2));
        bool ok = c.changeSeverity(2, 1);
        c.peekNext(out);
        check("C-10", "change severity: patient 2 becomes next", "true / 2 / 1",
              yn(ok) + " / " + std::to_string(out.id) + " / " + std::to_string(c.find(2)->severity));
        bool bad1 = c.changeSeverity(1, 7);
        bool bad2 = c.changeSeverity(99, 2);
        check("C-11", "invalid severity 7, and an absent id (severity stays 3)", "false / false / 3",
              yn(bad1) + " / " + yn(bad2) + " / " + std::to_string(c.find(1)->severity));
    }
    {
        Clinic c; Patient out;
        c.add(P(1, 1, 1)); c.add(P(2, 2, 2)); c.add(P(3, 3, 3));
        bool r1 = c.remove(1);
        bool r2 = c.remove(99);
        c.peekNext(out);
        check("C-12", "remove the top patient, an absent id, then who is next", "true / false / 2 / true",
              yn(r1) + " / " + yn(r2) + " / " + std::to_string(out.id) + " / " + yn(c.isConsistent()));
    }
    {
        Clinic c;
        c.add(P(1, 3, 1)); c.add(P(2, 1, 2)); c.add(P(3, 3, 3)); c.add(P(4, 2, 4)); c.add(P(5, 1, 5));
        std::string list = idsOf(c.waitingList());
        check("C-13", "waiting list is most urgent first", "2 5 4 1 3", list);
        check("C-14", "report does not change the queue", "5 / true", std::to_string(c.size()) + " / " + yn(c.isConsistent()));
    }
    {
        Clinic c; Patient out;
        c.add(P(1, 3, 12));
        check("C-15", "next arrival number after arrival 12", "13", std::to_string(c.nextArrival()));
        c.add(P(2, 2, 13)); c.add(P(3, 1, 14)); c.callNext(out); c.changeSeverity(1, 1); c.remove(2);
        check("C-16", "hash table and heap agree after mixed operations", "true / 1",
              yn(c.isConsistent()) + " / " + std::to_string(c.size()));
    }

    // ---------------- File load and save ----------------
    {
        std::vector<Patient> list; int skipped = 0;
        int n = loadPatients("patients.txt", list, &skipped);
        check("F-01", "load patients.txt", "12 / 0", std::to_string(n) + " / " + std::to_string(skipped));
        bool saved = savePatients("tmp_saved.txt", list);
        std::vector<Patient> again;
        int n2 = loadPatients("tmp_saved.txt", again);
        bool same = (again.size() == list.size());
        for (std::size_t i = 0; same && i < list.size(); ++i)
            same = list[i].id == again[i].id && list[i].name == again[i].name && list[i].age == again[i].age &&
                   list[i].severity == again[i].severity && list[i].arrival == again[i].arrival &&
                   list[i].complaint == again[i].complaint;
        check("F-02", "save, then load again: same data", "true / 12 / true", yn(saved) + " / " + std::to_string(n2) + " / " + yn(same));
        std::vector<Patient> none;
        check("F-03", "load a file that does not exist", "-1", std::to_string(loadPatients("no_such_file.txt", none)));
        check("F-04", "save to a folder that does not exist", "false", yn(savePatients("no_such_folder/out.txt", list)));

        Clinic c; int added = 0;
        for (const Patient& p : list) if (c.add(p)) ++added;
        Patient out; c.callNext(out);
        check("F-05", "load into the clinic: 12 added, first called is 2005", "12 / 2005 / true",
              std::to_string(added) + " / " + std::to_string(out.id) + " / " + yn(c.isConsistent()));
    }
    {
        std::ofstream bad("tmp_bad.txt");
        bad << "# header\n\n";
        bad << "3001,Good Patient,30,3,1,cough\n";    // good
        bad << "abc,Bad Id,30,3,2,cough\n";           // id is not a number
        bad << "3003,Bad Severity,30,9,3,cough\n";    // severity outside 1 to 5
        bad << "3004,Bad Age,200,3,4,cough\n";        // age outside 0 to 120
        bad << "3005,Too Few Fields,30\n";            // missing fields
        bad << "3006,Empty Complaint,30,3,6,\n";      // empty complaint
        bad.close();
        std::vector<Patient> mixed; int badCount = 0;
        int loaded = loadPatients("tmp_bad.txt", mixed, &badCount);
        check("F-06", "file with 5 bad lines: 1 loaded, 5 skipped", "1 / 5", std::to_string(loaded) + " / " + std::to_string(badCount));
    }

    std::cout << "\nSUMMARY\t" << passed << " passed, " << failed << " failed\n";
    std::remove("tmp_saved.txt");
    std::remove("tmp_bad.txt");
    return failed == 0 ? 0 : 1;
}
