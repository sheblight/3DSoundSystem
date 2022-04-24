/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_output.h"

#include "ga_frame_params.h"

#include "graphics/ga_material.h"
#include "graphics/ga_program.h"
#include "math/ga_mat4f.h"
#include "math/ga_quatf.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "audio/ga_audio_manager.h"
#include "audio/ga_audio_listener.h"
#include "ImGuiFileBrowser.h"

#include <cassert>
#include <iostream>
#include <SDL.h>

#include <windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

ga_output::ga_output(void* win, ga_audio_manager* audio_manager) : _window(win), _audio_manager(audio_manager) {
	int width, height;
	SDL_GetWindowSize(static_cast<SDL_Window* >(_window), &width, &height);

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	_default_material = new ga_constant_color_material();
	_default_material->init();

}

ga_output::~ga_output()
{
	delete _default_material;
}

void ga_output::update(ga_frame_params* params)
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Update viewport in case window was resized:
	int width, height;
	
	SDL_GetWindowSize(static_cast<SDL_Window* >(_window), &width, &height);
	
	glViewport(0, 0, width, height);

	// Clear viewport:
	glDepthMask(GL_TRUE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//SDL_SetRenderDrawColor((SDL_Renderer*)_renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
	//SDL_RenderClear((SDL_Renderer*)_renderer);

	// Compute projection matrices:
	ga_mat4f perspective;
	perspective.make_perspective_rh(ga_degrees_to_radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
	ga_mat4f view_perspective = params->_view * perspective;

	ga_mat4f ortho;
	ortho.make_orthographic(0.0f, (float)width, (float)height, 0.0f, 0.1f, 10000.0f);
	ga_mat4f view;
	view.make_lookat_rh(ga_vec3f::z_vector(), -ga_vec3f::z_vector(), ga_vec3f::y_vector());
	ga_mat4f view_ortho = view * ortho;

	// Draw all static geometry:
	for (auto& d : params->_static_drawcalls)
	{
		d._material->bind(view_perspective, d._transform);
		glBindVertexArray(d._vao);
		glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
	}

	// Draw all dynamic geometry:
	draw_dynamic(params->_dynamic_drawcalls, view_perspective);
	draw_dynamic(params->_gui_drawcalls, view_ortho);
	

	GLenum error = glGetError();
	assert(error == GL_NONE);

	draw_gui();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));	// opengl method
	
}

void ga_output::draw_dynamic(const std::vector<ga_dynamic_drawcall>& drawcalls, const ga_mat4f& view_proj)
{
	for (auto& d : drawcalls)
	{
		if (d._material)
		{
			d._material->set_color(d._color);
			d._material->bind(view_proj, d._transform);
		}
		else
		{
			_default_material->set_color(d._color);
			_default_material->bind(view_proj, d._transform);
		}

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint pos;
		glGenBuffers(1, &pos);
		glBindBuffer(GL_ARRAY_BUFFER, pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ga_vec3f) * d._positions.size(), &d._positions[0], GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		GLuint texcoord;
		if (!d._texcoords.empty())
		{
			glGenBuffers(1, &texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ga_vec2f) * d._texcoords.size(), &d._texcoords[0], GL_STREAM_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		GLuint indices;
		glGenBuffers(1, &indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * d._indices.size(), &d._indices[0], GL_STREAM_DRAW);

		glDrawElements(d._draw_mode, (GLsizei)d._indices.size(), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &indices);
		if (!d._texcoords.empty())
		{
			glDeleteBuffers(1, &texcoord);
		}
		glDeleteBuffers(1, &pos);
		glDeleteVertexArrays(1, &vao);
		glBindVertexArray(0);
	}
}

void ga_output::draw_gui() {
	bool done = false; // Needs to be a global or static boolean which breaks the main loop
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();		// opengl method
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	
	// window content
	static float f = 0.0f;
	static int counter = 0;
	std::vector<ga_audio_component*> components = _audio_manager->get_components();

	// set style
	ImGuiStyle& style = ImGui::GetStyle();
	style.GrabRounding = style.FrameRounding = 2;
	style.GrabMinSize = 8.0f;
	style.WindowPadding = ImVec2(13, 13);
	//style.FramePadding = ImVec2(5,4);

	ImGui::Begin("Audio GUI");
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	// table
	/*
	if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter)) {
		// Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
		ImGui::PushID(0);

		// Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		bool node_open = ImGui::TreeNode("Object", "%s_%u", components[0]->_name, 23);

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("More content to be placed here :P");

		if (node_open)
		{
			static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
			for (int i = 0; i < 8; i++)
			{
				ImGui::PushID(i); // Use field index as identifier.
				ImGui::ColorEdit3("clear color", (float*)&clear_color);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::EndTable();
	}
	*/

	// Output device selection
	std::vector<const char*> o_devices = _audio_manager->get_sound_device_list();
	static int selected_o_device = 0; // Here we store our selection data as an index.
	selected_o_device = selected_o_device >= o_devices.size() ? o_devices.size() - 1 : selected_o_device;

	const char* output_preview = o_devices[selected_o_device];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("Output", output_preview))
	{
		for (int n = 0; n < o_devices.size(); n++)
		{
			const bool is_selected = (selected_o_device == n);
			ImGui::PushID(n);
			if (ImGui::Selectable(o_devices[n], is_selected)) 
			{
				selected_o_device = n;
				_audio_manager->set_engine(selected_o_device);
				components[0]->update_sound_position();
			}
			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}

	// Record device selection
	std::vector<const char*> r_devices = _audio_manager->get_record_device_list();
	static int selected_device = 0; // Here we store our selection data as an index.
	selected_device = selected_device >= r_devices.size() ? r_devices.size()-1 : selected_device;
	
	const char* record_preview = r_devices[selected_device];  // Pass in the preview value visible before opening the combo (it could be anything)
	if (ImGui::BeginCombo("Input", record_preview))
	{
		for (int n = 0; n < r_devices.size(); n++)
		{
			const bool is_selected = (selected_device == n);
			ImGui::PushID(n);
			if (ImGui::Selectable(r_devices[n], is_selected))
			{
				selected_device = n;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	ImGui::NewLine();

	// Record functionality
	static bool is_recording = false;
	if (ImGui::Button("Record")) {
		is_recording = !is_recording;	
		if (is_recording) {
			_audio_manager->start_record(selected_device);
		}
		else {
			_audio_manager->stop_record();
		}
	}
	if (is_recording) {
		ImGui::SameLine();
		ImGui::Text("Recording...");
	}


	// Object list view
	static imgui_addons::ImGuiFileBrowser file_dialog;
	static int selected = 0;
	bool is_removed = false;
	float space = 300;
	bool is_listener = components[selected]->is_listener();

	// file browser
	static bool dialog_open = false;
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("Object List", ImVec2(150, space), true);
		for (int i = 0; i < components.size(); i++)
		{
			if (ImGui::Selectable(components[i]->_name, selected == i, ImGuiSelectableFlags_SpanAllColumns))
			{
				selected = i;
			}
		}
		ImGui::EndChild();
		if (ImGui::Button("New Sound Source"))
		{
			_audio_manager->make_source();
		}
		ImGui::SameLine();

		// remove sound source
		if (is_listener) {
			ImGui::BeginDisabled();
		}
		bool remove_button = ImGui::Button("Remove");
		if (is_listener) {
			ImGui::EndDisabled();
		}
		if (remove_button)			
		{
			is_removed = true;
		}
		ImGui::EndGroup();
	}
	
	ImGui::SameLine();

	// Object details
	if (components.size() > 0) 
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("Object View", ImVec2(0, space), false); // Leave room for 1 line below us

		
		// sound playback parameters
		if (!is_listener) {
			ImGui::Text("Sound Playback");
			ImGui::Separator();
			if (ImGui::Button("Play"))
			{
				components[selected]->play();
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
			{
				components[selected]->pause();
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				components[selected]->stop();
			}
			//ImGui::Text(components[selected]->_status);
			if (ImGui::Button("Browse File"))
			{
				dialog_open = true;
			}
			ImGui::SameLine();
			ImGui::Text(components[selected]->_filename);
			ImGui::Checkbox("Loop", (bool*)&components[selected]->is_looping());
			ImGui::NewLine();
		}
		

		// entity parameters
		ImGui::Text("Entity");
		ImGui::Separator();
		if (is_listener) {
			if (ImGui::DragFloat3("Listener Position", (float*)&components[selected]->get_position()))
			{
				components[selected]->update_sound_position();
			}
			ga_audio_listener* listener = dynamic_cast<ga_audio_listener*>(components[selected]);
			if (listener) {
				if (ImGui::SliderFloat("Rolloff Factor", (float*)&listener->get_rolloff_factor(), 0.f, 10.f))
				{
					listener->set_rolloff();
				}
			}
		}
		else {
			if (ImGui::SliderFloat3("Position", (float*)&components[selected]->get_position(), -40.f, 40.f))
			{
				components[selected]->update_sound_position();
			}
			if (ImGui::DragFloat("Min Radius", (float*)&components[selected]->get_min_radius()))
			{
				components[selected]->set_min_dist();
			}
		}
		

		// audio effect parameters
		ImGui::NewLine();
		ImGui::Text("Audio FX");
		ImGui::Separator();
		if (is_listener) {
			if (ImGui::SliderFloat("Master Volume", (float*)&components[selected]->get_volume(), 0.0f, 3.0f))
			{
				components[selected]->set_volume();
			}
		}
		else {
			if (ImGui::SliderFloat("Volume", (float*)&components[selected]->get_volume(), 0.0f, 1.0f))
			{
				components[selected]->set_volume();
			}
		}
		

		// misc parameters
		ImGui::NewLine();
		ImGui::Text("Miscellaneous");
		ImGui::Separator();
		ImGui::ColorEdit3("Color", (float*)&components[selected]->_color);

		ImGui::EndChild();
		ImGui::EndGroup();
	}
	
	// file dialog part
	if (dialog_open) 
	{
		ImGui::OpenPopup("Open File");
	}
	if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".wav,.mp3,.ogg"))
	{
		std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
		std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
		components[selected]->load_file(file_dialog.selected_path.c_str());
		strcpy(components[selected]->_filename, file_dialog.selected_fn.c_str());
		dialog_open = false;
	}
	else 
	{
		dialog_open = false;
	}

	ImGui::NewLine();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	
	if (is_removed) {
		int to_remove = selected;
		int next_size = components.size()-1;
		if (selected >= next_size) {
			selected = next_size == 0 ? 0 : next_size - 1;
		}
		_audio_manager->remove(to_remove);
		printf("Removed\n");
	}
	
}
