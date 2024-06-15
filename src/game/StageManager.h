#pragma once
#include <unordered_map>
#include <string>
#include "framework/framework.h"
#include "framework/includes.h"

class Stage;

class StageManager
{
public:
	static StageManager* instance;
	Vector3 ambient_night;
	Vector3 ambient_day;
	bool transitioning;

	Stage* currStage;
	std::unordered_map<std::string, Stage*> stages;

	float sensitivity = 1;

	SDL_Scancode k_walk = SDL_SCANCODE_W;
	SDL_Scancode k_jump = SDL_SCANCODE_SPACE;
	SDL_Scancode k_shoot = SDL_SCANCODE_Q;
	SDL_Scancode k_dash = SDL_SCANCODE_LSHIFT;

	SDL_Keycode kc_auto = SDLK_e;


	StageManager();

	void render();
	void update(double dt);
	void changeStage(std::string nextStage);
	void resize(float width, float height);
};

