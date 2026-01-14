# 📚 C++ Library Management System

Based on C++ and MySQL, implementing a console-based library management system with Admin and Student roles.
(基于 C++ 和 MySQL 的控制台图书馆管理系统)

## 🎥 Project Demo (演示视频)
[点击这里观看功能演示视频](https://pan.baidu.com/s/1rzPOYhJlyA5cV9E0SGpVWQ 提取码: bcj6)

## ✨ Features (功能列表)
* **Database Integration**: Uses MySQL Connector/C for data persistence.
* **Admin Module**:
    * Add new books & Register new students.
    * View all students list.
    * Manage borrowing/returning (Book circulation).
* **Student Module**:
    * Self-service borrowing & returning (with ownership validation).
    * Search books & View borrow records.
* **System**:
    * Secure Login System.
    * Input validation & Friendly UI feedback.

## 🛠️ How to Run (运行指南)
1.  **Environment**: Visual Studio 2022 + MySQL 8.0.
2.  **Dependencies**:
    * This project requires `mysql.h` and `libmysql.lib`.
    * Please configure the Include Directories and Library Directories in VS project settings.
3.  **Database Setup**:
    * Open MySQL Workbench.
    * Run the script `library_setup.sql` (included in this repo) to create tables and default data.
    * Update the database password in `main.cpp` (Line 16).
4.  **Build & Run**: Compile the `main.cpp` and enjoy!

## 📝 Author
* Name: HuangXiangpeng JiChangxuan HeYichen LiuHongxi
