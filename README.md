# CS 300 Portfolio

**Andre Evans**  
**Data Structures and Algorithms**

This repository contains my work on the ABCU course advising program. Project One compares the runtime and memory use of a vector, hash table, and binary search tree. Project Two implements a C++ program that loads course data, prints an alphanumeric course list, and displays a selected course with its prerequisites.

## Portfolio Files

| File | Purpose |
| --- | --- |
| [Project One Main Report](Andre_Evans_CS300_Project_One_Main_Report_2_Pages.docx) | Runtime and memory comparison, advantages and disadvantages, and data structure recommendation. |
| [Project One Detailed Appendix](Andre_Evans_CS300_Project_One_Detailed_Appendix.docx) | Supporting pseudocode and detailed runtime and memory analysis. |
| [ProjectTwo.cpp](ProjectTwo.cpp) | Working C++ course advising program using a binary search tree. |
| [ABCU_Advising_Program_Input.csv](ABCU_Advising_Program_Input.csv) | Course data used to run the program. |
| [README.md](README.md) | Project overview, reflection, and instructions for running the program. |

## What was the problem you were solving in the projects for this course?

The projects focused on helping ABCU advisors organize and access Computer Science course information. The program needed to load course data from a file, print courses in alphanumeric order, and show the prerequisites for a specific course. In Project One, I compared three data structures to decide which would fit these requirements. In Project Two, I used that planning to build the advising program in C++.

## How did you approach the problem? Consider why data structures are important to understand.

I started by breaking the requirements into loading, storing, searching, and printing course information. I compared vectors, hash tables, and binary search trees based on runtime, memory use, and how each would produce an ordered list. I chose a binary search tree because an in-order traversal prints the courses in alphanumeric order without a separate sorting step. This helped me understand that choosing a data structure affects the rest of the program, and that a regular BST can become slower if it becomes unbalanced.

## How did you overcome any roadblocks you encountered while going through the activities or project?

One challenge was making sure the course file was handled correctly before the program used its data. I addressed this by separating file parsing from tree operations and checking for missing course information, duplicate course numbers, and prerequisites that were not in the file. The implementation also allows blank optional prerequisite columns and checks prerequisite references after all courses have been loaded. Breaking these responsibilities into smaller functions made it easier to check each part and understand where a problem could occur.

## How has your work on this project expanded your approach to designing software and developing programs?

This project helped me think about performance and program structure earlier in the design process. I now consider how often a program will search, insert, or print data before choosing how to store it. Comparing average and worst-case performance also showed me why a solution that works well with a small file may behave differently as the amount of data grows. Writing pseudocode first gave me a clearer plan for turning the requirements into working code.

## How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

I have become more deliberate about using descriptive names and giving each function a clear responsibility. In this program, parsing input, loading courses, searching the tree, and displaying results are handled separately. That organization makes it easier to update one part of the program without changing everything else. I also see the value of useful comments, consistent formatting, and clear error messages because they make the code easier for another developer, or my future self, to understand.

## Running the Program

Use a C++11-compatible compiler or newer. Keep the source file and CSV in the same working folder.

```bash
g++ -std=c++11 -Wall -Wextra -pedantic ProjectTwo.cpp -o advising
./advising
```

On Windows, the executable may be named `advising.exe`.

1. Choose **1** to load the data.
2. Enter `ABCU_Advising_Program_Input.csv` when prompted for the filename.
3. Choose **2** to print all courses in alphanumeric order.
4. Choose **3** to look up a course, such as `CSCI300`, and its prerequisites.
5. Choose **9** to exit.

## Verification

The included source compiled with C++11 and the warning flags shown above without warnings. Checks confirmed that all eight courses print in alphanumeric order, lowercase course searches work, and prerequisite names display correctly. The program also blocks printing before a successful load, reports missing courses and files, and preserves previously loaded data when a later file load fails.
