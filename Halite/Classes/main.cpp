#include "LUtil.h"

#include <Windows.h>
#include <time.h>
#include <string>
#include <thread>

static Job myAction;

bool amDone = true;

unsigned short result = 0;

void renderLoop( int val );

void fileLoop();

void bothLoop( int val );

void pastLoop( int val );

int main( int argc, char* args[] )
{
	//Move console to second monitor
	HWND consoleWindow = GetConsoleWindow();
	//SetWindowPos(consoleWindow, 0, 1600, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	//End moveConsole

    srand(time(NULL));

	std::string in;
	std::cout << "Would you like to render a new game, output a new game to a file, do both, or render a past game? Please enter \"Render\", \"Output\", \"Both\", or \"Past\": ";
	std::cin >> in;
	while(true)
	{
		std::transform(in.begin(), in.end(), in.begin(), ::tolower);
		if(in != "render" && in != "r" && in != "output" && in != "o" && in != "both" && in != "b" && in != "past" && in != "p")
		{
			std::cout << "That is not a valid response. Please enter \"Render\", \"Output\", \"Both\", or \"Read\": ";
			std::cin >> in;
		}
		else if(in == "render" || in == "r")
		{
			myAction = RENDER;
			break;
		}
		else if(in == "output" || in == "o")
		{
			myAction = WRITE;
			break;
		}
		else if(in == "both" || in == "b")
		{
			myAction = BOTH;
			break;
		}
		else
		{
			myAction = PAST;
			break;
		}
	}

	unsigned short w, h;
	if(myAction != PAST)
	{
		std::cout << "Please enter the width of the map: ";
		while(true)
		{
			std::cin >> in;
			try
			{
				w = std::stoi(in);
				break;
			}
			catch(const std::invalid_argument& ia)
			{
				std::cout << "That is not an integer. Please enter an integer value: ";
			}
		}

		std::cout << "Please enter the height of the map: ";
		while(true)
		{
			std::cin >> in;
			try
			{
				h = std::stoi(in);
				break;
			}
			catch(const std::invalid_argument& ia)
			{
				std::cout << "That is not an integer. Please enter an integer value: ";
			}
		}
	}
	
	initColorCodes();

	mapDimensions pastMapD;
	if(myAction == PAST)
	{
		pastMapD = initPast();
	}

	if(myAction != WRITE)
	{
		//Initialize FreeGLUT
		glutInit(&argc, args);

		//Create OpenGL 2.1 context
		glutInitContextVersion(2, 1);

		//Create Double Buffered Window
		glutInitDisplayMode(GLUT_DOUBLE);
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		glutCreateWindow("Halite");

		//Do post window/context creation initialization
		if(myAction != PAST)
		{
			if(!initGL(myAction, w, h))
			{
				printf("Unable to initialize graphics library!\n");
				return 1;
			}
		}
		else
		{
			if(!initGL(myAction, pastMapD.w, pastMapD.h))
			{
				printf("Unable to initialize graphics library!\n");
				return 1;
			}
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

		glutFullScreen();
		//system("pause");
	}

	if(myAction != PAST)
	{
		init(w, h);		
	}
	if(myAction == WRITE || myAction == BOTH)
	{
		initOutput();
		doOutput(myMap.findWinner());
	}

	if(myAction == WRITE)
	{
		fileLoop();
	}
	else if(myAction == PAST)
	{
		//Set rendering function
		glutDisplayFunc(renderPast);
		glutTimerFunc(1000 / SCREEN_FPS, pastLoop, 0);
		glutMainLoop();
	}
	else
	{
		if(myAction == RENDER)
		{
			glutTimerFunc(1000 / SCREEN_FPS, renderLoop, 0);
		}
		else if(myAction == BOTH)
		{
			glutTimerFunc(1000 / SCREEN_FPS, bothLoop, 0);
		}

		//Set rendering function
		glutDisplayFunc(render);
		render();
		outputPlayerColorCodes();
		glutMainLoop();
	}

    return 0;
}

void renderLoop( int val )
{
	if (result == 0)
	{
		runPlayers();
		result = calculateResults();
	}
	render();

    //Run frame one more time
    glutTimerFunc( 1000 / SCREEN_FPS, renderLoop, val );
}

void fileLoop()
{
	while(true)
	{
		runPlayers();
		unsigned char result = calculateResults();
		doOutput(result);

		if(result != 0)
		{
			close();
			system("pause");
			exit(0);
		}
	}
}

void bothLoop( int val )
{
	if (result == 0)
	{
		runPlayers();
		result = calculateResults();
		if (result != 256)
		{
			doOutput(result);
			result = 256;
		}
	}
	
	//std::thread fileThread (doOutput, result);
	//std::thread renderThread (render);
	//fileThread.join();
	//renderThread.join();
	
	render();

	//Run frame one more time
	glutTimerFunc(1000 / SCREEN_FPS, bothLoop, val);
}

void pastLoop( int val )
{
	if (amDone)
	amDone = getPast();

	renderPast();

	//Run frame one more time
	glutTimerFunc(1000 / SCREEN_FPS, pastLoop, val);
}