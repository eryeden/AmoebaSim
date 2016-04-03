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

#include <iostream>
#include <Sim/MC.hpp>
#include <cmath>

using namespace Eigen;
using namespace MC;

MC::Generator::Generator()
	:
	vb0(Vector3d::Zero()), wb0(Vector3d::Zero()),
	xe0(Vector3d::Zero()), phie0(Vector3d::Zero()),
	dt(0.001)
{
	;
}

void MC::Generator::Add(MC::Block * _blk) {
	blks.push_back(_blk);


	switch (_blk->id_alternative_function)
	{
	case ID_AF_NONE:
		break;
	case ID_AF_MOTOR_PLOP:
		//クロスキャストというらしい　ダウンキャストしアップキャストする
		mtrplps.push_back(static_cast<StLMotorPlop *>(_blk));
		break;
	default:
		break;
	}

}

void MC::Generator::operator<<(MC::Block * _block) {
	Add(_block);
}

void MC::Generator::set_initialstate_vb(const Vector3d &tx) {
	vb0 = tx;
}

void MC::Generator::set_initialstate_wb(const Vector3d &tx) {
	wb0 = tx;
}

void MC::Generator::set_initialstate_xe(const Vector3d &tx) {
	xe0 = tx;
}

void MC::Generator::set_initialstate_phie(const Vector3d &tx) {
	phie0 = tx;
}

void MC::Generator::SetDt(double _dt) {
	dt = _dt;
}

void MC::Generator::SetInitialVelocityBodyspace(const Eigen::Vector3d & _v) {
	vb0 = _v;
}
void MC::Generator::SetInitialAngularVelocityBodyspace(const Eigen::Vector3d & _w) {
	wb0 = _w;
}
void MC::Generator::SetInitialPositionEarthspace(const Eigen::Vector3d & _p) {
	xe0 = _p;
}
void MC::Generator::SetInitialQuotanion(const Eigen::Vector4d & _q) {
	q0 = _q;
}


MC::Core MC::Generator::generate_core() {
	if (mtrplps.empty() && blks.empty()) { //何もなければ何もないCoreを返す
		return Core();
	}
	//全体の慣性テンソル、重心等を求める
	//重心再計算
	double m_sum = 0; //全質量
	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		m_sum += (*itr)->mass;
	}

	Vector3d cog; //全体重心
	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		cog += (*itr)->c_o_g * (*itr)->mass;
	}
	cog /= m_sum;

	//r代入 構成時位置更新
	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		(*itr)->r = (*itr)->c_o_g - cog;
	}

	//全体の慣性テンソルを求める
	Matrix3d J_sum = Matrix3d::Zero();
	Matrix3d T = Matrix3d::Zero();
	T <<
		0, 1, 1,
		1, 0, 1,
		1, 1, 0;

	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		J_sum = (*itr)->J + T * (*itr)->r.dot(((*itr)->r));
	}

	return Core(J_sum, m_sum, dt, mtrplps, blks, vb0, wb0, xe0, phie0);
}

MC::Core MC::Generator::GenerateCore_q() {
	if (mtrplps.empty() && blks.empty()) { //何もなければ何もないCoreを返す
		return Core();
	}
	//全体の慣性テンソル、重心等を求める
	//重心再計算
	double m_sum = 0; //全質量
	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		m_sum += (*itr)->mass;
	}

	Vector3d cog = Vector3d::Zero(); //全体重心
	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		cog += (*itr)->c_o_g * (*itr)->mass;
	}
	cog /= m_sum;

	//r代入 構成時位置更新
	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		(*itr)->r = (*itr)->c_o_g - cog;
	}

	//全体の慣性テンソルを求める
	Matrix3d J_sum = Matrix3d::Zero();
	Matrix3d T = Matrix3d::Zero();
	T <<
		0, 1, 1,
		1, 0, 1,
		1, 1, 0;
	Matrix3d addm = Matrix3d::Zero();
	Vector3d mpr;
	Vector3d r2;

	for (auto itr = blks.begin(); itr != blks.end(); ++itr) {
		r2 = Vector3d(
			(*itr)->r(0) * (*itr)->r(0)
			, (*itr)->r(1) * (*itr)->r(1)
			, (*itr)->r(2) * (*itr)->r(2)
			);
		mpr = (*itr)->mass * T * r2;
		addm(0, 0) = mpr(0);
		addm(1, 1) = mpr(1);
		addm(2, 2) = mpr(2);

		//J_sum = (*itr)->J + T * (*itr)->r.dot(((*itr)->r));
		J_sum = (*itr)->J + addm;
	}

	return Core(J_sum, m_sum, dt, mtrplps, blks, vb0, wb0, xe0, q0, cog);
}




