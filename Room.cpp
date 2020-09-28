/*
# Maze Generator
### Date: 28 March 2018
### Author: Pisica Alin
*/

#include "config.h"
#include "DudleyWatch.h"
#include "Room.h"

using namespace std;

Room::Room(int i, int j, int rw) {
  this->x = i;
  this->y = j;
  this->roomWidth = rw;
  walls[0] = true;
  walls[1] = true;
  walls[2] = true;
  walls[3] = true;
  visited = false;
}

void Room::removeWalls(Room &r) {
  if (this->x - r.x == -1) {
    this->removeWall(1);
    r.removeWall(3);
  } 
  if (this->x - r.x == 1) {
    this->removeWall(3);
    r.removeWall(1);
  } 
  if (this->y - r.y == -1) {
    this->removeWall(2);
    r.removeWall(0);
  } 
  if (this->y - r.y == 1) {
      this->removeWall(0);
      r.removeWall(2);
  } 
}

void Room::show(void) {
  int xCoord = this->x * roomWidth;
  int yCoord = this->y * roomWidth;
  if (this->walls[0]) {
    tft->drawLine(xCoord, yCoord, // top
		  xCoord + this->roomWidth, yCoord, TFT_RED);
  }
  if (this->walls[1]) {		// right
    tft->drawLine(xCoord + this->roomWidth, yCoord, 
		  xCoord + this->roomWidth, yCoord + this->roomWidth, TFT_RED);
  }
  if (this->walls[2]) {		// bottom
    tft->drawLine(xCoord, yCoord + this->roomWidth, 
		  xCoord + this->roomWidth, yCoord + this->roomWidth, TFT_RED);
  }
  if (this->walls[3]) {		// left
    tft->drawLine(xCoord, yCoord, 
		  xCoord, yCoord + this->roomWidth, TFT_RED);
  }
}

void Room::printWalls() {
  for (int i = 0; i < 4; i++) {
    std::cout << walls[i] << " ";
  }
  std::cout << "\n";
}

void Room::removeWall(int w) {
  this->walls[w] = false;
}

bool Room::hasWall(int w) {
  return this->walls[w];
}

void Room::visit(bool setact) {
  this->visited = setact;
}

int Room::getPositionInVector(int size) {
  return this->x * size + this->y;
}

int Room::getX() {
  return this->x;
}

int Room::getY() {
  return this->y;
}

bool Room::isVisited() {
  return this->visited;
}

