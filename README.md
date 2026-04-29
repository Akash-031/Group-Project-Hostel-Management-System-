# Group-Project-Hostel-Management-System

A **console-based Hostel Management System** built as a group project (54 members) for a **Data Structures** university course.

---

## Data Structures Used

| Data Structure | Usage |
|---|---|
| **Singly Linked List** | Master list of all registered students |
| **Binary Search Tree (BST)** | Fast O(log n) lookup / delete by student ID |
| **Queue (Linked List)** | Waiting list – students waiting for a room (FIFO) |
| **Stack (Linked List)** | Complaint log – most-recent complaints first (LIFO) |
| **Array** | Room inventory (20 rooms, 2 beds each) |

---

## Features

1. **Add Student** – Registers a student and auto-allocates a room (if available); otherwise adds to waiting list.
2. **Search Student by ID** – O(log n) BST lookup.
3. **Display All Students** – Tabular view via linked-list traversal.
4. **Delete Student** – Removes student; frees room and auto-assigns it to the next waiting student.
5. **Allocate Room** – Manually assign a room to a student.
6. **Deallocate Room** – Free a room; next waiting student is promoted.
7. **Display Rooms** – Shows occupancy status of all 20 rooms.
8. **Display Waiting List** – Shows the queue of students waiting for a room.
9. **Fee Management** – Pay fees, check fee status, list defaulters.
10. **Complaint Management** – File, view (stack order), and resolve complaints.
11. **Display Students Sorted by ID** – BST in-order traversal.
12. **Statistics** – Aggregate hostel occupancy and fee report.

---

## Build & Run

### Requirements
- A C++ compiler supporting C++17 (e.g. `g++` >= 7)

### Using `make`
```bash
make          # compiles to ./hostel_management
./hostel_management
make clean    # removes the binary
```

### Manual compilation
```bash
g++ -std=c++17 -Wall -Wextra -o hostel_management hostel_management.cpp
./hostel_management
```

---

## Project Structure

```
.
├── hostel_management.cpp   # Full source (all DS + logic)
├── Makefile
└── README.md
```

---

## Group

This project was developed as part of a **Data Structures** course group project with **54 members**.
