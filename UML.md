```mermaid
classDiagram
    class GameObject {
        <<abstract>>
        #int x
        #int y
        #string name
        +draw()*
    }
    class MovableObject {
        #int speed
        +move(dx, dy)
    }
    class PlayerTank {
        -int hp
        +draw()
        +fire()
    }
    GameObject <|-- MovableObject
    MovableObject <|-- PlayerTank
