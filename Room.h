/*
# Maze Generator
### Date: 28 March 2018
### Author: Pisica Alin
*/

#include <iostream>

class Room {
  private: 
    int x, y, roomWidth;
    bool walls[4];
    bool visited;
  public: 
    Room(int i, int j, int rw);
    void removeWalls(Room &r);
    void removeWall(int wall);
    bool hasWall(int wall);
    void printWalls();
    void visit(bool setact);
    void show();
    int getPositionInVector(int size);
    int getX();
    int getY();
    bool isVisited();
};
