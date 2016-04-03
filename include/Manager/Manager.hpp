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

#ifndef _MCSMANAGER_MCSMANAGER_
#define _MCSMANAGER_MCSMANAGER_

#include <Eigen/Dense>
#include <vector>
#include <string>
//using namespace Eigen;
#define MC_G 9.80665

#define USE_STL_VECTOR

#include <Sim/MC.hpp>
#include <Controller/Base.hpp>
#include <Supervisor.hpp>


namespace SimulationManager {

	//これを継承して使う
	//Sim、Controller、Worldをまとめる上位存在が最初にInitializeを呼び、Updateを更新時に呼ぶようにする


	class Manager {
	public:
		Manager(
			Controller::Base & _controller_base
			, MC::Core & _mc_core
			);

		//virtual ~Manager() = 0;

		bool Update();


	private:

		Controller::Base & controller_base;
		MC::Core & mc_core;
		Space::Supervisor gl_sv;


	protected:


	};


};


#endif //!_MCSMANAGER_MCSMANAGER_