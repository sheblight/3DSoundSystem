#pragma once
#include <irrklang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include "entity/ga_component.h"
#include "audio/ga_audio_component.h"

#include <vector>

class ga_audio_component;

class ga_audio_manager
{
public:
	ga_audio_manager();
	virtual ~ga_audio_manager();

	void push_back(ga_audio_component* component);

	irrklang::ISoundEngine* get_engine();
	std::vector<ga_audio_component*> get_components();

private:
	irrklang::ISoundEngine* _engine;
	std::vector<ga_audio_component*> _components;
};