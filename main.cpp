#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// 1. 遊戲對象基底
class GameObject {
public:
    int x, y;
    bool active;
    GameObject(int x, int y) : x(x), y(y), active(true) {}
    virtual void draw() = 0;
    virtual ~GameObject() {}
};

// 2. 子彈類別
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

// 3. 坦克基底
class Tank : public GameObject {
public:
    int hp, dir; // 0上, 1下, 2左, 3右
    Tank(int x, int y, int h) : GameObject(x, y), hp(h), dir(1) {}
    
    void move(int _dx, int _dy, int _dir) {
        dir = _dir;
        if (x + _dx >= 0 && x + _dx < 20) x += _dx;
        if (y + _dy >= 0 && y + _dy < 10) y += _dy;
    }

    virtual bool takeDamage(int dmg) {
        hp -= dmg;
        if (hp <= 0) active = false;
        return !active;
    }
};

// 4. 玩家坦克
class Player : public Tank {
public:
    Player(int x, int y) : Tank(x, y, 100) {}
    void draw() override {
        string shapes[] = {" ^ ", " v ", " < ", " > "};
        cout << shapes[dir];
    }
};

// 5. 敵人坦克 (具備 AI)
class Enemy : public Tank {
public:
    Enemy(int x, int y) : Tank(x, y, 50) {}
    void draw() override {
        string shapes[] = {"[A]", "[V]", "[<]", "[>]"};
        cout << shapes[dir];
    }
    void aiAction(vector<Bullet*>& bullets) {
        int act = rand() % 5;
        if (act < 4) { // 隨機移動
            int d = rand() % 4;
            if (d == 0) move(0, -1, 0);
            else if (d == 1) move(0, 1, 1);
            else if (d == 2) move(-1, 0, 2);
            else move(1, 0, 3);
        } else { // 隨機開火
            int bx = (dir == 2 ? -1 : (dir == 3 ? 1 : 0));
            int by = (dir == 0 ? -1 : (dir == 1 ? 1 : 0));
            bullets.push_back(new Bullet(x + bx, y + by, bx, by, true));
        }
    }
};

int main() {
    srand(time(0));
    Player player(10, 5);
    vector<Enemy*> enemies = {new Enemy(2, 2), new Enemy(17, 8), new Enemy(17, 2)};
    vector<Bullet*> bullets;

    char input;
    while (player.active && !enemies.empty()) {
        system("clear"); // Windows 請改為 system("cls");
        cout << "==== 坦克大戰：全動態 AI 版 ====\n";
        cout << "WASD:移動, J:開火, Q:退出 | 玩家HP: " << player.hp << endl;

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 20; j++) {
                bool prt = false;
                if (player.x == j && player.y == i) { player.draw(); prt = true; }
                for (auto e : enemies) if (e->active && e->x == j && e->y == i) { e->draw(); prt = true; break; }
                for (auto b : bullets) if (b->active && b->x == j && b->y == i) { b->draw(); prt = true; break; }
                if (!prt) cout << " . ";
            }
            cout << endl;
        }

        cin >> input;
        if (input == 'q') break;
        else if (input == 'w') player.move(0, -1, 0);
        else if (input == 's') player.move(0, 1, 1);
        else if (input == 'a') player.move(-1, 0, 2);
        else if (input == 'd') player.move(1, 0, 3);
        else if (input == 'j') {
            int bx = (player.dir == 2 ? -1 : (player.dir == 3 ? 1 : 0));
            int by = (player.dir == 0 ? -1 : (player.dir == 1 ? 1 : 0));
            bullets.push_back(new Bullet(player.x + bx, player.y + by, bx, by, false));
        }

        // 敵人行動
        for (auto e : enemies) e->aiAction(bullets);

        // 子彈更新與碰撞
        for (auto b : bullets) {
            if (!b->active) continue;
            b->update();
            if (b->isEnemyBullet) {
                if (player.x == b->x && player.y == b->y) { player.takeDamage(20); b->active = false; }
            } else {
                for (auto e : enemies) if (e->active && e->x == b->x && e->y == b->y) { e->takeDamage(50); b->active = false; }
            }
        }
        
        // 清理
        for (int i = 0; i < enemies.size(); ) {
            if (!enemies[i]->active) { delete enemies[i]; enemies.erase(enemies.begin() + i); }
            else i++;
        }
    }
    cout << (player.active ? "【勝利】敵人全滅！" : "【戰敗】你被摧毀了！") << endl;
    return 0;
}
