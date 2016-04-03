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
#include "controls.hpp"

using namespace Space;
using namespace glm;


World::World(GLFWwindow * _window)
//:pp_fxaa()
	:gnd(100, 100, 1)
#ifdef USE_SHADOWMAPPING
	, SHADOW_WIDTH(1024)
	, SHADOW_HEIGHT(1024)
#endif
{
	window = _window;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glEnable(GL_MULTISAMPLE);


	position_camera = vec3(10.0f, 10.0f, 10.0f);
	position_light = vec3(0.0f, 0.0f, 5.0f);

	glfwGetWindowSize(window, &width, &height);

	View = lookAt(position_camera
		, vec3(0, 0, 0)
		, vec3(0, 0, 1));

	Projection = perspective(45.0f, (float)width / (float)height, 0.1f, 10000.0f);

	gnd.InitChessBoard(100, 100, 1);


}

World::World() 
	:gnd()
#ifdef USE_SHADOWMAPPING
	, SHADOW_WIDTH(1024)
	, SHADOW_HEIGHT(1024)
#endif
{
	;
}

//コードの重複
void World::Initialize(GLFWwindow * _window) {
	window = _window;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	position_camera = vec3(-20.0f, 40.0f, -20.0f);
	position_light = vec3(0.0f, 0.0f, 5.0f);

	glfwGetWindowSize(window, &width, &height);
	
	View = lookAt(position_camera
		, vec3(0, 0, 0)
		, vec3(0, 0, 1));

	Projection = perspective(45.0f, (float)width / (float)height, 0.1f, 10000.0f);

	gnd.Initialize(100, 100, 1);
	gnd.InitChessBoard(100, 100, 1);
}

World::~World() {
#ifdef USE_POSTPROCESS
	delete pp_fxaa;
#endif
}

#ifdef USE_SHADOWMAPPING
void World::InitShadowMapping() {
	glGenFramebuffers(1, &fbo_depthmap);

	glGenTextures(1, &texture_depthmap);
	glBindTexture(GL_TEXTURE_2D, texture_depthmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT
		, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT
		, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//フレームバッファの設定を行う
	//ここでは深度マップのみが必要なのでカラーマップは計算する必要はない
	//そのためGLREAD/DRAWBUFFERでGL_NONEを指定する
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_depthmap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT
		, GL_TEXTURE_2D, texture_depthmap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//遠い面が小さいとそれより遠くに影はできないので注意
	GLfloat near_plane = 0.1f, far_plane = 1000.0f;	   
	Matrix_lightprojection =
		ortho(-80.0f, 80.0f, -80.0f, 80.0f
			, near_plane, far_plane);
	Matrix_lightview = lookAt(position_light, vec3(0.0f), vec3(0.0f, 0.0f, 1.0f));
	Matrix_worldspace_to_lightspace = Matrix_lightprojection * Matrix_lightview;
}
#endif

void World::BindShader(
	const std::string & _path_to_vshader_use_texture
	, const std::string & _path_to_fshader_use_texture
	, const std::string & _path_to_vshader_non_texture
	, const std::string & _path_to_fshader_non_texture) {

	id_shader_use_texture = LoadShaders(
		_path_to_vshader_use_texture.c_str()
		, _path_to_fshader_use_texture.c_str());
	id_shader_non_texture = LoadShaders(
		_path_to_vshader_non_texture.c_str()
		, _path_to_fshader_non_texture.c_str());

	info.id_shader_use_texture = id_shader_use_texture;
	info.id_shader_non_texture = id_shader_non_texture;

	info.id_handler_uniform_MVP_use_texture
		= glGetUniformLocation(id_shader_use_texture, "MVP");
	info.id_handler_uniform_MVP_non_texture
		= glGetUniformLocation(id_shader_non_texture, "MVP");

	info.id_handler_uniform_M_use_texture
		= glGetUniformLocation(id_shader_use_texture, "M");
	info.id_handler_uniform_M_non_texture
		= glGetUniformLocation(id_shader_non_texture, "M");

	info.id_handler_uniform_V_use_texture
		= glGetUniformLocation(id_shader_use_texture, "V");
	info.id_handler_uniform_V_non_texture
		= glGetUniformLocation(id_shader_non_texture, "V");

	info.id_handler_uniform_lightposition_use_texture
		= glGetUniformLocation(id_shader_use_texture, "LightPosition_worldspace");
	info.id_handler_uniform_lightposition_non_texture
		= glGetUniformLocation(id_shader_non_texture, "LightPosition_worldspace");


}


void World::BindShaderPostProcess(
	const std::string & _path_to_vshader_pasthrough
	, const std::string & _path_to_fshader_pasthrough
	, const std::string & _path_to_vshader_filter
	, const std::string & _path_to_fshader_filter) {
	id_shader_postprocess_passthrough = LoadShaders(
		_path_to_vshader_pasthrough.c_str()
		, _path_to_fshader_pasthrough.c_str());
	id_shader_postprocess_filter = LoadShaders(
		_path_to_vshader_filter.c_str()
		, _path_to_fshader_filter.c_str());

#ifdef USE_POSTPROCESS
	pp_fxaa = new PostProcessingFXAA(
		id_shader_postprocess_filter
		, id_shader_postprocess_passthrough);
	pp_fxaa->EnableFXAA();
#endif

	info.id_shader_postprocess_passthrough = id_shader_postprocess_passthrough;
	info.id_shader_postprocess_filter = id_shader_postprocess_filter;


}

//＃TODO　深度取得じゃない方も取得
void World::BindShaderShadowMapping(
	const std::string & _path_to_vshader_depth
	, const std::string & _path_to_fshader_depth
	, const std::string & _path_to_vshader_shadowmapping
	, const std::string & _path_to_fshader_shadowmapping
	) {

	//id_shader_shadowmapping_depth = LoadShaders(
	//	_path_to_vshader_depth.c_str()
	//	, _path_to_fshader_depth.c_str()
	//	);

	//id_shader_shadowmapping = LoadShaders(
	//	_path_to_vshader_shadowmapping.c_str()
	//	, _path_to_fshader_shadowmapping.c_str()
	//	);

	//info.id_shader_shadowmapping_depth = id_shader_shadowmapping_depth;
	//info.id_shader_shadowmapping = id_shader_shadowmapping;

	//info.id_handler_uniform_shadowmapping_depth_Mattrix_worldspace_to_lightspace
	//	= glGetUniformLocation(id_shader_shadowmapping_depth, "Matrix_Worldspace_to_Lightspace");
	//info.id_handler_uniform_shadowmapping_depth_Model
	//	= glGetUniformLocation(id_shader_shadowmapping_depth, "model");

	////Uniformlocation shadowmapping
	//info.id_handler_uniform_shadowmapping_projection
	//	= glGetUniformLocation(id_shader_shadowmapping, "projection");
	//info.id_handler_uniform_shadowmapping_view
	//	= glGetUniformLocation(id_shader_shadowmapping, "view");
	//info.id_handler_uniform_shadowmapping_model
	//	= glGetUniformLocation(id_shader_shadowmapping, "model");
	//info.id_handler_uniform_shadowmapping_mattrix_worldspace_to_lightspace
	//	= glGetUniformLocation(id_shader_shadowmapping, "Matrix_Worldspace_to_Lightspace");
	//info.id_handler_uniform_shadowmapping_view_position
	//	= glGetUniformLocation(id_shader_shadowmapping, "viewPos");
	//info.id_handler_uniform_shadowmapping_light_position
	//	= glGetUniformLocation(id_shader_shadowmapping, "lightPos");
	//info.id_handler_uniform_shadowmapping_texture_depth
	//	= glGetUniformLocation(id_shader_shadowmapping, "shadowMap");

	id_shader_shadowmapping_depth = LoadShaders(
		_path_to_vshader_depth.c_str()
		, _path_to_fshader_depth.c_str()
		);

	id_shader_shadowmapping2 = LoadShaders(
		_path_to_vshader_shadowmapping.c_str()
		, _path_to_fshader_shadowmapping.c_str()
		);

	info.id_shader_shadowmapping_depth = id_shader_shadowmapping_depth;
	info.id_shader_shadowmapping2 = id_shader_shadowmapping2;

	info.id_handler_uniform_shadowmapping_depth_Mattrix_worldspace_to_lightspace
		= glGetUniformLocation(id_shader_shadowmapping_depth, "Matrix_Worldspace_to_Lightspace");
	info.id_handler_uniform_shadowmapping_depth_Model
		= glGetUniformLocation(id_shader_shadowmapping_depth, "model");

	info.id_handler_uniform_MVP_shadowmapping
		= glGetUniformLocation(id_shader_shadowmapping2, "MVP");

	info.id_handler_uniform_M_shadowmapping
		= glGetUniformLocation(id_shader_shadowmapping2, "M");

	info.id_handler_uniform_V_shadowmapping
		= glGetUniformLocation(id_shader_shadowmapping2, "V");

	info.id_handler_uniform_lightposition_shadowmapping
		= glGetUniformLocation(id_shader_shadowmapping2, "LightPosition_worldspace");

	info.id_handler_uniform_Mattrix_worldspace_to_lightspace_shadowmapping
		= glGetUniformLocation(id_shader_shadowmapping2, "Matrix_Worldspace_to_Lightspace");

	info.id_handler_uniform_texture_depthmap_shadowmapping
		= glGetUniformLocation(id_shader_shadowmapping2, "shadowMap");


	//初期化
	InitShadowMapping();
}

void World::AddModel(Space::Model * _model) {
	models.push_back(_model);  //描画モデル追加
}


void World::BindGround() {
  ;
}


void World::BindSky() {
  ;
}

void World::SetPositionCamera(const glm::vec3 & _position_cam) {
	position_camera = _position_cam;
}

void World::SetPositionLight(const glm::vec3 & _position_light) {
	position_light = _position_light;


#ifdef USE_SHADOWMAPPING
	//For ShadowMapping
	Matrix_lightview = lookAt(position_light, vec3(0.0f), vec3(0.0f, 0.0f, 1.0f));
	Matrix_worldspace_to_lightspace = Matrix_lightprojection * Matrix_lightview;

#endif
}

void World::SetPowerLight(const GLfloat & _power) {
	power_light = _power;
}

void World::Render() {
	glfwGetWindowSize(window, &width, &height);

	computeMatricesFromInputs(window);

	//SetPositionLight(getPosition());

	//SetPositionLight(vec3(0, 50, 0));


#ifdef USE_POSTPROCESS
	pp_fxaa->Reshape(window);
	pp_fxaa->Bind();
#endif

	Projection = perspective(45.0f, (float)width / (float)height, 0.1f, 10000.0f);
	//View = lookAt(position_camera
	//	, vec3(0, 0, 0)
	//	, vec3(0, 0, 1));
	View = getViewMatrix();
	glViewport(0, 0, width, height);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < models.size(); ++i) {
		models[i]->Render(
			info
			, Projection
			, View
			, position_light
			, power_light
			);

	}


	mat4 M = mat4(
		1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, 1.0f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
		);

	mat4 MVP = Projection * View * M;

	glUseProgram(info.id_shader_non_texture);

	glUniformMatrix4fv(info.id_handler_uniform_MVP_non_texture
		, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(info.id_handler_uniform_M_non_texture
		, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(info.id_handler_uniform_V_non_texture
		, 1, GL_FALSE, &View[0][0]);
	glUniform3f(info.id_handler_uniform_lightposition_non_texture
		, position_light.x, position_light.y, position_light.z);

	//gnd.Render();
	//gnd.RenderGrid();
	gnd.RenderChess();

#ifdef USE_POSTPROCESS
	pp_fxaa->DeBind();
	pp_fxaa->Render();
#endif


}

#ifdef USE_SHADOWMAPPING
void World::RenderShadowMapping() {
	glfwGetWindowSize(window, &width, &height);

	//computeMatricesFromInputs(window);

   
	mat4 M = mat4(
		1.0f, 0.0f, 0.0f, 0.0f
		, 0.0f, 1.0f, 0.0f, 0.0f
		, 0.0f, 0.0f, 1.0f, 0.0f
		, 0.0f, 0.0f, 0.0f, 1.0f
		);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_depthmap);
	glUseProgram(info.id_shader_shadowmapping_depth);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

	glClear(GL_DEPTH_BUFFER_BIT);

	//glCullFace(GL_FRONT);

	//Render models
	for (size_t i = 0; i < models.size(); ++i) {
		models[i]->RenderDepth(
			info
			, Matrix_worldspace_to_lightspace
			);
	}

	//Render Ground
	mat4 MVP_depth = Matrix_worldspace_to_lightspace * M;
	glUseProgram(info.id_shader_shadowmapping_depth);

	glUniformMatrix4fv(info.id_handler_uniform_shadowmapping_depth_Mattrix_worldspace_to_lightspace
		, 1, GL_FALSE, &MVP_depth[0][0]);
	glUniformMatrix4fv(info.id_handler_uniform_shadowmapping_depth_Model
		, 1, GL_FALSE, &M[0][0]);
	gnd.RenderChess();

	//glCullFace(GL_BACK); // don't forget to reset original culling face
	
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



#ifdef USE_POSTPROCESS
	pp_fxaa->Reshape(window);
	pp_fxaa->Bind();
#endif

	

	Projection = perspective(45.0f, (float)width / (float)height, 0.1f, 10000.0f);
	//View = lookAt(position_camera
	//	, vec3(0, 0, 0)
	//	, vec3(0, 0, 1));
	//View = getViewMatrix();
	vec3 pos = vec3(
		models[0]->GetPosition()[1]
		, models[0]->GetPosition()[2]
		, models[0]->GetPosition()[0]
		);
	View = lookAt(position_camera, pos, vec3(0, 1, 0));

	glViewport(0, 0, width, height);
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (size_t i = 0; i < models.size(); ++i) {
		//models[i]->RenderShadowMapping(
		//	info
		//	, Projection
		//	, View
		//	, Matrix_worldspace_to_lightspace
		//	, position_light
		//	, power_light
		//	, texture_depthmap
		//	);

		models[i]->RenderShadowMapping(
			info
			, Projection
			, View
			, Matrix_worldspace_to_lightspace
			, position_light
			, power_light
			, texture_depthmap
			, v_d
			);
	}


	//地面の描画###############################################################
	glUseProgram(info.id_shader_shadowmapping2);

	////Activate texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_depthmap);

	mat4 MVP = Projection * View * M;

	glUniformMatrix4fv(info.id_handler_uniform_MVP_shadowmapping
		, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(info.id_handler_uniform_M_shadowmapping
		, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(info.id_handler_uniform_V_shadowmapping
		, 1, GL_FALSE, &View[0][0]);
	glUniform3f(info.id_handler_uniform_lightposition_shadowmapping
		, position_light.x, position_light.y, position_light.z);

	glUniform1i(info.id_handler_uniform_texture_depthmap_shadowmapping, 0);

	//gnd.Render();
	//gnd.RenderGrid();
	gnd.RenderChess();	 
	//地面の描画###############################################################

	//基準軸の描画#############################################################
	glUseProgram(info.id_shader_non_texture);
	M = translate(mat4(1.0f), vec3(0, 0.09, 0));
	MVP = Projection * View * M;

	glUniformMatrix4fv(info.id_handler_uniform_MVP_shadowmapping
		, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(info.id_handler_uniform_M_shadowmapping
		, 1, GL_FALSE, &M[0][0]);
	glUniformMatrix4fv(info.id_handler_uniform_V_shadowmapping
		, 1, GL_FALSE, &View[0][0]);
	glUniform3f(info.id_handler_uniform_lightposition_shadowmapping
		, 1, 1, 1);

	glUniform1i(info.id_handler_uniform_texture_depthmap_shadowmapping, 0);

	models[0]->RenderAxis();
	//基準軸の描画#############################################################

#ifdef USE_POSTPROCESS
	pp_fxaa->DeBind();
	pp_fxaa->Render();
#endif


}
#endif



