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

#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP

//Widnowsの場合
#if (_MSC_VER == 1900)	 //Visual Studio 2015
#pragma comment(lib, "glfw3-vc140.lib")
#pragma comment(lib, "assimp-vc140.lib")
#endif

#if (_MSC_VER == 1800)	 //Visual Studio 2013
#pragma comment(lib, "glfw3-vc120.lib")  
#pragma comment(lib, "assimp-vc120.lib")
#endif

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "GlU32.Lib")
#pragma comment(lib, "glew32.lib")

#include <gl/glew.h>
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <cstdlib>
#include <cstring>

#define GL_GLEXT_PROTOTYPES
//GLEWの拡張を使うには此れ↑が必要（glGenBuffers等）
//Linuxではこれだけで良い　おそらくGLEWのライブラリ不必要
//http://stackoverflow.com/questions/3032386/glgenbuffers-not-defined
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PostProcessingFXAA {
public:

	//シェーダーを読まないので初期化も行わない
	//そのため、のちにSetShaderが必要
	PostProcessingFXAA();
	
	PostProcessingFXAA(const GLuint & _shader_fxaa_id, const GLuint & _shader_passthrough_id);
	PostProcessingFXAA(
		const std::string & _path_to_vertexshader_fxaa
		, const std::string & _path_to_fragmentshader_fxaa
		, const std::string & _path_to_vertexshader_passthrough
		, const std::string & _path_to_fragmentshader_passthrough
		);

	~PostProcessingFXAA();

	//Initializeを呼ぶので別に初期化する必要はなし
	void SetShader(
		const GLuint & _shader_fxaa_id
		, const GLuint & _shader_passthrough_id
		);

	void Reshape(
		int width
		, int height);

	void Reshape(const GLFWwindow * window);

	void Bind();
	void DeBind();

	void Render();

	void EnableFXAA();
	void DisableFXAA();
	void ToggleFXAA();

private:

	GLsizei width;
	GLsizei height;

	GLuint shader_fxaa;
	GLuint shader_passthrough;

	GLuint framebuffer_object_id;
	GLuint texturebuffer_id;
	GLuint render_depthbuffer_id;
	GLuint quad_vertexbuffer_id;

	GLuint vertexbuffer_id;

	GLuint handler_texture_passthrough;
	GLuint handler_texture_faxx;

	GLuint handler_width_screen;
	GLuint handler_height_screen;

	bool is_use_fxaa;

	void Initialize();

	void RenderPassthrough();
	void RenderFXAA();
};



#endif //POSTPROCESSING




































