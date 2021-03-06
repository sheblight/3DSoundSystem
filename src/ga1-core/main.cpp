/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/ga_camera.h"
#include "framework/ga_compiler_defines.h"
#include "framework/ga_input.h"
#include "framework/ga_sim.h"
#include "framework/ga_output.h"
#include "jobs/ga_job.h"

#include "entity/ga_entity.h"
#include "entity/ga_lua_component.h"

#include "physics/ga_physics_component.h"

#include "graphics/ga_cube_component.h"
#include "graphics/ga_program.h"

#include "gui/ga_font.h"

#include "audio/ga_audio_component.h"
#include "audio/ga_audio_listener.h"
#include "audio/ga_audio_manager.h"



#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(GA_MINGW)
#include <unistd.h>
#endif

#include <iostream>

#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

ga_font* g_font = nullptr;
static void set_root_path(const char* exepath);
int test_sound();

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	ga_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	ga_input* input = new ga_input();
	ga_sim* sim = new ga_sim();
	ga_audio_manager* audio_manager = new ga_audio_manager(sim);
	ga_output* output = new ga_output(input->get_window(), audio_manager);

	// Create the default font:
	g_font = new ga_font("VeraMono.ttf", 16.0f, 512, 512);

	// Create camera.
	ga_camera* camera = new ga_camera({ 0.0f, 7.0f, 20.0f });
	ga_quatf rotation;
	rotation.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(ga_vec3f::x_vector(), ga_degrees_to_radians(15.0f));
	camera->rotate(rotation);


	// Create plane
	ga_entity* plane_ent = new ga_entity;
	struct ga_shape* plane_shape = new ga_plane;
	ga_physics_component* plane_physics = new ga_physics_component(plane_ent, plane_shape, 0);
	sim->add_entity(plane_ent);


	// Create listener
	ga_entity* listener = new ga_entity;
	ga_audio_listener* listener_audio = new ga_audio_listener(listener, audio_manager);
	sim->add_entity(listener);


	// Create an entity whose movement is driven by Lua script.
	ga_entity* lua = new ga_entity;
	//lua->translate({ 0.0f, 2.0f, 1.0f });
	ga_lua_component* lua_move = new ga_lua_component(lua, "data/scripts/move.lua");
	//ga_cube_component* lua_model = new ga_cube_component(lua, "data/textures/rpi.png");
	ga_audio_component* lua_audio = new ga_audio_component(lua, audio_manager);
	sim->add_entity(lua);
	

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		ga_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Perform the late update.
		sim->late_update(&params);

		// Draw to screen.
		output->update(&params);
	}

	delete output;
	delete sim;
	delete input;
	delete camera;
	delete audio_manager;

	ga_job::shutdown();

	return 0;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(GA_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(GA_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}



int test_sound() {
	const char* sound_file = "../../src/sounds/tick.wav";
	// start the sound engine with default parameters
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

	if (!engine)
	{
		printf("Could not startup engine\n");
		return 0; // error starting up the engine
	}

	// play some sound stream, looped
	engine->play2D(sound_file, true);
	//engine->drop(); // delete engine
	return 0;

}
