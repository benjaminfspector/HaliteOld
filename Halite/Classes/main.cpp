#include "LUtil.h"

#include <Windows.h>
#include <time.h>
#include <string>
#include <thread>

enum Job
{
	RENDER,
	WRITE,
	BOTH,
	PAST
};

Job action;

void runRenderLoop( int val );

void runFileLoop();

void bothLoop( int val );

void pastLoop( int val );

int main( int argc, char* args[] )
{
	//Move console to second monitor
	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, 1600, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
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
			action = RENDER;
			break;
		}
		else if(in == "output" || in == "o")
		{
			action = WRITE;
			break;
		}
		else if(in == "both" || in == "b")
		{
			action = BOTH;
			break;
		}
		else
		{
			action = PAST;
			break;
		}
	}
	
	if(action != WRITE)
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
		if(!initGL())
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

		//Set rendering function
		glutDisplayFunc(render);

		//glutFullScreen();
	}



	if(action != PAST)
	{
		init();
	}
	if(action == WRITE || action == BOTH)
	{
		initOutput();
		doOutput(myMap.findWinner());
	}

	if(action == WRITE)
	{
		fileLoop();
	}
	else if(action == PAST)
	{
		initPast();
		glutTimerFunc(1000 / SCREEN_FPS, pastLoop, 0);
		glutMainLoop();
	}
	else
	{
		if(action == RENDER)
		{
			glutTimerFunc(1000 / SCREEN_FPS, renderLoop, 0);
		}
		else if(action == BOTH)
		{
			glutTimerFunc(1000 / SCREEN_FPS, bothLoop, 0);
		}

		render();
		//Start GLUT main loop
		glutMainLoop();
	}

    return 0;
}

void renderLoop( int val )
{
	runPlayers();
	unsigned char result = calculateResults();
	render();

	if(result != 0)
	{
		close();
		system("pause");
		exit(0);
	}

    //Run frame one more time
    glutTimerFunc( 1000 / SCREEN_FPS, runRenderLoop, val );
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
	runPlayers();
	unsigned char result = calculateResults();
	
	std::thread fileThread (doOutput, result);
	std::thread renderThread (render);
	fileThread.join();
	renderThread.join();

	if(result != 0)
	{
		close();
		system("pause");
		exit(0);
	}

	//Run frame one more time
	glutTimerFunc(1000 / SCREEN_FPS, runRenderLoop, val);
}

void pastLoop( int val )
{
	bool result = renderPast();

	if(!result)
	{
		close();
		system("pause");
		exit(0);
	}

	//Run frame one more time
	glutTimerFunc(1000 / SCREEN_FPS, runRenderLoop, val);
}