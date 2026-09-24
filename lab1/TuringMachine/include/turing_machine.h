#include <iostream>
#include <vector>

class Tmachine
{
    int head;
    std::vector<bool> tape;

public:
    Tmachine();
    ~Tmachine();
    void moveLeft();
    void moveRight();
    bool readCell() const;
    void eraseCell();
};