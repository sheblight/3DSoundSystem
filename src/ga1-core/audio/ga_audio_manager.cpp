#include "ga_audio_manager.h"

#include "ga_audio_component.h"
#include "entity/ga_entity.h"

ga_audio_manager::ga_audio_manager() {
	_engine = irrklang::createIrrKlangDevice();
	if (!_engine)
	{
		printf("Could not startup engine\n");
	}
	//_ents = std::vector<ga_audio_manager*>();
}

ga_audio_manager::~ga_audio_manager() {
	_engine->drop();
}

void ga_audio_manager::push_back(ga_audio_component* component)
{
	_components.push_back(component);
}

irrklang::ISoundEngine* ga_audio_manager::get_engine() {
	return _engine;
}

std::vector<ga_audio_component*> ga_audio_manager::get_components() {
	return _components;
}