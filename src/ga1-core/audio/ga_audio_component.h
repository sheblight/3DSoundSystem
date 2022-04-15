#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/ga_component.h"

#include <cstdint>
#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

/*
** Renderable basic textured cubed.
*/
class ga_audio_component : public ga_component
{
public:
	ga_audio_component(class ga_entity* ent, irrklang::ISoundEngine* engine, const char* filepath);
	virtual ~ga_audio_component();

	virtual void update(struct ga_frame_params* params) override;

private:
	irrklang::ISoundEngine* _engine;
	irrklang::ISound* _source;

	//class ga_material* _material;
	//uint32_t _vao;
	//uint32_t _vbos[4];
	//uint32_t _index_count;
};