#include "ParticleEmitter.h"
#include "framework/camera.h"
#include "game/game.h"
#include "game/StageManager.h"

#include <algorithm>
#include <framework/input.h>




void ParticleEmitter::render(Camera* camera) {
	// Sort
	if (sort_distance) {
		std::sort(particles.begin(), particles.end(), [&](const sParticle& a, const sParticle& b) {
			return a.position.distance(camera->eye) > b.position.distance(camera->eye);
		});
	}
	
	//std::cout << "Sorting done!" << std::endl;

	std::vector<Vector2> uv_offsets;
	for (int j = 1; j <= texture_grid_size; ++j) {
		for (int i = 0; i < texture_grid_size; ++i) {
			uv_offsets.push_back(Vector2(i / (float)texture_grid_size, 1.f - j / (float)texture_grid_size));
		}
	}

	//std::cout << "UV Offsets done!" << std::endl;

	Mesh quad;
	Vector3 right = camera->getLocalVector(Vector3(1,0,0)); //camera local vector right
	Vector3 top = camera->getLocalVector(Vector3::UP); // camera top

	std::cout << "# OF ACTIVE PARTICLES: " << active_particles << std::endl;

	for (sParticle& p : particles) {
		if (!p.active) continue;

		Vector3 pos = p.position;
		pos = model * pos;

		float nt = clamp(p.ttl / max_ttl, 0, 1);
		
		Vector4 color = sample<Vector4>(nt, colors.size(), &colors[0]);
		float size = sample<float>(nt, sizes.size(), &sizes[0]);

		int grid_frames = texture_grid_size * texture_grid_size;
		int cloud_index = animated_texture ? nt * grid_frames : p.id % grid_frames;
		float d_uvs = 1.f / texture_grid_size;
		Vector2 uv_offset = uv_offsets[cloud_index];

		

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (- top - right) * size);
		quad.uvs.push_back(Vector2(0, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (- top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (-top + right) * size);
		quad.uvs.push_back(Vector2(1, 0) * d_uvs + uv_offset);
		quad.colors.push_back(color);
		quad.vertices.push_back(pos + (top + right) * size);
		quad.uvs.push_back(Vector2(1, 1) * d_uvs + uv_offset);
		quad.colors.push_back(color);

		//// Three vertices of the 1st triangle
		//quad.vertices.push_back(pos + (top - right) * size);
		//quad.uvs.push_back(Vector2(0, 1));
		//quad.vertices.push_back( pos + (-top - right) * size);
		//quad.uvs.push_back(Vector2(0, 0));
		//quad.vertices.push_back( pos + (- top + right) * size);
		//quad.uvs.push_back(Vector2(1, 0));

		//// Three vertices of the 2nd triangle
		//quad.vertices.push_back(pos + (top - right) * size);
		//quad.uvs.push_back(Vector2(0, 1));
		//quad.vertices.push_back( pos + (top + right) * size );
		//quad.uvs.push_back(Vector2(1, 0));
		//quad.vertices.push_back(Vector3(1, 1, 0));
		//quad.uvs.push_back(Vector2(1, 1));

	}

	//std::cout << "Particle quads done!" << std::endl;

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	

	if (!material.shader) {
		std::cout << "No shader!";
		material.shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	}



	material.shader->enable();

	Stage* stage = StageManager::instance->currStage;

	Matrix44 ide = Matrix44();
	ide.rotate(ide.getYawRotationToAimTo(camera->eye), Vector3::UP);

	material.shader->setUniform("u_model", Matrix44());
	material.shader->setUniform("u_color", Vector4(1.0f));
	material.shader->setUniform("u_viewprojection", camera->viewprojection_matrix);

	


	if (material.diffuse) {
		material.shader->setTexture("u_texture", material.diffuse, 0);
	}
	else {
		std::cout << "No diffuse";
	}

	quad.render(GL_TRIANGLES);

	material.shader->disable();

};


void ParticleEmitter::update(float delta_time) {
	emit_timer += delta_time;

	if (emit_timer > emit_rate) {
		emit();
	}
	for (sParticle& p : particles) {
		if (!p.active) continue;

		p.position += (p.velocity) * delta_time;
		p.ttl += delta_time;

		if (p.ttl > max_ttl) {
			p.active = false;
			active_particles--;
		}
	}
}

void ParticleEmitter::clearParticles() {
	particles.clear();
	particles.resize(max_particles);
}

void ParticleEmitter::emit() {
	emit_timer = 0.f;

	for (sParticle& p : particles) {
		if (p.active) continue;

		p.id = last_id++;
		p.ttl = 0.f;
		p.active = true;
		p.position = emit_position;
		p.velocity = emit_velocity;
		active_particles++;
		break;
	}
};
