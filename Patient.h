// Patient.h - the patient record, and loading / saving the text file (requirement 1).
#ifndef PATIENT_H   //HEADER FILE
#define PATIENT_H   //HEADER FILE

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// One patient. The id is the key we use to find a patient.
struct Patient {
    int id = 0;             // unique key
    std::string name;
    int age = 0;            // 0 to 120
    int severity = 5;       // 1 = most urgent ... 5 = least urgent
    int arrival = 0;        // arrival number: 1 for the first patient, 2 for the next ...
    std::string complaint;
};

// Turns text like "42" into a number. Returns false if it is not a whole number.
inline bool toInt(const std::string& text, int& out) {
    try {
        std::size_t used = 0;
        int value = std::stoi(text, &used);
        if (used != text.size()) return false;   // something extra, like "12abc"
        out = value;
        return true;
    } catch (...) {
        return false;                            // not a number at all, or too big
    }
}

// Checks one patient. Used when loading, and later when the user adds a patient.
inline bool isValidPatient(const Patient& p) {
    if (p.id <= 0) return false;
    if (p.name.empty()) return false;
    if (p.age < 0 || p.age > 120) return false;
    if (p.severity < 1 || p.severity > 5) return false;
    if (p.arrival <= 0) return false;
    if (p.complaint.empty()) return false;
    return true;
}

// Reads one line "id,name,age,severity,arrival,complaint" into a Patient.
// Returns false if the line is not in that shape.
inline bool parsePatientLine(const std::string& line, Patient& p) {
    std::stringstream ss(line);
    std::string idText, name, ageText, sevText, arrText, complaint;
    if (!std::getline(ss, idText, ',')) return false;
    if (!std::getline(ss, name, ',')) return false;
    if (!std::getline(ss, ageText, ',')) return false;
    if (!std::getline(ss, sevText, ',')) return false;
    if (!std::getline(ss, arrText, ',')) return false;
    if (!std::getline(ss, complaint)) return false;   // the rest of the line

    Patient temp;
    if (!toInt(idText, temp.id)) return false;
    if (!toInt(ageText, temp.age)) return false;
    if (!toInt(sevText, temp.severity)) return false;
    if (!toInt(arrText, temp.arrival)) return false;
    temp.name = name;
    temp.complaint = complaint;

    if (!isValidPatient(temp)) return false;
    p = temp;
    return true;
}

// Loads patients from the file into 'out'.
// Returns how many patients were loaded, or -1 if the file cannot be opened.
// Bad lines are skipped, and counted in *skipped if you pass it.
inline int loadPatients(const std::string& path, std::vector<Patient>& out, int* skipped = nullptr) {
    std::ifstream in(path);
    if (!in) return -1;

    int loaded = 0, bad = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;   // blank line or comment
        Patient p;
        if (parsePatientLine(line, p)) {
            out.push_back(p);
            ++loaded;
        } else {
            ++bad;
        }
    }
    if (skipped) *skipped = bad;
    return loaded;
}

// Saves all patients to the file, one per line. Returns false if the file cannot be written.
inline bool savePatients(const std::string& path, const std::vector<Patient>& list) {
    std::ofstream out(path);
    if (!out) return false;
    out << "# id,name,age,severity,arrival,complaint\n";
    for (const Patient& p : list) {
        out << p.id << ',' << p.name << ',' << p.age << ','
            << p.severity << ',' << p.arrival << ',' << p.complaint << '\n';
    }
    return static_cast<bool>(out);
}

#endif
