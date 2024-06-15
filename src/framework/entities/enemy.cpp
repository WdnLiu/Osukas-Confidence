#include "enemy.h"
#include <game/StageManager.h>
#include "bullet/bullet.h"
#include "game/GameStage.h"
#include "player.h"

Enemy::Enemy(Mesh* mesh, const Material& material, const std::string& name, bool is_dynamic, int HP)
	: EntityCollider(mesh, material, ENEMY, name, is_dynamic)
{
	loadTextures();

	this->maxHP = HP; this->currHP = HP;
	this->type = COL_TYPE::ENEMY;

	moving = true;
	startMoving = Game::instance->time;
	startFiring = -1;

	direction = Vector3(0.0f);

	m_spd = 10;
	v_spd = 0;

	direction.x = (rand() % 2) * 2 - 1;
	direction.z = (rand() % 2) * 2 - 1;

	particle_emitter = new ParticleEmitter();
}

void Enemy::render(Camera* camera)
{


	for (int i = bullets.size() - 1; i >= 0; i--)
		bullets[i]->render(camera);
	bullets_normal.render(camera);
	bullets_ball.render(camera);
	bullets_smallball.render(camera);
	bullets_giantball.render(camera);

	// Render the Shadow
	Matrix44 squash = model;
	squash.setTranslation(Vector3(model.getTranslation().x, ground_y, model.getTranslation().z));

	if (a_current == DEATH) {
		squash.rotate(squash.getYawRotationToAimTo(camera->eye), Vector3::UP);
		squash.translate(-0.25, 0, (Game::instance->time - a_timer - 3 > 0) ? - 1.5 * clamp((Game::instance->time - a_timer - 3)* (Game::instance->time - a_timer - 3), 0, 2 / 1.5) : 0);
	}
	squash.scale(3.5 / (1 + ground_below_y), 0.01, 3.5 / (1 + ground_below_y));

	//glDepthFunc(GL_GREATER);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	//glDepthMask(false);
	flat_shader->enable();

	flat_shader->setUniform("u_color", Vector4(0.0f, 0.0f, 0.0f, 0.5f));
	flat_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	flat_shader->setUniform("u_model", squash);

	shadow_mesh->render(GL_TRIANGLES);

	anim = animations[a_current];
	anim->assignTime(Game::instance->time - a_timer);
	Skeleton blended_skeleton = anim->skeleton;
	if (Game::instance->time - a_timer < ANIM_TRANSITION) {
		blendSkeleton(&animations[a_latest]->skeleton,
			&animations[a_current]->skeleton,
			(Game::instance->time - a_timer) / ANIM_TRANSITION, &blended_skeleton);
	}
	
	if (!material.shader) {
		material.shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	}
	anim->assignTime(Game::instance->time);
	material.shader->enable();
	material.shader->setUniform("u_color", material.color);
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0 /*Slot que ocupa en la CPU, cuando tengamos mas texturas ya nos organizamos*/);
	}
	material.shader->setUniform("u_model", model);
	material.shader->setUniform("u_time", Game::instance->time - a_timer);

	mesh->renderAnimated(GL_TRIANGLES, &blended_skeleton);
	// std::cout << isAnimated << std::endl;

	// Disable shader after finishing rendering
	material.shader->disable();

	Stage* stage = StageManager::instance->currStage;
	if (stage->secondPhase) particle_emitter->render(camera);

	//showHitbox(camera);
}

void Enemy::showHitbox(Camera* camera) {


	Matrix44 m = model;

	flat_shader->enable();

	float sphere_radius = HITBOX_RAD;
	m.translate(0.0f, getPosition().y + 1.2, 0.0f);
	m.scale(sphere_radius, sphere_radius, sphere_radius);

	flat_shader->setUniform("u_color", Vector4(touching_ground, 0.0f, colliding, 1.0f));
	flat_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	flat_shader->setUniform("u_model", m);

	hitbox_mesh->render(GL_LINES);

	flat_shader->disable();

	flat_shader->enable();

	sphere_radius = HITBOX_RAD;
	m.translate(0.0f, getPosition().y + 1.2 + HITBOX_RAD, 0.0f);
	m.scale(sphere_radius, sphere_radius, sphere_radius);

	flat_shader->setUniform("u_color", Vector4(touching_ground, 0.0f, colliding, 1.0f));
	flat_shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	flat_shader->setUniform("u_model", m);

	hitbox_mesh->render(GL_LINES);

	flat_shader->disable();
}

void Enemy::sphere_bullet_collision(Vector3 position, float radius) {
	Stage* stage = StageManager::instance->currStage;
	for (Bullet* bullet : StageManager::instance->currStage->player->bullets) {
		sCollisionData data;
		if (bullet->isInstanced) {
			for (Matrix44& instanced_model : bullet->models) {
				if (bullet->mesh->testSphereCollision(instanced_model, position, radius, data.colPoint, data.colNormal)) {
					colliding = true;
					bullet->to_delete = true;
					stage->anxiety_dt += bullet->damage;
				}
			}
		}
		else {
			if (bullet->active) {
				Vector3 newpos = position - model.frontVector() * 2 + Vector3(0,1*radius, 0);
				if (bullet->mesh->testSphereCollision(bullet->model, newpos, 2 * radius, data.colPoint, data.colNormal)) {
					colliding = true;
					bullet->active = false;
					stage->anxiety_dt += bullet->damage;
					Audio::Play("data/audio/whip.wav");
				}
			}
		}
	}
	BulletNormal& bns = stage->player->bullets_normal;
	for (int i = 0; i < bns.models.size(); i++) {
		Matrix44& m = stage->player->bullets_normal.models[i];
		sCollisionData data;
		if (bns.mesh->testSphereCollision(m, position, radius, data.colPoint, data.colNormal)) {
			bns.despawnBullet(i);
			stage->anxiety_dt += bns.damage;
		}
	}
	BulletAuto& bas = stage->player->bullets_auto;
	for (int i = 0; i < bas.models.size(); i++) {
		Matrix44& m = bas.models[i];
		sCollisionData data;
		if (bas.mesh->testSphereCollision(m, position, radius, data.colPoint, data.colNormal)) {
			bas.despawnBullet(i);
			stage->anxiety_dt += bas.damage;
		}
	}
}

Vector3 Enemy::updateSubframe(float time_elapsed) {
	Stage* stage = StageManager::instance->currStage;

	return NULL;
}

void Enemy::update(float time_elapsed)
{
	//std::cout << "Aqui llega? ";

	Stage* stage = StageManager::instance->currStage;

	if (stage->transitioningPhase && !a_transition_started) {
		a_timer = Game::instance->time;
		a_transition_started = true;
		if (!stage->secondPhase) {
			this->model.rotate(this->model.getYawRotationToAimTo(stage->player->model.getTranslation()) * time_elapsed * 10, Vector3::UP);
			a_current = RAGE;
			a_latest = RAGE;
		}
		else if (stage->beginIdle) {
			a_current = LAUGH;
			a_latest = LAUGH;
		}
		else {
			a_current = DEATH;
			a_latest = DEATH;
		}
		return;
	}


	a_transition_started = false;

	//std::cout << "Llega?" << std::endl;

	if (stage->secondPhase) {
		particle_emitter->setRate(0.1);
		particle_emitter->update(time_elapsed);
		particle_emitter->setSpawnPosition(model.getTranslation());
	}

	Vector3 player_center = getPosition() + Vector3(0, 1.2, 0);

	if (moving) {
		if (beginning_shoot) {
			this->model.rotate(this->model.getYawRotationToAimTo(Vector3(current_destination.x, 0, current_destination.y)) * 3 * time_elapsed, Vector3::UP);
		}
		else {
			this->model.rotate(this->model.getYawRotationToAimTo(Vector3(current_destination.x, 0, current_destination.y)) * time_elapsed, Vector3::UP);
		}
	}
	else if (looking_at_player) {
		this->model.rotate(this->model.getYawRotationToAimTo(stage->player->model.getTranslation()) * 3 * time_elapsed, Vector3::UP);
	}

	//player_center.y += PLAYER_HEIGHT;

	ground_below_y = 10000;
	ground_y = -10000;

	std::vector<sCollisionData> ground;
	//std::vector<sCollisionData> collisions;
	stage->ray_collided(stage->root, ground, player_center, -Vector3::UP, 1000, false, FLOOR);
	//stage->sphere_collided(stage->root, collisions, player_center, HITBOX_RAD/4, COL_TYPE::EBCOLS);

	//direction = model.getTranslation() - Vector3(current_destination.x, 0, current_destination.y);
	//direction.y = 0;
	//direction.normalize();
	Vector3 currDirection = model.frontVector();

	//if (Input::isKeyPressed(SDL_SCANCODE_K))
		//Patterns::circle(StageManager::instance->currStage->player->getPosition() + Vector3(0, 0.5, 0), Vector3(0, 0, 1), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
	
	//Vector3 movement = model.frontVector() * time_elapsed;

	//for (sCollisionData& g : collisions) {
	//	currDirection += g.colNormal;
	//	currDirection.y = 0;
	//}

	for (sCollisionData& g : ground) {
		if (ground_y < g.colPoint.y) {
			ground_y = g.colPoint.y;
			ground_below_y = player_center.y - g.colPoint.y;
			ground_normal = g.colNormal;
		}
		if (player_center.y - g.colPoint.y < PLAYER_HEIGHT + 0.01 && player_center.y - g.colPoint.y > PLAYER_HEIGHT - 0.3) {
			touching_ground = true;
			v_spd = g.colNormal.y * ground_below_y * ground_below_y;
		}
	}

	grounded = touching_ground;

	//if (!grounded) {
	//	v_spd -= GRAVITY * time_elapsed;
	//}

	Vector3 movement = (currDirection.normalize() * (1 + clamp(3 * (Game::instance->time - startMoving) - 1.5 * (startMoving - Game::instance->time) * (startMoving - Game::instance->time)), 0, 3) + Vector3::UP * (grounded ? (ground_y - getPosition().y) * 40 : v_spd)) * time_elapsed;
	//Vector3 movement = 3 * model.frontVector() * time_elapsed;


	if (moving)
	{
		this->model.translateGlobal(movement);
		if (startMoving + 3 <= Game::instance->time) {
			bulletCD = Game::instance->time - 0.5;
			moving = false;
			startFiring = Game::instance->time;
			float r = random(1); 
			std::cout << std::endl << "The random number is: " << r << std::endl;
			r *= stage->secondPhase ? 13 : 7;
			if (stage->secondPhase) {
				if (r < 7)  r = random(1) * 13;
			}
			current_pattern = (pattern)clamp(floor(r), 0, (stage->secondPhase) ? 12 : 6);
			//current_pattern = SHOTGUN;
			std::cout << current_pattern << " " << r << std::endl;
			burstCount = 0;

			a_latest = a_current;
			a_current = CASTING;
			a_timer = Game::instance->time;
		}
		if (beginning_shoot) {
			if (moving && bulletCD + .5 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//	Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m = model;
				Patterns::circle2(_m, bullets_normal, 20);
			}
		}
	}
	else
	{
		if (startFiring + patterndurations[current_pattern] <= Game::instance->time && burstCount == 0)
		{
			a_latest = a_current;
			a_current = WALKING;
			a_timer = Game::instance->time;

			moving = true; 
			looking_at_player = true;
			beginning_shoot = true;

			startMoving = Game::instance->time;

			int next_dest_id;
			float r = random() * possible_destinations.size();
			next_dest_id = (pattern)clamp(floor(r), 0, possible_destinations.size() - 1);

			Vector2 next_destination = possible_destinations[next_dest_id];
			if (next_destination.x == current_destination.x && next_destination.y == next_destination.y) {
				current_destination = possible_destinations[(next_dest_id + 1) % possible_destinations.size()];
			}
			else {
				current_destination = next_destination;
			}

			std::cout << "current dest: " << current_destination.x << " " << current_destination.y << "\n";
		}
		switch (current_pattern) {
		case SWIRL:
			if (bulletCD + 0.1 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//	Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m1 = model;
				_m1.rotate(PI / 2 + Game::instance->time / 1, Vector3::UP);
				Matrix44 _m2 = model;
				_m2.rotate(PI / 2 - Game::instance->time / 1, Vector3::UP);
				Patterns::circle2(_m1, bullets_normal, 6);
				Patterns::circle2(_m2, bullets_normal, 6);
			}
			break;
		case FLOWER:
			if (bulletCD + 0.1 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//	Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m1 = model;
				_m1.rotate(PI / 2 + Game::instance->time / 1, Vector3::UP);
				Matrix44 _m2 = model;
				_m2.rotate(PI / 2 - Game::instance->time / 1, Vector3::UP);
				Patterns::circle3(_m1, bullets_normal, 6);
				Patterns::circle3(_m2, bullets_normal, 6);
			}
			break;
		case HORIZONTAL:
			if (bulletCD + 0.3 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				Matrix44 _m = model;
				Patterns::horizontal2(_m, bullets_normal, 3); Patterns::horizontal2(_m, bullets_normal, 3, BULLET_SPD * 1.2); Patterns::horizontal2(_m, bullets_normal, 3, BULLET_SPD * 1.4);
				++burstCount;
				if (burstCount == 3) {
					_m.rotate(rand(), Vector3::UP);
					Patterns::circle2(_m, bullets_normal, 36);
					burstCount = 0;
				}
			}
			break;
		case SPIRAL:
			if (bulletCD + 0.1 <= Game::instance->time) {
				looking_at_player = false;
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//	Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m1 = model;
				Matrix44 _m2 = model;
				_m1.rotate(+2 * Game::instance->time / 1, Vector3::UP);
				_m2.rotate(-2 * Game::instance->time / 1, Vector3::UP);
				Patterns::circle2(_m1, bullets_normal, 4, BULLET_SPD);
				Patterns::circle2(_m2, bullets_normal, 4, BULLET_SPD);
				Patterns::circle2(_m1, bullets_normal, 4, BULLET_SPD / 1.5);
				Patterns::circle2(_m2, bullets_normal, 4, BULLET_SPD / 1.5);
			}
			break;
		case SHOTGUN:
			if (bulletCD + 2 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				Matrix44 _m = model;
				Patterns::circle3(_m, bullets_ball, 24, BULLET_SPD / 4 - 1, -2);

				_m.rotate(-PI / 6, Vector3::UP);

				for (int i = 0; i < 5; i++) {
					_m.rotate(PI / 12, Vector3::UP);
					for (int j = 0; j < 10; j++) {
						Patterns::circle2(_m, bullets_normal, 1, BULLET_SPD / 4 + j);
					}
				}
			}
			break;
		case RINGS:
			if (bulletCD + 0.3 <= Game::instance->time) {
				looking_at_player = false;
				bulletCD = Game::instance->time;
				Matrix44 _m1 = model;
				_m1.rotate(Game::instance->time, Vector3::UP);
				Matrix44 _m2 = model;
				_m2.rotate(-Game::instance->time, Vector3::UP);
				for (int i = 0; i < 6; i++) {
					_m1.rotate(PI / 3, Vector3::UP);
					Patterns::ring(_m1, bullets_smallball, 18, BULLET_SPD, 1.4);
				}
				if (burstCount == 3) {
					Patterns::circle2(_m2, bullets_normal, 48, BULLET_SPD + 2);
					burstCount = 0;
				}
				else ++burstCount;
			}
			break;
		case TRAP:
			if (bulletCD + 1.5 <= Game::instance->time) {
				if (burstCount == 0) {
					patternTarget = stage->player->model;
					patternTarget.translate(Vector3(0, 1, 0) * (model.getTranslation().y - stage->player->model.getTranslation().y));
				}
				if (burstCD + .05 <= Game::instance->time) {
					int count = 32;
					burstCD = Game::instance->time;
					Matrix44 _m = patternTarget;
					_m.rotate(burstCount * 2 * PI / count, Vector3::UP);
					_m.translate(4 * Vector3(1, 0, 0));
					Patterns::circle3(_m, bullets_normal, 8, 0, -5);
					if (burstCount == count) {
						burstCount = 0;
						bulletCD = Game::instance->time;
					}
					else ++burstCount;
				}
			}
			break;
		case REV:
			if (bulletCD + 0.3 <= Game::instance->time) {
				looking_at_player = false;
				bulletCD = Game::instance->time;
				Matrix44 _m = model;
				_m.rotate(Game::instance->time / 3, Vector3::UP);
				for (int i = 0; i < 22; i++) {
					Matrix44 _m1 = _m;
					_m1.rotate(2 * i * PI / 22, Vector3::UP);
					_m1.translate(1 * Vector3(1, 0, 0));
					Patterns::circle3(_m1, bullets_ball, 1, 5, -10);
				}
			}
			break;
		case WAVY:
			if (bulletCD + 0.1 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//	Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m1 = model;
				_m1.rotate(PI / 2 + Game::instance->time / 1, Vector3::UP);
				Patterns::circle4(_m1, bullets_normal, 7, 1);
				Matrix44 _m2 = model;
				_m1.rotate(PI / 2 - Game::instance->time / 1, Vector3::UP);
				Patterns::circle4(_m2, bullets_normal, 7, 1);
			}
			break;

		case WAVY2:
			if (bulletCD + 0.1 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//	Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m1 = model;
				_m1.rotate(PI / 2 + Game::instance->time / 1, Vector3::UP);
				Patterns::circle4(_m1, bullets_smallball, 7, 1);
				Matrix44 _m2 = model;
				_m2.rotate(PI / 2 - Game::instance->time / 1, Vector3::UP);
				Patterns::circle4(_m2, bullets_smallball, 7, -1, -PI / 300);
			}
			break;
		case SPIRALBURST:
			if (burstCD + .05 <= Game::instance->time) {
				looking_at_player = false;
				burstCD = Game::instance->time;
				Matrix44 _m = model;
				float ela = Game::instance->time - startFiring;
				_m.rotate(ela * ela, Vector3::UP);
				Patterns::circle2(_m, bullets_smallball, 9, 10);
			}
			break;
		case SUN:
			if (bulletCD + 1.5 <= Game::instance->time) {
				bulletCD = Game::instance->time;
				//for (int i = 0; i <= 3; ++i)
				//    Patterns::circle(stage->player->getPosition() + Vector3(0, 0.6, 0), Vector3((1.0f / 3.0f * i)*2-1, 0, 1).normalize(), model, bullets, amount[0], bullet_shaders[0], bullet_textures[0], bullet_meshes[0], false);
				Matrix44 rotate_matrix = Matrix44();
				rotate_matrix.setRotation(((int)Game::instance->time % 314) / 100, Vector3::UP);
				Matrix44 _m1 = model;
				_m1.rotate(PI / 2 + Game::instance->time / 3, Vector3::UP);
				Patterns::circle5(_m1, bullets_giantball, Vector2(0, 10), Vector2(0, 10), 6, 0, 0.2, 0.6, -0.1, 2);
			}
			if (burstCD + 0.1 <= Game::instance->time) {
				looking_at_player = false;
				burstCD = Game::instance->time;
				Matrix44 _m = model;
				_m.rotate(Game::instance->time, Vector3::UP);
				Patterns::circle3(_m, bullets_ball, 5, 4, 8);
			}
			break;
		case RINGSHOT:

			if (bulletCD + 1 <= Game::instance->time) {
				looking_at_player = false;
				bulletCD = Game::instance->time;
				Matrix44 _m1 = model;
				_m1.rotate(_m1.getYawRotationToAimTo(stage->player->model.getTranslation()), Vector3::UP);
				for (int i = 0; i < 6; i++) {
					_m1.rotate(random(PI / 8) - PI / 16, Vector3::UP);
					Patterns::ring(_m1, bullets_smallball, 14, (BULLET_SPD / 2) + i, 0.7);
				}
			}
			if (burstCD + 0.1 <= Game::instance->time) {
				Matrix44 _m2 = model;
				_m2.rotate(-Game::instance->time, Vector3::UP);
				burstCD = Game::instance->time;
				Matrix44 _m = model;
				_m.rotate(Game::instance->time, Vector3::UP);
				Patterns::circle2(_m, bullets_ball, 3, 15);
			}
			break;
		}
	}

	for (int i = 0; i < bullets.size(); i++) {
		Bullet* b = bullets[i];
		if (Game::instance->time - b->timer_spawn > 5 || b->to_delete) {
			bullets.erase((bullets.begin() + i));
			delete b;
		}
		else b->update(time_elapsed);
	}

	bullets_normal.update(time_elapsed);
	bullets_ball.update(time_elapsed);
	bullets_smallball.update(time_elapsed);
	bullets_giantball.update(time_elapsed);

	this->sphere_bullet_collision(player_center, HITBOX_RAD);
}