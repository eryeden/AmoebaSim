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

#include <Manager/Manager.hpp>
#include <Controller/Controller_test.hpp>
#include <Controller/Controller_PIDeuler.hpp>
#include <iostream>

using namespace SimulationManager;

Manager::Manager(
	Controller::Base & _controller_base
	, MC::Core & _mc_core)

	: controller_base(_controller_base)
	, mc_core(_mc_core)
	, gl_sv()
{

	gl_sv.GenerateModel(mc_core);
	gl_sv.GetWorldHandler().SetPositionLight(
		glm::vec3(0, 50, 0)
		);
}


bool Manager::Update() {

	controller_base.Update();
	mc_core.update_q();

	GLFWwindow * window = const_cast<GLFWwindow *>(gl_sv.GetWindowHandler());


	glm::vec3 position_worldspace;
	position_worldspace.x = mc_core.xq(6);
	position_worldspace.y = mc_core.xq(7);
	position_worldspace.z = mc_core.xq(8);

	glm::vec3 v_d;

	//v_d.x = mc_core.xq(0);
	//v_d.y = mc_core.xq(1);
	//v_d.z = mc_core.xq(2);

	Eigen::Vector3d uu = mc_core.uq.block<3, 1>(0, 0);
	v_d.x = uu(0);
	v_d.y = uu(1);
	v_d.z = uu(2);

	gl_sv.GetWorldHandler().v_d = v_d * 1000.0f;

	//gl_sv.GetModelHandler().SetModelPositionWorldSpace(
	//	Space::Utility::Convert_m_To_in(
	//		Space::Supervisor::ConvertVector3dToVec3(
	//			Eigen::Vector3d(
	//				mc_core.x(6), mc_core.x(7), mc_core.x(8)
	//				)
	//			)
	//		)
	//	);

	//gl_sv.GetModelHandler().SetModelPositionWorldSpace(
	//	Space::Utility::Convert_milli_To_in(position_worldspace)
	//	);		 

	gl_sv.GetModelHandler().SetModelPositionWorldSpace(
		position_worldspace
		);

	gl_sv.GetModelHandler().SetModelPositionWorldSpace(
		Space::Utility::Convert_metre_To_in(position_worldspace)
		);

	glm::mat4 att;
	//OpenGLのModel行列がオブジェクト座標系からワールド座標系への変換であるとすればこれでよい
	//つまり転地してよい
	Eigen::Matrix3d att33 = mc_core.GetAttitudeMatrix_q(); //USE QUOTANION
	//Eigen::Matrix3d att33 = dynamic_cast<Controller::Controller_test &>(controller_base).GetDCM();
	att = glm::mat4(
		att33(0, 0), att33(1, 0), att33(2, 0), 0.0f
		, att33(0, 1), att33(1, 1), att33(2, 1), 0.0f
		, att33(0, 2), att33(1, 2), att33(2, 2), 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
		);
	gl_sv.GetModelHandler().SetModelAttitude(att);

	//printf("%8llu:p: %4.5f\t%4.5f\t%4.5f\t%4.5f\n"
	//	, mc_core.GetTime()
	//	, mc_core.xq(6), mc_core.xq(7), mc_core.xq(8)
	//	, Eigen::Vector3d(mc_core.xq(0), mc_core.xq(1), mc_core.xq(2)).norm()
	//	);
	//printf("%8llu:q: %4.5f\t%4.5f\t%4.5f\t%4.5f\n"
	//	, mc_core.GetTime()
	//	, mc_core.xq(9), mc_core.xq(10), mc_core.xq(11), mc_core.xq(12));

	//Eigen::Vector3d euler = mc_core.GetEulerinDegrees();
	//printf("%8llu:d: %4.5f\t%4.5f\t%4.5f\n"
	//	, mc_core.GetTime()
	//	, euler(0), euler(1), euler(2));

	//std::cout << "A:\n" << mc_core.Zq.block<3, 3>(0, 0) << std::endl;
	//std::cout << "B:\n" << mc_core.Zq.block<3, 3>(3, 3) << std::endl;
	//std::cout << "E:\n" << mc_core.Zq.block<3, 3>(6, 0) << std::endl;
	//std::cout << "O:\n" << mc_core.Zq.block<4, 4>(9, 9) << std::endl;


	gl_sv.Render();

	glfwPollEvents();
	if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, 1);
	}	
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_C)) {
		//dynamic_cast<Controller::Controller_PID_Euler &>(controller_base).Command(20);
	}
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_X)) {
		//dynamic_cast<Controller::Controller_PID_Euler &>(controller_base).Command(0);
	}
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Z)) {
		//dynamic_cast<Controller::Controller_PID_Euler &>(controller_base).Command(-20);
	}
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Q)) {
		//dynamic_cast<Controller::Controller_PID_Euler &>(controller_base).SetPBase(5000);
	}
	else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
		//dynamic_cast<Controller::Controller_PID_Euler &>(controller_base).SetPBase(4300);
	}
	return glfwWindowShouldClose(window);
}
