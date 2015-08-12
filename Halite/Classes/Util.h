#ifndef LUTIL_H
#define LUTIL_H

#include "OpenGL.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

///Screen constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

static bool initGL(unsigned short w, unsigned short h)
{
	//Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, w, h, 0, 1.0, -1.0);

	//Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Save the default modelview matrix
	glPushMatrix();

	//Initialize clear color
	glClearColor(0, 0, 0, 0.f);

	//Check for error
	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		return false;
	}

	return true;
}

#endif
