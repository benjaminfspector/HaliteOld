#include <iostream>

#include "Util.h"
#include "GameLogic/Halite.h"

void handleMouse(int button, int state, int x, int y);
void handlePassiveMouseMotion(int x, int y);
void handleMouseMotion(int x, int y);
void handleKeys(unsigned char key, int x, int y);
void handleKeysUp(unsigned char key, int x, int y);
void handleSpecialKeys(int key, int x, int y);
void handleSpecialKeysUp(int key, int x, int y);

void render();
void doLogic();
void renderLoop(int val);

Halite my_game;
bool isPaused = false;
signed short turn_number = 0, fps = 30;

int main(int argc, char* args[])
{
	/*#ifdef _WIN32
	HANDLE consoleWindow = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT r; r.Left = 0; r.Top = 0; r.Right = 5000; r.Bottom = 5000; COORD c; c.X = 5001; c.Y = 5001;
	SetConsoleWindowInfo(consoleWindow, TRUE, &r);
	SetConsoleScreenBufferSize(consoleWindow, c);
	#endif*/

	std::string in;
	std::thread logicThread;
	unsigned short mapWidth, mapHeight;

	std::cout << "Would you like to run a new game or render a past one? Please enter \"New\" or \"Past\": ";
	while(true)
	{
		std::getline(std::cin, in);
		std::transform(in.begin(), in.end(), in.begin(), ::tolower);
		if(in == "p" || in == "past" || in == "n" || in == "new") break;
		std::cout << "That isn't a valid input. Please enter \"New\" or \"Past\": ";
	}

	if(in == "new" || in == "n")
	{
		std::cout << "Please enter the width of the map: ";
		std::getline(std::cin, in);
		while(true)
		{
			try
			{
				mapWidth = std::stoi(in);
				break;
			}
			catch(std::exception e)
			{
				std::cout << "That isn't a valid input. Please enter an integer width of the map: ";
				std::getline(std::cin, in);
			}
		}
		std::cout << "Please enter the height of the map: ";
		std::getline(std::cin, in);
		while(true)
		{
			try
			{
				mapHeight = std::stoi(in);
				break;
			}
			catch(std::exception e)
			{
				std::cout << "That isn't a valid input. Please enter an integer height of the map: ";
				std::getline(std::cin, in);
			}
		}

		my_game = Halite(mapWidth, mapHeight);

		my_game.init();

		logicThread = std::thread(doLogic);
	}
	else
	{
		std::cout << "Please enter the name of the file you'd like to play back from: ";
		std::getline(std::cin, in);
		while(!my_game.input(in, mapWidth, mapHeight))
		{
			std::cout << "I couldn't open the specified file. Please enter the name of another file to play back from: ";
			std::getline(std::cin, in);
		}
	}

	//Initialize FreeGLUT
	glutInit(&argc, args);

	//Create OpenGL 2.1 context
	glutInitContextVersion(2, 1);

	//Create Double Buffered Window
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Halite");

	if(!initGL(mapWidth, mapHeight))
	{
		printf("Unable to initialize graphics library!\n");
		return 1;
	}

	//Set leopard handler
	glutKeyboardFunc(handleKeys);

	glutKeyboardUpFunc(handleKeysUp);

	//Set special leopard handler
	glutSpecialFunc(handleSpecialKeys);

	glutSpecialUpFunc(handleSpecialKeysUp);

	//Set mouse handler
	glutMouseFunc(handleMouse);

	//Set passive mouse motion handler
	glutPassiveMotionFunc(handlePassiveMouseMotion);

	//Set mouse motion handler
	glutMotionFunc(handleMouseMotion);

	//Set display function
	glutDisplayFunc(render);


	glutTimerFunc(1000 / fps, renderLoop, 0);
	glutMainLoop();

	return 0;
}

void renderLoop(int val)
{
	render();
	if(!isPaused)
	{
		if(fps > 0) turn_number++;
		else if(fps < 0) turn_number--;
	}
	glutTimerFunc(1000 / float(abs(fps)), renderLoop, val);
}

void doLogic()
{
	std::string winner = my_game.runGame();
	std::cout << "Player " << winner << " has won the game!\n";
}

void handleMouse(int button, int state, int x, int y)
{

}

void handlePassiveMouseMotion(int x, int y)
{

}

void handleMouseMotion(int x, int y)
{

}

void handleKeys(unsigned char key, int x, int y)
{
	if(key == 27) //Escape
	{
		exit(0);
	}
	else if(key == 'f' || key == 'F')
	{
		glutFullScreenToggle();
	}
	else if(key == 'b' || key == 'B')
	{
		turn_number = 0;
	}
	else if(key == 'e' || key == 'E')
	{
		turn_number = 65535;
	}
	else if(key == 'r' || key == 'R')
	{
		fps = 0;
		std::cout << "FPS reset to " << fps << "!\n";
	}
	else if(key == ' ')
	{
		isPaused = !isPaused;
	}
	else if(key == 'c' || key == 'C')
	{
		my_game.getColorCodes();
	}
	else if(key == 'o' || key == 'O')
	{
		my_game.output();
	}
}

void handleKeysUp(unsigned char key, int x, int y)
{

}

static bool lShift = false, rShift = false;
void handleSpecialKeys(int key, int x, int y)
{
	if(key == GLUT_KEY_SHIFT_L)
	{
		lShift = true;
	}
	else if(key == GLUT_KEY_SHIFT_R)
	{
		rShift = true;
	}
	else if(key == GLUT_KEY_LEFT)
	{
		if(lShift || rShift) (turn_number) -= 15;
		else (turn_number)--;
		isPaused = true;
	}
	else if(key == GLUT_KEY_RIGHT)
	{
		if(lShift || rShift) (turn_number) += 15;
		else (turn_number)++;
		isPaused = true;
	}
	else if(key == GLUT_KEY_UP)
	{
		fps++;
		std::cout << "FPS increased to " << fps << "!\n";
	}
	else if(key == GLUT_KEY_DOWN)
	{
		fps--;
		std::cout << "FPS decreased to " << fps << "!\n";
	}
}

void handleSpecialKeysUp(int key, int x, int y)
{
	if(key == GLUT_KEY_SHIFT_L)
	{
		lShift = false;
	}
	else if(key == GLUT_KEY_SHIFT_R)
	{
		rShift = false;
	}
}

void render()
{
	//Ensure within game
	my_game.confirmWithinGame(turn_number);

	//Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//Take saved matrix off the stack and reset it
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	my_game.render(turn_number);

	//Render Test square
	/*glColor3ub(255, 255, 0);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(20, 0);
	glVertex2f(20, 20);
	glVertex2f(0, 20);
	glEnd();*/

	glutSwapBuffers();
}