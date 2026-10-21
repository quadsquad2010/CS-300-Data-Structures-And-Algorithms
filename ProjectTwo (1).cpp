// Southern New Hampshire University
// Project Two: Course Planner For ABCU
// Joshua Evans
// CS-300 DSA: Analysis and Design
//
// This program loads course information from a text file and stores it in a data structure so academic advisors can view the course
// details. The user can print a fill list of courses in alphanumeric order or loop up a single course and see its details including
// any prerequisites and their titles. The program uses a simple CSV parsing, a map for easy and quick lookups, and a menu system to
// allow the user to choose what they want to do. The program will continue to run until the user chooses to exit.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// Trim whitespace from both ends of a string
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");

    if (start == string::npos || end == string::npos) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

// Course object holds the basic info and prerequisite course numbers
// Also includes the Course Name, Course Number, and any Prerequisites (as a vector of strings)
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

// Converts a string to uppercase so users can type course numbers in any case (e.g., cs101, CS101, Cs101)
// and in any formaat (e.g., with or without spaces) and still find the course in the map
string toUpper(const string& s) {
    string result = s;
    for (char& c : result) {
        c = toupper(static_cast<unsigned char>(c));
    }
    return result;
}

// Reads one line from the CSV file and breaks it into pieces
// The first value is the course number, the second value is the course name, and any remaining values are prerequisites
Course parseCourseLine(const string& line) {
    Course course;
    string token;
    stringstream ss(line);

    // First field: courseNumber
    if (getline(ss, token, ',')) {
        course.courseNumber = token;
    }

    // Second field: courseName
    if (getline(ss, token, ',')) {
        // Trim leading space if present
        if (!token.empty() && token[0] == ' ') {
            token.erase(0, 1);
        }
        course.courseName = token;
    }

    // Remaining fields: prerequisites (course numbers)
    while (getline(ss, token, ',')) {
        if (!token.empty() && token[0] == ' ') {
            token.erase(0, 1);
        }
        if (!token.empty()) {
            course.prerequisites.push_back(token);
        }
    }

    return course;
}

// Loads all courses from the given file into the map
// The map then uses the course number as the key and the Course object as the value
bool loadCoursesFromFile(const string& fileName, map<string, Course>& courses) {
    ifstream inFile(fileName);

    // If the file can't be opened it lets the user know and returns an Error message
    if (!inFile.is_open()) {
        cout << "Error: Could not open file '" << fileName << "'." << endl;
        return false;
    }

    // Clear any old data before loading new data
    courses.clear();

    string line;

    // Read each line and convert it into a Course object then stores it in the map using the courseNumber as the key
    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }
        Course course = parseCourseLine(line);

        // Only adds valid courses
        if (!course.courseNumber.empty()) {
            courses[course.courseNumber] = course;
        }
    }

    // Lets user know that the data was loaded successfully and closes the file
    inFile.close();
    cout << "Data loaded successfully from '" << fileName << "'." << endl;
    return true;
}

// Print an alphanumeric list of all courses and helps Advisors see the full list of courses available to students
void printCourseList(const map<string, Course>& courses) {
    if (courses.empty()) {
        cout << "No course data loaded. Please load the data structure first." << endl;
        return;
    }

    // Collect all course numbers so we can sort them
    vector<string> courseNumbers;
    for (const auto& pair : courses) {
        courseNumbers.push_back(pair.first);
    }

    sort(courseNumbers.begin(), courseNumbers.end());

    cout << "Here is a sample schedule:" << endl;

    // Print each course number and name in sorted order
    for (const string& num : courseNumbers) {
        const Course& c = courses.at(num);
        cout << c.courseNumber << ", " << c.courseName << endl;
    }
}

// Print detailed information for a single course including its prerequisites and their titles, if available
void printCourseInfo(const map<string, Course>& courses) {
    if (courses.empty()) {
        // If no courses are loaded, inform the user and return
        cout << "No course data loaded. Please load the data structure first." << endl;
        return;
    }

    // Prompts user to enter a course number and retrieves the corresponding course information
    cout << "What course do you want to know about? ";
    string input;
    getline(cin, input);

    // Converts user input to uppercase so the lookup stays consistent
    string courseNum = toUpper(input);

    // Try to find the course in the map
    auto it = courses.find(courseNum);
    if (it == courses.end()) {
        cout << "Course '" << courseNum << "' not found." << endl;
        return;
    }

    const Course& course = it->second;
    cout << course.courseNumber << ", " << course.courseName << endl;

    // Print prerequisites if available, otherwise indicates that there are none
    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            const string& preNum = course.prerequisites[i];

            // Look up the prerequisite so we can print its name too
            auto preIt = courses.find(preNum);
            if (preIt != courses.end()) {
                cout << preIt->second.courseNumber;
                // Includes title as required by assignment
                cout << " (" << preIt->second.courseName << ")";
            } else {
                // If prerequisite not found in map, just print its number
                cout << preNum;
            }
            if (i + 1 < course.prerequisites.size()) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

// Displays the main menu
void printMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

// Main function that runs the course planner program
int main() {
    // Map to store all courses once loaded
    map<string, Course> courses;
    // Tracks whether the user has loaded the data yet or not
    bool dataLoaded = false;
    // Controls the main loop of the program
    bool running = true;

    cout << "Welcome to the course planner." << endl;

    // Main Program Loop: Continues until the user chooses to exit the program
    while (running) {
        printMenu();

        string choiceLine;
        getline(cin, choiceLine);

        int choice = 0;

        // Converts menu input into an interger, if conversion fails the user typed an invalid option
        // and the program will inform them of the invalid input and return to the main menu
        try {
            choice = stoi(choiceLine);
        } catch (...) {
            cout << choiceLine << " is not a valid option." << endl;
            continue;
        }

        // Handles Menu choices
        switch (choice) {
        case 1: {
            // Loads course data from a file
            cout << "Enter the file name containing the course data: ";
            string fileName;
            getline(cin, fileName);

            if (loadCoursesFromFile(fileName, courses)) {
                dataLoaded = true;
            } else {
                dataLoaded = false;
            }
            break;
        }
        case 2:
        // Prints sorted list of courses
            if (!dataLoaded) {
                cout << "Please load the data structure before printing the course list." << endl;
            } else {
                printCourseList(courses);
            }
            break;
        case 3:
        // Print detailed info for one course
            if (!dataLoaded) {
                cout << "Please load the data structure before printing course information." << endl;
            } else {
                printCourseInfo(courses);
            }
            break;
        case 9:
        // Exit the program
            cout << "Thank you for using the course planner!" << endl;
            running = false;
            break;
        default:
        // Handles invalid menu choices
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}