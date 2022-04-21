#pragma once
#include <irrklang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include "framework/ga_sim.h"
#include "entity/ga_component.h"
#include "physics/ga_shape.h"
#include "audio/ga_audio_component.h"

#include <vector>

class ga_audio_component;

class ga_audio_manager
{
public:
	ga_audio_manager(ga_sim* sim);
	virtual ~ga_audio_manager();

	void push_back(ga_audio_component* component);
	bool make_source();
	void remove(int selected);

	irrklang::ISoundEngine* get_engine();
	std::vector<ga_audio_component*> get_components();

private:
	ga_sim* _sim;
	irrklang::ISoundEngine* _engine;
	std::vector<ga_audio_component*> _components;

	// plane
	struct ga_shape* _plane;
};