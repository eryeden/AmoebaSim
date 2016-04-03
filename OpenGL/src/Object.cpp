/*
The MIT License (MIT)

Copyright (c) 2015 Kazuki Kikuchi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "World.hpp"
#include <iostream>

using namespace Space;
using namespace glm;


const mat3 Utility::A_WorldToGLSpace = mat3(
	vec3(0, 0, 1)
	, vec3(1, 0, 0)
	, vec3(0, 1, 0)
	);

const mat4 Utility::Att_WorldToGLSpace = mat4(
	vec4(0, 0, 1, 0)
	, vec4(1, 0, 0, 0)
	, vec4(0, 1, 0, 0)
	, vec4(0, 0, 0, 1)
	);

const vec3 Utility::ConvertWorldToGLSpace(const vec3 & _in) {
	return A_WorldToGLSpace * _in;
}

const mat4 Utility::ConvertWorldToGLSpace(const mat4 & _in) {
	return Att_WorldToGLSpace * _in;
}

const vec3 Utility::Convert_milli_To_in(const glm::vec3 & _milli) {
	return _milli * 0.0393f;
}

const vec3 Utility::Convert_metre_To_in(const glm::vec3 & _metre) {
	return _metre * 39.3f;
}



Object::Object()
	:importer()
	, M(mat4(1.0f))
	, M_translate(mat4(1.0f))
	, M_attitude(mat4(1.0f))
{
	
	//M = Utility::ConvertWorldToGLSpace(M);
	//M_translate = Utility::ConvertWorldToGLSpace(M_translate);
	//M_attitude = Utility::ConvertWorldToGLSpace(M_attitude);
	M = M_translate * M_attitude;
	color_object = vec3(1.0f, 1.0f, 1.0f);
	UpdateM();

}

void Object::LoadModel(const std::string & _file_path) {

	importer.Load(_file_path);

}

void Object::LoadModel(
	const std::string & _file_path
	, const glm::vec3 _color) {

	importer.Load(_file_path, _color);

}

void Object::SetObjectPositionModelSpace(const glm::vec3 & _position) {
	//M_translate = translate(mat4(1.0f), Utility::ConvertWorldToGLSpace(_position));
	M_translate = translate(mat4(1.0f), _position);
	M = M_translate * M_attitude;
}

void Object::SetObjectAttitude(const glm::mat4 & _attitude) {
	//M_attitude = Utility::ConvertWorldToGLSpace(_attitude);
	M_attitude = _attitude;
	M = M_translate * M_attitude;
}

void Object::SetObjectColor(const glm::vec3 & _color) {
	color_object = _color;
}

void Object::Render(
	const Space::InfoShader & _info
	, const glm::mat4 & _projection
	, const glm::mat4 & _view
	, const glm::mat4 & _model_modelspace
	, const glm::vec3 & _light_position
	, const GLfloat & _light_power
	) {
	mat4 MM = _model_modelspace * M;
	mat4 MVP = _projection * _view * MM;
	if (importer.IsUseTexture()) {
		glUseProgram(_info.id_shader_use_texture);

		glUniformMatrix4fv(_info.id_handler_uniform_MVP_use_texture, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(_info.id_handler_uniform_M_use_texture, 1, GL_FALSE, &MM[0][0]);
		glUniformMatrix4fv(_info.id_handler_uniform_V_use_texture, 1, GL_FALSE, &_view[0][0]);
		glUniform3f(_info.id_handler_uniform_lightposition_use_texture
			, _light_position.x, _light_position.y, _light_position.z);
	}
	else {
		glUseProgram(_info.id_shader_non_texture);

		glUniformMatrix4fv(_info.id_handler_uniform_MVP_non_texture, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(_info.id_handler_uniform_M_non_texture, 1, GL_FALSE, &MM[0][0]);
		glUniformMatrix4fv(_info.id_handler_uniform_V_non_texture, 1, GL_FALSE, &_view[0][0]);
		glUniform3f(_info.id_handler_uniform_lightposition_non_texture
			, _light_position.x, _light_position.y, _light_position.z);
	}

	importer.Render();
}

//呼ぶ前にテクスチャのバインドと、アクティブ化が必要
void Object::RenderShadowMapping(
	const struct InfoShader & _info
	, const glm::mat4 & _projection
	, const glm::mat4 & _view
	, const glm::mat4 & _model_modelspace
	, const glm::mat4 & _Mattrix_worldspace_to_lightspace_shadowmapping
	, const glm::vec3 & _light_position
	, const GLfloat & _light_power
	, const GLuint & _texture_depthmap
	) {
	mat4 MM = _model_modelspace * M;
	mat4 MVP = _projection * _view * MM;
	if (importer.IsUseTexture()) {
		//glUseProgram(_info.id_shader_use_texture);

		//glUniformMatrix4fv(_info.id_handler_uniform_MVP_use_texture, 1, GL_FALSE, &MVP[0][0]);
		//glUniformMatrix4fv(_info.id_handler_uniform_M_use_texture, 1, GL_FALSE, &MM[0][0]);
		//glUniformMatrix4fv(_info.id_handler_uniform_V_use_texture, 1, GL_FALSE, &_view[0][0]);
		//glUniform3f(_info.id_handler_uniform_lightposition_use_texture
		//	, _light_position.x, _light_position.y, _light_position.z);
	}
	else {
		glUseProgram(_info.id_shader_shadowmapping2);

		//Activate texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture_depthmap);

		glUniformMatrix4fv(_info.id_handler_uniform_MVP_shadowmapping
				   , 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(_info.id_handler_uniform_M_shadowmapping
				   , 1, GL_FALSE, &MM[0][0]);
		glUniformMatrix4fv(_info.id_handler_uniform_V_shadowmapping
				   , 1, GL_FALSE, &_view[0][0]);
		glUniformMatrix4fv(_info.id_handler_uniform_Mattrix_worldspace_to_lightspace_shadowmapping
			, 1, GL_FALSE, &_Mattrix_worldspace_to_lightspace_shadowmapping[0][0]);
		glUniform3f(_info.id_handler_uniform_lightposition_shadowmapping
			, _light_position.x, _light_position.y, _light_position.z);


		glUniform1i(_info.id_handler_uniform_texture_depthmap_shadowmapping, 0);

	}

	importer.Render();
}


void Object::RenderDepth(
	const struct InfoShader & _info
	, const glm::mat4 & _Mattrix_worldspace_to_lightspace
	, const glm::mat4 & _model_modelspace
	) {
	mat4 MM = _model_modelspace * M;

	glUseProgram(_info.id_shader_shadowmapping_depth);
	glUniformMatrix4fv(_info.id_handler_uniform_shadowmapping_depth_Mattrix_worldspace_to_lightspace
		, 1, GL_FALSE, &_Mattrix_worldspace_to_lightspace[0][0]);
	glUniformMatrix4fv(_info.id_handler_uniform_shadowmapping_depth_Model
		, 1, GL_FALSE, &MM[0][0]);

	importer.Render();
}

void Object::UpdateM() {
	M = M_translate * M_attitude;
}


Model::Model() {
	//M_translate = translate(mat4(1.0f), Utility::ConvertWorldToGLSpace(vec3(0.0, 0.0, 0.0)));
	//M_attitude = Utility::ConvertWorldToGLSpace(rotate(mat4(1.0), 0.0f, vec3(0, 0, 1.0)));

	M_translate = translate(mat4(1.0f), Utility::ConvertWorldToGLSpace(vec3(0.0, 0.0, 0.0)));
	M_attitude = Utility::ConvertWorldToGLSpace(rotate(mat4(1.0), 0.0f, vec3(0, 0, 1.0)));

	UpdateM();
	InitializeAxis();
	InitializeVRenderer();
}

void Model::UpdateM() {
	M = M_translate * M_attitude;
}

void Model::AddObject(Space::Object * _object) {
	objects.push_back(_object);
}

void Model::SetModelPositionWorldSpace(const glm::vec3 & _position) {
	position_worldspace = _position;
	M_translate = translate(mat4(1.0f), Utility::ConvertWorldToGLSpace(_position));
	
	M = M_translate * M_attitude;
}

void Model::SetModelAttitude(const glm::mat4 & _attitude) {
	M_attitude = Utility::ConvertWorldToGLSpace(_attitude);
	M = M_translate * M_attitude;
}

void Model::Render(
	const Space::InfoShader & _info
	, const glm::mat4 & _projection
	, const glm::mat4 & _view
	, const glm::vec3 & _light_position
	, const GLfloat & _light_power
	) {

	for (size_t i = 0; i < objects.size(); ++i) {
		objects[i]->Render(
			_info
			, _projection
			, _view
			, M
			, _light_position
			, _light_power
			);
		RenderAxisShort();
	}

	mat4 MVP = _projection * _view * M;
	glUseProgram(_info.id_shader_non_texture);
	glUniformMatrix4fv(_info.id_handler_uniform_MVP_non_texture
		, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(_info.id_handler_uniform_M_non_texture
		, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(_info.id_handler_uniform_V_non_texture
		, 1, GL_FALSE, &_view[0][0]);
	glUniform3f(_info.id_handler_uniform_lightposition_non_texture
		, _light_position.x, _light_position.y, _light_position.z);
	RenderAxis();
}

//呼ぶ前にテクスチャのアクティブ化とバインドが必要
void Model::RenderShadowMapping(
	const struct InfoShader & _info
	, const glm::mat4 & _projection
	, const glm::mat4 & _view
	, const glm::mat4 & _Mattrix_worldspace_to_lightspace_shadowmapping
	, const glm::vec3 & _light_position
	, const GLfloat & _light_power
	, const GLuint & _texture_depthmap
	) {

	for (size_t i = 0; i < objects.size(); ++i) {
		objects[i]->RenderShadowMapping(
			_info
			, _projection
			, _view
			, M
			, _Mattrix_worldspace_to_lightspace_shadowmapping
			, _light_position
			, _light_power
			, _texture_depthmap
			);
		RenderAxisShort();
	}

	glUseProgram(_info.id_shader_shadowmapping2);

	//Activate texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture_depthmap);

	mat4 MVP = _projection * _view * M;
	glUseProgram(_info.id_shader_non_texture);
	glUniformMatrix4fv(_info.id_handler_uniform_MVP_non_texture
		, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(_info.id_handler_uniform_M_non_texture
		, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(_info.id_handler_uniform_V_non_texture
		, 1, GL_FALSE, &_view[0][0]);
	glUniform3f(_info.id_handler_uniform_lightposition_non_texture
		, _light_position.x, _light_position.y, _light_position.z);
	glUniformMatrix4fv(_info.id_handler_uniform_Mattrix_worldspace_to_lightspace_shadowmapping
		, 1, GL_FALSE, &_Mattrix_worldspace_to_lightspace_shadowmapping[0][0]);

	glUniform1i(_info.id_handler_uniform_texture_depthmap_shadowmapping, 0);

	RenderAxis();
}

void Model::RenderShadowMapping(
	const struct InfoShader & _info
	, const glm::mat4 & _projection
	, const glm::mat4 & _view
	, const glm::mat4 & _Mattrix_worldspace_to_lightspace_shadowmapping
	, const glm::vec3 & _light_position
	, const GLfloat & _light_power
	, const GLuint & _texture_depthmap
	, const glm::vec3 & _v
	) {
	for (size_t i = 0; i < objects.size(); ++i) {
		objects[i]->RenderShadowMapping(
			_info
			, _projection
			, _view
			, M
			, _Mattrix_worldspace_to_lightspace_shadowmapping
			, _light_position
			, _light_power
			, _texture_depthmap
			);
		RenderAxisShort();
	}

	glUseProgram(_info.id_shader_shadowmapping2);

	//Activate texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture_depthmap);

	mat4 MVP = _projection * _view * M;
	glUseProgram(_info.id_shader_non_texture);
	glUniformMatrix4fv(_info.id_handler_uniform_MVP_non_texture
		, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(_info.id_handler_uniform_M_non_texture
		, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(_info.id_handler_uniform_V_non_texture
		, 1, GL_FALSE, &_view[0][0]);
	glUniform3f(_info.id_handler_uniform_lightposition_non_texture
		, _light_position.x, _light_position.y, _light_position.z);
	glUniformMatrix4fv(_info.id_handler_uniform_Mattrix_worldspace_to_lightspace_shadowmapping
		, 1, GL_FALSE, &_Mattrix_worldspace_to_lightspace_shadowmapping[0][0]);

	glUniform1i(_info.id_handler_uniform_texture_depthmap_shadowmapping, 0);

	RenderAxis();
	RenderVector(_v);

}


void Model::RenderDepth(
	const struct InfoShader & _info
	, const glm::mat4 & _Mattrix_worldspace_to_lightspace
	) {

	for (size_t i = 0; i < objects.size(); ++i) {
		objects[i]->RenderDepth(
			_info
			, _Mattrix_worldspace_to_lightspace
			, M
			);
	}
}

const vec3 & Model::GetPosition() {
	return position_worldspace;
}

void Model::InitializeAxis() {
	GLfloat vertices_axis[] = {
		0.0, 0.0, 0.0
		, 3.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 3.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 0.0, 3.0
	};
	GLfloat vertices_axis_short[] = {
		0.0, 0.0, 0.0
		, 1.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 1.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 0.0, 1.0
	};
	GLfloat colors_axis[] = {
		1.0, 0.0, 0.0
		, 1.0, 0.0, 0.0
		, 0.0, 1.0, 0.0
		, 0.0, 1.0, 0.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
	};

	GLfloat normal_axis[] = {
		0.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
	};

	glGenBuffers(1, &vertex_buffer_axis);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_axis);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, &vertices_axis
		, GL_STATIC_DRAW);

	glGenBuffers(1, &vertex_buffer_axis_short);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_axis_short);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, &vertices_axis_short
		, GL_STATIC_DRAW);

	glGenBuffers(1, &normal_buffer_axis);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_axis);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, normal_axis
		, GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_axis);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_axis);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, colors_axis
		, GL_STATIC_DRAW);
}

void Model::RenderAxis() {

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_axis);
	glVertexAttribPointer(
		0
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_axis);
	glVertexAttribPointer(
		1
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_axis);
	glVertexAttribPointer(
		2
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);

	glDrawArrays(GL_LINE_STRIP, 0, 3 * 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}

void Model::RenderAxisShort() {

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_axis_short);
	glVertexAttribPointer(
		0
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_axis);
	glVertexAttribPointer(
		1
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_axis);
	glVertexAttribPointer(
		2
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);

	glDrawArrays(GL_LINE_STRIP, 0, 3 * 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}

void Model::InitializeVRenderer() {
	GLfloat vertices_vector[] = {
		0.0, 0.0, 0.0
		, 3.0, 0.0, 0.0
	};

	GLfloat colors_vector[] = {
		1.0, 0.0, 0.0
		, 1.0, 0.0, 0.0
	};

	GLfloat normal_vector[] = {
		0.0, 0.0, 0.0
		, 0.0, 0.0, 0.0
	};

	glGenBuffers(1, &vertex_buffer_vector);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_vector);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 6
		, &vertices_vector
		, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &normal_buffer_vector);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_vector);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 6
		, normal_vector
		, GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_vector);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_vector);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 6
		, colors_vector
		, GL_STATIC_DRAW);
}

void Model::RenderVector(const glm::vec3 _v) {

	GLfloat vertices_vector[] = {
		0.0, 0.0, 0.0
		, _v[0], _v[1], _v[2]
	};
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_vector);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 6
		, &vertices_vector
		, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_vector);
	glVertexAttribPointer(
		0
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_vector);
	glVertexAttribPointer(
		1
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_vector);
	glVertexAttribPointer(
		2
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);

	glDrawArrays(GL_LINE_STRIP, 0, 3 * 2);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}


Ground::Ground(
	const GLfloat & _width
	, const GLfloat & _height
	, const GLfloat & _grid_interval
	) {
	GLfloat wd2 = _width / 2.0;
	GLfloat hd2 = _height / 2.0;
	GLfloat vertices_ground[] = {
		  -wd2, hd2, 0
		, -wd2, -hd2, 0
		, wd2, hd2, 0
		, -wd2, -hd2, 0
		, wd2, -hd2, 0
		, wd2, hd2, 0
	};

	GLfloat vertices_ground2[] = {
		-wd2, 0, -hd2
		, -wd2, 0, hd2
		, wd2, 0, -hd2
		, -wd2, 0, hd2
		, wd2, 0, hd2
		, wd2, 0, -hd2
	};

	GLfloat colors_ground[] = {
		  0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
	};

	GLfloat normal_ground[] = {
		0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
	};

	glGenBuffers(1, &vertex_buffer_ground);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ground);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, &vertices_ground2
		, GL_STATIC_DRAW);

	glGenBuffers(1, &normal_buffer_ground);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_ground);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, normal_ground
		, GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_ground);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_ground);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, colors_ground
		, GL_STATIC_DRAW);


	//GRID
	std::vector<vec3> vertices_grid;
	std::vector<vec3> normals_grid;
	std::vector<vec3> colors_grid;

	GLfloat i, j;
	i = j = 0;
	vec3 tmp;// = vec3(i, hd2, 0);
	vec3 color = vec3(0.6, 0.6, 0.6);
	vec3 normal = vec3(0, 0, 1);

	while (1) {
		if (i > wd2) {
			break;
		}
		tmp = vec3(i, 0.01, -hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(i, 0.01, hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-i, 0.01, -hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-i, 0.01, hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		i += _grid_interval;
	}

	i = 0;
	while (1) {
		if (i > hd2) {
			break;
		}
		tmp = vec3(wd2, 0.01, -i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-wd2, 0.01, -i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(wd2, 0.01, i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-wd2, 0.01, i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		i += _grid_interval;
	}

	glGenBuffers(1, &vertex_buffer_grid);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_grid);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3)  * vertices_grid.size()
		, &vertices_grid[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &normal_buffer_grid);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_grid);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3) * normals_grid.size()
		, &normals_grid[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_grid);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_grid);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3) * colors_grid.size()
		, &colors_grid[0]
		, GL_STATIC_DRAW);
	number_of_indices = vertices_grid.size();

}

Ground::Ground() {
	;
}

void Ground::Initialize(
	const GLfloat & _width
	, const GLfloat & _height
	, const GLfloat & _grid_interval
	) {
	GLfloat wd2 = _width / 2.0;
	GLfloat hd2 = _height / 2.0;
	GLfloat vertices_ground[] = {
		-wd2, hd2, 0
		, -wd2, -hd2, 0
		, wd2, hd2, 0
		, -wd2, -hd2, 0
		, wd2, -hd2, 0
		, wd2, hd2, 0
	};

	GLfloat vertices_ground2[] = {
		-wd2, 0, -hd2
		, -wd2, 0, hd2
		, wd2, 0, -hd2
		, -wd2, 0, hd2
		, wd2, 0, hd2
		, wd2, 0, -hd2
	};

	GLfloat colors_ground[] = {
		0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
		, 0.99, 0.99, 0.99
	};

	GLfloat normal_ground[] = {
		0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
		, 0.0, 0.0, 1.0
	};

	glGenBuffers(1, &vertex_buffer_ground);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ground);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, &vertices_ground2
		, GL_STATIC_DRAW);

	glGenBuffers(1, &normal_buffer_ground);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_ground);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, normal_ground
		, GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_ground);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_ground);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(GLfloat) * 18
		, colors_ground
		, GL_STATIC_DRAW);


	//GRID
	std::vector<vec3> vertices_grid;
	std::vector<vec3> normals_grid;
	std::vector<vec3> colors_grid;

	GLfloat i, j;
	i = j = 0;
	vec3 tmp;// = vec3(i, hd2, 0);
	vec3 color = vec3(0.6, 0.6, 0.6);
	vec3 normal = vec3(0, 0, 1);

	while (1) {
		if (i > wd2) {
			break;
		}
		tmp = vec3(i, 0.01, -hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(i, 0.01, hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-i, 0.01, -hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-i, 0.01, hd2);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		i += _grid_interval;
	}

	i = 0;
	while (1) {
		if (i > hd2) {
			break;
		}
		tmp = vec3(wd2, 0.01, -i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-wd2, 0.01, -i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(wd2, 0.01, i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		tmp = vec3(-wd2, 0.01, i);
		vertices_grid.push_back(tmp);
		normals_grid.push_back(normal);
		colors_grid.push_back(color);

		i += _grid_interval;
	}

	glGenBuffers(1, &vertex_buffer_grid);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_grid);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3)  * vertices_grid.size()
		, &vertices_grid[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &normal_buffer_grid);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_grid);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3) * normals_grid.size()
		, &normals_grid[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_grid);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_grid);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3) * colors_grid.size()
		, &colors_grid[0]
		, GL_STATIC_DRAW);
	number_of_indices = vertices_grid.size();
}


void Ground::Render() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ground);
	glVertexAttribPointer(
		0
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_ground);
	glVertexAttribPointer(
		1
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_ground);
	glVertexAttribPointer(
		2
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);

	glDrawArrays(GL_TRIANGLES, 0, 3 * 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}



void Ground::RenderGrid() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_grid);
	glVertexAttribPointer(
		0
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_grid);
	glVertexAttribPointer(
		1
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_grid);
	glVertexAttribPointer(
		2
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);

	glDrawArrays(GL_LINES, 0, number_of_indices);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Ground::InitChessBoard(GLfloat _width, GLfloat _height, GLfloat _interval) {
	GLfloat wd2, hd2;
	wd2 = _width / 2.0; hd2 = _height / 2.0;

	vec3 c1 = vec3(1, 1, 1);
	vec3 c2 = vec3(0.7, 0.7, 0.7);
	vec3 cc[2] = { c1, c2 };

	std::vector<vec3> vertices_chess;
	std::vector<vec3> normals_chess;
	std::vector<vec3> colors_chess;

	vec3 base1 = vec3(0, 0, 0);
	vec3 base2 = vec3(0, 0, -_interval);
	vec3 base3 = vec3(_interval, 0, -_interval);
	vec3 base4 = vec3(_interval, 0, 0);

	vec3 bb[6] = {
		base1, base3, base2
		, base1, base4, base3
	};

	vec3 bbo[6] = {
		base1, base3, base2
		, base1, base4, base3
	};

	vec3 normal = vec3(0, -1, 0);
	mat3 A1 = mat3{
		-1, 0, 0
		, 0, 1, 0
		, 0, 0, 1
	};
	mat3 A2 = mat3{
		1, 0, 0
		, 0, 1, 0
		, 0, 0, -1
	};
	mat3 A3 = mat3{
		-1, 0, 0
		, 0, 1, 0
		, 0, 0, -1
	};

	int ii;



	//十字を描く
	GLfloat i, j; i = 0, j = 0;

	while (1) {
		if (j > hd2) break;
		i = 0;
		while (1) {
			if (i > wd2) { break; }

			for (ii = 0; ii < 6; ii++) {
				bb[ii] = bbo[ii] + vec3(i, 0, -j);
			}

			for (ii = 0; ii < 6; ii++) {
				vertices_chess.push_back(bb[ii]);
				normals_chess.push_back(normal);
				colors_chess.push_back(cc[(int)(i + j) % 2]);
			}


			for (ii = 0; ii < 6; ii++) {
				bb[ii] = A1 * (bbo[ii] + vec3(i, 0, -j));
			}
			for (ii = 0; ii < 6; ii++) {
				vertices_chess.push_back(bb[5 - ii]);
				normals_chess.push_back(normal);
				colors_chess.push_back(cc[(int)(i + j + 1) % 2]);
			}

			for (ii = 0; ii < 6; ii++) {
				bb[ii] = A2 * (bbo[ii] + vec3(i, 0, -j));
			}
			for (ii = 0; ii < 6; ii++) {
				vertices_chess.push_back(bb[5 - ii]);
				normals_chess.push_back(normal);
				colors_chess.push_back(cc[(int)(i + j + 1) % 2]);
			}

			for (ii = 0; ii < 6; ii++) {
				bb[ii] = A3 * (bbo[ii] + vec3(i, 0, -j));
			}
			for (ii = 0; ii < 6; ii++) {
				vertices_chess.push_back(bb[ii]);
				normals_chess.push_back(normal);
				colors_chess.push_back(cc[(int)(i + j) % 2]);
			}

			i += _interval;
		}
		j += _interval;
	}

	glGenBuffers(1, &vertex_buffer_chess);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_chess);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3)  * vertices_chess.size()
		, &vertices_chess[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &normal_buffer_chess);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_chess);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3) * normals_chess.size()
		, &normals_chess[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer_chess);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_chess);
	glBufferData(GL_ARRAY_BUFFER
		, sizeof(vec3) * colors_chess.size()
		, &colors_chess[0]
		, GL_STATIC_DRAW);
	number_of_vertices_chess = vertices_chess.size();


}
void Ground::RenderChess() {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_chess);
	glVertexAttribPointer(
		0
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_chess);
	glVertexAttribPointer(
		1
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_chess);
	glVertexAttribPointer(
		2
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);

	glDrawArrays(GL_TRIANGLES, 0, number_of_vertices_chess);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	
}
