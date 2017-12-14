#pragma once
#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>
#include "math.h"
#include <iostream>
#include <fstream>
using namespace Eigen;
const double M_PI = 3.1415926;
const double ShoulderWidth = 0.297;
const double ShoulderLength = 0.435;
const double UpperArmLength = 0.296;
const double LowerArmLength = 0.384;
const double InitAngle[5] = {
	16.86, 14.83, 4.48, 5.92, 2.96
};

#define LEFT_ARM 1
#ifdef LEFT_ARM
const Vector3d AxisDirection[5] = {
	Vector3d(1.0,0,0),
	Vector3d(0,0,1.0),
	Vector3d(0,-1.0,0),
	Vector3d(0,-1.0,0),
	Vector3d(-1.0,0,0)
};
const Vector3d AxisPosition[5] = {
	Vector3d(-UpperArmLength - LowerArmLength,0,0),
	Vector3d(-UpperArmLength - LowerArmLength,0,0),
	Vector3d(-UpperArmLength - LowerArmLength,0,0),
	Vector3d(-LowerArmLength,0,0),
	Vector3d(-LowerArmLength,0,0)
};
#else
const Vector3d Axis[5] = {
	Vector3d(-1,0,0),
	Vector3d(0,0,-1),
	Vector3d(0,-1,0),
	Vector3d(0,-1,0),
	Vector3d(1,0,0)
};
const Vector3d AxisPosition[5] = {
	Vector3d(-UpperArmLength - LowerArmLength,0,0),
	Vector3d(-UpperArmLength - LowerArmLength,0,0),
	Vector3d(-UpperArmLength - LowerArmLength,0,0),
	Vector3d(-LowerArmLength,0,0),
	Vector3d(-LowerArmLength,0,0)
};
#endif

template<typename DerivedA, typename DerivedB, typename DerivedC>
void CalcTwist(const MatrixBase<DerivedA>& axis, const MatrixBase<DerivedB>&pos, MatrixBase<DerivedC>& twist) {
	Matrix3d axis_hat;
	twist.setZero();
	axis_hat <<
		0, -axis(2), axis(1),
		axis(2), 0, -axis(0),
		-axis(1), axis(0), 0;
	Vector3d w_q = axis.cross(pos);
	twist.block(0, 0, 3, 3) = axis_hat;
	twist.block(0, 3, 3, 1) = -w_q;
}

template<typename DerivedA, typename DerivedB,typename DerivedC>
void pinv(const MatrixBase<DerivedA>& A,const MatrixBase<DerivedB>&G, MatrixBase<DerivedC>& B)
{
	MatrixXd A_temp(2,2);
	A_temp=A.transpose()*G*A;
	B=(A_temp.inverse())*A.transpose()*G;

}
template<typename DerivedA, typename DerivedB>
void V2h(const MatrixBase<DerivedA>& X, MatrixBase<DerivedB>& Y)
{
	MatrixXd y(3,3);
    y.setZero();
	y(1,2)=-X(0);
	y(0,2)=X(1);
	y(0,1)=-X(2);
	Y=y-y.transpose();	
}
template<typename DerivedA, typename DerivedB>
void Ad426(const MatrixBase<DerivedA>& X, MatrixBase<DerivedB>& A)
{
	MatrixXd Y(3,3);
	Vector3d h(3);
	A.setZero();
	A.block(0, 0, 3, 3) = X.block(0, 0, 3, 3);
	A.block(3,3,3,3)=X.block(0,0,3,3);
	h=X.block(0,3,3,1);
	V2h(h,Y);
	A.block(3,0,3,3)=Y*X.block(0,0,3,3);
}
template<typename DerivedA, typename DerivedB>
void h2V(const MatrixBase<DerivedA>& X, MatrixBase<DerivedB>& b)
{
	b(0)=-X(1,2);
	b(1)=X(0,2);
	b(2)=-X(0,1);
	b.block(3,0,3,1)=X.block(0,3,3,1);
}
template<typename DerivedA, typename DerivedB>
void fwd_geo_coup(const MatrixBase<DerivedA>& U, MatrixBase<DerivedB>& theta)
{
	MatrixXd meta(5,2);
	VectorXd thetab(5);
	meta <<1,0,
		0.88,0,
		0,1,
		0,1.3214,
		0,0.6607;

	thetab << InitAngle[0] , InitAngle[1] , InitAngle[2] , InitAngle[3] , InitAngle[4];
	theta=thetab+meta*U;
}
template<typename DerivedA, typename DerivedB,typename DerivedC>
void damping_control(const MatrixBase<DerivedA>& Fh, MatrixBase<DerivedB>& U,MatrixBase<DerivedC>& Ub,double Fc,double a,double b)
{

	VectorXd theta(5);
	VectorXd theta_PI(5);

	MatrixXd con(2,2);


	VectorXd d(5);
	VectorXd diag(6);
	
	MatrixXd Jb(6,5);
	MatrixXd p_X(2,6);
	MatrixXd Co(2,2);
	VectorXd Co_tem(6);
	MatrixXd G(6,6);

	fwd_geo_coup(U,theta);

	con <<360.0/(2*M_PI),0,
		0,360.0/(2*M_PI);

	Matrix4d Bh[5];
	Matrix<double, 6, 1> bb[5];
	for (size_t i = 0; i < 5; ++i) {
		CalcTwist(AxisDirection[i], AxisPosition[i], Bh[i]);
		h2V(Bh[i], bb[i]);
	}
	Matrix4d m[4];
	for (size_t i = 0; i < 4; ++i) {
		m[i] = -Bh[i+1] * (2 * M_PI / 360.0)*theta(i+1);
	}
	Matrix4d exp_m[4];
	exp_m[3]=m[3].exp();
	exp_m[2]=exp_m[3]*(m[2].exp());
	exp_m[1]=exp_m[2]*(m[1].exp());
	exp_m[0]=exp_m[1]*(m[0].exp());
	Matrix<double, 6, 6> A[4];
	Ad426(exp_m[0],A[0]);
	Ad426(exp_m[1],A[1]);
	Ad426(exp_m[2],A[2]);
	Ad426(exp_m[3],A[3]);
	
	Jb.block(0, 0, 6, 1) = A[0] * bb[0];
	Jb.block(0, 1, 6, 1) = A[1] * bb[1];
	Jb.block(0, 2, 6, 1) = A[2] * bb[2];
	Jb.block(0, 3, 6, 1) = A[3] * bb[3];
	Jb.block(0, 4, 6, 1) = bb[4];
	diag << a, a, a, b, b, b;//度量矩阵
	Co_tem << 20, 20, 20, 1, 1, 1;//
	G=diag.asDiagonal();
	Co=Co_tem.asDiagonal();

	MatrixXd meta(5, 2);
	meta << 1, 0,
		0.88, 0,
		0, 1,
		0, 1.3214,
		0, 0.6607;
	Matrix<double, 6, 2> J = Jb*meta;
	pinv(J,G,p_X);//投影矩阵
	//Fc-导纳系数，Fh-六维力，co-六维力单位转换矩阵，con-六维速度单位转换矩阵（从弧度转为度），Ub-电机转速
	Ub=con*(p_X*Co*Fh*Fc*0.1);
}
template<typename DerivedA, typename DerivedB>
void fwd_geo_kineB(const MatrixBase<DerivedA>& theta, MatrixBase<DerivedB>& T0h)
{
	Matrix4d exp_m[5];
	Matrix4d Bh[5];
	for (size_t i = 0; i < 5; ++i) {
		CalcTwist(AxisDirection[i], AxisPosition[i], Bh[i]);
		exp_m[i] = Bh[i] * (2 * M_PI / 360)*theta(i);
	}
	Matrix4d T0h0;
	T0h0 <<
		1, 0, 0, ShoulderLength + UpperArmLength + LowerArmLength,
		0, 1, 0, 0,
		0, 0, 1, ShoulderWidth,
		0, 0, 0, 1;
	T0h = T0h0*(exp_m[0].exp())*(exp_m[1].exp())*(exp_m[2].exp())*(exp_m[3].exp())*(exp_m[4].exp());
}
template<typename DerivedA>
void Euler2RotMat(double IMU_yaw, double IMU_pitch, double IMU_roll, MatrixBase<DerivedA>& R)
{
	IMU_yaw = IMU_yaw * 2 * M_PI / 360;
	IMU_pitch = IMU_pitch * 2 * M_PI / 360;
	IMU_roll = IMU_roll * 2 * M_PI / 360;
	R(0, 0) = cos(IMU_yaw)*cos(IMU_pitch);
	R(0, 1) = cos(IMU_yaw)*sin(IMU_pitch)*sin(IMU_roll) - sin(IMU_yaw)*cos(IMU_roll);
	R(0, 2) = cos(IMU_yaw)*sin(IMU_pitch)*cos(IMU_roll) + sin(IMU_yaw)*sin(IMU_roll);
	R(1, 0) = sin(IMU_yaw)*cos(IMU_pitch);
	R(1, 1) = sin(IMU_yaw)*sin(IMU_pitch)*sin(IMU_roll) + cos(IMU_yaw)*cos(IMU_roll);
	R(1, 2) = sin(IMU_yaw)*sin(IMU_pitch)*cos(IMU_roll) - cos(IMU_yaw)*sin(IMU_roll);
	R(2, 0) = -sin(IMU_pitch);
	R(2, 1) = cos(IMU_pitch)*sin(IMU_roll);
	R(2, 2) = cos(IMU_pitch)*cos(IMU_roll);
}

template<typename DerivedA, typename DerivedB, typename DerivedC>
void handle_ori(const MatrixBase<DerivedA>& Rw0, MatrixBase<DerivedB>& Rwh, MatrixBase<DerivedC>& R0h)
{
	MatrixXd mat_convert(3,3);
	MatrixXd R0h_temp(3,3);
	mat_convert<<1,0,0,
		0,0,1,
		0,-1,0;
	R0h_temp = (Rw0.transpose())*Rwh;
	R0h=R0h_temp*mat_convert;
}
template<typename DerivedA, typename DerivedB, typename DerivedC, typename DerivedD>
void Cal_phg(const MatrixBase<DerivedA>& R0h, MatrixBase<DerivedB>& t, MatrixBase<DerivedC>& f, MatrixBase<DerivedD>& p)
{
	MatrixXd Rhs(3,3);
	MatrixXd A(3, 3);
	MatrixXd A_temp(3, 3);
	MatrixXd A_tem(3, 3);
	MatrixXd A_te(3, 3);
	MatrixXd A_t(3, 3);
	MatrixXd N(3, 3);
	double m = 1;
	Vector3d phs(3);
	Vector3d g(3);
	Vector3d b(3);
	VectorXd Co_tem(3);

	Co_tem << 1,1,1;
	phs << -0.0447, -0.1055, 0;
	Rhs = Co_tem.asDiagonal();
	V2h(-R0h.transpose()*m*g, A);
	V2h(phs, N);
	b = Rhs*t + N*Rhs*f;
	/*A_temp = A.transpose()*A;
	A_tem = A_temp.inverse();
	A_te = A.transpose();
	A_t = A_tem*A_te;*/
	p = b;
}  
template<typename DerivedA, typename DerivedB, typename DerivedC>
void Cal_CoAdFg(const MatrixBase<DerivedA>& phg, MatrixBase<DerivedB>& R0h, MatrixBase<DerivedC>&CoAdFg)
{
	
	
	Vector3d h(3);
	Vector3d n(3);
	MatrixXd v2(3, 3);
	g << 9.8, 0, 0;
	CoAdFg<<0,0,0,0,0,0;
	V2h(phg, v2);
	h = v2*R0h.transpose()*m*g;
	n = R0h.transpose()*m*g;
	CoAdFg.head(3) = h;
	CoAdFg.tail(3) = n;
}
template<typename DerivedA, typename DerivedB>
void fwd(const MatrixBase<DerivedA>& theta_Pi,MatrixBase<DerivedB>& P0)
{
	double offset=0.08;
	VectorXd d(5);
	VectorXd theta(5);
    d<<0.180,0.434,0.298,0.2706+31/1000,0.0614;
	double d1=0;
	double d2=0;
	double p[3]={0};
	d1=d(2);
	d2=d(3);
	theta=theta_Pi*M_PI/180;
    P0(0)=d1*cos(theta(1))*cos(theta(2)) - d2*(cos(theta(1))*sin(theta(2))*sin(theta(2)) - cos(theta(1))*cos(theta(2))*cos(theta(3))) + 217.0/500.0;
    P0(1)=d1*(sin(theta(0))*sin(theta(2)) - cos(theta(0))*cos(theta(2))*sin(theta(1))) + d2*(cos(theta(3))*(sin(theta(0))*sin(theta(2)) - cos(theta(0))*cos(theta(2))*sin(theta(1))) + sin(theta(2))*(cos(theta(2))*sin(theta(0)) +		cos(theta(0))*sin(theta(1))*sin(theta(2))));
    P0(2)=d1*(cos(theta(0))*sin(theta(2)) + cos(theta(2))*sin(theta(0))*sin(theta(1))) + d2*(cos(theta(3))*(cos(theta(0))*sin(theta(2)) + cos(theta(2))*sin(theta(0))*sin(theta(1))) + sin(theta(2))*(cos(theta(0))*cos(theta(2)) -		 sin(theta(0))*sin(theta(1))*sin(theta(2)))) + 9.0/50.0;

}
