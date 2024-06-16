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
		LEFT,
		BACK,
		RIGHT,
		JUMP,
		DASH,
		SHOOT,
		AUTO,
		S1,
		S2,
		S3
	};

	SDL_Keycode k_walk = SDLK_w;
	SDL_Keycode k_left = SDLK_a;
	SDL_Keycode k_right = SDLK_d;
	SDL_Keycode k_back = SDLK_s;
	SDL_Keycode k_jump = SDLK_SPACE;
	SDL_Keycode k_shoot = SDLK_q;
	SDL_Keycode k_dash = SDLK_LSHIFT;
	SDL_Keycode k_auto = SDLK_e;
	SDL_Keycode k_s1 = SDLK_1;
	SDL_Keycode k_s2 = SDLK_2;
	SDL_Keycode k_s3 = SDLK_3;

	std::vector<SDL_Keycode*> keyset = {&k_walk, &k_left, &k_back , &k_right , &k_jump, &k_dash, &k_shoot, &k_auto, &k_s1, &k_s2 , &k_s3 };

	StageManager();

	void render();
	void update(double dt);
	void changeStage(std::string nextStage);
	void resize(float width, float height);
};

