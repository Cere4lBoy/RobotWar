/**********|**********|**********|
Program: main.cpp
Course: Data Structures and Algorithms
Trimester: 2410
STUDENT 1:
    Name: Iman Thaqif
    ID: 242UC245G9
STUDENT 2:
    Name:MOHAMMAD IEMAN BIN ZAHARI
    ID: 242UC244SN
STUDENT 3:
    Name:
    ID:
STUDENT 4:
    Name:
    ID:
Lecture Section: TC
Tutorial Section: TT1L
Email: abc123@yourmail.com
Phone: 018-1234567
**********|**********|**********/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <sstream>
#include <windows.h>

using namespace std;

class Logger {
private:
    ofstream logFile;

public:
    Logger(const string& filename) {
        logFile.open(filename, ios::out);
        if (!logFile.is_open()) {
            cerr << "Failed to open log file.\n";
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void log(const string& message) {
        if (logFile.is_open()) {
            logFile << message << endl;
        }
    }
};

void clearScreen() {
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    FillConsoleOutputCharacter(hStdOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
    SetConsoleCursorPosition(hStdOut, coord);
}

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

    virtual void move(int dx, int dy, Logger* logger) = 0;
    virtual void fire(int targetX, int targetY, Logger* logger) = 0;
    virtual void look(int originalX, int originalY, Logger* logger) = 0;
    virtual void think(Logger* logger) = 0;

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

class GenericRobot : public Robot {
private:
    int shells = 10;

public:
    GenericRobot(string name, int x, int y) : Robot(name, x, y) {}

    void move(int dx, int dy, Logger* logger) override {
        int oldX = positionX, oldY = positionY;
        positionX = max(0, min(positionX + dx, 49));
        positionY = max(0, min(positionY + dy, 39));
        stringstream ss;
        ss << name << " moved from (" << oldX << "," << oldY << ") to (" << positionX << "," << positionY << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void fire(int targetX, int targetY, Logger* logger) override {
        if (shells <= 0) {
            cout << name << " is out of ammo!\n";
            if (logger) logger->log(name + " is out of ammo!");
            return;
        }
        shells--;
        stringstream ss;
        ss << name << " fires at (" << targetX << "," << targetY << "). Shells left: " << shells;
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void look(int x, int y, Logger* logger) override {
        stringstream ss;
        ss << name << " is looking at (" << x << "," << y << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void think(Logger* logger) override {
        if (logger) logger->log(name + " is thinking...");

        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;

        if (rand() % 2 == 0) {
            look(positionX + dx, positionY + dy, logger);
            fire(positionX + dx, positionY + dy, logger);
        } else {
            move(dx, dy, logger);
        }
    }
};

class Battlefield {
private:
    size_t currentRobotIndex = 0;
    int steps = 0;
    int width = 20;
    int height = 10;
    vector<unique_ptr<Robot>> robots;
    Logger* logger = nullptr;

public:
    void setLogger(Logger* logPtr) {
        logger = logPtr;
    }

    bool loadConfig(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            if (line.find("M by N") != string::npos) {
                sscanf(line.c_str(), "M by N : %d %d", &width, &height);
            } else if (line.find("steps:") != string::npos) {
                sscanf(line.c_str(), "steps: %d", &steps);
            } else if (line.find("GenericRobot") != string::npos) {
                string tag, name, xStr, yStr;
                istringstream iss(line);
                int x, y;
                iss >> tag >> name >> xStr >> yStr;
                x = (xStr == "random") ? rand() % width : stoi(xStr);
                y = (yStr == "random") ? rand() % height : stoi(yStr);
                robots.push_back(make_unique<GenericRobot>(name, x, y));
            }
        }
        return true;
    }

    int getSteps() const { return steps; }

    void runStep() {
        if (robots.empty()) return;
        clearScreen();
        if (currentRobotIndex >= robots.size()) currentRobotIndex = 0;
        robots[currentRobotIndex]->think(logger);
        currentRobotIndex++;
        display();
    }

    void display() const {
        cout << "    ";
        for (int x = 0; x < width; x++) cout << setw(2) << x;
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
};

int main() {
    srand(static_cast<unsigned>(time(0)));

    Logger logger("log.txt");

    Battlefield battlefield;
    battlefield.setLogger(&logger);

    if (!battlefield.loadConfig("config.txt")) {
        cout << "Failed to load config. Exiting.\n";
        return 1;
    }

    logger.log("Simulation started.");
    logger.log("Steps: " + to_string(battlefield.getSteps()));

    battlefield.display();

    for (int step = 0; step < battlefield.getSteps(); ++step) {
        logger.log("--- Step " + to_string(step + 1) + " ---");
        battlefield.runStep();
        Sleep(1000);
    }

    logger.log("Simulation ended.");
    cout << "\nSimulation ended.\n";
    return 0;
}
