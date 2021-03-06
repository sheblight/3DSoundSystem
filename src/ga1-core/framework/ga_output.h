#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_drawcall.h"
#include "math/ga_mat4f.h"

#include "audio/ga_audio_manager.h"

#include <vector>

/*
** Represents the output stage of the frame.
** Owns whatever is drawn on the screen.
*/
class ga_output
{
public:
	ga_output(void* win, ga_audio_manager* audio_manager);
	~ga_output();

	void update(struct ga_frame_params* params);

private:
	void draw_dynamic(const std::vector<ga_dynamic_drawcall>& drawcalls, const ga_mat4f& view_proj);
	void draw_gui();

	void* _window;
	ga_audio_manager* _audio_manager;

	class ga_constant_color_material* _default_material;

	// GUI
};
