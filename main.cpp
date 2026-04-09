#include <iostream>
#include <vector>
#include <string>

using namespace std;

// 1. 基底類別 (抽象概念)
class GameObject {
protected:
    int x, y;
    string name;
public:
    GameObject(int x, int y, string n) : x(x), y(y), name(n) {}
    virtual void draw() = 0; // 展示「多型」
    virtual ~GameObject() {}
};

// 2. 移動類別 (繼承概念)
class MovableObject : public GameObject {
protected:
    int speed;
public:
    MovableObject(int x, int y, string n, int s) : GameObject(x, y, n), speed(s) {}
    void move(int dx, int dy) {
        x += dx * speed;
        y += dy * speed;
        cout << name << " 已移動至 (" << x << ", " << y << ")\n";
    }
};

// 3. 玩家坦克類別 (封裝實作)
class PlayerTank : public MovableObject {
public:
    PlayerTank(int x, int y) : MovableObject(x, y, "我的坦克", 5) {}
    void draw() override {
        cout << "【繪製】正在渲染坦克圖片於 (" << x << ", " << y << ")\n";
    }
};

int main() {
    cout << "--- 坦克遊戲 C++ 物件導向架構啟動 ---\n";
    PlayerTank tank(50, 50);
    tank.draw();
    tank.move(2, -1);
    
    return 0;
}
