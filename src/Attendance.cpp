#include <iostream>
#include <iomanip>
#include <fstream>
#include <utility>
#include <vector>
#include <chrono>
#include <sstream>
#include <string>
#include <ctime>

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::setw;
using std::setfill;
using std::endl;

using matrix = vector<vector<string>>;

// Function declarations
matrix readCSV(const string& filename);
void printMatrix(const matrix& M);
string calcUpdatedTime(const string& time, int minutesToAdd);
string getCurrentDateTime();
void addDateToOutput(matrix& M);
void checkAttendance(matrix& M);
void writeCSV(const matrix& M, const string& filename);

//======================================================================

matrix readCSV(const string& filename) {
    char separator = ',';
    matrix result;
    string row, item;

    ifstream in(filename);

    if (!in.is_open()) {
        cerr << "Error: Unable to open file: " << filename << endl;
        return result;
    }

    while (getline(in, row)) {
        vector<string> R;
        stringstream ss(row);
        while (getline(ss, item, separator)) R.push_back(item);
        result.push_back(R);
    }

    in.close();
    return result;
}

//======================================================================

void printMatrix(const matrix& M) {
    for (const auto& row : M) {
        for (const auto& s : row) cout << setw(22) << left << s << " ";
        cout << '\n';
    }
}

//======================================================================

string calcUpdatedTime(const string& time, int minutesToAdd) {
    size_t colonPos = time.find(':');
    if (colonPos != string::npos) {
        int hours = stoi(time.substr(0, colonPos));
        int minutes = stoi(time.substr(colonPos + 1));
        minutes += minutesToAdd;
        if (minutes >= 60) {
            hours += 1;
            minutes -= 60;
        }
        stringstream updatedSchedule;
        updatedSchedule << setw(2) << setfill('0') << hours << ":" << setw(2) << setfill('0') << minutes;
        return updatedSchedule.str();
    }
    return "";
}

//======================================================================

string getCurrentDateTime() {
    auto now = chrono::system_clock::now();
    time_t currentTime = chrono::system_clock::to_time_t(now);
    struct tm* localTimeInfo = localtime(&currentTime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTimeInfo);
    return buffer;
}

//======================================================================

void addDateToOutput(matrix& M) {
    string currentDate = "Attendance Checked: " + getCurrentDateTime();
    vector<string> newRow = {currentDate};
    M.push_back(newRow);
}

//======================================================================

void checkAttendance(matrix& M) {
    string classSchedule = M[1][0];
    string teacherTimeIn;
    cout << "Class Schedule: " << classSchedule << endl;

    for (auto& row : M) {
        if (row.size() == 5 && row[2] == "Teacher") {
            teacherTimeIn = row[4];
            cout << "Updating attendance based on teacher's time in: " << teacherTimeIn << endl;

            if (teacherTimeIn.empty() || calcUpdatedTime(classSchedule, 15) < row[4]) {
                if (row[4] != "Time") row[4] = "Absent";
            } else if (calcUpdatedTime(classSchedule, -5) < row[4]) {
                if (row[4] != "Time") row[4] = "Late";
            } else {
                if (row[4] != "Time") row[4] = "Present";
            }
        } else if (row.size() == 5) {
            string studentTimeIn = row[4];
            if (studentTimeIn.empty() || calcUpdatedTime(teacherTimeIn, 15) < studentTimeIn) {
                if (row[4] != "Time") row[4] = "Absent";
            } else if (calcUpdatedTime(teacherTimeIn, -5) < studentTimeIn) {
                if (row[4] != "Time") row[4] = "Late";
            } else {
                if (row[4] != "Time") row[4] = "Present";
            }
        }
    }
    cout << "Attendance updated successfully!" << endl;
}

//======================================================================

void writeCSV(const matrix& M, const string& filename) {
    ofstream out(filename, std::ios::app);
    if (!out.is_open()) {
        cerr << "Error: Unable to create file: " << filename << endl;
        return;
    }

    for (const auto& row : M) {
        for (size_t i = 0; i < row.size(); ++i) {
            out << row[i];
            if (i < row.size() - 1) out << ",";
        }
        out << '\n';
    }
    out.close();
}

//======================================================================

int main() {
    matrix pets = readCSV("test.csv");
    printMatrix(pets);

    cout << "\n\n";

    checkAttendance(pets);

    addDateToOutput(pets);

    printMatrix(pets);

    writeCSV(pets, "updated_attendance.csv");

    return 0;
}
