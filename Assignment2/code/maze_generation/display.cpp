#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <SDL2/SDL.h>
using namespace std;

bool ** maze(int x,int y);
const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 700;


//The window we'll be rendering to
SDL_Window* gWindow = NULL;
	
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface( gWindow );
		}
	}

	return success;
}

bool loadMedia(string s)
{
	//Loading success flag
	bool success = true;
	const char * c= s.c_str();
	//Load splash image
	gHelloWorld = SDL_LoadBMP(c);
	if( gHelloWorld == NULL )
	{
		printf( "Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError() );
		success = false;
	}

	return success;
}

void close()
{
	//Deallocate surface
	SDL_FreeSurface( gHelloWorld );
	gHelloWorld = NULL;

	//Destroy window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc,char * argv[]){
	int x = (argc > 1) ? stoi(argv[1]) : 0;
    	int y = (argc > 2) ? stoi(argv[2]) : 0;
    	bool  ** Map=maze(x,y);
	string s="MAZE_20x20_n324.bmp";
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia(s) )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Apply the image
			SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );
			
			//Update the surface
			SDL_UpdateWindowSurface( gWindow );

			//Wait two seconds
			SDL_Delay( 2000 );
		}
	}
	/*for (int j=0;j<5 *16;j++){
		for (int i=0;i<5 * 16;i++){
		cout<<Map[i][j]<<",";
		}
		cout<<"\n";
	}*/

	//Free resources and close SDL
	close();
	return 0;
}
