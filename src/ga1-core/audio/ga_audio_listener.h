#pragma once

#include "entity/ga_component.h"
#include "ga_audio_manager.h"
#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class ga_audio_listener : public ga_audio_component {
public:
	ga_audio_listener(ga_entity* ent, ga_audio_manager* manager);
	~ga_audio_listener();

	void update(struct ga_frame_params* params) override;

	const float& get_rolloff_factor() const { return _rolloff_factor; }

	void update_sound_position() override;
	void set_volume() override;
	void set_rolloff();
private:
	float _rolloff_factor;
};