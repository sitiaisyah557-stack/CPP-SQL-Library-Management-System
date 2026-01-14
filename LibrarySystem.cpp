#include <iostream>
#include <string>
#include <mysql.h>
#include <sstream> // Used for building SQL strings

using namespace std;

// Global database connection object
MYSQL* conn;

// --- Database Connection ---
bool connectDatabase() {
    conn = mysql_init(0);


    conn = mysql_real_connect(conn, "localhost", "root", "@Ka1105ka", "library_system", 3306, NULL, 0);

    if (conn) {
        return true;
    }
    else {
        cout << ">> DB Connection Failed: " << mysql_error(conn) << endl;
        return false;
    }
}
// --- 功能 1: 查书 (Search Books) ---
// --- 功能: 查书 (Search Books) [升级版] ---
void searchBooks() {
    cout << "\n--- SEARCH BOOK ---" << endl;
    // 提示用户：可以直接回车查看全部
    cout << "Enter keyword (Press ENTER to view ALL books): ";

    cin.ignore();
    string keyword;
    getline(cin, keyword);

    stringstream sql;
    // 如果没输东西，或者输入了 "all"，都查全部
    if (keyword.empty() || keyword == "all") {
        sql << "SELECT * FROM books";
    }
    else {
        sql << "SELECT * FROM books WHERE title LIKE '%" << keyword << "%'";
    }

    if (mysql_query(conn, sql.str().c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        MYSQL_RES* result = mysql_store_result(conn);

        cout << "\n[ID] \t [Title] \t\t [Author] \t [Status]" << endl;
        cout << "--------------------------------------------------------" << endl;

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            string status = (string(row[4]) == "1") ? "Available" : "Borrowed";
            // 使用 \t 制表符稍微对齐一下，虽然控制台很难完美对齐
            cout << row[0] << " \t " << row[1] << " \t " << row[2] << " \t " << status << endl;
        }
        cout << "--------------------------------------------------------" << endl;
        mysql_free_result(result);
    }
    cout << "(Press Enter to return)";
    cin.get();
}

// --- 功能 2: 录入新书 (Add Book) - 管理员专用 ---
// --- 功能: 录入新书 (Add Book) [升级版] ---
void addBook() {
    cout << "\n--- ADD NEW BOOK ---" << endl;

    // 清理输入缓冲区，防止之前留下的回车键导致跳过输入
    // cin.ignore() 有时候如果不确定缓冲区状态，放在getline前比较保险
    string dummy; getline(cin, dummy);

    string title, author, category;

    cout << "Enter Book Title: ";
    getline(cin, title);
    if (title.empty()) { cout << ">> Title cannot be empty!" << endl; system("pause"); return; }

    cout << "Enter Author: ";
    getline(cin, author);

    cout << "Enter Category (e.g. CS, Novel): ";
    getline(cin, category);

    stringstream sql;
    // 插入数据，默认 status 为 1 (Available)
    sql << "INSERT INTO books (title, author, category, status) VALUES ('"
        << title << "', '" << author << "', '" << category << "', 1)";

    if (mysql_query(conn, sql.str().c_str()) == 0) {
        cout << "\n**************************************************" << endl;
        cout << ">> [SUCCESS] New book added to inventory!" << endl;
        cout << ">> Title: " << title << endl;
        cout << "**************************************************" << endl;
    }
    else {
        cout << ">> [Error] Failed to add book: " << mysql_error(conn) << endl;
    }
    system("pause"); // 暂停让我们看到结果
}

// --- 功能: 录入新学生 (Add Student) [新增] ---
void addStudent() {
    cout << "\n--- REGISTER NEW STUDENT ---" << endl;

    int id;
    string name;

    cout << "Enter New Student ID (e.g. 1003): ";
    if (!(cin >> id)) {
        cout << ">> Invalid ID format!" << endl;
        cin.clear(); cin.ignore(1000, '\n');
        system("pause"); return;
    }

    // 处理换行符，因为混合使用了 cin >> 和 getline
    cin.ignore();

    cout << "Enter Student Name: ";
    getline(cin, name);
    if (name.empty()) { cout << ">> Name cannot be empty!" << endl; system("pause"); return; }

    stringstream sql;
    // 默认密码设为 123
    sql << "INSERT INTO users (user_id, name, password) VALUES ("
        << id << ", '" << name << "', '123')";

    if (mysql_query(conn, sql.str().c_str()) == 0) {
        cout << "\n**************************************************" << endl;
        cout << ">> [SUCCESS] New student registered!" << endl;
        cout << ">> ID: " << id << " | Name: " << name << endl;
        cout << ">> Default Password: 123" << endl;
        cout << "**************************************************" << endl;
    }
    else {
        // 如果ID重复（Primary Key冲突），MySQL会报错
        cout << ">> [Error] Registration failed." << endl;
        cout << ">> Reason: " << mysql_error(conn) << endl; // 很可能是ID重复
    }
    system("pause");
}

// --- 功能: 显示所有学生信息 (Show All Students) ---
void showAllStudents() {
    cout << "\n--- LIST OF ALL STUDENTS ---" << endl;

    // 我们只查 ID 和 Name，不查密码，保护隐私
    string sql = "SELECT user_id, name FROM users";

    if (mysql_query(conn, sql.c_str())) {
        cout << ">> DB Error: " << mysql_error(conn) << endl;
        system("pause"); return;
    }

    MYSQL_RES* res = mysql_store_result(conn);

    // 打印表头
    cout << "[ID] \t [Name]" << endl;
    cout << "--------------------------------" << endl;

    MYSQL_ROW row;
    int count = 0;
    while ((row = mysql_fetch_row(res))) {
        // row[0] 是 ID, row[1] 是 Name
        cout << row[0] << " \t " << row[1] << endl;
        count++;
    }
    cout << "--------------------------------" << endl;
    cout << "Total Students: " << count << endl;

    mysql_free_result(res);

    // 暂停，防止一闪而过
    cout << "\n(Press Enter to return)";
    system("pause");
}

// --- 功能 3: 借书 (Borrow Book) - 管理员操作 ---
void borrowBook() {
    cout << "\n--- ADMIN: BORROW BOOK ---" << endl;
    int userId, bookId;

    cout << "Enter User ID: ";
    if (!(cin >> userId)) { cin.clear(); cin.ignore(1000, '\n'); return; }

    cout << "Enter Book ID: ";
    if (!(cin >> bookId)) { cin.clear(); cin.ignore(1000, '\n'); return; }

    // 检查书的状态
    stringstream checkSql;
    checkSql << "SELECT status FROM books WHERE book_id = " << bookId;

    if (mysql_query(conn, checkSql.str().c_str())) {
        cout << "DB Error: " << mysql_error(conn) << endl;
        system("pause"); return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (mysql_num_rows(res) == 0) {
        cout << ">> Error: Book ID not found!" << endl;
        mysql_free_result(res);
        system("pause"); return; // 暂停
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    int status = atoi(row[0]);
    mysql_free_result(res);

    if (status == 0) {
        cout << ">> Failed: Book is already borrowed!" << endl;
        system("pause"); return; // 暂停
    }

    // 执行借书
    stringstream borrowSql;
    borrowSql << "INSERT INTO borrow_records (user_id, book_id, borrow_date) VALUES ("
        << userId << ", " << bookId << ", CURDATE())";
    mysql_query(conn, borrowSql.str().c_str());

    // 更新图书状态
    stringstream updateSql;
    updateSql << "UPDATE books SET status = 0 WHERE book_id = " << bookId;
    mysql_query(conn, updateSql.str().c_str());

    cout << ">> Success! Book " << bookId << " borrowed to User " << userId << "." << endl;
    system("pause"); // 关键：暂停让我们看到成功信息
}
// --- 功能 3.5: 学生自助借书 (Student Self-Service Borrow) ---
// --- 功能 3.5: 学生自助借书 (Student Self-Service Borrow) ---
// 替换掉原有的同名函数
void studentBorrowBook(int myUserId) {
    cout << "\n--- SELF-SERVICE BORROW ---" << endl;
    int bookId;

    cout << "Enter Book ID you want to borrow: ";
    // 增加输入检查
    if (!(cin >> bookId)) {
        cout << ">> [Invalid Input] Please enter a number." << endl;
        cin.clear(); cin.ignore(1000, '\n');
        system("pause"); // 暂停让人看清报错
        return;
    }

    // 1. 检查书是否存在
    stringstream checkSql;
    checkSql << "SELECT status, title FROM books WHERE book_id = " << bookId;

    if (mysql_query(conn, checkSql.str().c_str())) {
        cout << ">> [DB Error] " << mysql_error(conn) << endl;
        system("pause"); return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (mysql_num_rows(res) == 0) {
        // --- 反馈点 1: 书号输错了 ---
        cout << "\n--------------------------------------------------" << endl;
        cout << ">> [Failed] Book ID " << bookId << " does not exist!" << endl;
        cout << "--------------------------------------------------" << endl;
        mysql_free_result(res);
        system("pause"); // 暂停
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    int status = atoi(row[0]);
    string bookTitle = row[1]; // 获取书名，让反馈更友好
    mysql_free_result(res);

    // 2. 检查书是否被借走
    if (status == 0) {
        // --- 反馈点 2: 书已经被别人借走了 ---
        cout << "\n--------------------------------------------------" << endl;
        cout << ">> [Failed] The book \"" << bookTitle << "\" is currently UNAVAILABLE." << endl;
        cout << ">> It has been borrowed by someone else." << endl;
        cout << "--------------------------------------------------" << endl;
        system("pause"); // 暂停
        return;
    }

    // 3. 执行借书
    stringstream borrowSql;
    borrowSql << "INSERT INTO borrow_records (user_id, book_id, borrow_date) VALUES ("
        << myUserId << ", " << bookId << ", CURDATE())";

    if (mysql_query(conn, borrowSql.str().c_str())) {
        cout << ">> [Error] Failed to create record: " << mysql_error(conn) << endl;
        system("pause"); return;
    }

    // 4. 更新图书状态
    stringstream updateSql;
    updateSql << "UPDATE books SET status = 0 WHERE book_id = " << bookId;
    mysql_query(conn, updateSql.str().c_str());

    // --- 反馈点 3: 借书成功 ---
    cout << "\n**************************************************" << endl;
    cout << ">> [SUCCESS] You have borrowed: \"" << bookTitle << "\"" << endl;
    cout << ">> Please remember to return it on time." << endl;
    cout << "**************************************************" << endl;

    // 关键：这里暂停一下，让学生确认成功了再回去
    system("pause");
}

// --- 功能: 学生自助还书 (Student Return Book) ---
void studentReturnBook(int myUserId) {
    cout << "\n--- SELF-SERVICE RETURN ---" << endl;
    int bookId;
    cout << "Enter Book ID you want to return: ";
    if (!(cin >> bookId)) {
        cin.clear(); cin.ignore(1000, '\n');
        return;
    }

    // 1. 验证：这本书是否是当前用户借的，且尚未归还？
    stringstream checkSql;
    checkSql << "SELECT record_id FROM borrow_records "
        << "WHERE book_id = " << bookId
        << " AND user_id = " << myUserId  // 关键：必须是自己借的
        << " AND return_date IS NULL";

    if (mysql_query(conn, checkSql.str().c_str())) {
        cout << ">> DB Error: " << mysql_error(conn) << endl;
        system("pause"); return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (mysql_num_rows(res) == 0) {
        // 如果查不到，说明不是你借的，或者已经还过了
        cout << "\n--------------------------------------------------" << endl;
        cout << ">> [Failed] You cannot return this book." << endl;
        cout << ">> Reason: You didn't borrow it, or it's already returned." << endl;
        cout << "--------------------------------------------------" << endl;
        mysql_free_result(res);
        system("pause");
        return;
    }
    mysql_free_result(res);

    // 2. 执行还书
    stringstream updateRecordSql;
    updateRecordSql << "UPDATE borrow_records SET return_date = CURDATE() "
        << "WHERE book_id = " << bookId << " AND return_date IS NULL";
    mysql_query(conn, updateRecordSql.str().c_str());

    // 3. 恢复图书库存
    stringstream updateBookSql;
    updateBookSql << "UPDATE books SET status = 1 WHERE book_id = " << bookId;
    mysql_query(conn, updateBookSql.str().c_str());

    cout << "\n**************************************************" << endl;
    cout << ">> [SUCCESS] Book returned successfully!" << endl;
    cout << "**************************************************" << endl;
    system("pause");
}

// --- 功能 4: 还书 (Return Book) - 管理员操作 ---
void returnBook() {
    cout << "\n--- ADMIN: RETURN BOOK ---" << endl;
    int bookId;
    cout << "Enter Book ID to return: ";
    if (!(cin >> bookId)) { cin.clear(); cin.ignore(1000, '\n'); return; }

    // 查找未归还记录
    stringstream checkSql;
    checkSql << "SELECT record_id FROM borrow_records WHERE book_id = " << bookId << " AND return_date IS NULL";

    if (mysql_query(conn, checkSql.str().c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        system("pause"); return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (mysql_num_rows(res) == 0) {
        cout << ">> Error: This book is not currently borrowed." << endl;
        mysql_free_result(res);
        system("pause"); return; // 暂停
    }
    mysql_free_result(res);

    // 更新记录和状态
    stringstream updateRecordSql;
    updateRecordSql << "UPDATE borrow_records SET return_date = CURDATE() WHERE book_id = " << bookId << " AND return_date IS NULL";
    mysql_query(conn, updateRecordSql.str().c_str());

    stringstream updateBookSql;
    updateBookSql << "UPDATE books SET status = 1 WHERE book_id = " << bookId;
    mysql_query(conn, updateBookSql.str().c_str());

    cout << ">> Success! Book " << bookId << " returned." << endl;
    system("pause"); // 关键：暂停让我们看到成功信息
}

// --- 功能 5: 我的借阅 (My Records) - 学生专用 ---
void myBorrowRecords(int userId) {
    cout << "\n--- MY BORROW RECORDS ---" << endl;
    stringstream sql;
    // 联合查询：不仅要查记录，还要顺便把书名(title)从 books 表里查出来
    sql << "SELECT b.title, r.borrow_date, r.return_date "
        << "FROM borrow_records r "
        << "JOIN books b ON r.book_id = b.book_id "
        << "WHERE r.user_id = " << userId;

    if (mysql_query(conn, sql.str().c_str())) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    cout << "[Book Title] \t\t [Borrow Date] \t [Return Date]" << endl;
    cout << "--------------------------------------------------------" << endl;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        string retDate = (row[2] ? row[2] : "Not Returned"); // 如果是NULL显示未还
        cout << row[0] << " \t " << row[1] << " \t " << retDate << endl;
    }
    cout << "--------------------------------------------------------" << endl;
    mysql_free_result(res);

    cout << "(Press Enter to return)";
    cin.ignore(); cin.get();
}


// --- Admin Menu (Placeholder) ---

void adminMenu(string adminName) {
    while (true) {
        system("cls"); // 清屏
        cout << "\n========== ADMIN DASHBOARD (" << adminName << ") ==========" << endl;
        cout << "1. Add New Book" << endl;
        cout << "2. Register New Student" << endl;
        cout << "3. Show All Students" << endl; // [新增选项]
        cout << "4. Search / View Books" << endl;
        cout << "5. Manage Borrow/Return" << endl;
        cout << "0. Logout" << endl;
        cout << "============================================" << endl;
        cout << "Select option: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(1000, '\n');
            continue;
        }

        if (choice == 0) break;

        switch (choice) {
        case 1: addBook(); break;
        case 2: addStudent(); break;
        case 3: showAllStudents(); break; // 调用新函数
        case 4: searchBooks(); break;
        case 5:
            // 二级菜单：借书还是还书
            cout << "\n--- Circulation Mgmt ---" << endl;
            cout << "1. Borrow Book (For Student)" << endl;
            cout << "2. Return Book" << endl;
            cout << "Choice: ";
            int sub; cin >> sub;
            if (sub == 1) borrowBook();
            else if (sub == 2) returnBook();
            break;
        default:
            cout << "Invalid choice!" << endl;
            system("pause");
        }
    }
}


// --- Student Menu (Placeholder) ---
void studentMenu(int userId, string userName) {
    while (true) {
        system("cls"); // 保持界面清爽
        cout << "\n========== STUDENT MENU (" << userName << ") ==========" << endl;
        cout << "1. Search / View All Books" << endl; // 名字改得直观一点
        cout << "2. Borrow Book" << endl;
        cout << "3. Return Book" << endl;        // 新增：还书
        cout << "4. My Borrow Records" << endl;
        cout << "0. Logout" << endl;
        cout << "============================================" << endl;
        cout << "Select option: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(1000, '\n');
            continue;
        }

        if (choice == 0) break;

        switch (choice) {
        case 1: searchBooks(); break;
        case 2: studentBorrowBook(userId); break;
        case 3: studentReturnBook(userId); break; // 调用新增的还书功能
        case 4: myBorrowRecords(userId); break;
        default: cout << "Invalid choice!" << endl; system("pause");
        }
    }
}

// --- Login Logic ---
void loginSystem() {
    int role;
    while (true) {
        cout << "\n========== LIBRARY SYSTEM LOGIN ==========" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. Student Login" << endl;
        cout << "0. Exit" << endl;
        cout << "Select Role: ";

        // Handle invalid input to prevent infinite loops if user enters text
        if (!(cin >> role)) {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        if (role == 0) {
            cout << "Goodbye!" << endl;
            exit(0);
        }

        if (role == 1) { // Admin Login
            string username, password;
            cout << "Enter Username: "; cin >> username;
            cout << "Enter Password: "; cin >> password;

            stringstream sql;
            sql << "SELECT username FROM admins WHERE username = '" << username << "' AND password = '" << password << "'";

            if (mysql_query(conn, sql.str().c_str())) {
                cout << "Query Error: " << mysql_error(conn) << endl;
            }
            else {
                MYSQL_RES* result = mysql_store_result(conn);
                if (mysql_num_rows(result) > 0) {
                    cout << ">> Login Successful!" << endl;
                    mysql_free_result(result);
                    adminMenu(username);
                }
                else {
                    cout << ">> Login Failed: Invalid username or password." << endl;
                }
            }
        }
        else if (role == 2) { // Student Login
            int userId;
            string password;
            cout << "Enter Student ID: ";
            if (!(cin >> userId)) { // Simple check for number input
                cout << "Invalid ID format." << endl;
                cin.clear(); cin.ignore(1000, '\n');
                continue;
            }
            cout << "Enter Password: "; cin >> password;

            stringstream sql;
            // Make sure to match the column names in your database
            sql << "SELECT name FROM users WHERE user_id = " << userId << " AND password = '" << password << "'";

            if (mysql_query(conn, sql.str().c_str())) {
                cout << "Query Error: " << mysql_error(conn) << endl;
            }
            else {
                MYSQL_RES* result = mysql_store_result(conn);
                if (mysql_num_rows(result) > 0) {
                    MYSQL_ROW row = mysql_fetch_row(result);
                    string name = row[0] ? row[0] : "Student"; // Safety check
                    cout << ">> Welcome, " << name << "!" << endl;
                    mysql_free_result(result);
                    studentMenu(userId, name);
                }
                else {
                    cout << ">> Login Failed: ID not found or wrong password." << endl;
                }
            }
        }
    }
}

// --- Main Program ---
int main() {
    // Attempt to connect
    if (connectDatabase()) {
        cout << "Database Connected Successfully!" << endl;
        loginSystem();
    }

    // Keep window open for VS users
    system("pause");
    return 0;
}
