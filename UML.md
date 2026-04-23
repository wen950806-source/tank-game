#include <iostream>
#include <vector>
#include <string>

using namespace std;

// ==========================================
// 1. 遊戲對象 (GameObject) - 展示「抽象」與「繼承」
// ==========================================
class GameObject {
protected:
    int x, y;          // 座標
    string name;       // 名稱

public:
    GameObject(int x, int y, string n) : x(x), y(y), name(n) {}
    
    // 純虛擬函式：強迫子類別實作自己的繪製方式 (展示多型)
    virtual void draw() = 0; 
    
    virtual ~GameObject() {} // 良好的 OOP 習慣：虛擬解構子
};

// ==========================================
// 2. 可移動對象 (MovableObject) - 展示「封裝」
// ==========================================
class MovableObject : public GameObject {
protected:
    int speed;         // 速度

public:
    MovableObject(int x, int y, string n, int s) 
        : GameObject(x, y, n), speed(s) {}
    
    // 統一的移動邏輯
    void move(int dx, int dy) {
        x += dx * speed;
        y += dy * speed;
        cout << "[移動系統] " << name << " 移動到了位置: (" << x << ", " << y << ")" << endl;
    }
};

// ==========================================
// 3. 玩家坦克 (PlayerTank) - 展示「多型」與具體實作
// ==========================================
class PlayerTank : public MovableObject {
private:
    int hp;            // 血量 (Private 封裝，不讓外部直接修改)

public:
    PlayerTank(int x, int y) 
        : MovableObject(x, y, "玩家坦克", 5), hp(100) {}

    // 實作父類別的純虛擬函式 (多型)
    void draw() override {
        cout << "[渲染系統] 正在繪製坦克圖形於 (" << x << ", " << y << ")，目前血量: " << hp << endl;
    }

    // 玩家特有的方法
    void fire() {
        cout << "[戰鬥系統] 玩家坦克開火！砰！！" << endl;
    }
};

// ==========================================
// 4. 主程式 (Main Loop)
// ==========================================
int main() {
    cout << "==== C++ 坦克專題核心架構展示 ====" << endl;

    // 建立一個玩家坦克物件
    PlayerTank myTank(10, 10);

    // 1. 呼叫繪製 (多型)
    myTank.draw();

    // 2. 呼叫移動
    myTank.move(1, 0); // 向右移動
    
    // 3. 呼叫開火
    myTank.fire();

    // 4. 移動後再次繪製
    myTank.draw();

    cout << "==================================" << endl;
    return 0;
}
