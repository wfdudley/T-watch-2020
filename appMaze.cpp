/*
# Maze Generator
### Date: 28 March 2018
### Author: Pisica Alin
*/

#include "config.h"
#include "DudleyWatch.h"

// Made in C++
// Maze generated using recursive backtracking.

#include <iostream> 
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <stack>

#include "Room.h"

#define WIDTH      240
#define HEIGHT     240
#define ROWS       10
#define COLUMNS    10
#define ROOM_WIDTH (WIDTH / ROWS)

using namespace std;
boolean maze_built;

bool unvisitedCells(vector <Room> mazeGrid) {
  for (uint32_t i = 0; i < mazeGrid.size(); i++) {
    if (!mazeGrid[i].isVisited()) {
      return true;
    }
  }
  return false;
}

int checkNeighbours(vector <Room> maze, Room r) {
  int x = r.getX();
  int y = r.getY();
  vector <Room> neighbours;
  if(!maze[(x - 1) * ROWS + y].isVisited() && x > 0) {
    neighbours.push_back(maze[(x - 1) * ROWS + y]);
  }
  if(!maze[(x + 1) * ROWS + y].isVisited() && x < ROWS - 1) {
    neighbours.push_back(maze[(x + 1) * ROWS + y]);
  }
  if(!maze[x * ROWS + y + 1].isVisited() && y < COLUMNS - 1) {
    neighbours.push_back(maze[x * ROWS + y + 1]);
  }
  if(!maze[x * ROWS + y - 1].isVisited() && y > 0) {
    neighbours.push_back(maze[x * ROWS + y - 1]);
  }
  if (neighbours.size() < 1) {
    return -1;
  }
  
  int randomIdx = rand() % neighbours.size();
  int nxt = neighbours[randomIdx].getY() + neighbours[randomIdx].getX() * ROWS;
  return nxt;
}

int hasNeighbor(vector <Room> maze, Room r, int direction) {
  int x = r.getX();
  int y = r.getY();
  // walls are: 0,1,2,3 -> up, right, down, left
  switch(direction) {
    case 0 :	// up
      if(y > 0) return x * ROWS + y - 1;
      break;
    case 1 :`	// right
      if(x < ROWS - 1) return (x + 1) * ROWS + y;
      break;
    case 2 :	// down
      if(y < COLUMNS - 1) return x * ROWS + y + 1;
      break;
    case 3 :	// left
      if(x > 0) return (x - 1) * ROWS + y;
      break;
  }
  return -1;
}

void appMaze (void) {
int16_t x, y;
uint16_t dcolor;
uint32_t chkn_count = 0;
int mSelect;
  maze_built = false;
  Serial.println(F("appMaze()"));
  tft->fillScreen(TFT_BLACK); // Clear screen
  tft->setTextColor(TFT_GREEN);
  tft->drawCentreString("maze", 120, 120, 4);

  bool running = true;

  srand(time(NULL));

  vector < Room > mazeGrid;
  stack < Room* > roomStack;

  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      Room newRoom(i, j, ROOM_WIDTH);
      mazeGrid.push_back(newRoom);
    }
  }

  Room *current = &mazeGrid[0]; 
  current->visit(true); 

  while (!maze_built && running) {
    my_idle();
    if(chkn_count > (ROWS*COLUMNS)+((ROWS*COLUMNS)>>1) // 10 x 10 x 1.5
    && !unvisitedCells(mazeGrid)) {
      maze_built = true;
      Serial.println(F("Done."));
      running = false;
    }
    if(ttgo->getTouch(x, y)) { // if touch
      running = false;
    }

    Serial.printf("checkNeighbours() %d\n", ++chkn_count);
    int next = checkNeighbours(mazeGrid, *current);    
    // If the current cell has any neighbours which have not been visited
    if (next != -1) {
      // Choose randomly one of the unvisited neighbours
      Room &nextRoom = mazeGrid[next];
      // Push the current cell to the stack
      roomStack.push(current);
      // Remove the wall between the current cell and the chosen cell
      current->removeWalls(nextRoom);
      // Make the chosen cell the current cell and mark it as visited
      current = &nextRoom; 
      current->visit(true);
    } else if (!roomStack.empty()) { // If stack is not empty
      // Pop a cell from the stack
      Room &previousRoom = *roomStack.top();
      roomStack.pop();
      // Make it the current cell
      current = &previousRoom;
    }

    // tft->fillScreen(TFT_BLACK);
    
    for (uint32_t i = 0; i < mazeGrid.size(); i++) {
      if (!mazeGrid[i].isVisited()) {
        dcolor = TFT_BLUE;
      } else {
        dcolor = TFT_WHITE;
      }
      // Serial.printf("l:%d %d %d %d %d %d\n", __LINE__, mazeGrid[i].getX() * ROOM_WIDTH, mazeGrid[i].getY() * ROOM_WIDTH, ROOM_WIDTH, ROOM_WIDTH, dcolor);
      tft->fillRect(mazeGrid[i].getX() * ROOM_WIDTH, mazeGrid[i].getY() * ROOM_WIDTH, ROOM_WIDTH, ROOM_WIDTH, dcolor);

      dcolor = TFT_BLACK;
      // Serial.println(F("before show()"));
      mazeGrid[i].show();
    }
    
    int xCoordHead = current->getX() * ROOM_WIDTH;
    int yCoordHead = current->getY() * ROOM_WIDTH;
    // Serial.printf("l:%d %d %d %d %d %d\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, dcolor);
    // this "cursor" shows the room we're "visiting":
    tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_DARKGREY);

  }
  bool touched = false;
  if(maze_built) {	// then "play" the game (as yet, unwritten)
  int xCoordHead = current->getX() * ROOM_WIDTH;
  int yCoordHead = current->getY() * ROOM_WIDTH;
  Serial.printf("l:%d %d %d %d %d %d\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_WHITE);
  tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_WHITE);
  current = &mazeGrid[rand() % ROWS]; 
  int xCoordHead = current->getX() * ROOM_WIDTH;
  int yCoordHead = current->getY() * ROOM_WIDTH;
  Serial.printf("l:%d %d %d %d %d %d\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_BLUE);
  tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_BLUE);
    do {
      my_idle();
      // walls are: 0,1,2,3 -> up, right, down, left
      mSelect = poll_swipe_or_menu_press();	// poll for touch, returns 0-15
      int next = -1;
      if(mSelect == (UP + 11)) { // swipe 12 = up, 13 = down, left = 14, rt = 15
	// if up wall missing, move up
	if(!current->hasWall(0)) {
	  next = hasNeighbour(mazeGrid, *current, 0);
	}
      }
      else if(mSelect == (DOWN + 11)) {
	// if down wall missing, move down
	if(!current->hasWall(2)) {
	  next = hasNeighbour(mazeGrid, *current, 2);
	}
      }
      else if(mSelect == (LEFT + 11)) {
	// if left wall missing, move left
	if(!current->hasWall(3)) {
	  next = hasNeighbour(mazeGrid, *current, 3);
	}
      }
      else if(mSelect == (RIGHT + 11)) {
	// if right wall missing, move right
	if(!current->hasWall(1)) {
	  next = hasNeighbour(mazeGrid, *current, 1);
	}
      }
      else if(mSelect == (CWCIRCLE + 11)) {
	touched = true;
	break;
      }
      if(next > -1) {
	int xCoordHead = current->getX() * ROOM_WIDTH;
	int yCoordHead = current->getY() * ROOM_WIDTH;
	Serial.printf("l:%d %d %d %d %d %d\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_WHITE);
	tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_WHITE);
	Room &nextRoom = mazeGrid[next];
	current = &nextRoom; 
	int xCoordHead = current->getX() * ROOM_WIDTH;
	int yCoordHead = current->getY() * ROOM_WIDTH;
	Serial.printf("l:%d %d %d %d %d %d\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_BLUE);
	// this "cursor" shows the room we're "visiting":
	tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_BLUE);
      }
      else {
	beep(3); beep(0);
      }
    } while(!touched);
  }
// Exit:
  while (ttgo->getTouch(x, y)) { // wait user lifts finger to return to clock
    my_idle();
  }
  tft->fillScreen(TFT_BLACK); // Clear screen
}
