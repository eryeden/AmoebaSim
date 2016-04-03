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

#include <Supervisor.hpp>
#include <Eigen/Dense>
#include <random>
#include <ctime>


using namespace Space;
using namespace glm;

Supervisor::Supervisor()
	:world()
{

	Initialize(800, 600);

}

Supervisor::~Supervisor() {
	glfwTerminate();
}

bool Supervisor::Initialize(
	const int & _width_window
	, const int & _height_window
	)
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	window = glfwCreateWindow(_width_window, _height_window, "MCS",
		NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);

	//GLEWを使う場合
#ifdef _MSC_VER
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("Version: %s\n", version);

	//ワールド初期化
	world.Initialize(window);

	//シェーダーのバインドをする
	world.BindShader(
		"./../../OpenGL/glsl/StandardShading_vs.glsl"
		, "./../../OpenGL/glsl/StandardShading_fs.glsl"
		, "./../../OpenGL/glsl/StandardShading_vs_non_tex.glsl"
		, "./../../OpenGL/glsl/StandardShading_fs_non_tex.glsl"
		);

	world.BindShaderPostProcess(
		"./../../OpenGL/glsl/pp_vs.glsl"
		, "./../../OpenGL/glsl/pp_fs.glsl"
		, "./../../OpenGL/glsl/fxaa_vs2.glsl"
		, "./../../OpenGL/glsl/fxaa_fs2.glsl"
		);

	world.BindShaderShadowMapping(
		"./../../OpenGL/glsl/shadow_mapping_depth_vs.glsl"
		, "./../../OpenGL/glsl/shadow_mapping_depth_fs.glsl"
		, "./../../OpenGL/glsl/shadow_mapping_standard_vs.glsl"
		, "./../../OpenGL/glsl/shadow_mapping_standard_fs.glsl"
		);

	//world.SetPositionLight(vec3(20, 50, 20));
}

//Worldへの参照を返す
World & Supervisor::GetWorldHandler() {
	return world;
}

void Supervisor::GenerateModel(MC::Core & _mc_core) {

	//いろ決定用乱数発生器
	std::mt19937 rand2(static_cast<unsigned int>(time(nullptr)));
	std::uniform_int_distribution<int> dist(0, 999);


	vec3 position_modelspace;
	vec3 position_worldspace;
	mat4 attitude;
	mat4 attitude_model;
	Eigen::Matrix3d att33;

	Model mdl_component;
	space_models.push_back(mdl_component);

	position_worldspace.x = _mc_core.x(6);
	position_worldspace.y = _mc_core.x(7);
	position_worldspace.z = _mc_core.x(8);

	att33 = _mc_core.GetAttitudeMatrix();
	attitude_model = mat4(
		att33(0, 0), att33(1, 0), att33(2, 0), 0.0f
		, att33(0, 1), att33(1, 1), att33(2, 1), 0.0f
		, att33(0, 2), att33(1, 2), att33(2, 2), 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
		);

//	(space_models.back()).SetModelPositionWorldSpace(Utility::Convert_milli_To_in(position_worldspace));
	(space_models.back()).SetModelPositionWorldSpace(Utility::Convert_metre_To_in(position_worldspace));
	(space_models.back()).SetModelAttitude(attitude_model);

	//プッシュバックでOBJECTを追加する方法だとVBO等に与えられるIDがうまく割り振られなかった
	//リサイズし、そこを設定する方針だとうまくいった
	space_objects.resize(_mc_core.components.size());
	for (int i = 0; i < _mc_core.components.size(); ++i) {
		if (_mc_core.components[i]->id != MC::ID_COMPONENT) {
			//STLコンポーネントでなければ飛ばす
			//TODO　用意したシリンダ３Dモデル等をリスケールすることでSTLでないコンポーネントにも対応する
			continue;
		}

		//ダウンキャスト		  static_castだと怒られない
		MC::StLComponent * comp = static_cast<MC::StLComponent *>(_mc_core.components[i]);
		//(space_objects[i]).LoadModel(comp->GetPathToModelFile(), vec3(0.964, 0.714, 0));
		//(space_objects[i]).LoadModel(comp->GetPathToModelFile()
		//	, vec3(dist(rand2) / 1000.0, dist(rand2) / 1000.0, dist(rand2) / 1000.0));
		(space_objects[i]).LoadModel(comp->GetPathToModelFile()
			, vec3(comp->GetColor()(0), comp->GetColor()(1), comp->GetColor()(2)));


		position_modelspace.x = comp->GetPositionModelspace()(0) - _mc_core.GetCGAssemblyspace()(0);
		position_modelspace.y = comp->GetPositionModelspace()(1) - _mc_core.GetCGAssemblyspace()(1);
		position_modelspace.z = comp->GetPositionModelspace()(2) - _mc_core.GetCGAssemblyspace()(2);

		//position_modelspace.x = comp->c_o_g(0);
		//position_modelspace.y = comp->c_o_g(1);
		//position_modelspace.z = comp->c_o_g(2);

		att33 = comp->GetAttitude();

		attitude = mat4(
			att33(0, 0), att33(1, 0), att33(2, 0), 0.0f
			, att33(0, 1), att33(1, 1), att33(2, 1), 0.0f
			, att33(0, 2), att33(1, 2), att33(2, 2), 0.0f
			, 0.0f, 0.0f, 0.0f, 1.0f
			);

//		(space_objects[i]).SetObjectPositionModelSpace(Utility::Convert_milli_To_in(position_modelspace));
		(space_objects[i]).SetObjectPositionModelSpace(Utility::Convert_metre_To_in(position_modelspace));
		(space_objects[i]).SetObjectAttitude(attitude);
	}



	//モデルにオブジェクトを追加
	//イテレータをポインタとして渡したらうまくいかなかった

	//これだとうまくポインタを渡せない　イテレータ自身のポインタということになってしまうのか？
	//for (auto itr : space_objects) {
	//	space_models[0].AddObject(&itr);
	//}

	for (int i = 0; i < space_objects.size(); ++i) {
		space_models[0].AddObject(&space_objects[i]);
	}


	world.AddModel(&space_models[0]);


}

void Space::Supervisor::RenderLoop() {
	while (!glfwWindowShouldClose(window)) {
		update_fps_counter(window);

		//world.Render();

		world.RenderShadowMapping();

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_X)) {

		}
		else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_C)) {

		}

	}
}

void Space::Supervisor::Render() {
	update_fps_counter(window);
	world.RenderShadowMapping();
	glfwSwapBuffers(window);
}

void Space::Supervisor::update_fps_counter(GLFWwindow * _window) {
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "MCS @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

const GLFWwindow * Space::Supervisor::GetWindowHandler() {
	return window;
}

Model & Supervisor::GetModelHandler() {
	return space_models[0];
}


//######## utilities ####################

const glm::vec3 & Space::Supervisor::ConvertVector3dToVec3(Eigen::Vector3d _eigen_vector3d) {
	return vec3(
		_eigen_vector3d(0)
		, _eigen_vector3d(1)
		, _eigen_vector3d(2)
		);
}

const glm::mat4 & Space::Supervisor::ConvertMatrix3dToMat4(Eigen::Matrix3d _eigen_matrix3d) {
	return mat4(
		_eigen_matrix3d(0, 0), _eigen_matrix3d(1, 0), _eigen_matrix3d(2, 0), 0.0f
		, _eigen_matrix3d(0, 1), _eigen_matrix3d(1, 1), _eigen_matrix3d(2, 1), 0.0f
		, _eigen_matrix3d(0, 2), _eigen_matrix3d(1, 2), _eigen_matrix3d(2, 2), 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
		);
}

//######## utilities ####################
