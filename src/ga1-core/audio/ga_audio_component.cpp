#include "ga_audio_component.h"
#include "entity/ga_entity.h"

#include "physics/ga_shape.h"

#include <irrklang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
#include <iostream>

using namespace irrklang;

ga_audio_component::ga_audio_component(ga_entity* ent, ISoundEngine* engine, const char* filepath) : ga_component(ent) {
	// Set engine
	if (!engine) {
		std::cout << "Error: Engine is not initialized!" << std::endl;
	}
	_engine = engine;

	// Set sound source
	std::cout << filepath << std::endl;
	_source = _engine->play3D(filepath, vec3df(0, 0, 0), true, false, true);
	if (!_source) {
		std::cout << "Error: Couldn't initialize sound source at " << filepath << std::endl;
	}

	// Set up transform and shape
	_transform = ent->get_transform();
	ga_oobb* box = new ga_oobb;
	_shape = box;

}

ga_audio_component::~ga_audio_component()
{
	//delete _shape;
	if (_source) {
		_source->drop();
	}
}

void ga_audio_component::update(ga_frame_params* params)
{
	ga_dynamic_drawcall draw;
	_shape->get_debug_draw(_transform, &draw);

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);
}