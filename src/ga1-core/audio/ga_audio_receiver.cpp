#include "ga_audio_receiver.h"
#include "ga_audio_manager.h"

ga_audio_receiver::ga_audio_receiver() {
	_data = calloc(0, sizeof(char));
	_format.FrameCount = 0;
	_format.ChannelCount = 1;
	_format.SampleFormat = irrklang::ESF_U8;
	_format.SampleRate = 44100;
}

ga_audio_receiver::~ga_audio_receiver() {
	delete _data;
}

void ga_audio_receiver::OnAudioDataReady(const void* data, int byte_count, int playback_rate) {
	//printf("Data received: bytecount %d, playback rate %d\n", byte_count, playback_rate);
	//irrklang::SAu
	_format.SampleRate = playback_rate;
	_format.FrameCount += byte_count;

	// store data
	printf("new size %d\n", _format.FrameCount + byte_count);
	unsigned char* newStorage = new unsigned char[_format.FrameCount + byte_count];
	memcpy(newStorage, (unsigned char*)_data, _format.FrameCount);
	memcpy(&(newStorage[_format.FrameCount]), (unsigned char*)data, byte_count);
	delete[] _data; 
	_data = (void*)newStorage;
}

void ga_audio_receiver::stop_record() {
	printf("Size of recorded data: %d\n", _format.FrameCount);
	ga_audio_manager::write_wave_file("recorded_engine.wav", _format, _data);
}