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


Core::Core()
	:
	x(Vector12d::Zero()),
	x_prev(Vector12d::Zero()),
	u(Vector12d::Zero()),
	m(0),
	J(Matrix3d::Zero()),
	Z(Matrix12d::Zero()),
	k1(Vector12d::Zero()),
	k2(Vector12d::Zero()),
	k3(Vector12d::Zero()),
	k4(Vector12d::Zero()),
	dt(0),
	dt2(0),
	dt6(0),
	motorplops(NULL),
	n_o_m(0)
	, time_ms(0)
{
	;
}

Core::Core(
	const Eigen::Matrix3d &tj, const double &tm, const double &tdt,
	MotorPlop* mps, const unsigned int &nom,
	const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
	const Eigen::Vector3d &x_e0, const Eigen::Vector3d &phi_e0
	)
	:
	x(Vector12d::Zero()),
	x_prev(Vector12d::Zero()),
	u(Vector12d::Zero()),
	m(0),
	J(Matrix3d::Zero()),
	Z(Matrix12d::Zero()),
	k1(Vector12d::Zero()),
	k2(Vector12d::Zero()),
	k3(Vector12d::Zero()),
	k4(Vector12d::Zero()),
	dt(0),
	dt2(0),
	dt6(0),
	motorplops(NULL),
	n_o_m(0)
	, time_ms(0)
{
	J = tj;
	m = tm;
	dt = tdt;
	dt2 = dt * 0.5;
	dt6 = dt / 0.6;

	motorplops = mps;
	n_o_m = nom;

	x.block<3, 1>(0, 0) = v_b0;
	x.block<3, 1>(3, 0) = w_b0;
	x.block<3, 1>(6, 0) = x_e0;
	x.block<3, 1>(9, 0) = phi_e0;
}

Core::Core(
	const Eigen::Matrix3d &tj, const double &tm, const double &tdt,
	const std::vector<MotorPlop*> &mplps, const std::vector<MC::Block*> &blks,
	const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
	const Eigen::Vector3d &x_e0, const Eigen::Vector3d &phi_e0
	)
	:
	x(Vector12d::Zero()),
	x_prev(Vector12d::Zero()),
	u(Vector12d::Zero()),
	m(0),
	J(Matrix3d::Zero()),
	Z(Matrix12d::Zero()),
	k1(Vector12d::Zero()),
	k2(Vector12d::Zero()),
	k3(Vector12d::Zero()),
	k4(Vector12d::Zero()),
	dt(0),
	dt2(0),
	dt6(0),
	mtrplps(mplps),
	components(blks)
	, time_ms(0)
{
	J = tj;
	m = tm;
	dt = tdt;
	dt2 = dt * 0.5;
	dt6 = dt / 6.0;

	x.block<3, 1>(0, 0) = v_b0;
	x.block<3, 1>(3, 0) = w_b0;
	x.block<3, 1>(6, 0) = x_e0;
	x.block<3, 1>(9, 0) = phi_e0;
}

//クォータニオン向けコンストラクタ
Core::Core(
	const Eigen::Matrix3d &tj, const double &tm, const double &tdt,
	const std::vector<MotorPlop*> &mplps, const std::vector<MC::Block*> &blks,
	const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
	const Eigen::Vector3d &x_e0, const Eigen::Vector4d &q_0
	)
	:
	xq(Vector13d::Zero()),
	xq_prev(Vector13d::Zero()),
	uq(Vector13d::Zero()),
	m(0),
	J(Matrix3d::Zero()),
	Zq(Matrix13d::Zero()),
	k1q(Vector13d::Zero()),
	k2q(Vector13d::Zero()),
	k3q(Vector13d::Zero()),
	k4q(Vector13d::Zero()),
	dt(0),
	dt2(0),
	dt6(0),
	mtrplps(mplps),
	components(blks)
	, time_ms(0)
{
	J = tj;
	m = tm;
	dt = tdt;
	dt2 = dt * 0.5;
	dt6 = dt / 6.0;

	//xq.block<3, 1>(0, 0) = v_b0;
	//xq.block<3, 1>(3, 0) = w_b0;
	//xq.block<3, 1>(6, 0) = x_e0;
	//xq.block<4, 1>(9, 0) = q_0;

	SetVelocityBodyspace(xq, v_b0);
	SetAngularVelocityBodyspace(xq, w_b0);
	SetPositionEarthspace(xq, x_e0);
	SetQuaternion(xq, q_0);
}

//コンストラクタのデリゲートというらしい
//古いコンストラクタでは動かない
//デリゲートコンストラクタではコンストラクタ以外の初期化子をかけない
Core::Core(const Eigen::Matrix3d &tj, const double &m, const double &dt,
	const std::vector<MC::MotorPlop*> &mplps, const std::vector<MC::Block*> &blks,
	const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
	const Eigen::Vector3d &x_e0, const Eigen::Vector4d &q_0
	, const Eigen::Vector3d & _cg)
	:Core(tj,  m,  dt,
		 mplps,  blks,
		 v_b0,  w_b0,
		 x_e0, q_0)
{
	cg_assemblyspace = _cg;
}


Matrix12d Core::mk_Z(const Vector12d & tx) {
	Matrix12d tZ = MatrixXd::Zero(12, 12);
	tZ.block<3, 3>(0, 0) = mk_sk(tx.block<3, 1>(3, 0));
	tZ.block<3, 3>(3, 3) = mk_B_mat(tx, J);
	tZ.block<3, 3>(6, 0) = mk_E_mat(tx);
	tZ.block<3, 3>(9, 3) = mk_D_mat(tx);
	return tZ;
}

Vector3d Core::mk_u11(const Vector12d &tx) {
	double phi, theta, psi; //φ、θ、ψ
	phi = tx(9);
	theta = tx(10);
	psi = tx(11);
	Vector3d u11;
	u11 <<
		MC_G * sin(theta),
		-MC_G * sin(phi) * cos(theta),
		-MC_G * cos(phi) * cos(theta);

	return u11;
}
//機構構成
Vector3d Core::mk_u12() {
	Vector3d u12 = Vector3d::Zero();
#ifndef USE_STL_VECTOR
	for (int i = 0; i < n_o_m; i++) {
		u12 += motorplops[i].get_f();
	}
#else
	for (auto itr = mtrplps.begin(); itr != mtrplps.end(); ++itr) {
		u12 += (*itr)->get_f();
	}
#endif
	u12 /= m;
	return u12;
}

Vector3d Core::mk_u2() {
	Matrix3d Jb_inv;
	Jb_inv <<
		1.0 / J(0, 0), 0, 0,
		0, 1.0 / J(1, 1), 0,
		0, 0, 1.0 / J(2, 2);

	Vector3d u2 = Vector3d::Zero();

#ifndef USE_STL_VECTOR
	for (int i = 0; i < n_o_m; i++) {
		u2 += (motorplops[i].r.cross(motorplops[i].get_f())) + motorplops[i].get_tau();
	}
#else
	for (auto itr = mtrplps.begin(); itr != mtrplps.end(); ++itr) {
		//u2 +=
		//	((*itr)->GetCOGPositionCOGModelspace().cross((*itr)->get_f())) + (*itr)->get_tau();
		u2 +=
			dynamic_cast<Block *>(*itr)->r.cross((*itr)->get_f()) + (*itr)->get_tau();
	}

	//for (int i = 0; i < mtrplps.size(); ++i) {
	//	u2 +=
	//		dynamic_cast<Block *>(mtrplps[i])->r.cross(mtrplps[i]->get_f()) + mtrplps[i]->get_tau();
	//}

#endif
	return Jb_inv * u2;
}

Vector12d Core::mk_u(const Vector12d &tx) {
	Vector12d u = Vector12d::Zero();
	u.block<3, 1>(0, 0) = mk_u11(tx) + mk_u12();
	u.block<3, 1>(3, 0) = mk_u2();
	return u;
}

Matrix3d Core::mk_sk(const Vector3d &v) {
	double v1, v2, v3;
	v1 = v(0);
	v2 = v(1);
	v3 = v(2);

	Matrix3d cp;
	cp <<
		0, -v3, v2
		, v3, 0, -v1
		, -v2, v1, 0;
	
	//cp << 0, v(2), -v(1),
	//	-v(2), 0, v(0),
	//	v(1), -v(0), 0;

	return cp;
}

Matrix3d Core::mk_B_mat(const Vector12d &tx, const Matrix3d &Jb) {
	Matrix3d Jb_inv;
	Jb_inv << 
		1.0 / Jb(0, 0), 0, 0,
		0, 1.0 / Jb(1, 1), 0,
		0, 0, 1.0 / Jb(2, 2);

	Matrix3d sk_L_sum = Matrix3d::Zero();


#ifndef USE_STL_VECTOR
	for (int i = 0; i < n_o_m; i++) {
		sk_L_sum += mk_sk(motorplops[i].get_l());
	}
#else
	for (auto itr = mtrplps.begin(); itr != mtrplps.end(); ++itr) {
		sk_L_sum += mk_sk((*itr)->get_l());
	}
#endif
	return Jb_inv * (mk_sk(tx.block<3, 1>(3, 0)) * Jb + sk_L_sum);
}

Matrix3d Core::mk_E_mat(const Vector12d &tx) {
	double phi, theta, psi; //φ、θ、ψ
	phi = tx(9);
	theta = tx(10);
	psi = tx(11);

	return
		(AngleAxisd(-psi, Vector3d(0, 0, 1)).matrix())
		* (AngleAxisd(-theta, Vector3d(0, 1, 0)).matrix())
		* (AngleAxisd(-phi, Vector3d(1, 0, 0)).matrix());
}

Matrix3d Core::mk_D_mat(const Vector12d &tx) {
	double phi, theta, psi; //φ、θ、ψ
	phi = tx(9);
	theta = tx(10);
	psi = tx(11);
	Matrix3d D;
	D <<
		1, sin(phi) * tan(theta), cos(phi) * tan(theta),
		0, cos(phi), -sin(phi),
		0, sin(phi) / cos(theta), cos(phi) / cos(theta);

	return D;
}


//KR4を用いて1ステップ進める
void Core::update() {
	Vector12d tx = Vector12d::Zero();

	k1 = mk_Z(x) * x + mk_u(x);
	tx = x + dt2 * k1;

	k2 = mk_Z(tx) * tx + mk_u(tx);
	tx = x + dt2 * k2;

	k3 = mk_Z(tx) * tx + mk_u(tx);
	tx = x + dt * k3;

	Z = mk_Z(tx);
	k4 = Z * tx + mk_u(tx);

	x_prev = x;
	x = x + dt6 * (k1 + 2.0 * (k2 + k3) + k4);

	time_ms += (unsigned long long)(dt * 1000);
}



Vector12d Core::get_state_vector() {
	return x;
}

Matrix12d Core::get_state_matrix() {
	return Z;
}

Matrix3d Core::GetAttitudeMatrix() {
	return mk_E_mat(x);
}

//#############################################クォータニオン関係################################################
//1ステップ前進積分　RK4
void Core::update_q() {
	Vector13d tx = Vector13d::Zero();

	k1q = mk_Z(xq) * xq + mk_u(xq);
	tx = xq + dt2 * k1q;
	NormalizeQuaternion(tx);

	k2q = mk_Z(tx) * tx + mk_u(tx);
	tx = xq + dt2 * k2q;
	NormalizeQuaternion(tx);

	k3q = mk_Z(tx) * tx + mk_u(tx);
	tx = xq + dt * k3q;
	NormalizeQuaternion(tx);

	Zq = mk_Z(tx);
	uq = mk_u(tx);
	k4q = Zq * tx + uq;

	xq_prev = xq;
	xq = xq + dt6 * (k1q + 2.0 * (k2q + k3q) + k4q);


	//Zq = mk_Z(xq);
	//xq_prev = xq;
	//xq += dt * (Zq * xq + mk_u(xq));

	NormalizeQuaternion(xq);
	
	//NormalizeQuaternion(xq_prev);

	time_ms += (unsigned long long)(dt * 1000.0);
}

Eigen::Matrix3d Core::GetAttitudeMatrix_q() {
	return mk_E_mat(xq);
}

Vector13d Core::get_state_vector_q() {
	return xq;
}

Matrix13d Core::get_state_matrix_q() {
	return Zq;
}

Matrix3d Core::MakeDCMfromQuaternion(const Vector4d & _q) {
	double q1, q2, q3, q4, q11, q22, q33, q44;
	q1 = _q(0);
	q2 = _q(1);
	q3 = _q(2);
	q4 = _q(3);

	q11 = q1 * q1;
	q22 = q2 * q2;
	q33 = q3 * q3;
	q44 = q4 * q4;

	Matrix3d dcm;
	dcm <<
		q11 - q22 - q33 + q44, 2.0*(q1 * q2 + q3 * q4), 2.0 * (q3 * q1 - q2 * q4)
		, 2.0 * (q1 * q2 - q3 * q4), q22 - q33 - q11 + q44, 2.0 * (q2 * q3 + q1 * q4)
		, 2.0 * (q3 * q1 + q2 * q4), 2.0 * (q2 * q3 - q1 * q4), q33 - q11 - q22 + q44
		;
	return dcm;

}

Eigen::Matrix4d Core::MakeOmegafromW(const Eigen::Vector3d & _w) {
	Matrix4d omg;

	double w1, w2, w3;
	w1 = _w(0);
	w2 = _w(1);
	w3 = _w(2);

	omg <<
		0, w3, -w2, w1
		, -w3, 0, w1, w2
		, w2, -w1, 0, w3
		, -w1, -w2, -w3, 0
		;

	return omg;
}

//!状態ベクトルからDCMを生成
Eigen::Matrix3d Core::mk_E_mat(const Vector13d & _x) {
	return MakeDCMfromQuaternion(GetQuaternion(_x)).transpose();
}
//!状態ベクトルから0.5 * Ωを生成
Eigen::Matrix4d Core::mk_Omega_2_mat(const Vector13d & _x) {

	return 0.5 * MakeOmegafromW(GetAngularVelocityBodyspace(_x));
}
//Bマトリックスの生成
Eigen::Matrix3d Core::mk_B_mat(const Vector13d &_x, const Eigen::Matrix3d &Jb) {
	Matrix3d Jb_inv;
	Jb_inv << 1.0 / Jb(0, 0), 0, 0,
		0, 1.0 / Jb(1, 1), 0,
		0, 0, 1.0 / Jb(2, 2);

	Vector3d w_bodyspace = GetAngularVelocityBodyspace(_x);
	Matrix3d sk_L_sum = Matrix3d::Zero();
#ifndef USE_STL_VECTOR
	for (int i = 0; i < n_o_m; i++) {
		sk_L_sum += mk_sk(motorplops[i].get_l());
	}
#else
	for (auto itr = mtrplps.begin(); itr != mtrplps.end(); ++itr) {
		sk_L_sum += mk_sk((*itr)->get_l());
	}
#endif
	return  Jb_inv * (-1.0 * mk_sk(w_bodyspace) * Jb + sk_L_sum);
}


//!状態ベクトルよりZの生成
Matrix13d Core::mk_Z(const Vector13d &_x) {
	Matrix13d tZ = MatrixXd::Zero(13, 13);
	tZ.block<3, 3>(0, 0) = -1.0 * mk_sk(GetAngularVelocityBodyspace(_x));
	tZ.block<3, 3>(3, 3) = mk_B_mat(_x, J);
	tZ.block<3, 3>(6, 0) = mk_E_mat(_x);
	tZ.block<4, 4>(9, 9) = mk_Omega_2_mat(_x);
	return tZ;
}
//状態ベクトルによりU11を生成
Eigen::Vector3d Core::mk_u11(const Vector13d &_x) {
	Vector3d g_earthspace(0, 0, -MC_G);
	Matrix3d dcm = MakeDCMfromQuaternion(GetQuaternion(_x));
	Vector3d u11 = dcm * g_earthspace;
	return u11;
}
//U生成 U = t[U11 + U12, U2, 0, 0]
Vector13d Core::mk_u(const Vector13d &_x) {
	Vector13d u = Vector13d::Zero();
	u.block<3, 1>(0, 0) = mk_u11(_x) + mk_u12();
	u.block<3, 1>(3, 0) = mk_u2();
	return u;
}


Eigen::Vector3d Core::GetVelocityBodyspace(const Vector13d & _x) {
	Vector3d x = Vector3d(_x(0), _x(1), _x(2));
	return x;
}
Eigen::Vector3d Core::GetAngularVelocityBodyspace(const Vector13d & _x) {
	Vector3d x = Vector3d(_x(3), _x(4), _x(5));
	return x;
}
Eigen::Vector3d Core::GetPositionEarthspace(const Vector13d & _x) {
	Vector3d x = Vector3d(_x(6), _x(7), _x(8));
	return x;
}
Eigen::Vector4d Core::GetQuaternion(const Vector13d & _x) {
	Vector4d x = Vector4d(_x(9), _x(10), _x(11), _x(12));
	return x;
}
void Core::SetVelocityBodyspace(Vector13d & _x, const Eigen::Vector3d &_v) {
	_x.block<3, 1>(0, 0) = _v;
}
void Core::SetAngularVelocityBodyspace(Vector13d & _x, const Eigen::Vector3d &_w) {
	_x.block<3, 1>(3, 0) = _w;
}
void Core::SetPositionEarthspace(Vector13d & _x, const Eigen::Vector3d &_s) {
	_x.block<3, 1>(6, 0) = _s;
}
void Core::SetQuaternion(Vector13d & _x, const Eigen::Vector4d &_q) {
	_x.block<4, 1>(9, 0) = _q;
}


//状態ベクトル中のクォータニオンの正規化を行う
void Core::NormalizeQuaternion(Vector13d & _x) {
	_x.block<4, 1>(9, 0).normalize();
}


//経過時間を取得
unsigned long long Core::GetTime() {
	return time_ms;
}

//DCMを1-2-3系オイラー角に変換する
Eigen::Vector3d Core::ConvertDCMtoEuler123(const Eigen::Matrix3d & _dcm) {

	unsigned char i, j, k;
	i = 1 - 1; j = 2 - 1; k = 3 - 1;
	double th1, th2, th3;

	th2 = asin(_dcm(i, k));
	th1 = atan(-1.0 * _dcm(j, k) / _dcm(k, k));
	th3 = atan(-1.0 * _dcm(i, j) / _dcm(i, i));

	return Vector3d(th1, th2, th3);

}

Eigen::Vector3d Core::ConvertDCMtoEuler123inDegrees(const Eigen::Matrix3d & _dcm) {
	return ConvertDCMtoEuler123(_dcm) * 180.0 / M_PI;
}

Eigen::Vector3d Core::GetEulerinDegrees() {
	return ConvertDCMtoEuler123inDegrees(GetAttitudeMatrix_q());
}

//DCMをi-j-k系オイラー角に変換する
Eigen::Vector3d Core::ConvertDCMtoEulerIJK(const Eigen::Matrix3d & _dcm
	, const Eigen::Vector3d & _ijk
	) {
	unsigned char i, j, k;
	i = _ijk(0) - 1; j = _ijk(1) - 1; k = _ijk(2) - 1;
	double th1, th2, th3;

	th2 = asin(_dcm(i, k));
	th1 = atan(-1.0 * _dcm(j, k) / _dcm(k, k));
	th3 = atan(-1.0 * _dcm(i, j) / _dcm(i, i));
	
	return Vector3d(th1, th2, th3);
}

//DCMをi-j-k系オイラー角に変換する
Eigen::Vector3d Core::ConvertDCMtoEulerIJKinDegrees(const Eigen::Matrix3d & _dcm
	, const Eigen::Vector3d & _ijk
	) {
	return ConvertDCMtoEulerIJK(_dcm, _ijk) * 180.0 / M_PI;
}

Eigen::Vector3d Core::GetEulerinDegreesIJK(const Eigen::Vector3d & _ijk) {
	return ConvertDCMtoEulerIJKinDegrees(GetAttitudeMatrix_q(), _ijk);
}

Eigen::Vector3d Core::GetCGAssemblyspace() {
	return cg_assemblyspace;
}

void Core::SetCGAssemblyspace(const Eigen::Vector3d & _cg) {
	cg_assemblyspace = _cg;
}



