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

	// Set up transform and shape
	_transform = ent->get_transform();

	ga_sphere* sphere = new ga_sphere;
	sphere->_radius = 1.5f;
	_shape = sphere;

	_name = "aduio sarce";
	_color = ImVec4(0,1,0,1);
	_world_position = ga_vec3f::zero_vector();
	_radius = 1;

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
	ga_dynamic_drawcall draw;
	draw._color = ga_vec3f({ _color.x, _color.y, _color.z });
	_shape->get_debug_draw(_transform, &draw);

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);
}


bool ga_audio_component::play(const char* filepath) 
{
	// Set sound source
	if (!_manager->get_engine()) {
		std::cout << "Error: Engine is not initialized!" << std::endl;
	}
	std::cout << "Loading " << filepath << std::endl;
	_source = _manager->get_engine()->play3D(filepath, vec3df(0, 0, 0), true, false, true);
	if (!_source) {
		std::cout << "Error: Couldn't initialize sound source at " << filepath << std::endl;
		return false;
	}
	return true;
}

bool ga_audio_component::pause()
{
	return false;
}

bool ga_audio_component::stop()
{
	return false;
}

bool ga_audio_component::set_fx(const char* effect, float param)
{
	return false;
}