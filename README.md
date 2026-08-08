# Automated-Parking-Garage
# 🅿️ Automated Parking Garage Management System

A production-grade, object-oriented console application written in Modern C++ to manage real-time tracking, time-based billing calculations, and data persistence for commercial parking lots.

## 🚀 Key Architectural Features
- **Data Persistence:** Automatically tracks active vehicle state changes and saves session logs using secure file stream I/O (`garage_database.txt`).
- **Simulated Time Engine:** Integrated with the `<chrono>` library to instantly mimic complex real-time billing structures safely.
- **Robust Exception Handling:** Integrated strict data-stream input fail-safes preventing crash loops caused by bad console entries.

## 🛠️ Tech Stack & Requirements
- **Language:** C++17 or higher
- **Libraries Used:** `<chrono>`, `<vector>`, `<fstream>`, `<limits>`
- **Supported OS:** Windows, macOS, Linux

## 📥 Compilation & Run Guide
To run the system locally, use any standard C++ compiler in your terminal:
```bash
g++ -std=c++17 main.cpp -o parking_garage
./parking_garage
```
