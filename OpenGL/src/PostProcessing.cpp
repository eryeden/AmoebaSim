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

#include "PostProcessing.hpp"
#include "shader.hpp"

PostProcessingFXAA::PostProcessingFXAA() {
	is_use_fxaa = false;

	//Initialize();
}

void PostProcessingFXAA::SetShader(
	const GLuint & _shader_fxaa_id
	, const GLuint & _shader_passthrough_id
	) {
	shader_fxaa = _shader_fxaa_id;
	shader_passthrough = _shader_passthrough_id;
	Initialize();
}

PostProcessingFXAA::PostProcessingFXAA(const GLuint & _shader_fxaa_id, const GLuint & _shader_passthrough_id) {

	shader_fxaa = _shader_fxaa_id;
	shader_passthrough = _shader_passthrough_id;

	is_use_fxaa = false;

	Initialize();
}

PostProcessingFXAA::PostProcessingFXAA(
	const std::string & _path_to_vertexshader_fxaa
	, const std::string & _path_to_fragmentshader_fxaa
	, const std::string & _path_to_vertexshader_passthrough
	, const std::string & _path_to_fragmentshader_passthrough
	) {

	shader_fxaa = LoadShaders(_path_to_vertexshader_fxaa.c_str(), _path_to_fragmentshader_fxaa.c_str());
	shader_passthrough = LoadShaders(_path_to_vertexshader_passthrough.c_str(), _path_to_fragmentshader_passthrough.c_str());

	is_use_fxaa = false;

	Initialize();
}

PostProcessingFXAA::~PostProcessingFXAA() {
	glDeleteFramebuffers(1, &framebuffer_object_id);
	glDeleteTextures(1, &texturebuffer_id);
	glDeleteRenderbuffers(1, &render_depthbuffer_id);
	glDeleteBuffers(1, &quad_vertexbuffer_id);
	//glDeleteVertexArrays(1, &vertexbuffer_id);

}

void PostProcessingFXAA::Reshape(
	int width
	, int height
	) {
	//スクリーンサイズ変更時にテクスチャサイズ、深度バッファ（レンダバッファ）サイズの変更が必要
	// On reshape
	//テクスチャのリサイズ
	glBindTexture(GL_TEXTURE_2D, texturebuffer_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	//レンダバッファ（深度バッファ）のリサイズ
	glBindRenderbuffer(GL_RENDERBUFFER, render_depthbuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

}

void PostProcessingFXAA::Reshape(const GLFWwindow * window) {

	//スクリーンサイズ変更時にテクスチャサイズ、深度バッファ（レンダバッファ）サイズの変更が必要
	// On reshape
	//テクスチャのリサイズ
	glfwGetWindowSize((GLFWwindow *)window, &width, &height);

	glBindTexture(GL_TEXTURE_2D, texturebuffer_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	//レンダバッファ（深度バッファ）のリサイズ
	glBindRenderbuffer(GL_RENDERBUFFER, render_depthbuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

}

void PostProcessingFXAA::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id);
}

void PostProcessingFXAA::DeBind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessingFXAA::Render() {
	glViewport(0, 0, width, height);
	//ここからポストプロセシング
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (is_use_fxaa) {
		RenderFXAA();
	}
	else {
		RenderPassthrough();
	}
	//頂点バッファ
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer_id);
	glVertexAttribPointer(
		0
		, 3
		, GL_FLOAT
		, GL_FALSE
		, 0
		, (void *)0
		);
	//描画
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
}

void PostProcessingFXAA::RenderPassthrough() {
	glUseProgram(shader_passthrough);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturebuffer_id);
	glUniform1i(handler_texture_passthrough, 0);
}

void PostProcessingFXAA::RenderFXAA() {
	glUseProgram(shader_fxaa);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturebuffer_id);
	glUniform1i(handler_texture_faxx, 0);
	glUniform1f(handler_width_screen, (float)width);
	glUniform1f(handler_height_screen, (float)height);
}


void PostProcessingFXAA::EnableFXAA() {
	is_use_fxaa = true;
}

void PostProcessingFXAA::DisableFXAA() {
	is_use_fxaa = false;
}

void PostProcessingFXAA::ToggleFXAA() {
	is_use_fxaa = !is_use_fxaa;
}

void PostProcessingFXAA::Initialize() {
	GLfloat g_quad_vertex_buffer_data[]
		= {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	//GLuint framebuffer_object_id = 0;
	glGenFramebuffers(1, &framebuffer_object_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id);

	// The texture we're going to render to
	//GLuint texturebuffer_id;
	glGenTextures(1, &texturebuffer_id);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, texturebuffer_id);

	// Give an empty image to OpenGL ( the last "0" means "empty" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// The depth buffer
	//GLuint render_depthbuffer_id;
	glGenRenderbuffers(1, &render_depthbuffer_id);
	glBindRenderbuffer(GL_RENDERBUFFER, render_depthbuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_depthbuffer_id);

	//// Alternative : Depth texture. Slower, but you can sample it later in your shader
	//GLuint depthTexture;
	//glGenTextures(1, &depthTexture);
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT24, 1024, 768, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texturebuffer_id, 0);

	//// Depth texture alternative : 
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);


	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

								   // Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("FB failed?\n");
	//return false;

	//GLuint quad_vertexbuffer_id;
	glGenBuffers(1, &quad_vertexbuffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);


	handler_texture_passthrough = glGetUniformLocation(shader_passthrough, "renderedTexture");

	handler_texture_faxx = glGetUniformLocation(shader_fxaa, "renderedTexture");
	handler_width_screen = glGetUniformLocation(shader_fxaa, "rt_w");
	handler_height_screen = glGetUniformLocation(shader_fxaa, "rt_h");

}
