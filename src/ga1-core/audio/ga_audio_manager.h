#pragma once
#include <irrklang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include "framework/ga_sim.h"
#include "entity/ga_component.h"
#include "physics/ga_shape.h"
#include "audio/ga_audio_component.h"
#include "ga_audio_receiver.h"

#include <vector>

class ga_audio_component;
class ga_audio_receiver;

class ga_audio_manager
{
public:
	ga_audio_manager(ga_sim* sim);
	virtual ~ga_audio_manager();

	void set_engine(int selected);
	void push_back(ga_audio_component* component);
	bool make_source();
	void remove(int selected);

	irrklang::ISoundEngine* get_engine();
	std::vector<ga_audio_component*> get_components();
	std::vector<const char*> get_sound_device_list();
	std::vector<const char*> get_record_device_list();

	static void write_wave_file(const char* filename, irrklang::SAudioStreamFormat format, void* data);
	void start_record(int device_id);
	void stop_record();
	void start_record_engine();
	void stop_record_engine();

private:
	ga_sim* _sim;
	irrklang::ISoundEngine* _engine;
	irrklang::IAudioRecorder* _recorder;
	ga_audio_receiver* _receiver;
	std::vector<ga_audio_component*> _components;

	// plane
	struct ga_shape* _plane;
};