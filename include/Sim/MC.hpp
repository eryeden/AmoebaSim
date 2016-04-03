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

#ifndef _SIM_MC_
#define _SIM_MC_

#include <Eigen/Dense>
#include <vector>
#include <string>
//using namespace Eigen;
#define MC_G 9.80665

#define USE_STL_VECTOR

namespace MC { // NAMESPACE MC


	using Matrix12d = Eigen::Matrix<double, 12, 12>;
	using Vector12d = Eigen::Matrix<double, 12, 1>;
	//For Quotanion
	using Matrix13d = Eigen::Matrix<double, 13, 13>;
	using Vector13d = Eigen::Matrix<double, 13, 1>;

	static const unsigned char ID_NONE = 0x00; //設定なし
	static const unsigned char ID_CYLINDER = 0x01; //円柱
	static const unsigned char ID_CUBOID = 0x02; //直方体
	static const unsigned char ID_COMPONENT = 0x03; //STLファイルにより形状を指定

	//特殊機能付きコンポーネント
	static const unsigned char ID_AF_NONE = 0x00;
	static const unsigned char ID_AF_MOTOR_PLOP = 0x01;


	class Block {
	public:
		Block();
		Block(double _mass);
		Block(const double & _mass
			, const Eigen::Matrix3d & _J
			, const Eigen::Vector3d & _c_o_g);
		Block(const double & _mass
			, const Eigen::Matrix3d & _J
			, const Eigen::Vector3d & _c_o_g
			, const unsigned char & _id);
		double mass;
		Eigen::Matrix3d J; //慣性テンソル
		Eigen::Vector3d c_o_g; //構成時重心座標 
							   //アセンブリ時の原点を使用した本パーツの重心位置
		Eigen::Vector3d r; //全体重心からの距離　	//これはGenerate時に設定されるので初期化時に値を設定する必要がない
						//モデル（すべてのパーツをあわせたもの）重心を原点としたときの本パーツの重心位置
		//virtual void calc_m_o_i() = 0; //慣性テンソルの計算
		unsigned char id; //形状ID
		unsigned char id_alternative_function; //特殊機能ID
	};


	class Cylinder : public Block {
	public:
		Cylinder(const double &_mass, const double & _r, const double &_h);
		double R;
		double h;
		void calc_m_o_i();
	};


	class Cuboid : public Block {
	public:
		Cuboid(const double &_mass, const double &_w, const double &_h, const double &_d);
		double w;
		double h;
		double d;
		void calc_m_o_i();
	};

	//モデル描画にSTLファイルを使用する
	class StLComponent : public Block {
	public:
		StLComponent();
		StLComponent(const std::string &p2stl);
		StLComponent(
			const std::string &p2stl
			, const Eigen::Matrix3d &_J
			, const Eigen::Vector3d &_c_o_g);
		StLComponent(
			const std::string &p2stl
			, const Eigen::Matrix3d &_J
			, const Eigen::Vector3d &_c_o_g
			, const Eigen::Matrix3d &m_att
			);
		//アセンブリデータから読む場合、この方がらく
		StLComponent(
			const std::string & _path_to_stl
			, const double & _mass
			, const Eigen::Matrix3d & _J
			, const Eigen::Matrix3d & _m_attitude
			, const Eigen::Vector3d & _position_c_o_g_blockspace
			, const Eigen::Vector3d & _position_modelspace
			);
		//カラー情報
		StLComponent(
			const std::string & _path_to_stl
			, const double & _mass
			, const Eigen::Matrix3d & _J
			, const Eigen::Matrix3d & _m_attitude
			, const Eigen::Vector3d & _position_c_o_g_blockspace
			, const Eigen::Vector3d & _position_modelspace
			, const Eigen::Vector3d & _color
			);

		void set_attitude(Eigen::Matrix3d _m_att);
		void set_path_to_stl(std::string _path_to_stl);
		void SetColor(const Eigen::Vector3d & _color);

		const std::string & GetPathToModelFile();
		const Eigen::Vector3d & GetPositionModelspace();
		const Eigen::Matrix3d & GetAttitude();
		const Eigen::Vector3d & GetColor();

	private:

		Eigen::Vector3d	position_c_o_g_blockspace;
		//本ブロック座標における重心位置
		Eigen::Vector3d position_modelspace;
		//アセンブリ空間における本ブロック座標系の原点位置
		Eigen::Matrix3d mat_attitude;
		//３Dモデルファイルへのパス
		std::string path_to_stl;
		//色情報
		Eigen::Vector3d color;

	};


	class MotorPlop {
	public:
		MotorPlop(double c_t, double c_q);
		MotorPlop(
			const double & _c_t, const double & _c_q
			, const double & _w_m, const Eigen::Matrix3d & _Jr
			);

		//モデルの重心位置を原点としたモデル空間におけるコンポーネントの重心位置
		virtual Eigen::Vector3d GetCOGPositionCOGModelspace() = 0;

		double c_t;
		double c_q;
		double w_m; //ローター角速度
		Eigen::Matrix3d Jr; //回転部慣性テンソル
		Eigen::Vector3d get_f(); //スラスト
		Eigen::Vector3d get_tau(); //トルク
		Eigen::Vector3d get_l(); //角運動量

		bool is_ccw; //回転方向の設定　初期設定は反時計回り
	};

	//シリンダー型のモーター/プロペラ
	class CylinderMotorPlop
		: public Cylinder
		, public MotorPlop
	{
		//モデルの重心位置を原点としたモデル空間におけるコンポーネントの重心位置
		Eigen::Vector3d GetCOGPositionCOGModelspace();
	};

	//モーター描画にSTLファイルを使用する
	class StLMotorPlop
		: public StLComponent
		, public MotorPlop
	{
	public:
		StLMotorPlop(double &_c_t, double &_c_q, Eigen::Matrix3d &_Jr);
		StLMotorPlop(
			double &_c_t, double &_c_q, Eigen::Matrix3d &_Jr
			, const std::string &p2stl, const Eigen::Matrix3d &_J
			, const Eigen::Vector3d &_c_o_g, const Eigen::Matrix3d &m_att);
		StLMotorPlop(
			const double &_c_t
			, const double &_c_q
			, const Eigen::Matrix3d &_Jr
			, const std::string & _path_to_stl
			, const double & _mass
			, const Eigen::Matrix3d & _J
			, const Eigen::Matrix3d & _m_attitude
			, const Eigen::Vector3d & _position_c_o_g_blockspace
			, const Eigen::Vector3d & _position_modelspace
			);
		StLMotorPlop(
			const double &_c_t
			, const double &_c_q
			, const Eigen::Matrix3d &_Jr
			, const std::string & _path_to_stl
			, const double & _mass
			, const Eigen::Matrix3d & _J
			, const Eigen::Matrix3d & _m_attitude
			, const Eigen::Vector3d & _position_c_o_g_blockspace
			, const Eigen::Vector3d & _position_modelspace
			, const Eigen::Vector3d & _color
			);
		//モデルの重心位置を原点としたモデル空間におけるコンポーネントの重心位置
		Eigen::Vector3d GetCOGPositionCOGModelspace();


	};

	class Core {
	public:

		Core();
		Core(
			const Eigen::Matrix3d &tj, const double &m, const double &dt,
			MotorPlop* mps, const unsigned int &nom,
			const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
			const Eigen::Vector3d &x_e0, const Eigen::Vector3d &phi_e0
			);

		Core(
			const Eigen::Matrix3d &tj, const double &m, const double &dt,
			const std::vector<MC::MotorPlop*> &mplps, const std::vector<MC::Block*> &blks,
			const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
			const Eigen::Vector3d &x_e0, const Eigen::Vector3d &phi_e0
			);

		Core(
			const Eigen::Matrix3d &tj, const double &m, const double &dt,
			const std::vector<MC::MotorPlop*> &mplps, const std::vector<MC::Block*> &blks,
			const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
			const Eigen::Vector3d &x_e0, const Eigen::Vector4d &q_0
			);
		
		Core(
			const Eigen::Matrix3d &tj, const double &m, const double &dt,
			const std::vector<MC::MotorPlop*> &mplps, const std::vector<MC::Block*> &blks,
			const Eigen::Vector3d &v_b0, const Eigen::Vector3d &w_b0,
			const Eigen::Vector3d &x_e0, const Eigen::Vector4d &q_0
			, const Eigen::Vector3d & _cg
			);

		//################################オイラー角############################################
		Vector12d x;
		Vector12d x_prev;
		Vector12d u;
		Matrix12d Z;
		Vector12d k1, k2, k3, k4;
		Eigen::Matrix3d GetAttitudeMatrix(); //回転行列の取得
		Vector12d get_state_vector(); //状態ベクトル
		Matrix12d get_state_matrix(); //状態行列
		void update(); //1ステップ前進積分　RK4
		//################################オイラー角############################################

		//################################クォータニオン############################################
		Vector13d xq;
		Vector13d xq_prev;
		Vector13d uq;
		Matrix13d Zq;
		Vector13d k1q, k2q, k3q, k4q;
		Eigen::Matrix3d GetAttitudeMatrix_q(); //回転行列の取得
		Vector13d get_state_vector_q(); //状態ベクトル
		Matrix13d get_state_matrix_q(); //状態行列
		void update_q(); //1ステップ前進積分　RK4
		//################################クォータニオン############################################

		double m;
		Eigen::Matrix3d J;

		double dt, dt2, dt6;			 //頻度

		MC::MotorPlop* motorplops; //モーター部配列先頭アドレス
		unsigned int n_o_m;      //モーター個数

		std::vector<MC::MotorPlop*> mtrplps; //モーター／プロペラモデル
		std::vector<MC::Block*> components; //機体構造材

		unsigned long long GetTime(); //現在の時間を得る

		Eigen::Vector3d GetEulerinDegrees();	//1-2-3系のDCMを得る
		Eigen::Vector3d GetEulerinDegreesIJK(const Eigen::Vector3d & _ijk); // i-j-k系のDCMを得る

		Eigen::Vector3d GetCGAssemblyspace();
		void SetCGAssemblyspace(const Eigen::Vector3d & _cg);

	private:
		Matrix12d mk_Z(const Vector12d &tx);  //状態ベクトルによりZを生成
		Eigen::Vector3d mk_u11(const Vector12d &tx); //状態ベクトルによりU11を生成
		Eigen::Vector3d mk_u12(); //U12生成
		Eigen::Vector3d mk_u2(); //U2生成
		Vector12d mk_u(const Vector12d &tx); //U生成 U = t[U11 + U12, U2, 0, 0]
		Eigen::Matrix3d mk_sk(const Eigen::Vector3d &v); //外積マトリクス
		Eigen::Matrix3d mk_B_mat(const Vector12d &tx, const Eigen::Matrix3d &Jb); //Bマトリックスの生成
		Eigen::Matrix3d mk_E_mat(const Vector12d &tx); //回転行列の生成
		Eigen::Matrix3d mk_D_mat(const Vector12d &tx); //角速度用回転行例角生成


		//################################クォータニオン############################################
		//クォータニオン関係
		//クォータニオンの正規化を忘れない

		Eigen::Matrix3d MakeDCMfromQuaternion(const Eigen::Vector4d & _q); //! クォータニオンからDCMを生成
													//!このDCMは座標系の変換を行う
		Eigen::Matrix4d MakeOmegafromW(const Eigen::Vector3d & _w);
		//! 角速度からクォータニオンの時間微分を生成するための行列を生成
		Eigen::Matrix3d mk_E_mat(const Vector13d & _x); //!状態ベクトルからDCMを生成
		Eigen::Matrix4d mk_Omega_2_mat(const Vector13d & _x); //!状態ベクトルから0.5 * Ωを生成
		Eigen::Matrix3d mk_B_mat(const Vector13d &_x, const Eigen::Matrix3d &Jb); //Bマトリックスの生成

		Matrix13d mk_Z(const Vector13d &_x);  //状態ベクトルによりZを生成
		Eigen::Vector3d mk_u11(const Vector13d &_x); //状態ベクトルによりU11を生成
		Vector13d mk_u(const Vector13d &_x); //U生成 U = t[U11 + U12, U2, 0, 0]

		Eigen::Vector3d GetVelocityBodyspace(const Vector13d & _x);
		Eigen::Vector3d GetAngularVelocityBodyspace(const Vector13d & _x);
		Eigen::Vector3d GetPositionEarthspace(const Vector13d & _x);
		Eigen::Vector4d GetQuaternion(const Vector13d & _x);

		void SetVelocityBodyspace(Vector13d & _x, const Eigen::Vector3d &_v);
		void SetAngularVelocityBodyspace(Vector13d & _x, const Eigen::Vector3d &_w);
		void SetPositionEarthspace(Vector13d & _x, const Eigen::Vector3d &_s);
		void SetQuaternion(Vector13d & _x, const Eigen::Vector4d &_q);

		//クォータニオンの正規化を行う
		void NormalizeQuaternion(Vector13d & _x);
		//################################クォータニオン############################################
		
		//################################Common########################

		//DCMを1-2-3系オイラー角に変換する
		Eigen::Vector3d ConvertDCMtoEuler123(const Eigen::Matrix3d & _dcm);
		Eigen::Vector3d ConvertDCMtoEuler123inDegrees(const Eigen::Matrix3d & _dcm);

		//DCMをi-j-k系オイラー角に変換する
		Eigen::Vector3d ConvertDCMtoEulerIJK(const Eigen::Matrix3d & _dcm
			, const Eigen::Vector3d & _ijk
			);
		Eigen::Vector3d ConvertDCMtoEulerIJKinDegrees(const Eigen::Matrix3d & _dcm
			, const Eigen::Vector3d & _ijk
			);

		//################################Common########################


		unsigned long long time_ms; //積算時間

		//アセンブリ空間における重心
		Eigen::Vector3d cg_assemblyspace;

	};


	class Generator {
	public:
		Generator();

		void set_initialstate_vb(const Eigen::Vector3d &tx);
		void set_initialstate_wb(const Eigen::Vector3d &tx);
		void set_initialstate_xe(const Eigen::Vector3d &tx);
		void set_initialstate_phie(const Eigen::Vector3d &tx);

		void SetInitialVelocityBodyspace(const Eigen::Vector3d & _v);
		void SetInitialAngularVelocityBodyspace(const Eigen::Vector3d & _w);
		void SetInitialPositionEarthspace(const Eigen::Vector3d & _p);
		void SetInitialQuotanion(const Eigen::Vector4d & _q);

		void SetDt(double dt);


		MC::Core generate_core();
		MC::Core GenerateCore_q();

		void Add(MC::Block * _blk);
		void operator<<(MC::Block * _block);

	private:

		Eigen::Vector3d vb0, wb0, xe0, phie0;
		Eigen::Vector4d q0;

		std::vector<MC::MotorPlop*> mtrplps;
		std::vector<MC::Block*> blks;

		std::vector<MC::StLComponent *> components_model;
		std::vector<MC::MotorPlop *> motor_plops;

		double dt;
	};





} // NAMESPACE MC

#endif // !_SIM_MC_








