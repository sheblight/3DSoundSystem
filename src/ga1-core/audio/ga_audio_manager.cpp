#include "ga_audio_manager.h"

#include "ga_audio_component.h"
#include "framework/ga_sim.h"
#include "entity/ga_entity.h"

ga_audio_manager::ga_audio_manager(ga_sim* sim) : _sim(sim) {
	// Initialize engine
	_engine = irrklang::createIrrKlangDevice();
	if (!_engine)
	{
		printf("Could not startup engine\n");
	}
	irrklang::vec3df position(0, 0, 0);        // position of the listener
	irrklang::vec3df lookDirection(0, 0, 1); // the direction the listener looks into
	irrklang::vec3df velPerSecond(0, 0, 0);    // only relevant for doppler effects
	irrklang::vec3df upVector(0, 1, 0);        // where 'up' is in your 3D scene
	_engine->setListenerPosition(position, lookDirection, velPerSecond, upVector);

	// Recording setup
	irrklang::ISoundDeviceList* deviceList = irrklang::createAudioRecorderDeviceList();
	printf("Devices available:\n\n");
	for (int i = 0; i < deviceList->getDeviceCount(); ++i)
		printf("%d: %s\n", i, deviceList->getDeviceDescription(i));

}

ga_audio_manager::~ga_audio_manager() {
	_engine->drop();
}



void ga_audio_manager::push_back(ga_audio_component* component)
{
	_components.push_back(component);
}


bool ga_audio_manager::make_source()
{
	ga_entity* lua = new ga_entity;
	lua->translate({ 1.0f, -2.0f, 0.0f });
	//ga_lua_component lua_move(&lua, "data/scripts/move.lua");
	//ga_cube_component lua_model(&lua, "data/textures/rpi.png");
	ga_audio_component* lua_audio = new ga_audio_component(lua, this);
	_sim->add_entity(lua);
	return true;
}

//void ga_audio_manager::make_source(ga_audio_component*)

irrklang::ISoundEngine* ga_audio_manager::get_engine() {
	return _engine;
}

std::vector<ga_audio_component*> ga_audio_manager::get_components() {
	return _components;
}