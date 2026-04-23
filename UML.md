```mermaid
classDiagram
    direction TB
    class GameObject {
        <<abstract>>
        #int x
        #int y
        #string name (名稱)
        +draw()* (繪製)
    }
    class MovableObject {
        #int speed (速度)
        +move(dx, dy) (移動)
    }
    class PlayerTank {
        -int hp (血量)
        +draw() (繪製)
        +fire() (開火)
    }

    GameObject <|-- MovableObject : 繼承
    MovableObject <|-- PlayerTank : 繼承
