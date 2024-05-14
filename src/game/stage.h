/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#pragma once

#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"

class Stage
{
public:
	static Stage* instance;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	float zoom = 1.f;

	Entity* root;

	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Stage();

	//main functions
	void render( void );
	void update( double dt );


	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
};