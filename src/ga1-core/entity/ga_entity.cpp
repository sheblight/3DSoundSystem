/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_entity.h"
#include "ga_component.h"

ga_entity::ga_entity()
{
	_transform.make_identity();
	_world_position = ga_vec3f::zero_vector();
}

ga_entity::~ga_entity()
{
	for (auto& c : _components)
	{
		delete c;
	}
	delete this;
}

void ga_entity::add_component(ga_component* comp)
{
	_components.push_back(comp);
}

void ga_entity::update(ga_frame_params* params)
{
	for (auto& c : _components)
	{
		c->update(params);
	}
}

void ga_entity::late_update(ga_frame_params* params)
{
	for (auto& c : _components)
	{
		c->late_update(params);
	}
}

void ga_entity::translate(const ga_vec3f& translation)
{
	//_transform.translate(translation);
	_world_position += translation;
}

void ga_entity::rotate(const ga_quatf& rotation)
{
	ga_mat4f rotation_m;
	rotation_m.make_rotation(rotation);
	_transform = rotation_m * _transform;
}
