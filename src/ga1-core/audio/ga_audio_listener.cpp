#include "entity/ga_entity.h"
#include "entity/ga_component.h"
#include "audio/ga_audio_listener.h"
#include "ga_audio_component.h"
#include "ga_audio_manager.h"


ga_audio_listener::ga_audio_listener(ga_entity* ent, ga_audio_manager* manager) : ga_audio_component(ent, manager)
{
	// parameterize things
	irrklang::vec3df position(get_position().x, get_position().y, get_position().z);        // position of the listener
	irrklang::vec3df lookDirection(0, 0, 1); // the direction the listener looks into
	irrklang::vec3df velPerSecond(0, 0, 0);    // only relevant for doppler effects
	irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene
	manager->get_engine()->setListenerPosition(position, lookDirection, velPerSecond, upVector);

	// Set up shape
	ga_crosshair* cross = new ga_crosshair;
	_shape = cross;
	
	_is_listener = true;
	//_name = "aduio lisssssener";
	_name = "Audio Listener";
	_color = ImVec4(1.f, 0.2f, 0.5f, 1);
	_rolloff_factor = 1;
}

ga_audio_listener::~ga_audio_listener() 
{
	
}

void ga_audio_listener::update(struct ga_frame_params* params) {
	// Translate by velocity first
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	ga_vec3f displacement = _velocity.scale_result(dt);
	get_entity()->translate(displacement);

	// debug draw
	ga_mat4f initial_transform;
	initial_transform.make_identity();
	initial_transform.translate(get_position());

	ga_dynamic_drawcall draw;

	//ga_sphere* sphere = new ga_sphere;
	//sphere->get_debug_draw(initial_transform, &draw);
	ga_crosshair* cross = (ga_crosshair*)_shape;
	_shape->get_debug_draw(initial_transform, &draw);

	draw._color = ga_vec3f({ _color.x, _color.y, _color.z });

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);


}

void ga_audio_listener::update_sound_position() {
	irrklang::vec3df position(get_position().x, get_position().y, get_position().z);        // position of the listener
	irrklang::vec3df lookDirection(0, 0, 1); // the direction the listener looks into
	irrklang::vec3df velPerSecond(0, 0, 0);    // only relevant for doppler effects
	irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene
	_manager->get_engine()->setListenerPosition(position, lookDirection, velPerSecond, upVector);
}

void ga_audio_listener::set_volume() {
	_manager->get_engine()->setSoundVolume(_volume);
}

void ga_audio_listener::set_rolloff() {
	_manager->get_engine()->setRolloffFactor(_rolloff_factor);
}