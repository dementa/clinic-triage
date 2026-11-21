UNIVERSITY CLINIC TRIAGE SYSTEM  (C++17)
Structures: our own binary min-heap (PatientHeap.h) + hash table (std::unordered_map) joined in Clinic.h

BUILD AND RUN (in this folder)
  make            builds the program (clinic) and the tests
  ./clinic        runs the menu program
  make test       runs the test table (42 tests)
  make asan       runs the same tests with AddressSanitizer (memory check)

FILES
  Patient.h       the Patient record, reading one line, load and save of patients.txt       (requirement 1)
  PatientHeap.h   our own heap: push, pop, top, updateSeverity, remove, position index      (requirement 3)
  Clinic.h        hash table + heap together: add, find, callNext, changeSeverity, remove,
                  waitingList (ordered report), allByArrival (for saving)                    (requirements 2, 4, 5)
  main.cpp        the menu, input checks, load at start, save at exit                        (requirements 6, 7)
  tests.cpp       test table: heap, clinic and file tests
  patients.txt    12 sample patients: id,name,age,severity,arrival,complaint

HOW THE PARTS WORK TOGETHER
  - The hash table maps id -> full patient record. Search, update and delete by id are O(1) on average.
  - The heap keeps the most urgent patient at the top (lower severity first, then earlier arrival).
    Call next is O(log n). The heap also keeps a position index (id -> place in the heap),
    so changing severity or removing a patient is O(log n).
  - The ordered report copies the heap and calls next on the copy: O(n log n).

NOTES
  - Severity: 1 = most urgent ... 5 = least urgent.
  - A patient who is called leaves the waiting list, so the saved file holds only waiting patients.
  - Names and complaints cannot contain commas (the comma separates the fields in the file).
  - Memory: the heap and the hash table use std::vector and std::unordered_map, which free their
    own memory when the program ends, so there is no manual new/delete.
