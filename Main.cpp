#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Robot {
private:
    string name;
    int positionX;
    int positionY;
    char symbol;

public:
    Robot(string name, int x, int y) : name(name), positionX(x), positionY(y) {
        symbol = name.empty() ? 'R' : toupper(name[0]);
    }

    string getName() const { return name; }
    int getX() const { return positionX; }
    int getY() const { return positionY; }
    char getSymbol() const { return symbol; }
};

class Battlefield {
private:
    int width = 40;  // Default preset width
    int height = 20; // Default preset height
    vector<Robot> robots;

public:
    Battlefield() {
        // Add preset robots
        robots.emplace_back("Alpha", 5, 5);
        robots.emplace_back("Bravo", 35, 5);
        robots.emplace_back("Charlie", 5, 15);
        robots.emplace_back("Delta", 35, 15);
    }

    void display() const {
        // Display top border
        cout << "+";
        for (int x = 0; x < width; x++) cout << "-";
        cout << "+" << endl;

        // Display battlefield with robots
        for (int y = 0; y < height; y++) {
            cout << "|"; // Left border
            for (int x = 0; x < width; x++) {
                bool robotHere = false;
                for (const auto& robot : robots) {
                    if (robot.getX() == x && robot.getY() == y) {
                        cout << robot.getSymbol();
                        robotHere = true;
                        break;
                    }
                }
                if (!robotHere) cout << " ";
            }
            cout << "|" << endl; // Right border
        }

        // Display bottom border
        cout << "+";
        for (int x = 0; x < width; x++) cout << "-";
        cout << "+" << endl;

        // Display robot legend
        cout << "\nRobot Legend:" << endl;
        for (const auto& robot : robots) {
            cout << robot.getSymbol() << ": " << robot.getName()
                 << " (" << robot.getX() << "," << robot.getY() << ")" << endl;
        }
    }

    void fireAt(int x, int y){
        bool hit=false;
        for (auto it=robots.begin(); it !=robots.end(); ++it){
            if (it->getX() == x && it->getY() == y){
                cout<<"Robot hit"<< it->getName() << "at("<< x << ","<< y <<")!" << endl;
                robots.erase(it);
                hit = (true);
                break;
            }
        }
         if (!hit){
            cout << "No robot at ("<< x << "," << y <<")." << endl;
         }
        }
};

void showMainMenu() {
    cout << "\n=== ROBOT WAR SIMULATOR ===" << endl;
    cout << "1. Start simulation" << endl;
    cout << "2. Fire at location" << endl;
    cout << "3. Exit" << endl;
    cout << "Enter your choice: ";
}

int main() {
    Battlefield battlefield; // Create preset battlefield with robots
    int choice;

    do {
        showMainMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "\nStarting simulation with preset battlefield and robots...\n";
                battlefield.display();
                cout << "\nSimulation running... (Press any key to return to menu)";
                cin.ignore();
                cin.get();
                break;
            case 2:{
                int fireX,fireY;
                cout << "\nEnter X coordinate to fire (0-" << 39 <<"):";
                cin >>fireX;
                cout << "\nEnter y coordinate to fire (0-" << 19 <<"):";
                cin >>fireY;
                battlefield.fireAt(fireX,fireY);
                break;
            }
            case 3:
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice! Try again." << endl;
        }
    } while (choice != 2);

    return 0;
}
