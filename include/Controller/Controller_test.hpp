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

#ifndef _CONTROLLER_CONTROLLER_TEST_
#define _CONTROLLER_CONTROLLER_TEST_

#include <Eigen/Dense>
#include <vector>
#include <string>
//using namespace Eigen;
#define MC_G 9.80665

#define USE_STL_VECTOR

#include <Sim/MC.hpp>
#include <Controller/Base.hpp>

namespace Controller {

	class Controller_test : public Controller::Base {
	public:
		Controller_test(MC::Core & _mc_core);

		~Controller_test();


		void Initialize();
		void Update();

		void Setw(const Eigen::Vector3d & _w);
		Eigen::Matrix3d GetDCM();

	private:

		void Set_w_m_All(float _wm);

		//クォータニオンテスト うまくいった　微分値を普通に積分すれば回転しているクオータニオンを得られる
		Eigen::Vector3d w_bodyspace;
		Eigen::Vector4d q; //慣性座標系から機体座標系への変換を示すクォータニオン
		Eigen::Vector4d dq; //qの時間微分
		Eigen::Matrix3d DCM; //クォータニオンをDCMで示したもの 慣性座標系から機体座標系への変換を示す
								//よってこれをOpenGLに渡すには転置して逆行列とし、機体座標系から慣性座標系への変換行列にする必要がある

		double dt;

		Eigen::Matrix3d ConvertQtoDCM(const Eigen::Vector4d & _q); //クォータニオンからDCMへの変換
		Eigen::Matrix4d GetOmega(const Eigen::Vector3d & _w);	//クォータニオンとその時間微分をつなぐ行列Ω
		void Normalize(Eigen::Vector4d & _q);	  //クォータニオンの正規化



	};


};


#endif //_CONTROLLER_CONTROLLER_TEST_








