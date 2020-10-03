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

int hasNeighbour(vector <Room> maze, Room r, int direction) {
  int x = r.getX();
  int y = r.getY();
  // walls are: 0,1,2,3 -> up, right, down, left
  switch(direction) {
    case 0 :	// up
      if(y > 0) return x * ROWS + y - 1;
      break;
    case 1 :	// right
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
int16_t xpos, ypos;
  ttgo->bma->begin();
  ttgo->bma->enableAccel();
  Accel acc;

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

    ++chkn_count;
    // Serial.printf("checkNeighbours() %d\n", chkn_count);
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
    // Serial.printf("l:%d %d %d %d %d %04x\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, dcolor);
    // this "cursor" shows the room we're "visiting":
    tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_DARKGREY);

  }
  bool touched = false;
  if(maze_built) {	// then "play" the game
    int xCoordHead = current->getX() * ROOM_WIDTH;
    int yCoordHead = current->getY() * ROOM_WIDTH;
#if DEBUGGAME
    Serial.printf("old:%d %d %d %d %d %04x\n", __LINE__, xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_WHITE);
#endif
    tft->fillRect(xCoordHead, yCoordHead, ROOM_WIDTH, ROOM_WIDTH, TFT_WHITE);
    Room *destination = &mazeGrid[(rand() % ROWS) + (ROWS-1)*COLUMNS]; 
    int xDest = destination->getX();
    int xDestCoord = xDest * ROOM_WIDTH;
    int yDest = destination->getY();
    int yDestCoord = yDest * ROOM_WIDTH;
#if DEBUGGAME
    Serial.printf("dest:%d %d %d %d %d %04x\n", __LINE__, xDestCoord, yDestCoord, ROOM_WIDTH, ROOM_WIDTH, TFT_RED);
#endif
    tft->fillCircle(xDestCoord+(ROOM_WIDTH>>1), yDestCoord+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-2, TFT_RED);
    current = &mazeGrid[rand() % ROWS]; 
    int xCurs = current->getX();
    int xCursCoord = xCurs * ROOM_WIDTH;
    int yCurs = current->getY();
    int yCursCoord = yCurs * ROOM_WIDTH;
#if DEBUGGAME
    Serial.printf("curs:%d %d %d %d %d %04x\n", __LINE__, xCursCoord, yCursCoord, ROOM_WIDTH, ROOM_WIDTH, TFT_BLUE);
#endif
    tft->fillCircle(xCursCoord+(ROOM_WIDTH>>1), yCursCoord+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-2, TFT_BLUE);
#define TILT_MIN 20
#define TILT_THRESH 40
    do {
      my_idle();
      // walls are: 0,1,2,3 -> up, right, down, left
      mSelect = poll_swipe_or_menu_press(12);	// poll for touch or gesture
      int next = -1;
      if(mSelect == -1) {
	ttgo->bma->getAccel(acc);
	xpos = acc.x / 10;
	ypos = acc.y / 10;
#if DEBUGGAME
	Serial.printf("xpos = %d, ypos = %d\n", xpos, ypos);
#endif
	if(xpos > TILT_THRESH && abs(ypos) < TILT_MIN ) {
#if DEBUGGAME
	  Serial.println(F("tilt right"));
#endif
	  mSelect = RIGHT;
	}
	else if(xpos < (0 - TILT_THRESH) && abs(ypos) < TILT_MIN ) {
#if DEBUGGAME
	  Serial.println(F("tilt left"));
#endif
	  mSelect = LEFT;
	}
	if(ypos > TILT_THRESH && abs(xpos) < TILT_MIN ) {
#if DEBUGGAME
	  Serial.println(F("tilt down"));
#endif
	  mSelect = DOWN;
	}
	if(ypos < (0 - TILT_THRESH) && abs(xpos) < TILT_MIN ) {
#if DEBUGGAME
	  Serial.println(F("tilt up"));
#endif
	  mSelect = UP;
	}
      }
      if(mSelect == (UP)) { // swipe 32 = up, 33 = down, left = 34, rt = 35
#if DEBUGGAME
	Serial.println(F("up"));
#endif
	// if up wall missing, move up
	if(!current->hasWall(0)) {
#if DEBUGGAME
	  Serial.println(F("does not have 'up' wall"));
#endif
	  next = hasNeighbour(mazeGrid, *current, 0);
	}
      }
      else if(mSelect == DOWN) {
#if DEBUGGAME
	Serial.println(F("down"));
#endif
	// if down wall missing, move down
	if(!current->hasWall(2)) {
#if DEBUGGAME
	  Serial.println(F("does not have 'down' wall"));
#endif
	  next = hasNeighbour(mazeGrid, *current, 2);
	}
      }
      else if(mSelect == LEFT) {
#if DEBUGGAME
	Serial.println(F("left"));
#endif
	// if left wall missing, move left
	if(!current->hasWall(3)) {
#if DEBUGGAME
	  Serial.println(F("does not have 'left' wall"));
#endif
	  next = hasNeighbour(mazeGrid, *current, 3);
	}
      }
      else if(mSelect == RIGHT) {
#if DEBUGGAME
	Serial.println(F("right"));
#endif
	// if right wall missing, move right
	if(!current->hasWall(1)) {
#if DEBUGGAME
	  Serial.println(F("does not have 'right' wall"));
#endif
	  next = hasNeighbour(mazeGrid, *current, 1);
	}
      }
      else if(mSelect == CWCIRCLE) {
	touched = true;
	break;
      }
      if(next > -1) {
	int OldXCursCoord = xCursCoord;
	int OldYCursCoord = yCursCoord;
	Room &nextRoom = mazeGrid[next];
	current = &nextRoom; 
	xCurs = current->getX();
	xCursCoord = xCurs * ROOM_WIDTH;
	yCurs = current->getY();
	yCursCoord = yCurs * ROOM_WIDTH;
#define ANIMATE 1
#if ANIMATE
	if(xCursCoord != OldXCursCoord) {
	  int increment = (xCursCoord - OldXCursCoord) / ROOM_WIDTH;
#if DEBUGGAME
	  Serial.printf("R/L oldX = %d, newX = %d, increment = %d\n", OldXCursCoord, xCursCoord, increment);
#endif
	  for(int i = OldXCursCoord ; i != (xCursCoord - increment) ; i += increment) {
	    tft->fillCircle(i+(ROOM_WIDTH>>1), yCursCoord+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-1, TFT_WHITE);
	    tft->fillCircle(i+increment+(ROOM_WIDTH>>1), yCursCoord+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-2, TFT_BLUE);
	    delay(5);
	  }
	}
	else if(yCursCoord != OldYCursCoord) {
	  int increment = (yCursCoord - OldYCursCoord) / ROOM_WIDTH;
#if DEBUGGAME
	  Serial.printf("U/D: oldY = %d, newY = %d, increment = %d\n", OldYCursCoord, yCursCoord, increment);
#endif
	  for(int i = OldYCursCoord ; i != (yCursCoord - increment) ; i += increment) {
	    tft->fillCircle(xCursCoord+(ROOM_WIDTH>>1), i+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-1, TFT_WHITE);
	    tft->fillCircle(xCursCoord+(ROOM_WIDTH>>1), i+increment+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-2, TFT_BLUE);
	    delay(5);
	  }
	}
#else
#if DEBUGGAME
	Serial.printf("hasNeighbour() -> %d\n", next);
	Serial.printf("old:%d %d %d %d %d %04x\n", __LINE__, OldXCursCoord, OldYCursCoord, ROOM_WIDTH, ROOM_WIDTH, TFT_WHITE);
#endif
	tft->fillCircle(OldXCursCoord+(ROOM_WIDTH>>1), OldYCursCoord+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-1, TFT_WHITE);
#if DEBUGGAME
	Serial.printf("curs:%d %d %d %d %d %04x\n", __LINE__, xCursCoord+1, yCursCoord+1, ROOM_WIDTH-2, ROOM_WIDTH-2, TFT_BLUE);
#endif
	// this "cursor" shows the room we're "visiting":
	tft->fillCircle(xCursCoord+(ROOM_WIDTH>>1), yCursCoord+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-2, TFT_BLUE);
#endif
	if(xDest == xCurs && yDest == yCurs) {
	  tft->fillCircle(xCursCoord+(ROOM_WIDTH>>1), yCursCoord+(ROOM_WIDTH>>1), (ROOM_WIDTH>>1)-2, TFT_PURPLE);
	  beep(6); beep(7);	// ta-daaaa!
	}
      }
      else if(mSelect > 11) {	// bumped into something, couldn't move
	// beep(3); beep(0);
      }
    } while(!touched);
  }
// Exit:
  while (ttgo->getTouch(x, y)) { // wait user lifts finger to return to clock
    my_idle();
  }
  tft->fillScreen(TFT_BLACK); // Clear screen
}
