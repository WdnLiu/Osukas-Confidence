#include "entityUI.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/StageManager.h"

#include <algorithm>
#include <framework/input.h>




void EntityUI::render(Camera* camera2D) {
	if (!visible) return;

	if (!is3d) glEnable(GL_DEPTH_TEST);
	
	if (!mesh) {
		std::cout << "NO MESH";
		return;
	}

	if (posteffects) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	material.shader->enable();

	Stage* stage = StageManager::instance->currStage;

	//material.shader->setUniform("u_camera_pos")
	material.shader->setUniform("u_model", Matrix44());
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera2D->viewprojection_matrix);
	//material.shader->setUniform("u_mask", mask);

	if (button_id == SensBar) {
		float percent = (sqrt(StageManager::instance->sensitivity) / sqrt(5));
		if (StageManager::instance->sensitivity < 0.6) {
			percent -= 0.1 * (0.6 - StageManager::instance->sensitivity) / 0.6;
		}
		material.shader->setUniform("u_percentage", percent);
		//std::cout << " " << percent << std::endl;
	}

	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0);
	}

	if (is3d) {
		Vector2 _size = size;
		float max_dist = 5.0f;
		float dist = clamp(stage->camera->eye.distance(pos3d), 0.01f, max_dist);
		_size *= 1.f - (dist / max_dist);

		Mesh quad;
		quad.createQuad(position.x, position.y, _size.x, _size.y, true);
		quad.render(GL_TRIANGLES);
	}
	else {
		mesh->render(GL_TRIANGLES);
	}
	material.shader->disable();


	if (posteffects) {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}


	// Render hijos
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->render(camera2D);
	}
};


void EntityUI::update(float delta_time) {
	Vector2 mouse_pos = Input::mouse_position;

	if (button_id != UndefinedButton &&
		mouse_pos.x > (position.x - size.x * 0.5) &&
		mouse_pos.x < (position.x + size.x * 0.5) &&
		mouse_pos.y >(position.y - size.y * 0.5) &&
		mouse_pos.y < (position.y + size.y * 0.5)) {
		if (material.color.x == default_color.x &&
			material.color.y == default_color.y &&
			material.color.z == default_color.z &&
			material.color.w == default_color.w)
		if (button_id != SensBar) {
			Audio::Play("data/audio/menu/select.wav");
		}
		Stage* stage = StageManager::instance->currStage;
		material.color = selected_color;
		float percentage;
		if (Input::isMousePressed(SDL_BUTTON_LEFT) && !stage->mouse_clicked) {
			stage->mouse_clicked = true;
			switch (button_id) {
			case PlayButton:
				stage->play_button_pressed = true;
				break;
			case OptionsButton:
				stage->options = true;
				break;
			case ExitButton:
				stage->options = false;
				break;
			case KeyWalk:
				stage->selected_keybind = StageManager::WALK;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyJump:
				stage->selected_keybind = StageManager::JUMP;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyDash:
				stage->selected_keybind = StageManager::DASH;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyShoot:
				stage->selected_keybind = StageManager::SHOOT;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			case KeyAuto:
				stage->selected_keybind = StageManager::AUTO;
				stage->keybinds[stage->selected_keybind] = true;
				break;
			}
		}
		else if (Input::isMousePressed(SDL_BUTTON_LEFT)) {
			stage->mouse_clicked = true;
			switch (button_id) {
			case SensBar:
				float percentage = (mouse_pos.x - (position.x - size.x * 0.5)) / size.x;
				//std::cout << percentage;
				StageManager::instance->sensitivity = percentage * percentage * 4.9 + 0.1;
				break;
			}
		}
	}
	else {
		material.color = default_color;
	}



	Entity::update(delta_time);
}

void EntityUI::update3D(Vector3 position3d) {
	pos3d = position3d;

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	Stage* stage = StageManager::instance->currStage;

	visible = true;

	Vector3 pos = stage->camera->project(position3d, width, height);
	if (pos.z < 0.0f || pos.z > 1.0f) {
		visible = false;
	}
	else {
		pos.y = height - pos.y;
		position = Vector2(pos.x, pos.y);
	}
}

void EntityUI::addLOD(sMeshLOD mesh_lod) {
	mesh_lods.push_back(mesh_lod);
	std::sort(mesh_lods.begin(), mesh_lods.end(), [](sMeshLOD a, sMeshLOD b) { return a.distance < b.distance; });
}