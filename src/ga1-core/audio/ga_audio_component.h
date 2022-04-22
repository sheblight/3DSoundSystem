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

	const float& is_listener() const { return _is_listener; }
	const ga_vec3f& get_position() const { return get_entity()->get_position(); }
	const float &get_volume() const { return _volume; }
	const float& get_min_radius() const { return _min_radius; }
	const float& get_max_radius() const { return _max_radius; }

	bool play();
	bool pause();
	bool stop();
	bool load_file(const char* filepath);
	
	virtual void update_sound_position();
	void set_min_dist();
	void set_max_dist();
	virtual void set_volume();

	void enable_reverb1(bool on);
	void set_reverb1(float gain, float mix, float reverb_time, float hf_reverb_time_ratio);
	void enable_reverb2(bool on);
	void set_reverb2();


	// GUI properties
	char* _name;
	ImVec4 _color;
	char* _status;
	char* _filename;

protected:
	ga_audio_manager* _manager;
	irrklang::ISoundEngine* _engine;
	struct ga_shape* _shape;
	bool _is_listener;

	float _volume;
private:
	irrklang::ISound* _source;

	// GUI properties
	char* _filepath;

	float _min_radius;
	float _max_radius;
};