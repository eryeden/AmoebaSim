#include <Controller/Controller_test.hpp>
#include <iostream>
#include <random>
#include <ctime>

using namespace Controller;
using namespace Eigen;

Controller_test::Controller_test(MC::Core & _mc_core) 
	:Base(_mc_core)
{

	w_bodyspace = Vector3d(0, 0, 0);
	q = Vector4d(0, 0, 0, 1);
	dq = Vector4d::Zero();
	DCM = ConvertQtoDCM(q);
	dt = 0.016;

}

Controller_test::~Controller_test() {
	
}

void Controller_test::Initialize() {
	Set_w_m_All(0);
}

void Controller_test::Update() {
	Set_w_m_All(4598.2);
	//Set_w_m_All(0);

	//dqを得る
	//dq = 0.5 * GetOmega(w_bodyspace) * q;

	//積分し、qを得る
	//q = q + dq * dt;

	//正規化
	//Normalize(q);
	//q.normalize();

	//DCMに出力
	//DCM = ConvertQtoDCM(q).transpose();

	//std::cout << "dq:\n" << dq.norm() << std::endl;
	//std::cout << "q:\n" << q.norm() << std::endl;

}

void Controller_test::Setw(const Vector3d & _w) {
	w_bodyspace = _w;
}

Eigen::Matrix3d Controller_test::GetDCM() {
	return DCM;
}
//いろ決定用乱数発生器
static std::mt19937 rand2(static_cast<unsigned int>(time(nullptr)));
static std::uniform_int_distribution<int> dist(-1000, 1000);
void Controller_test::Set_w_m_All(float _wm) {
	//for (auto itr = core.mtrplps.begin(); itr != core.mtrplps.end(); itr++) {
	//	(*itr)->w_m = _wm;
	//}

	//これでFor-Each風に使えるらしい
	for (auto itr : core.mtrplps) {
		itr->w_m = _wm + dist(rand2) / 100.0f;
	}

	//core.mtrplps[0]->w_m = _wm * 0.0;


}

Eigen::Matrix3d Controller_test::ConvertQtoDCM(const Eigen::Vector4d & _q) {
	Matrix3d dcm;
	double q1, q2, q3, q4, q11, q22, q33, q44;
	q1 = _q(0);
	q2 = _q(1);
	q3 = _q(2);
	q4 = _q(3);

	q11 = q1 * q1;
	q22 = q2 * q2;
	q33 = q3 * q3;
	q44 = q4 * q4;

	dcm <<
		q11 - q22 - q33 + q44, 2.0*(q1 * q2 + q3 * q4), 2.0 * (q3 * q1 - q2 * q4)
		, 2.0 * (q1 * q2 - q3 * q4), q22 - q33 - q11 + q44, 2.0 * (q2 * q3 + q1 * q4)
		, 2.0 * (q3 * q1 + q2 * q4), 2.0 * (q2 * q3 - q1 * q4), q33 - q11 - q22 + q44
		;
	
	
	return dcm;
}

Eigen::Matrix4d Controller_test::GetOmega(const Eigen::Vector3d & _w) {
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

void Controller_test::Normalize(Eigen::Vector4d & _q) {
	Vector4d q = _q;
	double norm = sqrt(q(0)*q(0) + q(1)*q(1) + q(2)*q(2) + q(3)*q(3));
	_q(0) = q(0) / norm;
	_q(1) = q(1) / norm;
	_q(2) = q(2) / norm;
	_q(3) = q(3) / norm;
}