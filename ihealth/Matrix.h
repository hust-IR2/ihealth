#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>

#include <Eigen/Core>
#include <unsupported/Eigen/MatrixFunctions>


using namespace Eigen;
const double ShoulderWidth = 0.297;
const double ShoulderLength = 0.435;
const double UpperArmLength = 0.296;
const double LowerArmLength = 0.384;
const double InitAngle[5] = {
	16.86, 14.83, 4.48, 5.92, 2.96
};

//#define LEFT_ARM 1
#ifdef LEFT_ARM
//这里的AxisDirection就是定义了每个角速度方向上的单位矢量w
//这个就是根据我们的旋转怎么转决定的。
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
const Vector3d AxisDirection[5] = {
	//Vector3d(-1,0,0),
	//Vector3d(0,0,-1),
	//Vector3d(0,-1,0),
	//Vector3d(0,-1,0),
	//Vector3d(1,0,0)

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
#endif

//这个函数实际上就是在计算运动旋量ζ
template<typename DerivedA, typename DerivedB, typename DerivedC>
void CalcTwist(const MatrixBase<DerivedA>& axis, const MatrixBase<DerivedB>&pos, MatrixBase<DerivedC>& twist) {
	Matrix3d axis_hat;
	twist.setZero();
	axis_hat <<
		0, -axis(2), axis(1),
		axis(2), 0, -axis(0),
		-axis(1), axis(0), 0;
	//这里算出来的w_q就是运动w × q
	Vector3d w_q = axis.cross(pos);
	twist.block(0, 0, 3, 3) = axis_hat;
	twist.block(0, 3, 3, 1) = -w_q;
}

template<typename DerivedA, typename DerivedB,typename DerivedC>
void pinv(const MatrixBase<DerivedA>& A,const MatrixBase<DerivedB>&G, MatrixBase<DerivedC>& B)
{
	//这里就是在算这个投影矩阵p，最后的结果用B输出来，这里的A就是那个Γ，这里的G就是那个G
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

//这个函数的意思居然是求伴随矩阵，从4*4的转换到6*6的
template<typename DerivedA, typename DerivedB>
void Ad426(const MatrixBase<DerivedA>& X, MatrixBase<DerivedB>& A) {
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
void h2V(const MatrixBase<DerivedA>& X, MatrixBase<DerivedB>& b) {
	//这里出来的这个b就是旋转矢量ξ，只不过是因为我们之前的Bh是个4*4，这里又把它弄回来了而已
	//这里的b就是6*1的了
	b(0)=-X(1,2);
	b(1)=X(0,2);
	b(2)=-X(0,1);
	b.block(3,0,3,1)=X.block(0,3,3,1);
}

template<typename DerivedA, typename DerivedB>
void fwd_geo_coup(const MatrixBase<DerivedA>& U, MatrixBase<DerivedB>& theta) {
	MatrixXd meta(5,2);
	VectorXd thetab(5);

	//meta就是传动矩阵η
	meta <<1,0,
		0.88,0,
		0,1,
		0,1.3214,
		0,0.6607;

	thetab << InitAngle[0] , InitAngle[1] , InitAngle[2] , InitAngle[3] , InitAngle[4];
	//电机的角度就是初始角度加上转换后的角度
	theta=thetab+meta*U;
}
template<typename DerivedA, typename DerivedB,typename DerivedC>
void  damping_control(const MatrixBase<DerivedA>& Fh, MatrixBase<DerivedB>& U,MatrixBase<DerivedC>& Ub,double Fc,double a,double b)
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
	//这里的U是电机的输出角度2*1（我们可以通过获取当前角度直接获得），然后theta是5*1，分别是
	//5个关节的角度。下面这个函数就是把电机的角度转换为关节的角度。
	fwd_geo_coup(U,theta);
	

	con <<360.0/(2*M_PI),0,
		0,360.0/(2*M_PI);

	//这里的Bh和bb就是为了算J的每一列得到的
	Matrix4d Bh[5];
	Matrix<double, 6, 1> bb[5];
	for (size_t i = 0; i < 5; ++i) {
		//AxisDirection:3*3 AxisPosition:3*3 Bh:4*4
		//这里的Bh输出的是一个4*4的矩阵，在灿神的文本中这个叫做B_hat.然后h2V是把矩阵变成向量。
		//这里的bb[i]是一个6*1的向量
		CalcTwist(AxisDirection[i], AxisPosition[i], Bh[i]);
		h2V(Bh[i], bb[i]);
	}
	/*
	这里的m应该是ζhat，这一点我还不知道是什么东西。
	从这里看来，这里的计算只计算了四个关节的。
	也就是把θ5看为了0.这样就导致了最后一个直接就是关节旋量。
	*/
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
	//这里就是Γ = J * η。所以说这个meta就是η，Jb就是J
	Matrix<double, 6, 2> J = Jb*meta;
	pinv(J,G,p_X);//投影矩阵
	//Fc-导纳系数，Fh-六维力，co-六维力单位转换矩阵，con-六维速度单位转换矩阵（从弧度转为度），Ub-电机转速
	//这里p_X就是这个投影矩阵了，最后的Vd = ACF,这里Co就是这个C，然后Fc就是这个导纳系数。最后多乘个0.1应该是太
	//灵敏了的调整。然后最后要转换，把电机的速度变为角度，因为我们外面是用的角度。
	Ub=con*(p_X*Co*Fh*Fc*0.15);
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
