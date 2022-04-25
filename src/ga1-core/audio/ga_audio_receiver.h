#pragma once

#include "entity/ga_component.h"
#include "ga_audio_manager.h"
#include <irrKlang.h>
#include <vector>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class ga_audio_receiver : public irrklang::ISoundMixedOutputReceiver {
public:
	ga_audio_receiver();
	~ga_audio_receiver();

	void push_data(const void* data, int byte_count);
	void OnAudioDataReady(const void* data, int byte_count, int playback_rate) override;
	void stop_record();
	
private:
	irrklang::SAudioStreamFormat _format;
	void* _data;
	int _test = 0;
	std::vector<void*> _datas;
};