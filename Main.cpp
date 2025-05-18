#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <memory>

using namespace std;

// Abstract base class
class Robot {
protected:
    string name;
    int positionX;
    int positionY;
    char symbol;

public:
    Robot(string name, int x, int y) : name(name), positionX(x), positionY(y) {
        symbol = name.empty() ? 'R' : toupper(name[0]);
    }
    virtual ~Robot() = default;

    virtual void move(int dx, int dy) = 0;
    virtual void fire(int targetX, int targetY) = 0;

    string getName() const { return name; }
    int getX() const { return positionX; }
    int getY() const { return positionY; }
    char getSymbol() const { return symbol; }

    bool operator==(const Robot& other) const {
        return positionX == other.positionX && positionY == other.positionY;
    }

    friend ostream& operator<<(ostream& os, const Robot& robot) {
        os << robot.name << " (" << robot.symbol << ") at ["
           << robot.positionX << "," << robot.positionY << "]";
        return os;
    }
};

// Derived class
class GenericRobot : public Robot {
private:
    int shells = 10;

public:
    GenericRobot(string name, int x, int y) : Robot(name, x, y) {}

    void move(int dx, int dy) override {
        positionX += dx;
        positionY += dy;
        cout << name << " moved to (" << positionX << "," << positionY << ")\n";
    }

    void fire(int targetX, int targetY) override {
        if (shells <= 0) {
            cout << name << " is out of ammo!\n";
            return;
        }
        shells--;
        cout << name << " fires at (" << targetX << "," << targetY
             << "). Shells left: " << shells << "\n";
    }
};

class Battlefield {
private:
    int width = 20;
    int height = 10;
    vector<unique_ptr<Robot>> robots;

public:
    Battlefield() {
        robots.push_back(make_unique<GenericRobot>("Thaqif", 5, 4));
        robots.push_back(make_unique<GenericRobot>("Marcuz", 16, 2));
        robots.push_back(make_unique<GenericRobot>("Rara", 3, 7));
        robots.push_back(make_unique<GenericRobot>("Ieman", 16, 7));
    }

    void display() const {
        cout << "    ";
        for (int x = 0; x < width; x++) {
            cout << setw(2) << x;
        }
        cout << "\n   +";
        for (int x = 0; x < width; x++) cout << "--";
        cout << "+\n";

        for (int y = 0; y < height; y++) {
            cout << setw(2) << y << " |";
            for (int x = 0; x < width; x++) {
                bool robotHere = false;
                for (const auto& robot : robots) {
                    if (robot->getX() == x && robot->getY() == y) {
                        cout << " " << robot->getSymbol();
                        robotHere = true;
                        break;
                    }
                }
                cout << (robotHere ? "" : " .");
            }
            cout << " |\n";
        }

        cout << "   +";
        for (int x = 0; x < width; x++) cout << "--";
        cout << "+\n";

        cout << "\nRobot Status:\n";
        for (const auto& robot : robots) {
            cout << *robot << "\n";
        }
    }

    void checkAndHitRobot(int x, int y, Robot* shooter) {
        for (auto it = robots.begin(); it != robots.end(); ++it) {
            if ((*it)->getX() == x && (*it)->getY() == y) {
                if (shooter == it->get()) {//this code is to show that we are shooting ourselves
                    cout << "You can't shoot yourself!\n";
                    return;
                }
                cout << "Target hit! " << (*it)->getName() << " was destroyed!\n";
                robots.erase(it);//delete the robot that is being shoot need to be update later since only 70% chance should hit
                return;
            }
        }
        cout << "Missed! No robot at (" << x << "," << y << ").\n";
    }

    void commandLoop() {
        string command;
        while (true) {
            cout << "\nEnter command (help/quit/move/fire): ";
            getline(cin, command);
            transform(command.begin(), command.end(), command.begin(), ::tolower);

            if (command == "quit") {
                cout << "Returning to main menu...\n";
                break;
            } else if (command == "help") {
                cout << "Commands:\n"
                     << "move [direction]      - Move robot (e.g. 'move up')\n"
                     << "fire [x] [y]          - Fire at coordinates\n"
                     << "quit                  - Exit to menu\n";
            } else if (command.find("move") == 0) {
                // Example: always moves first robot down by 1
                robots[0]->move(0, 1);
            } else if (command.find("fire") == 0) {//this code will find the word fire %d show the location number of it
                int tx, ty;
                if (sscanf(command.c_str(), "fire %d %d", &tx, &ty) == 2) {
                    robots[0]->fire(tx, ty);
                    checkAndHitRobot(tx, ty, robots[0].get());
                } else {
                    cout << "Invalid fire command! Type 'help' for options.\n";
                }
            }

            display();  // Update battlefield after each command
        }
    }
};

void showMainMenu() {
    cout << "\n=== ROBOT WAR SIMULATOR ===\n"
         << "1. Start simulation\n"
         << "2. Commands and Guide\n"
         << "3. Exit\n"
         << "Enter your choice: ";
}

int main() {
    Battlefield battlefield;
    int choice;

    do {
        showMainMenu();
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                cout << "\nStarting simulation...\n";
                battlefield.display();
                battlefield.commandLoop();
                break;
            case 2:
                cout << "Command Guide:\n"
                     << "move [direction] - Change robot position\n"
                     << "fire [x] [y]     - Attack a location\n";
                break;
            case 3:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 3);

    return 0;
}
