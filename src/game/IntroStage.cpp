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

void IntroStage::renderLetter(Texture* font, Vector2 tileSize, char letter, Vector2 position, Vector2 size) {
	Vector2 selectedTile;
	if (letter >= 'a' && letter <= 'z') {
		if (letter > 'n') {
			selectedTile = Vector2(letter - 'a' + 1, 2);
		}
		else {
			selectedTile = Vector2(letter - 'a', 2);
		}
	}
	else if (letter >= 'A' && letter <= 'Z') {
		if (letter > 'N') {
			selectedTile = Vector2(letter - 'A' + 1, 1);
		}
		else {
			selectedTile = Vector2(letter - 'A', 1);
		}
	}
	else if (letter == -111) {
		std::cout << letter;
		selectedTile = Vector2('N' + 1 - 'A', 1);
	}
	else if (letter == -79) {
		std::cout << letter;
		selectedTile = Vector2('n' + 1 - 'a', 2);
	}
	else if (letter == ' ') {
		selectedTile = Vector2(26, 0);
	}
	else if (letter == '.') selectedTile = Vector2(5, 0);
	else if (letter == ',') selectedTile = Vector2(4, 0);
	else if (letter == ':') selectedTile = Vector2(2, 0);
	else if (letter == '!') selectedTile = Vector2(0, 0);
	else if (letter == '?') selectedTile = Vector2(1, 0);
	else  selectedTile = Vector2(26, 0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	textShader->enable();

	Mesh quad;
	quad.createQuad(position.x, position.y, size.x, size.y, false);

	Stage* stage = StageManager::instance->currStage;

	textShader->setUniform("u_model", Matrix44());
	textShader->setUniform("u_color", Vector4(1.0f));
	textShader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	textShader->setUniform("u_tileSize", tileSize);
	textShader->setUniform("u_tileGrid", Vector2(27, 3));
	textShader->setUniform("u_selectedTile", selectedTile);

	if (font) {
		textShader->setTexture("u_texture", font, 0);
	}

	quad.render(GL_TRIANGLES);

	textShader->disable();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void IntroStage::renderText(Texture* font, std::string text, float interval, Vector2 position, float fontsize, Vector2 tilesize, Vector2 textbox) {
	float origin = position.x;
	std::istringstream wordStream(text);
	std::string word;

	while (wordStream >> word) {
		// Calculate the width of the word
		float wordWidth = word.length() * tilesize.x * fontsize;

		// Check if the word fits in the current line
		if (position.x + wordWidth > textbox.x) {
			// Move to the next line if the word doesn't fit
			position.x = origin;
			position.y -= tilesize.y * fontsize * 1.1;
		}

		// Render each letter of the word
		for (size_t i = 0; i < word.length(); ++i) {
			if (word[i] == static_cast<char>(-79)) {
				// Ignore the previous character if the current character is 'ñ'
				if (i > 0) {
					position.x -= tilesize.x * fontsize;
				}
				renderLetter(font, tilesize, word[i], position, tilesize * fontsize);
			}
			else {
				renderLetter(font, tilesize, word[i], position, tilesize * fontsize);
			}
			position.x += tilesize.x * fontsize;
		}

		// Add space after the word
		position.x += tilesize.x * fontsize;
	}
}

IntroStage::IntroStage()
{
	font = Texture::Get("data/textures/fontpool.PNG");


	keynames = { "Walk", "Jump", "Dash", "Shoot", "AutoShoot" };

    // Create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f, 100.f, 100.f), Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f, Game::instance->window_width / (float)Game::instance->window_height, 0.1f, 10000.f); //set the projection, we want to be perspective

	camera2D = new Camera();
	camera2D->view_matrix.setIdentity();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

	float width = Game::instance->window_width, height = Game::instance->window_height;

    nextStage = "GameStage";

	Vector2 pos = Vector2(width/2, height/2), size = Vector2(300, 100);
	Material material;
	material.color = Vector4(1);
	material.diffuse = Texture::Get("data/textures/menu/playbutton.PNG");
	material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	eButtonID buttontype = PlayButton;
	EntityUI* play = new EntityUI(pos, size, material, height, buttontype);
	buttons.push_back(play);

	pos = Vector2(width / 2, height / 2 + 120); size = Vector2(300, 100);
	material.diffuse = Texture::Get("data/textures/menu/settingsbutton.PNG");
	buttontype = OptionsButton;
	EntityUI* settings = new EntityUI(pos, size, material, height ,buttontype);
	buttons.push_back(settings);
	material.diffuse = Texture::Get("data/textures/menu/backbutton.PNG");
	buttontype = ExitButton;
	EntityUI* back = new EntityUI(pos, size, material, height, buttontype);
	optionbuttons.push_back(back);

	pos = Vector2(width / 2, height / 2 + 35), size = Vector2(400, 30);
	material.diffuse = Texture::Get("data/textures/menu/bar.PNG");
	material.shader = Shader::Get("data/shaders/basic2.vs", "data/shaders/texture3.fs");
	buttontype = SensBar;
	EntityUI* sensbar = new EntityUI(pos, size, material, height, buttontype);
	optionbuttons.push_back(sensbar);

	pos = Vector2(width / 2, height / 2 - 50), size = Vector2(250, 40);
	material.diffuse = Texture::Get("data/textures/menu/sens.PNG");
	material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	buttontype = UndefinedButton;
	EntityUI* senst = new EntityUI(pos, size, material, height, buttontype);
	optionbuttons.push_back(senst);

	Material squaremat;
	squaremat.color = Vector4(0,0,0,0.5);
	squaremat.diffuse = Texture::Get("data/textures/menu/bar.PNG");
	squaremat.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	pos = Vector2(width / 2, height / 2), size = Vector2(width,height);
	bigblacksquare = EntityUI(pos, size, squaremat, height, UndefinedButton);

	for (int i = 0; i < 5; i++) {
		pos = Vector2(width / 2 + 100, height / 2 - 250 - 120 + 60 * i), size = Vector2(250, 50);
		material.diffuse = Texture::Get("data/textures/menu/bar.PNG");
		material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		buttontype = (eButtonID) (KeyWalk + i);
		EntityUI* key1 = new EntityUI(pos, size, material, height, buttontype);
		optionbuttons.push_back(key1);
	}

	Audio::Get("data/audio/menu/menubgm.mp3", BASS_SAMPLE_LOOP);
}



void IntroStage::render()
{
	if (StageManager::instance->transitioning) return;
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

	if (options) {
		std::string filler;
		if (StageManager::instance->sensitivity >= 1) {
			filler = "";
		}
		else if (StageManager::instance->sensitivity >= 0.1) {
			filler = "0";
		}
		else {
			filler = "00";
		}
		drawText(Game::instance->window_width / 2.0f - 20, Game::instance->window_height / 2.0f - 20,filler + std::to_string(((int)floor(StageManager::instance->sensitivity * 100))), Vector3(1), 2);
		for (int i = 0; i < optionbuttons.size(); i++) {
			optionbuttons[i]->render(camera2D);
		}
		for (int i = 0; i < 5; ++i) {
			float width = Game::instance->window_width, height = Game::instance->window_height;
			Vector2 pos = Vector2(width / 2 - 120, height / 2 - 250 - 120 + 60 * i);
			drawText(pos.x + 110, pos.y - 10, SDL_GetKeyName(*StageManager::instance->keyset[i]), Vector3(0), 3);
			drawText(pos.x - 100, pos.y - 10, keynames[i], Vector3(1), 3);
		}
		if (keybinds[selected_keybind]) {
			bigblacksquare.color = Vector4(0, 0, 0, 0.5);
			bigblacksquare.render(camera2D);
		}
		for (int i = 0; i < 5; ++i) {
			if (i == selected_keybind) {
				float width = Game::instance->window_width, height = Game::instance->window_height;
				Vector2 pos = Vector2(width / 2 - 120, height / 2 - 250 - 120 + 60 * i);
				drawText(pos.x - 100, pos.y - 10, keynames[i], Vector3(1), 3);
			}
		}
	}
	else {
		for (int i = 0; i < buttons.size(); i++) {
			buttons[i]->render(camera2D);
		}
	}


	for (bool kb : keybinds) {
		std::cout << kb << " ";
	}
	std::cout << std::endl;

    //drawText(Game::instance->window_width / 2.0f, Game::instance->window_height / 2.0f, "Press the key A to continue!", Vector3(((int) Game::instance->time)%2), 5);
}

void IntroStage::update(double seconds_elapsed)
{
	if (StageManager::instance->transitioning) return;
    if (play_button_pressed) {
        StageManager::instance->transitioning = true;
    }

	if (options) {
		if (!keybinds[selected_keybind]) {
			for (int i = 0; i < optionbuttons.size(); i++) {
				optionbuttons[i]->update(seconds_elapsed);
			}
		}
	}
	else {
		for (int i = 0; i < buttons.size(); i++) {
			buttons[i]->update(seconds_elapsed);
		}
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
		backgmusic = Audio::Play("data/audio/menu/menubgm.mp3");
	}
}

//Keyboard event handler (sync input)
void IntroStage::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: Game::instance->must_exit = true; break; //ESC key, kill the app
	}

	if (keybinds[selected_keybind]) {
		*StageManager::instance->keyset[selected_keybind] = event.keysym.sym;
		keybinds[selected_keybind] = false;
	}
}

void IntroStage::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) //middle mouse
	{
		mouse_clicked = false;
	}
}