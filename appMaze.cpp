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
  vector < Room > neighbours;
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

void appMaze (void) {
int16_t x, y;
uint16_t dcolor;
uint32_t chkn_count = 0;
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
  current->visit(); 

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
      current->visit();
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
    
    dcolor = TFT_DARKGREY;
    int xCoordHead = current->getX() * ROOM_WIDTH;
    int yCoordHead = current->getY() * ROOM_WIDTH;
    // Serial.printf("l:%d %d %d %d %d %d\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, dcolor);
    // this "cursor" shows the room we're "visiting":
    tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, dcolor);

  }
  bool touched = false;
  if(maze_built) {	// then "play" the game (as yet, unwritten)
    do {
      my_idle();
      if(ttgo->getTouch(x, y)) { // if touch
	touched = true;
	break;
      }
    } while(!touched);
  }
// Exit:
  while (ttgo->getTouch(x, y)) { // wait user lifts finger to return to clock
    my_idle();
  }
  tft->fillScreen(TFT_BLACK); // Clear screen
}
