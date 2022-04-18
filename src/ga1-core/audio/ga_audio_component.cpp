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
	sphere->_radius = 1.5f;
	_shape = sphere;

	_name = "aduio sarce";
	_color = ImVec4(0,1,0,1);
	//_world_position = ga_vec3f::zero_vector();
	_min_radius = 5;
	_max_radius = 10;

	manager->push_back(this);
}

ga_audio_component::~ga_audio_component()
{
	//delete _shape;
	if (_source != NULL) {
		_source->drop();
	}
}

void ga_audio_component::update(ga_frame_params* params)
{
	ga_mat4f initial_transform;
	initial_transform.make_identity();
	initial_transform.translate(get_position());

	ga_dynamic_drawcall draw;
	draw._color = ga_vec3f({ _color.x, _color.y, _color.z });
	_shape->get_debug_draw(initial_transform, &draw);

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);
}


bool ga_audio_component::play(const char* filepath) 
{
	// Check if source can be played
	if (!_manager->get_engine())
	{ 
		std::cout << "Error: Missing engine reference! " << filepath << std::endl;
		return false; 
	}

	// Load file to sound source
	if (_source == NULL || _source->isFinished()) 
	{
		printf("Playing at (%f,%f,%f)\n", get_position().x, get_position().y, get_position().z);
		_source = _manager->get_engine()->play3D(filepath, vec3df(get_position().x, get_position().y, get_position().z), true, false, true);
		if (!_source) {
			std::cout << "Error: Couldn't initialize sound source at " << filepath << std::endl;
			return false;
		}
		_source->setMinDistance(_min_radius);
		_source->setMaxDistance(_max_radius);
	}
	// Unpause if sound exists
	else 
	{
		_source->setIsPaused(false);
	}
	
	return true;
}

bool ga_audio_component::pause()
{
	if (_source == NULL)
	{
		return false;
	}
	_source->setIsPaused();
	return true;
}

bool ga_audio_component::stop()
{
	if (_source == NULL) 
	{
		return false;
	}
	_source->stop();
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

bool ga_audio_component::set_fx(const char* effect, float param)
{
	return false;
}