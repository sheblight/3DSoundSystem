#include "ga_audio_receiver.h"
#include "ga_audio_manager.h"
#include <thread>

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
	// store data
	std::thread (&ga_audio_receiver::push_data, this, data, byte_count).detach();
	_format.SampleRate = playback_rate;
	_format.FrameCount += byte_count;
}

void ga_audio_receiver::stop_record() {
	printf("Size of recorded data: %d\n", _format.FrameCount);
	printf("Counter load: %d\n", _test);
	unsigned char* new_storage;
	int size_remaining = _format.FrameCount;
	int current_size = 0;
	int index = 0;
	while (size_remaining > 0 && index < _datas.size()) {
		int alloc = size_remaining >= 1024 ? 1024 : size_remaining;
		new_storage = new unsigned char[current_size + alloc];
		memcpy(new_storage, (unsigned char*)_data, current_size);
		memcpy(&(new_storage[current_size]), (unsigned char*)_datas[index], alloc);
		delete[] _data;
		_data = (void*)new_storage;
		index++;
		size_remaining -= alloc;
		current_size += alloc;
	}
	ga_audio_manager::write_wave_file("recorded_engine.wav", _format, _data);
}

void ga_audio_receiver::push_data(const void* data, int byte_count) {
	_test++;
	unsigned char* storage = new unsigned char[byte_count];
	memcpy(storage, (unsigned char*)data, byte_count);
	_datas.push_back(storage);
}