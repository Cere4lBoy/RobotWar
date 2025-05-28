#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <memory>
#include <sstream>
#include <windows.h>
#include <set>

using namespace std;

enum UpgradeArea { NONE, MOVE, SHOOT, SEE };

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

// Abstract class for movement
class MovingRobot {
public:
    virtual void move(int dx, int dy, int maxWidth, int maxHeight, Logger* logger) = 0;
    virtual ~MovingRobot() = default;
};

// Abstract class for shooting
class ShootingRobot {
public:
    virtual void fire(int targetX, int targetY, Logger* logger) = 0;
    virtual ~ShootingRobot() = default;
};

// Abstract class for vision
class SeeingRobot {
public:
    virtual void look(int offsetX, int offsetY, Logger* logger) = 0;
    virtual ~SeeingRobot() = default;
};

// Abstract class for strategy/thinking
class ThinkingRobot {
public:
    virtual void think(class Battlefield* battlefield, int maxWidth, int maxHeight, Logger* logger) = 0;
    virtual ~ThinkingRobot() = default;
};


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

class Battlefield {
private:
    size_t currentRobotIndex = 0;
    int steps = 0;
    //Default Value
    int width = 20;
    int height = 10;
    vector<unique_ptr<Robot>> robots;
    Logger* logger = nullptr;

public:
    void setLogger(Logger* logPtr) {
        logger = logPtr;
    }

    bool loadConfig(const string& filename);
    void runStep();
    void display() const;
    void checkAndHitRobot(int x, int y, Robot* shooter);
    int getSteps() const { return steps; }
};

class GenericRobot : public Robot, public MovingRobot, public ShootingRobot, public SeeingRobot, public ThinkingRobot {
private:
    int shells = 10;
    set<UpgradeArea> chosenUpgrades;

    bool hasHide = false;
    bool hasJump = false;
    bool hasLongShot = false;
    bool hasSemiAuto = false;
    bool hasThirtyShot = false;
    bool hasScout = false;
    bool hasTrack = false;

    int hideCount = 3;
    int jumpCount = 3;
    int scoutCount = 3;
    int trackerCount = 3;

public:
    bool isHideBot() const { return hasHide; }
    bool isJumpBot() const { return hasJump; }
    bool isLongShot() const { return hasLongShot; }
    bool isSemiAuto() const { return hasSemiAuto; }
    bool isThirtyShot() const { return hasThirtyShot; }
    bool isScout() const { return hasScout; }
    bool isTrackBot() const { return hasTrack; }

    GenericRobot(string name, int x, int y) : Robot(name, x, y) {}

    void applyUpgrade(const string& upgradeName);

    void move(int dx, int dy, int maxWidth, int maxHeight, Logger* logger) override {
        int oldX = positionX, oldY = positionY;
        positionX = max(0, min(positionX + dx, maxWidth - 1));
        positionY = max(0, min(positionY + dy, maxHeight - 1));

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

    void think(Battlefield* battlefield, int maxWidth, int maxHeight, Logger* logger) override {
        if (logger) logger->log(name + " is thinking...");
        cout << name << " is thinking..." << endl;

        // Use JumpBot
        if (hasJump && jumpCount > 0 && (rand() % 10 == 0)) {
            positionX = rand() % maxWidth;
            positionY = rand() % maxHeight;
            jumpCount--;

            stringstream ss;
            ss << name << " used JumpBot ability to jump to (" << positionX << "," << positionY << ")";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
            return;
        }

        // Use HideBot
        if (hasHide && hideCount > 0 && (rand() % 10 == 0)) {
            hideCount--;

            stringstream ss;
            ss << name << " used HideBot ability and is hiding this turn.";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
            return;
        }

        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;
        int tx = positionX + dx;
        int ty = positionY + dy;

        // Use ScoutBot
        if (hasScout && scoutCount > 0 && (rand() % 10 == 0)) {
            scoutCount--;

            stringstream ss;
            ss << name << " used ScoutBot to scan the entire battlefield.";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
            // Optional: implement scan display logic here
        } else {
            look(tx, ty, logger);
        }

        // FIRE logic with upgrade + hit check
        if (shells <= 0) {
            cout << name << " is out of ammo!\n";
            if (logger) logger->log(name + " is out of ammo!");
        } else {
            if (hasSemiAuto) {
                for (int i = 0; i < 3 && shells > 0; ++i) {
                    shells--;
                    stringstream ss;
                    ss << name << " fires at (" << tx << "," << ty << "). Shells left: " << shells;
                    cout << ss.str() << endl;
                    if (logger) logger->log(ss.str());
                    battlefield->checkAndHitRobot(tx, ty, this);
                }
            } else if (hasLongShot) {
                int range = 1 + rand() % 3;
                int lx = positionX + dx * range;
                int ly = positionY + dy * range;
                shells--;
                stringstream ss;
                ss << name << " fires (LongShot) at (" << lx << "," << ly << "). Shells left: " << shells;
                cout << ss.str() << endl;
                if (logger) logger->log(ss.str());
                battlefield->checkAndHitRobot(lx, ly, this);
            } else {
                shells--;
                stringstream ss;
                ss << name << " fires at (" << tx << "," << ty << "). Shells left: " << shells;
                cout << ss.str() << endl;
                if (logger) logger->log(ss.str());
                battlefield->checkAndHitRobot(tx, ty, this);
            }
        }

        // Normal move (if not used jump)
        if (rand() % 2 == 0) {
            move(dx, dy, maxWidth, maxHeight, logger);
        }
    }
};

void GenericRobot::applyUpgrade(const string& upgradeName) {
    if (chosenUpgrades.size() >= 3) {
        cout << name << " cannot be upgraded anymore.\n";
        return;
    }

    if (upgradeName == "HideBot") {
        if (chosenUpgrades.count(UpgradeArea::MOVE)) {
            cout << name << " already chose a movement upgrade.\n";
            return;
        }
        hasHide = true;
        hideCount = 3;
        chosenUpgrades.insert(UpgradeArea::MOVE);
        cout << name << " upgraded to HideBot!\n";
    } else if (upgradeName == "JumpBot") {
        if (chosenUpgrades.count(UpgradeArea::MOVE)) {
            cout << name << " already chose a movement upgrade.\n";
            return;
        }
        hasJump = true;
        jumpCount = 3;
        chosenUpgrades.insert(UpgradeArea::MOVE);
        cout << name << " upgraded to JumpBot!\n";
    } else if (upgradeName == "LongShotBot") {
        if (chosenUpgrades.count(UpgradeArea::SHOOT)) {
            cout << name << " already chose a shooting upgrade.\n";
            return;
        }
        hasLongShot = true;
        chosenUpgrades.insert(UpgradeArea::SHOOT);
        cout << name << " upgraded to LongShotBot!\n";
    } else if (upgradeName == "SemiAutoBot") {
        if (chosenUpgrades.count(UpgradeArea::SHOOT)) {
            cout << name << " already chose a shooting upgrade.\n";
            return;
        }
        hasSemiAuto = true;
        chosenUpgrades.insert(UpgradeArea::SHOOT);
        cout << name << " upgraded to SemiAutoBot!\n";
    } else if (upgradeName == "ThirtyShotBot") {
        if (chosenUpgrades.count(UpgradeArea::SHOOT)) {
            cout << name << " already chose a shooting upgrade.\n";
            return;
        }
        shells = 30;
        hasThirtyShot = true;
        chosenUpgrades.insert(UpgradeArea::SHOOT);
        cout << name << " upgraded to ThirtyShotBot with 30 shells!\n";
    } else if (upgradeName == "ScoutBot") {
        if (chosenUpgrades.count(UpgradeArea::SEE)) {
            cout << name << " already chose a vision upgrade.\n";
            return;
        }
        hasScout = true;
        scoutCount = 3;
        chosenUpgrades.insert(UpgradeArea::SEE);
        cout << name << " upgraded to ScoutBot!\n";
    } else if (upgradeName == "TrackBot") {
        if (chosenUpgrades.count(UpgradeArea::SEE)) {
            cout << name << " already chose a vision upgrade.\n";
            return;
        }
        hasTrack = true;
        trackerCount = 3;
        chosenUpgrades.insert(UpgradeArea::SEE);
        cout << name << " upgraded to TrackBot!\n";
    } else {
        cout << "Unknown upgrade: " << upgradeName << "\n";
    }
}



bool Battlefield::loadConfig(const string& filename) {
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

void Battlefield::runStep() {
    if (robots.empty()) return;
    clearScreen();
    if (currentRobotIndex >= robots.size()) currentRobotIndex = 0;

    // Cast to ThinkingRobot
    ThinkingRobot* thinker = dynamic_cast<ThinkingRobot*>(robots[currentRobotIndex].get());
    if (thinker) {
        thinker->think(this, width, height, logger);
    } else {
        cerr << "Error: Robot does not implement ThinkingRobot!" << endl;

    }
    currentRobotIndex++;
    display();
}

void Battlefield::checkAndHitRobot(int x, int y, Robot* shooter) {
    for (auto it = robots.begin(); it != robots.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y) {
            if (shooter == it->get()) {
                cout << "You can't shoot yourself!\n";
                return;
            }

            cout << "Target hit! " << (*it)->getName() << " was destroyed!\n";
            GenericRobot* gr = dynamic_cast<GenericRobot*>(shooter);
            if (gr) {
                vector<string> upgrades;
                if (!gr->isHideBot() && !gr->isJumpBot()) upgrades.push_back("HideBot"), upgrades.push_back("JumpBot");
                if (!gr->isLongShot() && !gr->isSemiAuto() && !gr->isThirtyShot()) upgrades.push_back("SemiAutoBot"), upgrades.push_back("ThirtyShotBot");
                if (!gr->isScout() && !gr->isTrackBot()) upgrades.push_back("ScoutBot"), upgrades.push_back("TrackBot");

                if (!upgrades.empty()) {
                    string chosen = upgrades[rand() % upgrades.size()];
                    gr->applyUpgrade(chosen);
                }
            }

            robots.erase(it);
            return;
        }
    }
    cout << "Missed! No robot at (" << x << "," << y << ").\n";
}

void Battlefield::display() const {
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
        Sleep(100);
    }

    logger.log("Simulation ended.");
    cout << "\nSimulation ended.\n";
    return 0;
}
