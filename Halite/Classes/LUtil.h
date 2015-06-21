#ifndef LUTIL_H
#define LUTIL_H

#include "LOpenGL.h"
#include "GameRun.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

///Screen constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 1000;
const int SCREEN_FPS = 30;

bool initGL();

void render();

void handleKeys( unsigned char key, int x, int y );

void handleKeysUp(unsigned char key, int x, int y);

void handleSpecialKeys(int key, int x, int y);

void handleSpecialKeysUp(int key, int x, int y);

void handleMouse(int button, int state, int x, int y);

void handlePassiveMouseMotion(int x, int y);

void handleMouseMotion(int x, int y);

void drawCircle(float cx, float cy, float r, int num_segments, int begin_type);

void textToScreen(int x, int y, float r, float g, float b, void *font, std::string str);

#endif
