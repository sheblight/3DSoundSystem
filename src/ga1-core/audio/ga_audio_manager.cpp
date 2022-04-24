#include "ga_audio_manager.h"

#include "ga_audio_component.h"
#include "ga_audio_receiver.h"
#include "framework/ga_sim.h"
#include "entity/ga_entity.h"

ga_audio_manager::ga_audio_manager(ga_sim* sim) : _sim(sim) {
	// Initialize engine
	_engine = NULL;
	set_engine(0);

	_receiver = new ga_audio_receiver;
}

ga_audio_manager::~ga_audio_manager() {
	_engine->drop();
	delete _receiver;
}

// Must have zero sound sources running in order to switch devices
// winMM driver allows output recording (if functional) but messes with 3d sound output
void ga_audio_manager::set_engine(int selected) {
	if (_engine != NULL) {
		_engine->drop();
	}

	irrklang::ISoundDeviceList* deviceList = irrklang::createSoundDeviceList();
	printf("Playing from %s\n", deviceList->getDeviceDescription(selected));
	_engine = irrklang::createIrrKlangDevice(irrklang::ESOD_AUTO_DETECT, irrklang::ESEO_DEFAULT_OPTIONS, deviceList->getDeviceID(selected));
	deviceList->drop();
	if (!_engine)
	{
		printf("Could not startup engine\n");
	}
}

void ga_audio_manager::push_back(ga_audio_component* component)
{
	_components.push_back(component);
}


bool ga_audio_manager::make_source()
{
	ga_entity* lua = new ga_entity;
	lua->translate({ 1.0f, -2.0f, 0.0f });
	ga_audio_component* lua_audio = new ga_audio_component(lua, this);
	_sim->add_entity(lua);
	return true;
}

void ga_audio_manager::remove(int selected) {
	ga_audio_component* audio = _components[selected];
	ga_entity* ent = audio->get_entity();
	_sim->remove(ent);
	_components.erase(_components.begin() + selected);
	delete audio;
}

irrklang::ISoundEngine* ga_audio_manager::get_engine() {
	return _engine;
}

std::vector<ga_audio_component*> ga_audio_manager::get_components() {
	return _components;
}

std::vector<const char*> ga_audio_manager::get_sound_device_list() {
	std::vector<const char*> devices;
	irrklang::ISoundDeviceList* deviceList = irrklang::createSoundDeviceList();
	for (int i = 0; i < deviceList->getDeviceCount(); ++i)
	{
		devices.push_back(deviceList->getDeviceDescription(i));
	}
	deviceList->drop();
	return devices;
}

std::vector<const char*> ga_audio_manager::get_record_device_list() {
	std::vector<const char*> devices;
	irrklang::ISoundDeviceList* deviceList = irrklang::createAudioRecorderDeviceList();
	for (int i = 0; i < deviceList->getDeviceCount(); ++i)
	{
		devices.push_back(deviceList->getDeviceDescription(i));
	}
	deviceList->drop();
	return devices;
}

void ga_audio_manager::start_record(int device_id) {
	irrklang::ISoundDeviceList* deviceList = irrklang::createAudioRecorderDeviceList();
	_recorder = irrklang::createIrrKlangAudioRecorder(_engine, irrklang::ESOD_AUTO_DETECT, deviceList->getDeviceID(device_id));
	deviceList->drop();

	_recorder->startRecordingBufferedAudio();
}

void ga_audio_manager::stop_record() {
	_recorder->stopRecordingAudio();
	write_wave_file("recorded.wav", _recorder->getAudioFormat(), _recorder->getRecordedAudioData());
}

void ga_audio_manager::start_record_engine() {
	_engine->setMixedDataOutputReceiver(_receiver);
}

void ga_audio_manager::stop_record_engine() {
	_engine->setMixedDataOutputReceiver(NULL);
	_receiver->stop_record();
}


void ga_audio_manager::write_wave_file(const char* filename, irrklang::SAudioStreamFormat format, void* data)
{
	if (!data)
	{
		printf("Could not save recorded data to %s, nothing recorded\n", filename);
		return;
	}

	FILE* file = fopen(filename, "wb");

	if (file)
	{
		// write wave header 
		unsigned short formatType = 1;
		unsigned short numChannels = format.ChannelCount;
		unsigned long  sampleRate = format.SampleRate;
		unsigned short bitsPerChannel = format.getSampleSize() * 8;
		unsigned short bytesPerSample = format.getFrameSize();
		unsigned long  bytesPerSecond = format.getBytesPerSecond();
		unsigned long  dataLen = format.getSampleDataSize();

		const int fmtChunkLen = 16;
		const int waveHeaderLen = 4 + 8 + fmtChunkLen + 8;

		unsigned long totalLen = waveHeaderLen + dataLen;

		fwrite("RIFF", 4, 1, file);
		fwrite(&totalLen, 4, 1, file);
		fwrite("WAVE", 4, 1, file);
		fwrite("fmt ", 4, 1, file);
		fwrite(&fmtChunkLen, 4, 1, file);
		fwrite(&formatType, 2, 1, file);
		fwrite(&numChannels, 2, 1, file);
		fwrite(&sampleRate, 4, 1, file);
		fwrite(&bytesPerSecond, 4, 1, file);
		fwrite(&bytesPerSample, 2, 1, file);
		fwrite(&bitsPerChannel, 2, 1, file);

		// write data

		fwrite("data", 4, 1, file);
		fwrite(&dataLen, 4, 1, file);
		fwrite(data, dataLen, 1, file);

		// finish

		printf("Saved audio as %s\n", filename);
		fclose(file);
	}
	else
		printf("Could not open %s to write audio data\n", filename);
}

