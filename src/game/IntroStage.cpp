#include "IntroStage.h"
#include "framework/utils.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "graphics/fbo.h"
#include "graphics/shader.h"
#include "framework/input.h"
#include "graphics/material.h"
#include "framework/entities/enemy.h"
#include "framework/entities/player.h"
#include "framework/entities/entityUI.h"
#include "StageManager.h"

void Stage::resize()
{
	float width = Game::instance->window_width, height = Game::instance->window_height;
	camera->aspect = width / (float)height;
	camera2D->aspect = width / (float)height;
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);
}

IntroStage::IntroStage()
{
    // Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

	float width = Game::instance->window_width, height = Game::instance->window_height;

    nextStage = "GameStage";

	Vector2 pos = Vector2(width/2, height/2), size = Vector2(width/8, 200);

	Texture* text = Texture::Get("data/meshes/asian.png");
	Material material;
	material.color = Vector4(1);
	material.diffuse = text;
	material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	eButtonID buttontype = PlayButton;
	EntityUI* play = new EntityUI(pos, size, material, UndefinedButton);
	buttons.push_back(play);
}

void IntroStage::render()
{
    // Set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the camera as default
	camera->enable();

	// Set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < buttons.size(); i++) {
		buttons[i]->render(camera2D);
	}

    //drawText(Game::instance->window_width / 2.0f, Game::instance->window_height / 2.0f, "Press the key A to continue!", Vector3(((int) Game::instance->time)%2), 5);
}

void IntroStage::update(double seconds_elapsed)
{
    if (play_button_pressed) {
        StageManager::instance->transitioning = true;
    }

	for (int i = 0; i < buttons.size(); i++) {
		buttons[i]->update(seconds_elapsed);
	}

}

void IntroStage::switchstage(int flag) {
	if (flag == -1) {
		Game::instance->mouse_locked = true;
		SDL_ShowCursor(!Game::instance->mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(Game::instance->mouse_locked));
	}
	else {
		play_button_pressed = false;
		Game::instance->mouse_locked = false;
		SDL_ShowCursor(!Game::instance->mouse_locked);
		SDL_SetRelativeMouseMode((SDL_bool)(Game::instance->mouse_locked));
	}
}

//Keyboard event handler (sync input)
void IntroStage::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: Game::instance->must_exit = true; break; //ESC key, kill the app
	}
}