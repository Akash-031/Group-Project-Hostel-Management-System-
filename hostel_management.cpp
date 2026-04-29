/*
 * ============================================================
 *  Hostel Management System
 *  DS Course Group Project  (Group of 54)
 * ============================================================
 *
 *  Data Structures used:
 *   - Singly Linked List  : Student records (owns all Student objects)
 *   - BST Index (BSTNode) : Fast O(log n) lookup by Student ID
 *   - Queue (Linked List) : Waiting list for room allocation (FIFO)
 *   - Stack (Linked List) : Complaint / activity log (LIFO)
 *   - Array               : Room inventory
 * ============================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;

// ---------------------------------------------------------
//  Constants
// ---------------------------------------------------------
const int TOTAL_ROOMS      = 20;  // hostel capacity (rooms)
const int STUDENTS_PER_ROOM = 2;  // beds per room

// ---------------------------------------------------------
//  Utility
// ---------------------------------------------------------
static string currentTimestamp() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

// ---------------------------------------------------------
//  Student  (linked-list node – owns all student data)
// ---------------------------------------------------------
struct Student {
    int    id;
    string name;
    string course;
    int    year;
    int    roomNumber;  // 0 = not allocated
    double feesPaid;
    double feesDue;
    Student* next;      // linked-list chain

    Student(int id, const string& name, const string& course, int year)
        : id(id), name(name), course(course), year(year),
          roomNumber(0), feesPaid(0.0), feesDue(5000.0),
          next(nullptr) {}
};

// ---------------------------------------------------------
//  BSTNode  (separate BST index – holds id + pointer to
//            the Student in the linked list; the two
//            structures never share node memory)
// ---------------------------------------------------------
struct BSTNode {
    int      id;
    Student* student;   // pointer into the linked list
    BSTNode* left;
    BSTNode* right;

    BSTNode(Student* s)
        : id(s->id), student(s), left(nullptr), right(nullptr) {}
};

// ---------------------------------------------------------
//  Complaint  (stack node)
// ---------------------------------------------------------
struct Complaint {
    int    studentId;
    string description;
    string timestamp;
    string status;  // "Pending" / "Resolved"
    Complaint* next;

    Complaint(int sid, const string& desc)
        : studentId(sid), description(desc),
          timestamp(currentTimestamp()), status("Pending"),
          next(nullptr) {}
};

// ---------------------------------------------------------
//  WaitNode  (queue node)
// ---------------------------------------------------------
struct WaitNode {
    int studentId;
    WaitNode* next;
    explicit WaitNode(int id) : studentId(id), next(nullptr) {}
};

// ---------------------------------------------------------
//  Room
// ---------------------------------------------------------
struct Room {
    int  number;
    int  capacity;
    int  occupied;
    bool isAvailable() const { return occupied < capacity; }
    Room() : number(0), capacity(STUDENTS_PER_ROOM), occupied(0) {}
};

// ==========================================================
//  Hostel Management System
// ==========================================================
class HostelManagementSystem {
private:
    // Linked list (owns Student objects)
    Student* head;

    // BST index (separate BSTNodes, never share Student memory)
    BSTNode* bstRoot;

    // Room array (1-indexed)
    Room rooms[TOTAL_ROOMS + 1];

    // Waiting-list queue
    WaitNode* qFront;
    WaitNode* qRear;

    // Complaint stack
    Complaint* complaintTop;

    int totalStudents;
    int nextId;

    // -------------------------------------------------------
    //  BST helpers
    // -------------------------------------------------------
    BSTNode* bstInsert(BSTNode* root, Student* s) {
        if (!root) return new BSTNode(s);
        if (s->id < root->id)
            root->left  = bstInsert(root->left,  s);
        else if (s->id > root->id)
            root->right = bstInsert(root->right, s);
        return root;
    }

    // Returns pointer to BSTNode; caller must NOT free it.
    BSTNode* bstFind(BSTNode* root, int id) const {
        if (!root)          return nullptr;
        if (id == root->id) return root;
        if (id  < root->id) return bstFind(root->left,  id);
        return              bstFind(root->right, id);
    }

    // Returns the minimum BSTNode in a subtree.
    BSTNode* bstMin(BSTNode* node) const {
        while (node->left) node = node->left;
        return node;
    }

    // Removes the BSTNode for 'id', frees the BSTNode memory,
    // and returns the updated subtree root.
    // Does NOT touch the linked-list Student object.
    BSTNode* bstDelete(BSTNode* root, int id) {
        if (!root) return nullptr;
        if (id < root->id) {
            root->left  = bstDelete(root->left,  id);
        } else if (id > root->id) {
            root->right = bstDelete(root->right, id);
        } else {
            // Node found
            if (!root->left) {
                BSTNode* tmp = root->right;
                delete root;
                return tmp;
            }
            if (!root->right) {
                BSTNode* tmp = root->left;
                delete root;
                return tmp;
            }
            // Two children: copy successor's data into this node,
            // then delete the successor node further down.
            BSTNode* succ = bstMin(root->right);
            root->id      = succ->id;
            root->student = succ->student;
            root->right   = bstDelete(root->right, succ->id);
        }
        return root;
    }

    void bstInOrder(BSTNode* root) const {
        if (!root) return;
        bstInOrder(root->left);
        const Student* s = root->student;
        cout << "  [" << setw(4) << s->id << "]  "
             << left << setw(20) << s->name
             << "  Room: " << (s->roomNumber ? to_string(s->roomNumber) : "N/A")
             << "\n";
        bstInOrder(root->right);
    }

    // -------------------------------------------------------
    //  Queue helpers
    // -------------------------------------------------------
    void enqueue(int studentId) {
        WaitNode* node = new WaitNode(studentId);
        if (!qRear) { qFront = qRear = node; }
        else        { qRear->next = node; qRear = node; }
    }

    int dequeue() {
        if (!qFront) return -1;
        WaitNode* tmp = qFront;
        int id = tmp->studentId;
        qFront = qFront->next;
        if (!qFront) qRear = nullptr;
        delete tmp;
        return id;
    }

    bool isQueueEmpty() const { return qFront == nullptr; }

    bool isInQueue(int studentId) const {
        WaitNode* cur = qFront;
        while (cur) {
            if (cur->studentId == studentId) return true;
            cur = cur->next;
        }
        return false;
    }

    // -------------------------------------------------------
    //  Stack helpers
    // -------------------------------------------------------
    void pushComplaint(int sid, const string& desc) {
        Complaint* c = new Complaint(sid, desc);
        c->next = complaintTop;
        complaintTop = c;
    }

    // -------------------------------------------------------
    //  Room helper
    // -------------------------------------------------------
    int findAvailableRoom() const {
        for (int i = 1; i <= TOTAL_ROOMS; i++)
            if (rooms[i].isAvailable()) return i;
        return -1;
    }

    // -------------------------------------------------------
    //  Output helpers
    // -------------------------------------------------------
    void separator(char c = '-', int w = 60) const {
        for (int i = 0; i < w; i++) cout << c;
        cout << "\n";
    }

    void printStudent(const Student* s) const {
        separator('-', 45);
        cout << "  Student ID  : " << s->id         << "\n"
             << "  Name        : " << s->name       << "\n"
             << "  Course      : " << s->course     << "\n"
             << "  Year        : " << s->year       << "\n"
             << "  Room No.    : " << (s->roomNumber ? to_string(s->roomNumber) : "N/A") << "\n"
             << "  Fees Paid   : " << fixed << setprecision(2) << s->feesPaid  << "\n"
             << "  Fees Due    : " << fixed << setprecision(2) << s->feesDue   << "\n";
        separator('-', 45);
    }

public:
    // -------------------------------------------------------
    //  Constructor
    // -------------------------------------------------------
    HostelManagementSystem()
        : head(nullptr), bstRoot(nullptr),
          qFront(nullptr), qRear(nullptr),
          complaintTop(nullptr), totalStudents(0), nextId(1001) {
        for (int i = 1; i <= TOTAL_ROOMS; i++) {
            rooms[i].number   = i;
            rooms[i].capacity = STUDENTS_PER_ROOM;
            rooms[i].occupied = 0;
        }
    }

    // -------------------------------------------------------
    //  1. Add Student
    // -------------------------------------------------------
    void addStudent() {
        string name, course;
        int year;

        cout << "\n  Enter student name    : ";
        cin.ignore();
        getline(cin, name);
        cout << "  Enter course          : ";
        getline(cin, course);
        cout << "  Enter year of study   : ";
        cin >> year;

        int id = nextId++;
        Student* s = new Student(id, name, course, year);

        // Append to linked list
        if (!head) {
            head = s;
        } else {
            Student* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = s;
        }

        // Insert into BST index
        bstRoot = bstInsert(bstRoot, s);
        totalStudents++;

        // Auto-allocate a room
        int room = findAvailableRoom();
        if (room != -1) {
            s->roomNumber = room;
            rooms[room].occupied++;
            cout << "\n  Student added successfully!\n"
                 << "    Student ID : " << id   << "\n"
                 << "    Room No.   : " << room << "\n";
        } else {
            enqueue(id);
            cout << "\n  Student added. No room available right now.\n"
                 << "    Student ID : " << id << "\n"
                 << "    Added to waiting list.\n";
        }
    }

    // -------------------------------------------------------
    //  2. Search Student by ID  (BST O(log n))
    // -------------------------------------------------------
    void searchStudentById() const {
        int id;
        cout << "\n  Enter Student ID to search: ";
        cin >> id;

        BSTNode* node = bstFind(bstRoot, id);
        if (!node) { cout << "  Student not found.\n"; return; }
        printStudent(node->student);
    }

    // -------------------------------------------------------
    //  3. Display All Students  (linked-list traversal)
    // -------------------------------------------------------
    void displayAllStudents() const {
        if (!head) { cout << "  No students registered.\n"; return; }

        separator();
        cout << "  " << left
             << setw(6) << "ID"   << setw(22) << "Name"
             << setw(18) << "Course" << setw(8) << "Year"
             << setw(8) << "Room"  << setw(12) << "Fees Due" << "\n";
        separator();

        Student* cur = head;
        while (cur) {
            cout << "  " << left
                 << setw(6)  << cur->id
                 << setw(22) << cur->name
                 << setw(18) << cur->course
                 << setw(8)  << cur->year
                 << setw(8)  << (cur->roomNumber ? to_string(cur->roomNumber) : "N/A")
                 << setw(12) << fixed << setprecision(2) << cur->feesDue << "\n";
            cur = cur->next;
        }
        separator();
        cout << "  Total students: " << totalStudents << "\n";
    }

    // -------------------------------------------------------
    //  4. Delete Student
    // -------------------------------------------------------
    void deleteStudent() {
        int id;
        cout << "\n  Enter Student ID to remove: ";
        cin >> id;

        // Find in linked list
        Student* prev = nullptr;
        Student* cur  = head;
        while (cur && cur->id != id) { prev = cur; cur = cur->next; }

        if (!cur) { cout << "  Student not found.\n"; return; }

        // Free room if allocated
        if (cur->roomNumber) {
            rooms[cur->roomNumber].occupied--;
            // Promote next waiting student
            if (!isQueueEmpty()) {
                int wid = dequeue();
                BSTNode* wn = bstFind(bstRoot, wid);
                if (wn) {
                    wn->student->roomNumber = cur->roomNumber;
                    rooms[cur->roomNumber].occupied++;
                    cout << "  Room " << cur->roomNumber
                         << " assigned to waiting student ID " << wid << ".\n";
                }
            }
        }

        // Remove from linked list
        if (prev) prev->next = cur->next;
        else      head       = cur->next;

        // Remove BSTNode (frees BSTNode memory, does NOT free Student)
        bstRoot = bstDelete(bstRoot, id);

        // Free the Student object (owned by linked list)
        delete cur;

        totalStudents--;
        cout << "  Student " << id << " removed.\n";
    }

    // -------------------------------------------------------
    //  5. Allocate Room  (manual)
    // -------------------------------------------------------
    void allocateRoom() {
        int id;
        cout << "\n  Enter Student ID to allocate room: ";
        cin >> id;

        BSTNode* node = bstFind(bstRoot, id);
        if (!node) { cout << "  Student not found.\n"; return; }
        Student* s = node->student;

        if (s->roomNumber) {
            cout << "  Student already has room " << s->roomNumber << ".\n";
            return;
        }

        int room = findAvailableRoom();
        if (room == -1) {
            if (!isInQueue(id)) enqueue(id);
            cout << "  No rooms available. Student added to waiting list.\n";
            return;
        }
        s->roomNumber = room;
        rooms[room].occupied++;
        cout << "  Room " << room << " allocated to student " << id << ".\n";
    }

    // -------------------------------------------------------
    //  6. Deallocate Room
    // -------------------------------------------------------
    void deallocateRoom() {
        int id;
        cout << "\n  Enter Student ID to deallocate room: ";
        cin >> id;

        BSTNode* node = bstFind(bstRoot, id);
        if (!node) { cout << "  Student not found.\n"; return; }
        Student* s = node->student;

        if (!s->roomNumber) { cout << "  Student has no room allocated.\n"; return; }

        int oldRoom = s->roomNumber;
        rooms[oldRoom].occupied--;
        s->roomNumber = 0;
        cout << "  Room " << oldRoom << " deallocated from student " << id << ".\n";

        // Promote next waiting student
        if (!isQueueEmpty()) {
            int wid = dequeue();
            BSTNode* wn = bstFind(bstRoot, wid);
            if (wn) {
                wn->student->roomNumber = oldRoom;
                rooms[oldRoom].occupied++;
                cout << "  Room " << oldRoom
                     << " now assigned to waiting student ID " << wid << ".\n";
            }
        }
    }

    // -------------------------------------------------------
    //  7. Display Rooms
    // -------------------------------------------------------
    void displayRooms() const {
        separator();
        cout << "  " << left
             << setw(8) << "Room" << setw(12) << "Capacity"
             << setw(12) << "Occupied" << "Status\n";
        separator();
        int available = 0;
        for (int i = 1; i <= TOTAL_ROOMS; i++) {
            string status = rooms[i].isAvailable() ? "Available" : "Full";
            if (rooms[i].isAvailable()) available++;
            cout << "  " << left
                 << setw(8)  << rooms[i].number
                 << setw(12) << rooms[i].capacity
                 << setw(12) << rooms[i].occupied
                 << status << "\n";
        }
        separator();
        cout << "  Available rooms : " << available << " / " << TOTAL_ROOMS << "\n";
    }

    // -------------------------------------------------------
    //  8. Display Waiting List  (queue traversal)
    // -------------------------------------------------------
    void displayWaitingList() const {
        if (!qFront) { cout << "  Waiting list is empty.\n"; return; }

        separator();
        cout << "  Waiting List (Queue - FIFO)\n";
        separator();
        WaitNode* cur = qFront;
        int pos = 1;
        while (cur) {
            BSTNode* node = bstFind(bstRoot, cur->studentId);
            cout << "  " << pos++ << ". ID " << cur->studentId;
            if (node) cout << "  |  " << node->student->name;
            cout << "\n";
            cur = cur->next;
        }
        separator();
    }

    // -------------------------------------------------------
    //  9. Fee Management
    // -------------------------------------------------------
    void feeManagement() {
        int choice;
        cout << "\n  Fee Management\n";
        separator('-', 40);
        cout << "  1. Pay fees\n"
             << "  2. View fee status\n"
             << "  3. View all defaulters\n"
             << "  Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            int id; double amount;
            cout << "  Student ID : "; cin >> id;
            BSTNode* node = bstFind(bstRoot, id);
            if (!node) { cout << "  Student not found.\n"; return; }
            Student* s = node->student;
            cout << "  Amount to pay (due: " << s->feesDue << "): ";
            cin >> amount;
            if (amount <= 0) { cout << "  Invalid amount.\n"; return; }
            s->feesPaid += amount;
            s->feesDue  -= amount;
            if (s->feesDue < 0) s->feesDue = 0;
            cout << "  Payment recorded. Remaining due: " << s->feesDue << "\n";
        } else if (choice == 2) {
            int id;
            cout << "  Student ID : "; cin >> id;
            BSTNode* node = bstFind(bstRoot, id);
            if (!node) { cout << "  Student not found.\n"; return; }
            const Student* s = node->student;
            cout << "  Name       : " << s->name     << "\n"
                 << "  Fees Paid  : " << s->feesPaid << "\n"
                 << "  Fees Due   : " << s->feesDue  << "\n";
        } else if (choice == 3) {
            separator();
            cout << "  Fee Defaulters\n";
            separator();
            Student* cur = head;
            bool found = false;
            while (cur) {
                if (cur->feesDue > 0) {
                    cout << "  ID " << setw(5) << cur->id
                         << "  " << left << setw(20) << cur->name
                         << "  Due: " << cur->feesDue << "\n";
                    found = true;
                }
                cur = cur->next;
            }
            if (!found) cout << "  No defaulters.\n";
            separator();
        } else {
            cout << "  Invalid choice.\n";
        }
    }

    // -------------------------------------------------------
    // 10. Complaint Management  (stack)
    // -------------------------------------------------------
    void complaintManagement() {
        int choice;
        cout << "\n  Complaint Management\n";
        separator('-', 40);
        cout << "  1. File a complaint\n"
             << "  2. View all complaints\n"
             << "  3. Resolve top complaint\n"
             << "  Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            int id; string desc;
            cout << "  Student ID  : "; cin >> id;
            if (!bstFind(bstRoot, id)) { cout << "  Student not found.\n"; return; }
            cout << "  Description : "; cin.ignore(); getline(cin, desc);
            pushComplaint(id, desc);
            cout << "  Complaint filed.\n";
        } else if (choice == 2) {
            if (!complaintTop) { cout << "  No complaints.\n"; return; }
            separator();
            cout << "  Complaints (most recent first - Stack)\n";
            separator();
            Complaint* cur = complaintTop;
            int i = 1;
            while (cur) {
                cout << "  #" << i++ << "\n"
                     << "    Student ID  : " << cur->studentId   << "\n"
                     << "    Description : " << cur->description << "\n"
                     << "    Filed at    : " << cur->timestamp   << "\n"
                     << "    Status      : " << cur->status      << "\n\n";
                cur = cur->next;
            }
        } else if (choice == 3) {
            if (!complaintTop) { cout << "  No complaints to resolve.\n"; return; }
            complaintTop->status = "Resolved";
            cout << "  Top complaint marked as Resolved: \""
                 << complaintTop->description << "\"\n";
        } else {
            cout << "  Invalid choice.\n";
        }
    }

    // -------------------------------------------------------
    // 11. Display Students Sorted by ID  (BST in-order)
    // -------------------------------------------------------
    void displaySortedById() const {
        separator();
        cout << "  Students sorted by ID (BST In-Order Traversal)\n";
        separator();
        if (!bstRoot) { cout << "  No students.\n"; return; }
        bstInOrder(bstRoot);
        separator();
    }

    // -------------------------------------------------------
    // 12. Statistics
    // -------------------------------------------------------
    void showStatistics() const {
        int totalBeds = TOTAL_ROOMS * STUDENTS_PER_ROOM;
        int occupied  = 0;
        int waiting   = 0;
        double totalDue = 0;

        Student* cur = head;
        while (cur) {
            if (cur->roomNumber) occupied++;
            totalDue += cur->feesDue;
            cur = cur->next;
        }
        WaitNode* wc = qFront;
        while (wc) { waiting++; wc = wc->next; }

        separator('=', 45);
        cout << "  HOSTEL STATISTICS\n";
        separator('=', 45);
        cout << "  Total rooms        : " << TOTAL_ROOMS  << "\n"
             << "  Beds per room      : " << STUDENTS_PER_ROOM << "\n"
             << "  Total capacity     : " << totalBeds    << "\n"
             << "  Occupied beds      : " << occupied     << "\n"
             << "  Vacant beds        : " << (totalBeds - occupied) << "\n"
             << "  Registered students: " << totalStudents << "\n"
             << "  Waiting list size  : " << waiting      << "\n"
             << "  Total fees due     : "
             << fixed << setprecision(2) << totalDue << "\n";
        separator('=', 45);
    }
};

// ==========================================================
//  Main Menu
// ==========================================================
static void printMenu() {
    cout << "\n";
    for (int i = 0; i < 60; i++) cout << "=";
    cout << "\n       HOSTEL MANAGEMENT SYSTEM\n";
    for (int i = 0; i < 60; i++) cout << "=";
    cout << "\n\n"
         << "   1.  Add Student\n"
         << "   2.  Search Student by ID\n"
         << "   3.  Display All Students\n"
         << "   4.  Delete Student\n"
         << "   5.  Allocate Room\n"
         << "   6.  Deallocate Room\n"
         << "   7.  Display Rooms\n"
         << "   8.  Display Waiting List\n"
         << "   9.  Fee Management\n"
         << "  10.  Complaint Management\n"
         << "  11.  Display Students Sorted by ID\n"
         << "  12.  Statistics\n"
         << "   0.  Exit\n\n"
         << "  Enter choice: ";
}

int main() {
    HostelManagementSystem hms;
    int choice;

    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  Invalid input.\n";
            continue;
        }

        switch (choice) {
            case  1: hms.addStudent();          break;
            case  2: hms.searchStudentById();   break;
            case  3: hms.displayAllStudents();  break;
            case  4: hms.deleteStudent();       break;
            case  5: hms.allocateRoom();        break;
            case  6: hms.deallocateRoom();      break;
            case  7: hms.displayRooms();        break;
            case  8: hms.displayWaitingList();  break;
            case  9: hms.feeManagement();       break;
            case 10: hms.complaintManagement(); break;
            case 11: hms.displaySortedById();   break;
            case 12: hms.showStatistics();      break;
            case  0: cout << "\n  Goodbye!\n\n"; return 0;
            default: cout << "  Invalid choice. Try again.\n";
        }
    }
}
