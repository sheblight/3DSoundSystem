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
	const bool& is_looping() const { return _is_looping; }
	const ga_vec3f& get_velocity() const { return _velocity; }

	const bool is_fx_available() const { return _source->getSoundEffectControl() != NULL; }
	const bool& is_reverb1_on() const { return _wave_reverb_enabled; }
	const bool& is_reverb2_on() const { return _i3dl2_reverb_enabled; }
	const float* get_reverb1_wave_params() const { return _reverb1_wave_params.data(); }
	const float* get_reverb2_wave_params() const { return _reverb2_wave_params.data(); }

	bool play();
	bool pause();
	bool stop();
	bool load_file(const char* filepath);
	
	virtual void update_sound_position();
	void set_min_dist();
	void set_max_dist();
	virtual void set_volume();
	void set_loop();
	void reset_velocity() { _velocity = ga_vec3f::zero_vector(); };

	void enable_wave_reverb();
	void set_wave_reverb();
	void enable_i3dl2_reverb();
	void set_i3dl2_reverb();


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
	ga_vec3f _velocity;
private:
	irrklang::ISound* _source;

	// GUI properties
	char* _filepath;

	float _min_radius;
	float _max_radius;
	bool _is_looping;

	// Reverb properties
	bool _wave_reverb_enabled;
	std::vector<float> _reverb1_wave_params;
	bool _i3dl2_reverb_enabled;
	std::vector<float> _reverb2_wave_params;
};