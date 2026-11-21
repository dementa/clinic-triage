// main.cpp - the menu program for the University Clinic Triage System.
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "Clinic.h"

static const char* FILE_NAME = "patients.txt";

// ---------- input helpers (validate, and never crash on bad input) ----------

// Reads one line. Returns false if the input has ended (Ctrl+D or end of file).
static bool readLine(const std::string& prompt, std::string& line) {
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, line));
}

// Asks until the user types a whole number from low to high.
static bool readInt(const std::string& prompt, int low, int high, int& out) {
    std::string line;
    for (;;) {
        if (!readLine(prompt, line)) return false;
        if (toInt(line, out) && out >= low && out <= high) return true;
        std::cout << "  Please enter a whole number from " << low << " to " << high << ".\n";
    }
}

// Asks until the user types text that is not empty and has no comma (the comma splits the file).
static bool readText(const std::string& prompt, std::string& out) {
    for (;;) {
        if (!readLine(prompt, out)) return false;
        if (!out.empty() && out.find(',') == std::string::npos) return true;
        std::cout << "  Text cannot be empty and cannot contain a comma.\n";
    }
}

// ---------- printing ----------

static void showMenu() {
    std::cout << "\n====================================\n";
    std::cout << "   UNIVERSITY CLINIC TRIAGE SYSTEM\n";
    std::cout << "====================================\n";
    std::cout << "1. Register patient\n";
    std::cout << "2. Call next patient\n";
    std::cout << "3. Show who is next\n";
    std::cout << "4. Show waiting list (most urgent first)\n";
    std::cout << "5. Search patient by id\n";
    std::cout << "6. Change severity\n";
    std::cout << "7. Remove patient\n";
    std::cout << "8. Save\n";
    std::cout << "0. Save and exit\n";
    std::cout << "------------------------------------\n";
}

static void printPatient(const Patient& p) {
    std::cout << "  Id " << p.id << " | " << p.name << " | age " << p.age
              << " | severity " << p.severity << " | arrival #" << p.arrival
              << " | " << p.complaint << "\n";
}

// ---------- menu actions ----------

static void registerPatient(Clinic& clinic) {
    Patient p;
    if (!readInt("Patient id (1 to 99999999): ", 1, 99999999, p.id)) return;
    if (clinic.find(p.id) != nullptr) {
        std::cout << "  A patient with id " << p.id << " is already waiting.\n";
        return;
    }
    if (!readText("Name: ", p.name)) return;
    if (!readInt("Age (0 to 120): ", 0, 120, p.age)) return;
    if (!readInt("Severity (1 = most urgent ... 5 = least urgent): ", 1, 5, p.severity)) return;
    if (!readText("Complaint: ", p.complaint)) return;
    p.arrival = clinic.nextArrival();
    if (clinic.add(p)) std::cout << "  Registered. Patients waiting: " << clinic.size() << "\n";
    else std::cout << "  Could not register this patient.\n";
}

static void callNext(Clinic& clinic) {
    Patient p;
    if (clinic.callNext(p)) {
        std::cout << "  Now calling:\n";
        printPatient(p);
    } else {
        std::cout << "  No patients are waiting.\n";
    }
}

static void showNext(const Clinic& clinic) {
    Patient p;
    if (clinic.peekNext(p)) {
        std::cout << "  Next patient:\n";
        printPatient(p);
    } else {
        std::cout << "  No patients are waiting.\n";
    }
}

static void showWaitingList(const Clinic& clinic) {
    std::vector<Patient> list = clinic.waitingList();
    if (list.empty()) {
        std::cout << "  No patients are waiting.\n";
        return;
    }
    std::cout << "\n  " << std::left << std::setw(4) << "#" << std::setw(10) << "Id"
              << std::setw(22) << "Name" << std::setw(5) << "Age" << std::setw(10) << "Severity"
              << std::setw(9) << "Arrival" << "Complaint\n";
    std::cout << "  " << std::string(76, '-') << "\n";
    int rank = 1;
    for (const Patient& p : list) {
        std::cout << "  " << std::left << std::setw(4) << rank++ << std::setw(10) << p.id
                  << std::setw(22) << p.name << std::setw(5) << p.age << std::setw(10) << p.severity
                  << std::setw(9) << p.arrival << p.complaint << "\n";
    }
}

static void searchPatient(const Clinic& clinic) {
    int id;
    if (!readInt("Patient id: ", 1, 99999999, id)) return;
    const Patient* p = clinic.find(id);
    if (p) printPatient(*p);
    else std::cout << "  No waiting patient has id " << id << ".\n";
}

static void changeSeverity(Clinic& clinic) {
    int id, severity;
    if (!readInt("Patient id: ", 1, 99999999, id)) return;
    if (clinic.find(id) == nullptr) {
        std::cout << "  No waiting patient has id " << id << ".\n";
        return;
    }
    if (!readInt("New severity (1 to 5): ", 1, 5, severity)) return;
    if (clinic.changeSeverity(id, severity)) std::cout << "  Severity changed.\n";
    else std::cout << "  Could not change the severity.\n";
}

static void removePatient(Clinic& clinic) {
    int id;
    if (!readInt("Patient id: ", 1, 99999999, id)) return;
    if (clinic.remove(id)) std::cout << "  Patient removed.\n";
    else std::cout << "  No waiting patient has id " << id << ".\n";
}

static void save(const Clinic& clinic) {
    if (savePatients(FILE_NAME, clinic.allByArrival()))
        std::cout << "  Saved " << clinic.size() << " patients to " << FILE_NAME << ".\n";
    else
        std::cout << "  Could not save to " << FILE_NAME << ".\n";
}

int main() {
    Clinic clinic;

    // Load the file at the start.
    std::vector<Patient> loaded;
    int skipped = 0;
    int n = loadPatients(FILE_NAME, loaded, &skipped);
    if (n < 0) {
        std::cout << "No " << FILE_NAME << " found. Starting with an empty clinic.\n";
    } else {
        for (const Patient& p : loaded)
            if (!clinic.add(p)) ++skipped;          // for example a repeated id
        std::cout << "Loaded " << clinic.size() << " patients from " << FILE_NAME;
        if (skipped > 0) std::cout << " (" << skipped << " bad lines skipped)";
        std::cout << ".\n";
    }

    for (;;) {
        showMenu();
        int choice;
        if (!readInt("Choose: ", 0, 8, choice)) choice = 0;   // input ended: save and exit
        switch (choice) {
            case 1: registerPatient(clinic); break;
            case 2: callNext(clinic); break;
            case 3: showNext(clinic); break;
            case 4: showWaitingList(clinic); break;
            case 5: searchPatient(clinic); break;
            case 6: changeSeverity(clinic); break;
            case 7: removePatient(clinic); break;
            case 8: save(clinic); break;
            default: break;
        }
        if (choice == 0) break;
    }

    save(clinic);
    std::cout << "Goodbye.\n";
    return 0;   // the Clinic is destroyed here and frees all its memory
}
