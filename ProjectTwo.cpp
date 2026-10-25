// ProjectTwo.cpp
// Andre Evans
// CS 300 - Project Two: ABCU Advising Assistance Program
// Uses a binary search tree to store courses and print them in alphanumeric order.

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// Each course has a number, a title, and zero or more prerequisites.
struct Course {
    string number;
    string title;
    vector<string> prerequisites;
};

// Remove spaces from the beginning and end of input.
string trim(const string& text) {
    const string whitespace = " \t\r\n";
    size_t first = text.find_first_not_of(whitespace);
    if (first == string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
}

// Standardize course numbers so users can search using lowercase letters.
string normalizeCourseNumber(string number) {
    number = trim(number);
    transform(number.begin(), number.end(), number.begin(),
              [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return number;
}

// Read CSV fields without an external CSVParser library.
// Handles regular comma-separated fields and quoted fields containing commas.
bool parseCSVLine(const string& line, vector<string>& fields) {
    fields.clear();
    string current;
    bool insideQuotes = false;
    bool afterClosingQuote = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char character = line[i];

        if (insideQuotes) {
            if (character == '"') {
                if (i + 1 < line.length() && line[i + 1] == '"') {
                    current += '"';
                    ++i;
                } else {
                    insideQuotes = false;
                    afterClosingQuote = true;
                }
            } else {
                current += character;
            }
        } else if (character == ',') {
            fields.push_back(trim(current));
            current.clear();
            afterClosingQuote = false;
        } else if (character == '"') {
            // Quotes must surround a field, not appear in the middle of it.
            if (!trim(current).empty() || afterClosingQuote) {
                return false;
            }
            insideQuotes = true;
        } else if (afterClosingQuote && !isspace(static_cast<unsigned char>(character))) {
            return false;
        } else {
            current += character;
        }
    }

    if (insideQuotes) {
        return false;
    }
    fields.push_back(trim(current));
    return true;
}

// A binary search tree keeps course numbers in sorted order.
class CourseTree {
private:
    struct Node {
        Course course;
        unique_ptr<Node> left;
        unique_ptr<Node> right;

        explicit Node(const Course& value) : course(value) {}
    };

    unique_ptr<Node> root;
    size_t courseCount = 0;

    static void printInOrder(const Node* node) {
        if (node == nullptr) {
            return;
        }

        printInOrder(node->left.get());
        cout << node->course.number << ", " << node->course.title << '\n';
        printInOrder(node->right.get());
    }

public:
    CourseTree() = default;
    CourseTree(const CourseTree&) = delete;
    CourseTree& operator=(const CourseTree&) = delete;
    CourseTree(CourseTree&&) = default;
    CourseTree& operator=(CourseTree&&) = default;

    // Returns false when the course number is already in the tree.
    bool insert(const Course& course) {
        unique_ptr<Node>* current = &root;
        while (*current != nullptr) {
            if (course.number == (*current)->course.number) {
                return false;
            }
            if (course.number < (*current)->course.number) {
                current = &((*current)->left);
            } else {
                current = &((*current)->right);
            }
        }

        current->reset(new Node(course));
        ++courseCount;
        return true;
    }

    // Search for one course by its number.
    const Course* find(const string& number) const {
        const Node* current = root.get();
        while (current != nullptr) {
            if (number == current->course.number) {
                return &current->course;
            }
            current = (number < current->course.number)
                          ? current->left.get()
                          : current->right.get();
        }
        return nullptr;
    }

    void printAllCourses() const {
        printInOrder(root.get());
    }

    size_t size() const {
        return courseCount;
    }
};

// Load a CSV file into a new tree. The previous tree is only replaced
// after the new file has been checked successfully.
bool loadCourses(const string& fileName, CourseTree& courses) {
    ifstream inputFile(fileName.c_str());
    if (!inputFile.is_open()) {
        cout << "Error: Could not open file '" << fileName << "'.\n";
        return false;
    }

    CourseTree newCourses;
    vector<Course> loadedCourses;
    string line;
    size_t lineNumber = 0;

    while (getline(inputFile, line)) {
        ++lineNumber;

        // Remove the UTF-8 byte order mark if present at the start of the file.
        if (lineNumber == 1 && line.size() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line.erase(0, 3);
        }

        if (trim(line).empty()) {
            continue;
        }

        vector<string> fields;
        if (!parseCSVLine(line, fields) || fields.size() < 2) {
            cout << "Error on line " << lineNumber
                 << ": Expected a course number and course title.\n";
            return false;
        }

        Course course;
        course.number = normalizeCourseNumber(fields[0]);
        course.title = trim(fields[1]);
        if (course.number.empty() || course.title.empty()) {
            cout << "Error on line " << lineNumber
                 << ": Course number and title cannot be empty.\n";
            return false;
        }

        // The ABCU CSV includes empty columns for optional prerequisites.
        // Ignore those blanks, including trailing commas after course titles.
        for (size_t i = 2; i < fields.size(); ++i) {
            string prerequisite = normalizeCourseNumber(fields[i]);
            if (!prerequisite.empty()) {
                course.prerequisites.push_back(prerequisite);
            }
        }

        if (!newCourses.insert(course)) {
            cout << "Error on line " << lineNumber << ": Duplicate course number "
                 << course.number << ".\n";
            return false;
        }
        loadedCourses.push_back(course);
    }

    if (inputFile.bad()) {
        cout << "Error: The course file could not be read completely.\n";
        return false;
    }
    if (newCourses.size() == 0) {
        cout << "Error: The course file does not contain any courses.\n";
        return false;
    }

    // Ensure every prerequisite references a course included in the file.
    for (size_t i = 0; i < loadedCourses.size(); ++i) {
        const Course& course = loadedCourses[i];
        for (size_t j = 0; j < course.prerequisites.size(); ++j) {
            if (newCourses.find(course.prerequisites[j]) == nullptr) {
                cout << "Error: " << course.number << " lists unknown prerequisite "
                     << course.prerequisites[j] << ".\n";
                return false;
            }
        }
    }

    courses = std::move(newCourses);
    cout << "Successfully loaded " << courses.size() << " courses.\n";
    return true;
}

void printCourseInformation(const CourseTree& courses, const string& courseNumber) {
    const Course* course = courses.find(normalizeCourseNumber(courseNumber));
    if (course == nullptr) {
        cout << "Error: Course not found. Please enter a valid course number.\n";
        return;
    }

    cout << course->number << ", " << course->title << '\n';
    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None\n";
    } else {
        cout << "Prerequisites:\n";
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            const Course* prerequisite = courses.find(course->prerequisites[i]);
            // All prerequisites were validated when loading the course file.
            cout << "  " << prerequisite->number << ", " << prerequisite->title << '\n';
        }
    }
}

void printMenu() {
    cout << "\n1. Load Data Structure\n"
         << "2. Print Course List\n"
         << "3. Print Course\n"
         << "9. Exit\n";
}

int main() {
    CourseTree courses;
    bool dataLoaded = false;
    string choice;

    cout << "Welcome to the ABCU course planner.\n";

    while (true) {
        printMenu();
        cout << "What would you like to do? ";
        if (!getline(cin, choice)) {
            cout << "\nGoodbye!\n";
            break;
        }

        choice = trim(choice);
        if (choice == "1") {
            string fileName;
            cout << "Enter the course data file name: ";
            if (!getline(cin, fileName)) {
                cout << "\nGoodbye!\n";
                break;
            }
            fileName = trim(fileName);
            if (fileName.empty()) {
                cout << "Error: File name cannot be empty.\n";
            } else if (loadCourses(fileName, courses)) {
                dataLoaded = true;
            }
        } else if (choice == "2") {
            if (!dataLoaded) {
                cout << "Please select option 1 to load course data first.\n";
            } else {
                cout << "\nHere is a sample schedule:\n";
                courses.printAllCourses();
            }
        } else if (choice == "3") {
            if (!dataLoaded) {
                cout << "Please select option 1 to load course data first.\n";
            } else {
                string courseNumber;
                cout << "What course do you want to know about? ";
                if (!getline(cin, courseNumber)) {
                    cout << "\nGoodbye!\n";
                    break;
                }
                if (trim(courseNumber).empty()) {
                    cout << "Error: Course number cannot be empty.\n";
                } else {
                    printCourseInformation(courses, courseNumber);
                }
            }
        } else if (choice == "9") {
            cout << "Thank you for using the course planner!\n";
            break;
        } else {
            cout << "Error: Invalid menu option. Please enter 1, 2, 3, or 9.\n";
        }
    }

    return 0;
}
