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

	enum keys : uint8 {
		WALK,
		JUMP,
		SHOOT,
		DASH,
		AUTO
	};

	SDL_Keycode k_walk = SDLK_w;
	SDL_Keycode k_jump = SDLK_SPACE;
	SDL_Keycode k_shoot = SDLK_q;
	SDL_Keycode k_dash = SDLK_LSHIFT;
	SDL_Keycode k_auto = SDLK_e;

	std::vector<SDL_Keycode*> keyset = {&k_walk, &k_jump, &k_shoot, &k_dash, &k_auto};

	StageManager();

	void render();
	void update(double dt);
	void changeStage(std::string nextStage);
	void resize(float width, float height);
};

