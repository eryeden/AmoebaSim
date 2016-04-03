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

#ifndef _WORLD_SUPERVISOR_
#define _WORLD_SUPERVISOR_

//Widnowsの場合
#if (_MSC_VER == 1900)	 //Visual Studio 2015
#pragma comment(lib, "glfw3-vc140.lib")
#pragma comment(lib, "assimp-vc140.lib")
#endif

#if (_MSC_VER == 1800)	 //Visual Studio 2013
#pragma comment(lib, "glfw3-vc120")  // glfw3-vc120.lib
#pragma comment(lib, "assimp-vc120.lib")  //assimp-vc120.lib
#endif

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "GlU32.Lib")
#pragma comment(lib, "glew32.lib")

#include <gl/glew.h>
#define _CRT_SECURE_NO_WARNINGS
#endif

#define GL_GLEXT_PROTOTYPES
//GLEWの拡張を使うには此れ↑が必要（glGenBuffers等）
//Linuxではこれだけで良い　おそらくGLEWのライブラリ不必要
//http://stackoverflow.com/questions/3032386/glgenbuffers-not-defined
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
//using namespace glm;

#include <World.hpp>
#include <Sim/MC.hpp>

namespace Space {

	class Supervisor {
	public:

		Supervisor();
		~Supervisor();

		bool Initialize(
			const int & _width_window
			, const int & _height_window
			);

		//Worldへの参照を返す
		Space::World & GetWorldHandler();


		void GenerateModel(MC::Core & _mc_core);
		//void SetModelParameter

		void RenderLoop();
		void Render();

		const GLFWwindow * GetWindowHandler();

		static const glm::vec3 & ConvertVector3dToVec3(Eigen::Vector3d _eigen_vector3d);
		static const glm::mat4 & ConvertMatrix3dToMat4(Eigen::Matrix3d _eigen_matrix3d);

		Space::Model & GetModelHandler();

	private:
		//OpenGL関係はManagerがすべて管理する
		Space::World  world;
		//これらはmc_coreにより自動生成される
		std::vector<Space::Model> space_models;
		std::vector<Space::Object> space_objects;

		GLFWwindow * window;

		void update_fps_counter(GLFWwindow * window);

	};


};





#endif	//!_WORLD_SUPERVISOR_