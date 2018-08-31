#pragma once
// Adlink head file////////
#include <APS168.h>
#include <ErrorCodeDef.h>
#include <APS_define.h>
#include <type_def.h>

static const bool MotorOn = 1;
static const bool MotorOff = 0;
static const bool ClutchOn = 0;
static const bool ClutchOff = 1;
static const int OutputChannels = 24;
static const int InputChannels = 24;
static const int ShoulderAxisId = 0;
static const int ElbowAxisId = 1;
static const double Unit_Convert = 0.009;

class ControlCard {
public:
	static ControlCard& GetInstance();
	~ControlCard();

	bool IsInitial();
	int Initial();
	void SetClutch(bool onOroff = ClutchOn);
	void SetMotor(bool onOroff = MotorOn);
	void VelocityMove(I32 axis_id, double vel);

	// 限制极限位置的速度模式
	void ProtectedVelocityMove(I32 axis_id, double vel);
	//获取两电机当前的角度
	void GetEncoderData(double EncoderData[2]);
	void SetParamZero();
	void ResetPosition();
	void Set_hWnd(HWND hWnd);
	bool AtShoulderZero();
	bool AtShoulderLimit();
	bool AtElbowZero();
	bool AtElbowLimit();
	void GetDigitInput(bool *out);
	bool IsEmergencyStop();
	void GetJointVelocity(double *buffer);
	void Close();

private:
	ControlCard();
	void LoadAxisParam(I32 board_id);
	void VelMove(short AxisId, double Vel);
	void SetLimitSwitchsByAxisId(I32 axis_id, bool *limit_switchs);
	void UpdateDigitInput();
	bool IsReset();
	bool NoError(I32 error_code);
	bool OutOfWorkingArea(double vel, bool *limit_switches);
	I32 ValidBoardId(I32 board_id_in_bits);

private:
	static const I32 InvalidBoardId = -1;
	static const double ResetVel;
	static const double MaxVel;
	static const double kElbowLimitInDegree;
	static const double kShoulderLimitInDegree;

	HWND m_hWnd = NULL;
	bool axis_status_;
	bool clutch_status_;
	bool is_initialed_;
	bool at_shoulder_zero_;
	bool at_shoulder_limit_;
	bool at_elbow_zero_;
	bool at_elbow_limit_;
	bool emergency_stop_status_;

	I32 total_axis_;
	I32 start_axis_id_;
	I32 board_id_;
};