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

#include <Controller/Controller_PIDeuler.hpp>
#include <iostream>
#include <random>
#include <ctime>

using namespace Controller;
using namespace Eigen;


std::mt19937 rand2(static_cast<unsigned int>(time(nullptr)));
std::uniform_int_distribution<int> dist(-1000, 1000);

Controller_PID_Euler::Controller_PID_Euler(MC::Core & _mc_core, const double & _dt)
	:Base(_mc_core)
	, Kp(0), Ti(0), Td(0)
	, theta_command(0)
	, theta_present(0)
	, theta_previous(0)
	, theta_derivative(0)
	, theta_integral(0)
	, error_present(0)
	, error_previous(0)
	, dt(_dt)
	, p_base(4600)
#ifdef _MSC_VER //Windows のみ
	, xboxctrlr(1)
#endif
	, controller_altitude(_dt)
	, controller_pitch(_dt)
	, controller_roll(_dt)
	, controller_yawrate(_dt)
{
	Initialize();
}

Controller_PID_Euler::~Controller_PID_Euler() {

}

void Controller_PID_Euler::Initialize() {
	Set_w_m_All(0);
}

void Controller_PID_Euler::Update() {
	Vector3d angle_euler123 = core.GetEulerinDegreesIJK(Vector3d(1, 2, 3));
	Vector3d angle_euler231 = core.GetEulerinDegreesIJK(Vector3d(2, 3, 1));
	Vector3d angle_euler312 = core.GetEulerinDegreesIJK(Vector3d(3, 1, 2));
	Vector3d position = core.get_state_vector_q().block<3, 1>(6, 0);
	Vector3d rate_dps = core.get_state_vector_q().block<3, 1>(3, 0) * 180.0 / M_PI;

       

#ifdef _MSC_VER //Windows のみ
	Matrix<double, 6, 1> ctrlr_state = xboxctrlr.GetSticksTrigers();
#endif
	Matrix<double, 6, 1> ctrlr_state = MatrixXd::Zero(6, 1);
	//printf("LX:%f\tLY:%f\tRX:%f\tRY:%f\tLT:%f\tRT:%f\n"
	//	, ctrlr_state(0)
	//	, ctrlr_state(1)
	//	, ctrlr_state(2)
	//	, ctrlr_state(3)
	//	, ctrlr_state(4)
	//	, ctrlr_state(5)
	//	);

	controller_altitude.Command(0.5 + 0.4 * (ctrlr_state(5) - ctrlr_state(4)));
	double base_ctrl = controller_altitude.Update(position(2));
	//controller_altitude.Output();

	double p_base_out = base_ctrl;// p_base + 90.0 * (ctrlr_state(5) - ctrlr_state(4));	//base_ctrl;
	controller_pitch.Command(ctrlr_state(0) * 10.0);
	controller_roll.Command(ctrlr_state(1) * 10.0);
	controller_yawrate.Command(ctrlr_state(2) * 200.0);
	//Command(ctrlr_state(0) * 20.0);



	//Set_w_m_All(4598.2);
	Set_w_m_All(p_base_out);

	//double wdiff_pitch = controller_pitch.Update(angle_euler123(0));
	////double wdiff_roll =  controller_roll.Update(angle_euler231(0));
	//double wdiff_roll = controller_roll.Update(angle_euler123(1));

	double wdiff_pitch = controller_pitch.Update(angle_euler312(1));
	double wdiff_roll = controller_roll.Update(angle_euler312(2));
	double yaw_factor = controller_yawrate.Update(rate_dps(2));

	//controller_pitch.Output();
	//controller_roll.Output();

	controller_yawrate.Output();

	printf("P:%f\tR:%f\n", angle_euler123(0), angle_euler231(0));

	core.mtrplps[0]->w_m += -wdiff_pitch - wdiff_roll - yaw_factor;
	core.mtrplps[1]->w_m += +wdiff_pitch - wdiff_roll + yaw_factor;

	core.mtrplps[2]->w_m += +wdiff_pitch + wdiff_roll - yaw_factor;
	core.mtrplps[3]->w_m += -wdiff_pitch + wdiff_roll + yaw_factor;

	double upper = 7000;
	double lower = 0;
	for (auto itr : core.mtrplps) {
		itr->w_m = (itr->w_m > upper) ? upper : itr->w_m;
		itr->w_m = (itr->w_m < lower) ? lower : itr->w_m;
	}


}

void Controller_PID_Euler::Set_w_m_All(float _wm) {

	for (auto itr : core.mtrplps) {
		itr->w_m = _wm;// +dist(rand2) / 10.0f;
	}

}



ControllerPID::ControllerPID(const double & _dt)
	: Kp(0), Ti(1000), Td(0)
	, x_command(0)
	, x_present(0)
	, x_previous(0)
	, error_derivative(0)
	, error_integral(0)
	, error_present(0)
	, error_previous(0)
	, dt(_dt)
{
	Initialize();
}

ControllerPID::ControllerPID()
	: Kp(0), Ti(1000), Td(0)
	, x_command(0)
	, x_present(0)
	, x_previous(0)
	, error_derivative(0)
	, error_integral(0)
	, error_present(0)
	, error_previous(0)
	, dt(1)
{
	Initialize();
}

void ControllerPID::Initialize() {
	;
}

void ControllerPID::SetDt(const double & _dt) {
	dt = _dt;
}

double ControllerPID::Update(const double & _x_in) {
	x_previous = x_present;
	x_present = _x_in;

	error_previous = error_present;
	error_present = (x_command - x_present);

	error_integral += error_present * dt;
	error_derivative = ((error_present)-(error_previous)) / dt;

	wdiff
		= Kp * error_present
		+ Kp * Td * error_derivative
		+ Kp * (1.0 / Ti) * error_integral
		;

	return wdiff;

}

void ControllerPID::Output() {
	printf("the_pres:%f\twd:%f\tdiv:%f\tintg:%f\n", x_present, wdiff, error_derivative, error_integral);
}
