#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/
#include "entity/ga_entity.h"
#include "entity/ga_component.h"
#include "audio/ga_audio_manager.h"
#include "imgui.h"

#include <cstdint>
#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class ga_audio_manager;

/*
** Renderable basic textured cubed.
*/
class ga_audio_component : public ga_component
{
public:
	ga_audio_component(class ga_entity* ent, ga_audio_manager* manager);
	virtual ~ga_audio_component();

	virtual void update(struct ga_frame_params* params) override;

	const ga_vec3f& get_position() const { return get_entity()->get_position(); }
	const float& get_min_radius() const { return _min_radius; }
	const float& get_max_radius() const { return _max_radius; }

	bool play();
	bool pause();
	bool stop();
	bool load_file(const char* filepath);
	
	void update_sound_position();
	void set_min_dist();
	void set_max_dist();
	bool set_fx(const char* effect, float param);


	// GUI properties
	char* _name;
	ImVec4 _color;
	char* _status;
	char* _filename;

private:
	ga_audio_manager* _manager;
	irrklang::ISoundEngine* _engine;
	irrklang::ISound* _source;

	struct ga_shape* _shape;

	// GUI properties
	char* _filepath;

	float _volume;
	float _min_radius;
	float _max_radius;
};