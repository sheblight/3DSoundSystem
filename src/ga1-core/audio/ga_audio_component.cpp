#include "ga_audio_component.h"
#include "entity/ga_entity.h"

#include <irrklang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll
#include <iostream>

using namespace irrklang;

ga_audio_component::ga_audio_component(ga_entity* ent, ISoundEngine* engine, const char* filepath) : ga_component(ent) {
	if (!engine) {
		std::cout << "Error: Engine is not initialized!" << std::endl;
	}
	_engine = engine;

	std::cout << filepath << std::endl;
	//_source = _engine->play2D(filepath, true);
	_source = _engine->play3D(filepath, vec3df(0, 0, 0), true, false, true);

	if (_source) {
		std::cout << "Error: Couldn't initialize sound source at " << filepath << std::endl;
	}
}

ga_audio_component::~ga_audio_component()
{
	if (_source) {
		_source->drop();
	}
}

void ga_audio_component::update(ga_frame_params* params)
{
}