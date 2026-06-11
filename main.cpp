#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <memory>

using namespace std;

// 1. 遊戲對象基底
class GameObject {
public:
    int x, y;
    bool active;
    GameObject(int x, int y) : x(x), y(y), active(true) {}
    virtual void draw() = 0;
    virtual ~GameObject() = default; // 確保衍生類別能正確解構
};

// 2. 障礙物類別
class Wall : public GameObject {
public:
    Wall(int x, int y) : GameObject(x, y) {}
    void draw() override { cout << " # "; }
};

// 3. 子彈類別
class Bullet : public GameObject {
public:
    int dx, dy;
    bool isEnemyBullet;
    Bullet(int x, int y, int dx, int dy, bool enemy) : GameObject(x, y), dx(dx), dy(dy), isEnemyBullet(enemy) {}
    
    void update() {
        x += dx; y += dy;
        if (x < 0 || x >= 20 || y < 0 || y >= 10) active = false;
    }
    void draw() override { cout << (isEnemyBullet ? " @ " : " * "); }
};

// 4. 坦克基底
class Tank : public GameObject {
public:
    int hp, dir; // 0上, 1下, 2左, 3右
    Tank(int x, int y, int h) : GameObject(x, y), hp(h), dir(1) {}
    
    // 將移動邏輯改為回傳預測位置，由 Game 類別判斷是否碰撞
    void setDirection(int _dir) { dir = _dir; }
    int getNextX(int _dx) { return x + _dx; }
    int getNextY(int _dy) { return y + _dy; }

    virtual void takeDamage(int dmg) {
        hp -= dmg;
        if (hp <= 0) active = false;
    }
};

// 5. 玩家坦克
class Player : public Tank {
public:
    Player(int x, int y) : Tank(x, y, 100) {}
    void draw() override {
        string shapes[] = {" ^ ", " v ", " < ", " > "};
        cout << shapes[dir];
    }
};

// 6. 敵人坦克基底
class Enemy : public Tank {
public:
    Enemy(int x, int y, int hp) : Tank(x, y, hp) {}
    virtual void aiAction(vector<unique_ptr<Bullet>>& bullets, const vector<unique_ptr<Wall>>& walls) = 0;
};

// 6a. 輕型坦克 (一般 AI)
class LightTank : public Enemy {
public:
    LightTank(int x, int y) : Enemy(x, y, 50) {}
    void draw() override {
        string shapes[] = {"[A]", "[V]", "[<]", "[>]"};
        cout << shapes[dir];
    }
    void aiAction(vector<unique_ptr<Bullet>>& bullets, const vector<unique_ptr<Wall>>& walls) override {
        int act = rand() % 5;
        if (act < 3) { // 輕型坦克更愛移動
            int d = rand() % 4;
            setDirection(d);
        } else if (act == 4) { // 隨機開火
            int bx = (dir == 2 ? -1 : (dir == 3 ? 1 : 0));
            int by = (dir == 0 ? -1 : (dir == 1 ? 1 : 0));
            bullets.push_back(make_unique<Bullet>(x + bx, y + by, bx, by, true));
        }
    }
};

// 6b. 重型坦克 (血厚，移動慢)
class HeavyTank : public Enemy {
public:
    HeavyTank(int x, int y) : Enemy(x, y, 100) {}
    void draw() override {
        string shapes[] = {"[H]", "[H]", "[H]", "[H]"}; // 裝甲厚重，看不出砲管方向
        cout << shapes[dir];
    }
    void aiAction(vector<unique_ptr<Bullet>>& bullets, const vector<unique_ptr<Wall>>& walls) override {
        int act = rand() % 6;
        if (act < 2) { // 移動頻率低
            int d = rand() % 4;
            setDirection(d);
        } else if (act > 4) { // 火力壓制
            int bx = (dir == 2 ? -1 : (dir == 3 ? 1 : 0));
            int by = (dir == 0 ? -1 : (dir == 1 ? 1 : 0));
            bullets.push_back(make_unique<Bullet>(x + bx, y + by, bx, by, true));
        }
    }
};

// 7. 遊戲核心控制類別
class Game {
private:
    Player player;
    vector<unique_ptr<Enemy>> enemies;
    vector<unique_ptr<Bullet>> bullets;
    vector<unique_ptr<Wall>> walls;

    bool isPassable(int nx, int ny) {
        if (nx < 0 || nx >= 20 || ny < 0 || ny >= 10) return false;
        for (const auto& w : walls) {
            if (w->x == nx && w->y == ny) return false;
        }
        return true;
    }

public:
    Game() : player(10, 8) {
        // 佈置敵人
        enemies.push_back(make_unique<LightTank>(2, 2));
        enemies.push_back(make_unique<LightTank>(17, 2));
        enemies.push_back(make_unique<HeavyTank>(10, 1)); // Boss 級重型坦克

        // 佈置地形障礙 (掩體)
        walls.push_back(make_unique<Wall>(5, 5));
        walls.push_back(make_unique<Wall>(6, 5));
        walls.push_back(make_unique<Wall>(14, 5));
        walls.push_back(make_unique<Wall>(13, 5));
        walls.push_back(make_unique<Wall>(10, 4));
    }

    void run() {
        char input;
        while (player.active && !enemies.empty()) {
            render();
            
            // 處理玩家輸入
            cin >> input;
            if (input == 'q') break;
            
            int dx = 0, dy = 0, nextDir = player.dir;
            if (input == 'w') { dy = -1; nextDir = 0; }
            else if (input == 's') { dy = 1; nextDir = 1; }
            else if (input == 'a') { dx = -1; nextDir = 2; }
            else if (input == 'd') { dx = 1; nextDir = 3; }
            else if (input == 'j') {
                int bx = (player.dir == 2 ? -1 : (player.dir == 3 ? 1 : 0));
                int by = (player.dir == 0 ? -1 : (player.dir == 1 ? 1 : 0));
                bullets.push_back(make_unique<Bullet>(player.x + bx, player.y + by, bx, by, false));
            }

            // 玩家移動判定
            player.setDirection(nextDir);
            if (dx != 0 || dy != 0) {
                if (isPassable(player.getNextX(dx), player.getNextY(dy))) {
                    player.x += dx; player.y += dy;
                }
            }

            // 敵人行動與移動判定
            for (auto& e : enemies) {
                e->aiAction(bullets, walls);
                int edx = (e->dir == 2 ? -1 : (e->dir == 3 ? 1 : 0));
                int edy = (e->dir == 0 ? -1 : (e->dir == 1 ? 1 : 0));
                // 簡單防呆：確保敵人只在自己的回合內嘗試向前走一步
                if (rand() % 2 == 0 && isPassable(e->getNextX(edx), e->getNextY(edy))) {
                    e->x += edx; e->y += edy;
                }
            }

            // 子彈更新與碰撞判定
            for (auto& b : bullets) {
                if (!b->active) continue;
                b->update();
                
                // 撞牆判定
                if (!isPassable(b->x, b->y)) {
                    b->active = false;
                    continue;
                }

                if (b->isEnemyBullet) {
                    if (player.x == b->x && player.y == b->y) { 
                        player.takeDamage(25); 
                        b->active = false; 
                    }
                } else {
                    for (auto& e : enemies) {
                        if (e->active && e->x == b->x && e->y == b->y) { 
                            e->takeDamage(50); 
                            b->active = false; 
                            break; 
                        }
                    }
                }
            }
            
            // 清理無效物件 (C++20 的 erase_if 寫法或傳統迭代器清理)
            for (auto it = enemies.begin(); it != enemies.end(); ) {
                if (!(*it)->active) it = enemies.erase(it);
                else ++it;
            }
            for (auto it = bullets.begin(); it != bullets.end(); ) {
                if (!(*it)->active) it = bullets.erase(it);
                else ++it;
            }
        }
        
        system("clear"); // Windows 改 cls
        if (player.active) cout << "\n\n  🏆 【戰鬥結束】敵方載具全數摧毀，指揮官，我們贏了！\n\n";
        else cout << "\n\n  💥 【戰鬥失敗】載具被摧毀，請重新挑戰...\n\n";
    }

private:
    void render() {
        system("clear"); // Windows 系統請改為 system("cls");
        cout << "==== 裝甲風暴：戰術掩護版 ====\n";
        cout << "WASD:移動, J:開火, Q:退出 | 玩家HP: " << player.hp << endl;
        cout << "==============================\n";

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 20; j++) {
                bool printed = false;
                
                // 渲染玩家
                if (player.x == j && player.y == i) { player.draw(); printed = true; }
                
                // 渲染牆壁
                if (!printed) {
                    for (const auto& w : walls) {
                        if (w->x == j && w->y == i) { w->draw(); printed = true; break; }
                    }
                }
                
                // 渲染敵人
                if (!printed) {
                    for (const auto& e : enemies) {
                        if (e->active && e->x == j && e->y == i) { e->draw(); printed = true; break; }
                    }
                }
                
                // 渲染子彈
                if (!printed) {
                    for (const auto& b : bullets) {
                        if (b->active && b->x == j && b->y == i) { b->draw(); printed = true; break; }
                    }
                }
                
                if (!printed) cout << " . ";
            }
            cout << endl;
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0)));
    Game game;
    game.run();
    return 0;
}
