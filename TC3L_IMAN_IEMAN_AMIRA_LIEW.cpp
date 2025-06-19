/**********|**********|**********|
Program: main.cpp
Course: Data Structures and Algorithms
Trimester: 2410
STUDENT 1:
    Name: Iman Thaqif
    ID: 242UC245G9
STUDENT 2:
    Name: MOHAMMAD IEMAN BIN ZAHARI
    ID: 242UC244SN
STUDENT 3:
    Name: AMIRA RAHEEMA BINTI MOHAMAD KAMAROL
    ID: 242UC244MB
STUDENT 4:
    Name: LIEW ZHI YONG
    ID: 242UC244TH
Lecture Section: TC3L
Tutorial Section: TT1L
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
    int lives;

public:
    Robot(string name, int x, int y) : name(name), positionX(x), positionY(y), lives(3) {
        symbol = name.empty() ? 'R' : toupper(name[0]);
    }
    virtual ~Robot() = default;

    string getName() const { return name; }
    int getX() const { return positionX; }
    int getY() const { return positionY; }
    int getLives() const { return lives; }
    void setLives(int l) { lives = 3; } //-- Setter
    void loseLife() { if (lives>0) --lives; }
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
    bool runStep();
    void display() const;
    void checkAndHitRobot(int x, int y, Robot* shooter);
    int getSteps() const { return steps; }
    void markSelfDestruct(Robot* robot);
    const vector<unique_ptr<Robot>>& getRobots() const { return robots; }

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
    int lastDx = 0;
    int lastDy = 0;

public:
    bool isHideBot() const { return hasHide; }
    bool isJumpBot() const { return hasJump; }
    bool isLongShot() const { return hasLongShot; }
    bool isSemiAuto() const { return hasSemiAuto; }
    bool isThirtyShot() const { return hasThirtyShot; }
    bool isScout() const { return hasScout; }
    bool isTrackBot() const { return hasTrack; }

    const set<UpgradeArea>& getChosenUpgrades() const {
        return chosenUpgrades;
    }

    GenericRobot(string name, int x, int y) : Robot(name, x, y) {}

    void applyUpgrade(const string& upgradeName, Logger* logger);


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

        // Get all robots through proper access
        const auto& allRobots = battlefield->getRobots();

        // 1. Use special abilities (JumpBot/HideBot)
        if (hasJump && jumpCount > 0 && (rand() % 10 == 0)) {
            positionX = rand() % maxWidth;
            positionY = rand() % maxHeight;
            jumpCount--;
            stringstream ss;
            ss << name << " used JumpBot ability to jump to (" << positionX << "," << positionY << ")";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
            return; // Jumping counts as the move for this turn
        }

        if (hasHide && hideCount > 0 && (rand() % 10 == 0)) {
            hideCount--;
            stringstream ss;
            ss << name << " used HideBot ability and is hiding this turn.";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
            return; // Hiding skips the rest of the turn
        }

        // 2. LOOK: Always scan the environment
        Robot* nearestEnemy = nullptr;
        int minDist = INT_MAX;
        for (const auto& robot : allRobots) {
            if (robot.get() != this) {
                int dist = abs(robot->getX() - positionX) + abs(robot->getY() - positionY);
                if (dist < minDist) {
                    minDist = dist;
                    nearestEnemy = robot.get();
                }
            }
        }

        // Use ScoutBot if available
        if (hasScout && scoutCount > 0 && (minDist > 3 || nearestEnemy == nullptr)) {
            scoutCount--;
            stringstream ss;
            ss << name << " used ScoutBot to scan the entire battlefield.";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
        }

        // Standard looking behavior
        int lookX = positionX, lookY = positionY;
        if (nearestEnemy) {
            lookX = nearestEnemy->getX();
            lookY = nearestEnemy->getY();
        } else {
            lookX += (rand() % 3) - 1;
            lookY += (rand() % 3) - 1;
        }
        look(lookX, lookY, logger);

        // 3. FIRE: Attempt to fire if conditions are right
        if (shells <= 0) {
            cout << name << " is out of ammo and will self-destruct!\n";
            if (logger) logger->log(name + " is out of ammo and will self-destruct!");
            battlefield->markSelfDestruct(this);
            return;
        }

        if (nearestEnemy && minDist <= (hasLongShot ? 4 : 1)) {
            if (hasSemiAuto) {
                for (int i = 0; i < 3 && shells > 0; ++i) {
                    fire(nearestEnemy->getX(), nearestEnemy->getY(), logger);
                    battlefield->checkAndHitRobot(nearestEnemy->getX(), nearestEnemy->getY(), this);
                }
            }
            else if (hasLongShot) {
                int dx = nearestEnemy->getX() - positionX;
                int dy = nearestEnemy->getY() - positionY;
                int tx = nearestEnemy->getX() + (dx > 0 ? 1 : -1);
                int ty = nearestEnemy->getY() + (dy > 0 ? 1 : -1);
                fire(tx, ty, logger);
                battlefield->checkAndHitRobot(tx, ty, this);
            }
            else {
                fire(nearestEnemy->getX(), nearestEnemy->getY(), logger);
                battlefield->checkAndHitRobot(nearestEnemy->getX(), nearestEnemy->getY(), this);
            }
        }

        // 4. MOVE: Always move to a new position
        int oldX = positionX, oldY = positionY;
        int moveDx = 0, moveDy = 0;

        if (nearestEnemy) {
            int dx = nearestEnemy->getX() - positionX;
            int dy = nearestEnemy->getY() - positionY;

            // Move toward enemy if far, away if too close
            if (minDist > 2) {
                moveDx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
                moveDy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
            }
            else if (minDist < 2) {
                moveDx = (dx > 0) ? -1 : (dx < 0) ? 1 : 0;
                moveDy = (dy > 0) ? -1 : (dy < 0) ? 1 : 0;
            }
        }
        else {
            // Random exploration
            moveDx = (rand() % 3) - 1;
            moveDy = (rand() % 3) - 1;
        }

        // Ensure we actually move to a new position
        if (moveDx == 0 && moveDy == 0) {
            // If no movement was calculated, pick a random direction
            do {
                moveDx = (rand() % 3) - 1;
                moveDy = (rand() % 3) - 1;
            } while (moveDx == 0 && moveDy == 0);
        }

        positionX = max(0, min(positionX + moveDx, maxWidth - 1));
        positionY = max(0, min(positionY + moveDy, maxHeight - 1));

        stringstream ss;
        ss << name << " moved from (" << oldX << "," << oldY << ") to (" << positionX << "," << positionY << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }
};

    void GenericRobot::applyUpgrade(const string& upgradeName, Logger* logger) {
        if (chosenUpgrades.size() >= 3) {
            string msg = name + " cannot be upgraded anymore.";
            cout << msg << endl;
            if (logger) logger->log(msg);
            return;
        }

        if (upgradeName == "HideBot") {
            if (chosenUpgrades.count(UpgradeArea::MOVE)) {
                string msg = name + " already chose a movement upgrade.";
                cout << msg << endl;
                if (logger) logger->log(msg);
                return;
            }
            hasHide = true;
            hideCount = 3;
            chosenUpgrades.insert(UpgradeArea::MOVE);
            string msg = name + " upgraded to HideBot!";
            cout << msg << endl;
            if (logger) logger->log(msg);

        } else if (upgradeName == "JumpBot") {
            if (chosenUpgrades.count(UpgradeArea::MOVE)) {
                string msg = name + " already chose a movement upgrade.";
                cout << msg << endl;
                if (logger) logger->log(msg);
                return;
            }
            hasJump = true;
            jumpCount = 3;
            chosenUpgrades.insert(UpgradeArea::MOVE);
            string msg = name + " upgraded to JumpBot!";
            cout << msg << endl;
            if (logger) logger->log(msg);

        } else if (upgradeName == "LongShotBot") {
            if (chosenUpgrades.count(UpgradeArea::SHOOT)) {
                string msg = name + " already chose a shooting upgrade.";
                cout << msg << endl;
                if (logger) logger->log(msg);
                return;
            }
            hasLongShot = true;
            chosenUpgrades.insert(UpgradeArea::SHOOT);
            string msg = name + " upgraded to LongShotBot!";
            cout << msg << endl;
            if (logger) logger->log(msg);

        } else if (upgradeName == "SemiAutoBot") {
            if (chosenUpgrades.count(UpgradeArea::SHOOT)) {
                string msg = name + " already chose a shooting upgrade.";
                cout << msg << endl;
                if (logger) logger->log(msg);
                return;
            }
            hasSemiAuto = true;
            chosenUpgrades.insert(UpgradeArea::SHOOT);
            string msg = name + " upgraded to SemiAutoBot!";
            cout << msg << endl;
            if (logger) logger->log(msg);

        } else if (upgradeName == "ThirtyShotBot") {
            if (chosenUpgrades.count(UpgradeArea::SHOOT)) {
                string msg = name + " already chose a shooting upgrade.";
                cout << msg << endl;
                if (logger) logger->log(msg);
                return;
            }
            shells = 30;
            hasThirtyShot = true;
            chosenUpgrades.insert(UpgradeArea::SHOOT);
            string msg = name + " upgraded to ThirtyShotBot with 30 shells!";
            cout << msg << endl;
            if (logger) logger->log(msg);

        } else if (upgradeName == "ScoutBot") {
            if (chosenUpgrades.count(UpgradeArea::SEE)) {
                string msg = name + " already chose a vision upgrade.";
                cout << msg << endl;
                if (logger) logger->log(msg);
                return;
            }
            hasScout = true;
            scoutCount = 3;
            chosenUpgrades.insert(UpgradeArea::SEE);
            string msg = name + " upgraded to ScoutBot!";
            cout << msg << endl;
            if (logger) logger->log(msg);

        } else if (upgradeName == "TrackBot") {
            if (chosenUpgrades.count(UpgradeArea::SEE)) {
                string msg = name + " already chose a vision upgrade.";
                cout << msg << endl;
                if (logger) logger->log(msg);
                return;
            }
            hasTrack = true;
            trackerCount = 3;
            chosenUpgrades.insert(UpgradeArea::SEE);
            string msg = name + " upgraded to TrackBot!";
            cout << msg << endl;
            if (logger) logger->log(msg);

        } else {
            string msg = "Unknown upgrade: " + upgradeName;
            cout << msg << endl;
            if (logger) logger->log(msg);
        }
    }

class KamikazeBot : public Robot, public MovingRobot, public ShootingRobot, public SeeingRobot, public ThinkingRobot {
private:
    bool willExplode = false;
    
public:
    KamikazeBot(string name, int x, int y) : Robot(name, x, y) {}

    void move(int dx, int dy, int maxWidth, int maxHeight, Logger* logger) override {
        if (willExplode) return;
        
        int oldX = positionX, oldY = positionY;
        positionX = max(0, min(positionX + dx, maxWidth - 1));
        positionY = max(0, min(positionY + dy, maxHeight - 1));

        stringstream ss;
        ss << name << " moved from (" << oldX << "," << oldY << ") to (" << positionX << "," << positionY << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void fire(int targetX, int targetY, Logger* logger) override {
        stringstream ss;
        ss << name << " is preparing its explosive charge";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void look(int x, int y, Logger* logger) override {
        stringstream ss;
        ss << name << " is scanning area around (" << x << "," << y << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void think(Battlefield* battlefield, int maxWidth, int maxHeight, Logger* logger) override {
        if (logger) logger->log(name + " is thinking...");
        cout << name << " is thinking..." << endl;

        // 1. Look phase
        Robot* nearestEnemy = nullptr;
        int minDist = INT_MAX;
        const auto& allRobots = battlefield->getRobots();
        for (const auto& robot : allRobots) {
            if (robot.get() != this) {
                int dist = abs(robot->getX() - positionX) + abs(robot->getY() - positionY);
                if (dist < minDist) {
                    minDist = dist;
                    nearestEnemy = robot.get();
                }
            }
        }

        int lookX = positionX, lookY = positionY;
        if (nearestEnemy) {
            lookX = nearestEnemy->getX();
            lookY = nearestEnemy->getY();
        } else {
            lookX += (rand() % 3) - 1;
            lookY += (rand() % 3) - 1;
        }
        look(lookX, lookY, logger);

        // 2. Fire phase (Kamikaze only explodes when adjacent)
        if (nearestEnemy && minDist <= 1) {
            willExplode = true;
            stringstream ss;
            ss << name << " is exploding to take out nearby enemies!";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
            
            // Damage all adjacent robots
            for (int x = positionX-1; x <= positionX+1; x++) {
                for (int y = positionY-1; y <= positionY+1; y++) {
                    if (x >= 0 && x < maxWidth && y >= 0 && y < maxHeight) {
                        battlefield->checkAndHitRobot(x, y, this);
                    }
                }
            }
            battlefield->markSelfDestruct(this);
            return;
        }
        else {
            fire(positionX, positionY, logger); // Standard fire action
        }

        // 3. Move phase
        if (nearestEnemy) {
            // Move toward enemy
            int dx = nearestEnemy->getX() - positionX;
            int dy = nearestEnemy->getY() - positionY;
            move(dx > 0 ? 1 : -1, dy > 0 ? 1 : -1, maxWidth, maxHeight, logger);
        } else {
            // Random movement if no enemy found
            move((rand() % 3) - 1, (rand() % 3) - 1, maxWidth, maxHeight, logger);
        }
    }
};

class MineLayerBot : public Robot, public MovingRobot, public ShootingRobot, public SeeingRobot, public ThinkingRobot {
private:
    int minesLeft = 5;
    vector<pair<int, int>> placedMines;
    
public:
    MineLayerBot(string name, int x, int y) : Robot(name, x, y) {}

    void move(int dx, int dy, int maxWidth, int maxHeight, Logger* logger) override {
        int oldX = positionX, oldY = positionY;
        positionX = max(0, min(positionX + dx, maxWidth - 1));
        positionY = max(0, min(positionY + dy, maxHeight - 1));

        // 30% chance to lay mine when moving
        if (minesLeft > 0 && rand() % 10 < 3) {
            minesLeft--;
            placedMines.emplace_back(oldX, oldY);
            stringstream ss;
            ss << name << " laid a mine at (" << oldX << "," << oldY << ")";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
        }

        stringstream ss;
        ss << name << " moved from (" << oldX << "," << oldY << ") to (" << positionX << "," << positionY << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void fire(int targetX, int targetY, Logger* logger) override {
        stringstream ss;
        ss << name << " checks its mine inventory (remaining: " << minesLeft << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void look(int x, int y, Logger* logger) override {
        stringstream ss;
        ss << name << " is scanning area around (" << x << "," << y << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void think(Battlefield* battlefield, int maxWidth, int maxHeight, Logger* logger) override {
        if (logger) logger->log(name + " is thinking...");
        cout << name << " is thinking..." << endl;

        // 1. Look phase
        Robot* nearestEnemy = nullptr;
        int minDist = INT_MAX;
        const auto& allRobots = battlefield->getRobots();
        for (const auto& robot : allRobots) {
            if (robot.get() != this) {
                int dist = abs(robot->getX() - positionX) + abs(robot->getY() - positionY);
                if (dist < minDist) {
                    minDist = dist;
                    nearestEnemy = robot.get();
                }
            }
        }

        int lookX = positionX, lookY = positionY;
        if (nearestEnemy) {
            lookX = nearestEnemy->getX();
            lookY = nearestEnemy->getY();
        } else {
            lookX += (rand() % 3) - 1;
            lookY += (rand() % 3) - 1;
        }
        look(lookX, lookY, logger);

        // 2. Fire phase (Mine checking)
        fire(positionX, positionY, logger);

        // Check for mine triggers
        for (auto it = placedMines.begin(); it != placedMines.end(); ) {
            bool mineTriggered = false;
            for (const auto& robot : battlefield->getRobots()) {
                if (robot->getX() == it->first && robot->getY() == it->second) {
                    stringstream ss;
                    ss << "Mine at (" << it->first << "," << it->second << ") triggered by " << robot->getName();
                    cout << ss.str() << endl;
                    if (logger) logger->log(ss.str());
                    
                    battlefield->checkAndHitRobot(it->first, it->second, this);
                    mineTriggered = true;
                    break;
                }
            }
            if (mineTriggered) {
                it = placedMines.erase(it);
            } else {
                ++it;
            }
        }

        // 3. Move phase
        if (nearestEnemy) {
            // Move to maintain distance
            int dx = nearestEnemy->getX() - positionX;
            int dy = nearestEnemy->getY() - positionY;
            if (minDist > 3) {
                move(dx > 0 ? 1 : -1, dy > 0 ? 1 : -1, maxWidth, maxHeight, logger);
            } else if (minDist < 2) {
                move(dx > 0 ? -1 : 1, dy > 0 ? -1 : 1, maxWidth, maxHeight, logger);
            } else {
                move(0, 0, maxWidth, maxHeight, logger); // Stay in place
            }
        } else {
            // Random exploration
            move((rand() % 3) - 1, (rand() % 3) - 1, maxWidth, maxHeight, logger);
        }
    }
};

class CloneBot : public Robot, public MovingRobot, public ShootingRobot, public SeeingRobot, public ThinkingRobot {
private:
    int cloneCharges = 3;
    vector<unique_ptr<Robot>> clones;
    
public:
    CloneBot(string name, int x, int y) : Robot(name, x, y) {}

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
        stringstream ss;
        ss << name << " checks its clone charges (remaining: " << cloneCharges << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void look(int x, int y, Logger* logger) override {
        stringstream ss;
        ss << name << " is observing (" << x << "," << y << ")";
        cout << ss.str() << endl;
        if (logger) logger->log(ss.str());
    }

    void think(Battlefield* battlefield, int maxWidth, int maxHeight, Logger* logger) override {
        if (logger) logger->log(name + " is thinking...");
        cout << name << " is thinking..." << endl;

        // 1. Look phase
        Robot* nearestEnemy = nullptr;
        int minDist = INT_MAX;
        const auto& allRobots = battlefield->getRobots();
        for (const auto& robot : allRobots) {
            if (robot.get() != this) {
                int dist = abs(robot->getX() - positionX) + abs(robot->getY() - positionY);
                if (dist < minDist) {
                    minDist = dist;
                    nearestEnemy = robot.get();
                }
            }
        }

        int lookX = positionX, lookY = positionY;
        if (nearestEnemy) {
            lookX = nearestEnemy->getX();
            lookY = nearestEnemy->getY();
        } else {
            lookX += (rand() % 3) - 1;
            lookY += (rand() % 3) - 1;
        }
        look(lookX, lookY, logger);

        // 2. Fire phase (Clone creation)
        fire(positionX, positionY, logger);
        if (cloneCharges > 0 && rand() % 5 == 0) {
            cloneCharges--;
            int cloneX = max(0, min(positionX + (rand() % 3) - 1, maxWidth - 1));
            int cloneY = max(0, min(positionY + (rand() % 3) - 1, maxHeight - 1));
            
            auto clone = make_unique<Robot>(name + "'s Clone", cloneX, cloneY);
            clones.push_back(std::move(clone));
            
            stringstream ss;
            ss << name << " created a clone at (" << cloneX << "," << cloneY << ")";
            cout << ss.str() << endl;
            if (logger) logger->log(ss.str());
        }

        // 3. Move phase
        if (nearestEnemy) {
            // Move to flank enemy
            int dx = nearestEnemy->getX() - positionX;
            int dy = nearestEnemy->getY() - positionY;
            if (abs(dx) > abs(dy)) {
                move(dx > 0 ? 1 : -1, 0, maxWidth, maxHeight, logger);
            } else {
                move(0, dy > 0 ? 1 : -1, maxWidth, maxHeight, logger);
            }
        } else {
            // Random movement
            move((rand() % 3) - 1, (rand() % 3) - 1, maxWidth, maxHeight, logger);
        }

        // Clean up destroyed clones
        clones.erase(
            remove_if(clones.begin(), clones.end(), 
                [](const unique_ptr<Robot>& clone) { return clone->getLives() <= 0; }),
            clones.end()
        );
    }
};




bool Battlefield::loadConfig(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("M by N") != string::npos) {
            size_t colon_pos = line.find(':');
            if (colon_pos != string::npos) {
                string dimensions = line.substr(colon_pos + 1);
                istringstream iss(dimensions);
                iss >> width >> height;
                cout << "M by N : " << width << " by " << height << endl;
            }
        } else if (line.find("steps:") != string::npos) {
            size_t colon_pos = line.find(':');
            if (colon_pos != string::npos) {
                string step_str = line.substr(colon_pos + 1);
                istringstream iss(step_str);
                iss >> steps;
                cout << "Steps : " << steps << endl;
            }
        } else if (line.find("GenericRobot") != string::npos) {
            string tag, name, xStr, yStr;
            istringstream iss(line);
            int x, y;
            iss >> tag >> name >> xStr >> yStr;
            x = (xStr == "random") ? rand() % width : stoi(xStr);
            y = (yStr == "random") ? rand() % height : stoi(yStr);
            robots.push_back(make_unique<GenericRobot>(name, x, y));
            cout << tag << " " << name << " " << xStr << " " << yStr << endl;
        }
        else if (line.find("KamikazeBot") != string::npos) {
            string tag, name, xStr, yStr;
            istringstream iss(line);
            int x, y;
            iss >> tag >> name >> xStr >> yStr;
            x = (xStr == "random") ? rand() % width : stoi(xStr);
            y = (yStr == "random") ? rand() % height : stoi(yStr);
            robots.push_back(make_unique<KamikazeBot>(name, x, y));
            cout << tag << " " << name << " " << xStr << " " << yStr << endl;
        }
        else if (line.find("MineLayerBot") != string::npos) {
            string tag, name, xStr, yStr;
            istringstream iss(line);
            int x, y;
            iss >> tag >> name >> xStr >> yStr;
            x = (xStr == "random") ? rand() % width : stoi(xStr);
            y = (yStr == "random") ? rand() % height : stoi(yStr);
            robots.push_back(make_unique<MineLayerBot>(name, x, y));
            cout << tag << " " << name << " " << xStr << " " << yStr << endl;
        }
        else if (line.find("CloneBot") != string::npos) {
            string tag, name, xStr, yStr;
            istringstream iss(line);
            int x, y;
            iss >> tag >> name >> xStr >> yStr;
            x = (xStr == "random") ? rand() % width : stoi(xStr);
            y = (yStr == "random") ? rand() % height : stoi(yStr);
            robots.push_back(make_unique<CloneBot>(name, x, y));
            cout << tag << " " << name << " " << xStr << " " << yStr << endl;
        }
    }
    return true;
}

bool Battlefield::runStep() {
    //if (robots.empty()) return false;
    if (robots.size() <= 1) return false;
    clearScreen();

    if (currentRobotIndex >= robots.size()) currentRobotIndex = 0;

    // Capture current battlefield state to string
    stringstream battlefieldState;
    streambuf* oldCoutBuffer = cout.rdbuf();
    cout.rdbuf(battlefieldState.rdbuf());
    display(); // This now outputs to our stringstream
    cout.rdbuf(oldCoutBuffer); // Restore cout

    // Log using existing logger
    if (logger) {
        logger->log(battlefieldState.str());
    }
    Robot* currentRobotPtr = robots[currentRobotIndex].get();

    ThinkingRobot* thinker = dynamic_cast<ThinkingRobot*>(currentRobotPtr);
    if (thinker) {
        thinker->think(this, width, height, logger);
    } else {
        cerr << "Error: Robot does not implement ThinkingRobot!" << endl;
    }

    // Only increment if robot wasn't destroyed
    if (currentRobotIndex < robots.size() && robots[currentRobotIndex].get() == currentRobotPtr) {
        currentRobotIndex++;
    }

    display();
    if (robots.size() <= 1) return false;

    return true;
}




void Battlefield::checkAndHitRobot(int x, int y, Robot* shooter) {
    for (auto it = robots.begin(); it != robots.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y) {
            if (shooter == it->get()) {
                cout << "You can't shoot yourself!\n";
                if (logger) logger->log(shooter->getName() + " attempted to shoot itself. Ignored.");
                return;
            }

        int chance = rand()% 100;
        if (chance < 70){
            cout << "Target hit! " << (*it)->getName() << " !\n";
            if (logger) logger->log("Target hit! " + (*it)->getName() + " !");


            Robot* target = it->get();
            target->loseLife();
            cout << "Target hit! " << (*it)->getName() << " lost a life. Lives left: " << target->getLives() << "\n";
            if (logger) logger->log("Target hit! " + (*it)->getName() + " lost a life. Lives left: " + to_string(target->getLives()));

            GenericRobot* gr = dynamic_cast<GenericRobot*>(shooter);
            if (gr) {
                vector<string> upgrades;

                const set<UpgradeArea>& upgradesTaken = gr->getChosenUpgrades();

                if (!upgradesTaken.count(UpgradeArea::MOVE)) {
                    upgrades.push_back("HideBot");
                    upgrades.push_back("JumpBot");
                }
                if (!upgradesTaken.count(UpgradeArea::SHOOT)) {
                    upgrades.push_back("LongShotBot");
                    upgrades.push_back("SemiAutoBot");
                    upgrades.push_back("ThirtyShotBot");
                }
                if (!upgradesTaken.count(UpgradeArea::SEE)) {
                    upgrades.push_back("ScoutBot");
                    upgrades.push_back("TrackBot");
                }

                if (!upgrades.empty()) {
                    string chosen = upgrades[rand() % upgrades.size()];
                    gr->applyUpgrade(chosen, logger);
                }

            }

            if (target->getLives() <= 0) {
                logger->log(target->getName() + " was destroyed by " + shooter->getName());
                robots.erase(it);
                return;
            }


        } else {
                string noTargetMsg = "Missed! No robot at (" + to_string(x) + "," + to_string(y) + ").";
                cout << noTargetMsg << endl;
                if (logger) logger->log(noTargetMsg);

            }
        }
    }
    string noTargetMsg = "Missed! No robot at (" + to_string(x) + "," + to_string(y) + ").";
    cout << noTargetMsg << endl;
    if (logger) logger->log(noTargetMsg);

}

void Battlefield::markSelfDestruct(Robot* robot) {
    for (auto it = robots.begin(); it != robots.end(); ++it) {
        if (it->get() == robot) {
            cout << robot->getName() << " has self-destructed!\n";
            if (logger) logger->log(robot->getName() + " has self-destructed.");
            robots.erase(it);
            return;
        }
    }
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

    battlefield.display();

    int maxSteps = battlefield.getSteps();
    for (int step = 0; step < battlefield.getSteps(); ++step) {
         // EXTRA CHECK: if runStep() returned true but there's only one robot this one is due to a bug oawhdowahdoa
        if (battlefield.getRobots().size() <= 1) {
            logger.log("Simulation ended early only one robot left in the battlefield");
            cout << "Simulation ended early robot above is the last one standing\n";
            cout << "Simulation stopped at Step : " << step + 1 << endl;
            
            break;
        }

        logger.log("--- Step " + to_string(step + 1) + " ---");

        if (!battlefield.runStep()) {
            logger.log("Simulation ended early only one robot left in the battlefield");
            cout << "Simulation ended early robot above is the last one standing\n";
            cout << "Simulation stopped at Step : " << step + 1 << endl;
            break;
        }



        Sleep(500);
    }





    logger.log("Simulation ended.");
    cout << "\nSimulation ended.\n";
    return 0;
}
