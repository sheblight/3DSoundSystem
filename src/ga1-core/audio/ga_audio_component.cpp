#include "ga_audio_component.h"
#include "entity/ga_entity.h"
#include "ga_audio_manager.h"

#include "physics/ga_shape.h"

#include <irrklang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
#include <iostream>

using namespace irrklang;

ga_audio_component::ga_audio_component(ga_entity* ent, ga_audio_manager* manager) : ga_component(ent) {
	// Set engine
	_manager = manager;
	_source = NULL;

	// Set up shape
	ga_sphere* sphere = new ga_sphere;
	sphere->_radius = 1.0f;
	_shape = sphere;

	// Set up audio
	_filepath = new char[1024];
	_filename = new char[1024];
	strcpy(_filename, "Select a file");
	_status = "File to be loaded";

	_is_listener = false;
	//_name = "aduio sarce";
	_name = "Audio Source";
	_color = ImVec4(0,1,0,1);
	_volume = 1;
	_min_radius = 5;
	_max_radius = 10;
	_velocity = { 0,0,0 };

	_wave_reverb_enabled = false;
	_i3dl2_reverb_enabled = false;
	_reverb1_wave_params = { 0.f, 0.f, 1000.f, 0.001f };
	_reverb2_wave_params = {-1000, -100, 0, 1.49f, 0.83f, -2602, 0.007f, 200, 0.011f, 100.0f, 100.0f, 5000.0f };

	manager->push_back(this);
}

ga_audio_component::~ga_audio_component()
{
	if (_source != NULL) {
		_source->stop();
		_source->drop();
	}
	delete _shape;
	delete _filepath;
	delete _filename;
}

void ga_audio_component::update(ga_frame_params* params)
{
	// Translate by velocity first
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	ga_vec3f displacement = _velocity.scale_result(dt);
	get_entity()->translate(displacement);

	// Doppler effect
	if (_source != NULL) {
		_source->setVelocity({ _velocity.x, _velocity.y, _velocity.z });
	}

	// Draw current transformation
	ga_mat4f initial_transform;
	initial_transform.make_identity();
	initial_transform.translate(get_position());

	ga_dynamic_drawcall draw;
	draw._color = ga_vec3f({ _color.x, _color.y, _color.z });
	ga_sphere* sphere = (ga_sphere*)_shape;
	sphere->_radius = _min_radius;
	_shape->get_debug_draw(initial_transform, &draw);

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);
}


bool ga_audio_component::play() 
{
	// Check if source can be played
	if (!_manager->get_engine())
	{ 
		std::cout << "Error: Missing engine reference! " << _filepath << std::endl;
		return false; 
	}

	// Load file to sound source
	if (_source == NULL || _source->isFinished()) 
	{
		printf("Playing sounds lalala\n");
		_source = _manager->get_engine()->play3D(_filepath, vec3df(get_position().x, get_position().y, get_position().z), _is_looping, false, false, irrklang::ESM_AUTO_DETECT, true);
		if (!_source) 
		{
			std::cout << "Error: Couldn't initialize sound source at " << _filepath << std::endl;
			return false;
		}
		printf("%s\n", is_fx_available() ? "Effects available!" : "Effects unavailable...");
		_source->setVolume(_volume);
		_source->setMinDistance(_min_radius);
		_source->setMaxDistance(_max_radius);
		if (is_fx_available()) {
			enable_wave_reverb();
		}
	}
	// Unpause if sound exists
	else 
	{
		_source->setIsPaused(false);
	}
	
	return true;
}

// Pauses currently playing sound
bool ga_audio_component::pause()
{
	if (_source == NULL)
	{
		return false;
	}
	_source->setIsPaused();
	return true;
}

// Stops currently playing sound
bool ga_audio_component::stop()
{
	if (_source == NULL)
	{
		return false;
	}
	_source->stop();
	_source->drop();
	_source = NULL;
	return true;
}

bool ga_audio_component::load_file(const char* filepath) {
	strcpy(_filepath, filepath);
	return true;
}


void ga_audio_component::update_sound_position() 
{
	if (_source == NULL) return;
	_source->setPosition(vec3df(get_position().x, get_position().y, get_position().z));
}

void ga_audio_component::set_min_dist() 
{
	if (_source == NULL) return;
	_source->setMinDistance(_min_radius);
}

void ga_audio_component::set_max_dist()
{
	if (_source == NULL) return;
	_source->setMaxDistance(_max_radius);
}

void ga_audio_component::set_volume()
{
	if (_source == NULL) return;
	_source->setVolume(_volume);
}

void ga_audio_component::set_loop() 
{
	if (_source == NULL) return;
	_source->setIsLooped(_is_looping);
}

void ga_audio_component::enable_wave_reverb() {
	if (_source == NULL) return;
	if (_wave_reverb_enabled) {
		set_wave_reverb();
	}
	else {
		_source->getSoundEffectControl()->disableWavesReverbSoundEffect();
	}
}
void ga_audio_component::set_wave_reverb()
{
	if (_source == NULL) return;
	_source->getSoundEffectControl()->enableWavesReverbSoundEffect(_reverb1_wave_params[0], _reverb1_wave_params[1], _reverb1_wave_params[2], _reverb1_wave_params[3]);
}

void ga_audio_component::enable_i3dl2_reverb() {
	if (_source == NULL) return;
	if (_i3dl2_reverb_enabled) {
		set_i3dl2_reverb();
	}
	else {
		_source->getSoundEffectControl()->disableI3DL2ReverbSoundEffect();
	}
}
void ga_audio_component::set_i3dl2_reverb()
{
	float* params = _reverb2_wave_params.data();
	if (_source == NULL) return;
	_source->getSoundEffectControl()->enableI3DL2ReverbSoundEffect(*params, *(params+1), *(params + 2), *(params + 3), *(params + 4), *(params + 5), *(params + 6), *(params + 7), *(params + 8), *(params + 9), *(params + 10), *(params + 11));
}