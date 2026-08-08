#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <cmath>
#include <limits>

using namespace std;

// Represents a vehicle parked in the garage
class Vehicle {
public:
    string licensePlate;
    string type; // "Car" or "Bike"
    chrono::time_point<chrono::system_clock> entryTime;

    Vehicle(string lp, string t) {
        licensePlate = lp;
        type = t;
        entryTime = chrono::system_clock::now();
    }

    // Constructor used when loading saved data from a file
    Vehicle(string lp, string t, long long epochSeconds) {
        licensePlate = lp;
        type = t;
        entryTime = chrono::system_clock::from_time_t(static_cast<time_t>(epochSeconds));
    }
};

// Represents an individual parking spot
class ParkingSpot {
public:
    int id;
    string type; // "Car" or "Bike"
    bool isOccupied;
    Vehicle* parkedVehicle;

    ParkingSpot(int spotId, string spotType) {
        id = spotId;
        type = spotType;
        isOccupied = false;
        parkedVehicle = nullptr;
    }

    ~ParkingSpot() {
        delete parkedVehicle;
    }
};

// Manages the operations, billing, and file storage of the garage
class ParkingGarage {
private:
    vector<ParkingSpot> spots;
    double totalRevenue;
    const string dbFilename = "garage_database.txt";

public:
    ParkingGarage(int carSpots, int bikeSpots) {
        totalRevenue = 0.0;
        int currentId = 1;

        for (int i = 0; i < carSpots; ++i) {
            spots.push_back(ParkingSpot(currentId++, "Car"));
        }
        for (int i = 0; i < bikeSpots; ++i) {
            spots.push_back(ParkingSpot(currentId++, "Bike"));
        }

        loadFromDatabase();
    }

    ~ParkingGarage() {
        saveToDatabase();
    }

    void parkVehicle(string lp, string type) {
        // Enforce basic validation
        if (type != "Car" && type != "Bike") {
            cout << "❌ Invalid vehicle type! Choose 'Car' or 'Bike'.\n";
            return;
        }

        // Search for an open spot matching the vehicle type
        for (auto& spot : spots) {
            if (!spot.isOccupied && spot.type == type) {
                spot.parkedVehicle = new Vehicle(lp, type);
                spot.isOccupied = true;
                cout << "✅ " << type << " successfully parked at Spot #" << spot.id << ".\n";
                saveToDatabase();
                return;
            }
        }
        cout << "⚠️ No available spots for a " << type << " at the moment.\n";
    }

    void removeVehicle(string lp) {
        for (auto& spot : spots) {
            if (spot.isOccupied && spot.parkedVehicle->licensePlate == lp) {
                auto now = chrono::system_clock::now();
                auto duration = chrono::duration_cast<chrono::seconds>(now - spot.parkedVehicle->entryTime).count();

                // For demonstration, 1 second in-game represents 1 hour of parking time
                double hours = max(1.0, ceil(static_cast<double>(duration)));
                double rate = (spot.type == "Car") ? 5.0 : 2.0; // $5/hr for cars, $2/hr for bikes
                double fee = hours * rate;

                totalRevenue += fee;

                cout << "\n--- 🧾 PARKING RECEIPT ---" << "\n";
                cout << "License Plate: " << lp << "\n";
                cout << "Vehicle Type:  " << spot.type << "\n";
                cout << "Spot Unlocked: #" << spot.id << "\n";
                cout << "Time Parked:   " << hours << " simulated hour(s)\n";
                cout << "Total Fee:     $" << fee << "\n";
                cout << "-------------------------\n";

                // Free up the spot memory
                delete spot.parkedVehicle;
                spot.parkedVehicle = nullptr;
                spot.isOccupied = false;

                saveToDatabase();
                return;
            }
        }
        cout << "❌ Vehicle with license plate " << lp << " not found in the garage.\n";
    }

    void displayStatus() const {
        cout << "\n--- 🅿️ GARAGE STATUS INTERFACE ---\n";
        int availableCar = 0, availableBike = 0;

        for (const auto& spot : spots) {
            if (!spot.isOccupied) {
                if (spot.type == "Car") availableCar++;
                else availableBike++;
            }

            cout << "Spot #" << spot.id << " [" << spot.type << "]: ";
            if (spot.isOccupied) {
                cout << "🔴 OCCUPIED BY (" << spot.parkedVehicle->licensePlate << ")\n";
            } else {
                cout << "🟢 VACANT\n";
            }
        }
        cout << "\n📊 Available Spots -> Cars: " << availableCar << " | Bikes: " << availableBike << "\n";
        cout << "💰 Total Revenue Maintained: $" << totalRevenue << "\n";
        cout << "---------------------------------\n";
    }

    void saveToDatabase() {
        ofstream outFile(dbFilename);
        if (!outFile) return;

        // Save total earnings on the first line
        outFile << totalRevenue << "\n";

        // Save active vehicle data structure
        for (const auto& spot : spots) {
            if (spot.isOccupied) {
                // Convert chrono time_point to numeric epoch timestamp for clean file writing
                auto timeT = chrono::system_clock::to_time_t(spot.parkedVehicle->entryTime);
                outFile << spot.id << " "
                        << spot.parkedVehicle->licensePlate << " "
                        << spot.parkedVehicle->type << " "
                        << static_cast<long long>(timeT) << "\n";
            }
        }
        outFile.close();
    }

    void loadFromDatabase() {
        ifstream inFile(dbFilename);
        if (!inFile) return; // No database file exists yet, start fresh

        if (!(inFile >> totalRevenue)) {
            totalRevenue = 0.0;
        }

        int spotId;
        string lp, type;
        long long epochSeconds;

        // Read active vehicles line by line back into structural memory
        while (inFile >> spotId >> lp >> type >> epochSeconds) {
            for (auto& spot : spots) {
                if (spot.id == spotId) {
                    spot.parkedVehicle = new Vehicle(lp, type, epochSeconds);
                    spot.isOccupied = true;
                    break;
                }
            }
        }
        inFile.close();
    }
};

// Safe helper function to clear bad input choices from stream buffers
void clearInputError() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    // Instantiate a garage config with 4 car spots and 3 bike spots
    ParkingGarage garage(4, 3);
    int choice = 0;

    while (true) {
        cout << "\n===== AUTOMATED GARAGE MANAGEMENT =====\n";
        cout << "1. Park a Vehicle (Check-In)\n";
        cout << "2. Remove a Vehicle (Check-Out)\n";
        cout << "3. Display Garage Layout Status\n";
        cout << "4. Save & Exit Program\n";
        cout << "Enter selection (1-4): ";

        if (!(cin >> choice)) {
            cout << "❌ Invalid numeric entry.\n";
            clearInputError();
            continue;
        }

        if (choice == 4) {
            cout << "💾 Finalizing state database... Goodbye!\n";
            break;
        }

        string lp, type;
        switch (choice) {
            case 1:
                cout << "Enter License Plate (no spaces): ";
                cin >> lp;
                cout << "Enter Vehicle Type (Car/Bike): ";
                cin >> type;
                garage.parkVehicle(lp, type);
                break;

            case 2:
                cout << "Enter License Plate to process exit: ";
                cin >> lp;
                garage.removeVehicle(lp);
                break;

            case 3:
                garage.displayStatus();
                break;

            default:
                cout << "❌ Invalid Choice. Choose between options 1-4.\n";
        }
    }
    return 0;
}

