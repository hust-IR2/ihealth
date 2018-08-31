#include "control_card.h"

#include <cmath>

using namespace std;

const double ControlCard::ResetVel = -4.0;
const double ControlCard::MaxVel = 5.0;
const double ControlCard::kElbowLimitInDegree = 50.0;
const double ControlCard::kShoulderLimitInDegree = 60.0;

ControlCard &ControlCard::GetInstance() {
	static ControlCard instance;
	return instance;
}

ControlCard::ControlCard()
	: total_axis_(0),
	board_id_(0),
	is_initialed_(0),
	axis_status_(MotorOff),
	clutch_status_(ClutchOff),
	at_shoulder_zero_(false),
	at_shoulder_limit_(false),
	at_elbow_zero_(false),
	at_elbow_limit_(false),
	emergency_stop_status_(true) {
}

ControlCard::~ControlCard() {}

bool ControlCard::IsInitial(void) {
	return is_initialed_;
}

int ControlCard::Initial() {
	I32 board_id_in_bits = 0;
	I32 mode = 0;

	if (NoError(APS_initial(&board_id_in_bits, mode))) {
		board_id_ = ValidBoardId(board_id_in_bits);
		if (board_id_ == InvalidBoardId) {
			is_initialed_ = false;
			return -1;
		}
		LoadAxisParam(board_id_);
		SetParamZero();
		APS_load_parameter_from_flash(board_id_);
		is_initialed_ = true;
	}
	return 0;
}

bool ControlCard::NoError(I32 error_code) {
	return error_code == ERR_NoError;
}

I32 ControlCard::ValidBoardId(I32 board_id_in_bits) {
	for (I32 id = 0; id < 16; id++) {
		int t = (board_id_in_bits >> id) & 1;
		if (t == 1) {
			I32 card_name = 0;
			APS_get_card_name(id, &card_name);
			if (card_name == DEVICE_NAME_PCI_8258 || card_name == DEVICE_NAME_AMP_82548) {
				return id;
			}
		}
	}
	return InvalidBoardId;
}

void ControlCard::LoadAxisParam(I32 board_id) {
	APS_get_first_axisId(board_id, &start_axis_id_, &total_axis_);
}

void ControlCard::SetClutch(bool onOroff) {
	bool do_ch[OutputChannels] = { 0 };
	I32 digital_output_value = 0;
	I32 returnCode = 0;
	do_ch[8] = onOroff;
	do_ch[9] = onOroff;
	for (int i = 0; i < OutputChannels; i++) {
		digital_output_value |= (do_ch[i] << i);
	}
	if (NoError(APS_write_d_output(board_id_, 0, digital_output_value))) {
		clutch_status_ = onOroff;
	}
	//sleep for clutch to react
	Sleep(500);
}

void ControlCard::SetParamZero() {
	for (I32 axis_id = 0; axis_id < total_axis_; axis_id++) {
		APS_set_command_f(axis_id, 0.0);//设置命令位置为0
		APS_set_position_f(axis_id, 0.0);//设置反馈位置为0
	}
}

void ControlCard::SetMotor(bool onOroff) {
	for (int id = ShoulderAxisId; id <= ElbowAxisId; ++id) {
		APS_set_axis_param_f(id, PRA_CURVE, 0.5);//Set acceleration rate
		APS_set_axis_param_f(id, PRA_ACC, 300000.0); //Set acceleration rate
		APS_set_axis_param_f(id, PRA_DEC, 300000.0); //Set deceleration rate
		APS_set_axis_param_f(id, PRA_STP_DEC, 10000.0);
		APS_set_servo_on(id, onOroff);
	}
	axis_status_ = onOroff;
}

void ControlCard::VelMove(short AxisId, double Vel) {
	double afterConvert = (fabs(Vel) / Unit_Convert);
	if (afterConvert > MaxVel / Unit_Convert) {
		afterConvert = MaxVel / Unit_Convert;
	}

	////如果电机没开，打开电机
	//if (!axis_status_)
	//	SetMotor(MotorOn);
	////如果离合器没开，打开离合器
	//if (!clutch_status_)
	//	SetClutch(ClutchOn);

	if (Vel > 0) {
		APS_vel(AxisId, 0, afterConvert, 0);
	} else {
		APS_vel(AxisId, 1, afterConvert, 0);
	}
}

void ControlCard::VelocityMove(I32 axis_id, double vel) {
	UpdateDigitInput();
	bool limit_switchs[2];
	SetLimitSwitchsByAxisId(axis_id, limit_switchs);
	if (OutOfWorkingArea(vel, limit_switchs)) {
		APS_stop_move(axis_id);
	} else {
		VelMove(axis_id, vel);
	}
}

void ControlCard::ProtectedVelocityMove(I32 axis_id, double vel) {
	UpdateDigitInput();
	bool limit_switchs[2];
	SetLimitSwitchsByAxisId(axis_id, limit_switchs);
	if (OutOfWorkingArea(vel, limit_switchs)) {
		APS_stop_move(axis_id);
	}
	else {
		//在这里要检查肩部和肘部的角度,angle[0] = shoulder, angle[1] = elbow
		double angle[2];
		GetEncoderData(angle);

		if (axis_id == ShoulderAxisId && ((angle[0] <= 0 && vel < 0) || (angle[0] >= kShoulderLimitInDegree && vel > 0))) {
			APS_stop_move(axis_id);
		}
		else if (axis_id == ElbowAxisId && ((angle[1] <= 0 && vel < 0) || (angle[1] >= kElbowLimitInDegree && vel > 0))) {
			APS_stop_move(axis_id);
		}
		else {
			VelMove(axis_id, vel);
		}
	}
}

bool ControlCard::OutOfWorkingArea(double vel, bool *limit_switches) {
	if ((!limit_switches[0]) && (!limit_switches[1])) {
		return false;
	} else if (limit_switches[0] && (vel > 0)) {
		return false;
	} else if (limit_switches[1] && (vel < 0)) {
		return false;
	}
	return true;
}

/*根据轴号给limit_switchs赋值*/
void ControlCard::SetLimitSwitchsByAxisId(I32 axis_id, bool *limit_switchs) {
	if (axis_id == ElbowAxisId) {
		limit_switchs[0] = at_elbow_zero_;
		limit_switchs[1] = at_elbow_limit_;
	} else if (axis_id == ShoulderAxisId) {
		limit_switchs[0] = at_shoulder_zero_;
		limit_switchs[1] = at_shoulder_limit_;
	}
}

/*获得光电开关和急停开关的值*/
void ControlCard::UpdateDigitInput() {
	I32 DI_Group = 0;
	I32 input = 0;
	bool di_ch[InputChannels];
	APS_read_d_input(0, DI_Group, &input);
	for (int i = 0; i < InputChannels; i++) {
		di_ch[i] = ((input >> i) & 1);
	}

	at_elbow_zero_ = di_ch[16];
	at_elbow_limit_ = di_ch[17];
	at_shoulder_zero_ = di_ch[18];
	at_shoulder_limit_ = di_ch[19];
	emergency_stop_status_ = di_ch[20];
}

void ControlCard::GetEncoderData(double EncoderData[2]) {
	int ret = 0;
	double raw_arm = 0;
	double  raw_shoulder = 0;
	ret = APS_get_position_f(ElbowAxisId, &raw_arm);
	ret = APS_get_position_f(ShoulderAxisId, &raw_shoulder);
	EncoderData[0] = raw_shoulder*Unit_Convert;
	EncoderData[1] = raw_arm*Unit_Convert;
}

void ControlCard::ResetPosition() {
	SetMotor(MotorOn);
	SetClutch(ClutchOn);
	UpdateDigitInput();
	while (!IsReset()) {
		VelocityMove(ElbowAxisId, ResetVel);
		VelocityMove(ShoulderAxisId, ResetVel);
	}
	SetMotor(MotorOff);
	SetClutch(ClutchOff);
	SetParamZero();
}

bool ControlCard::IsReset() {
	return at_elbow_zero_ && at_shoulder_zero_;
}

void ControlCard::Set_hWnd(HWND hWnd) {
	m_hWnd = hWnd;
}

bool ControlCard::AtShoulderZero() {
	UpdateDigitInput();
	return at_shoulder_zero_;
}

bool ControlCard::AtShoulderLimit() {
	UpdateDigitInput();
	return at_shoulder_limit_;
}

bool ControlCard::AtElbowZero() {
	UpdateDigitInput();
	return at_elbow_zero_;
}

bool ControlCard::AtElbowLimit() {
	UpdateDigitInput();
	return at_elbow_limit_;
}

bool ControlCard::IsEmergencyStop() {
	UpdateDigitInput();
	return emergency_stop_status_;
}

void ControlCard::GetDigitInput(bool *out) {
	UpdateDigitInput();
	out[0] = at_elbow_zero_;
	out[1] = at_elbow_limit_;
	out[2] = at_shoulder_zero_;
	out[3] = at_shoulder_limit_;
}

void ControlCard::GetJointVelocity(double *buffer) {
	double raw_arm_vel = 0;
	double  raw_shoulder_vel = 0;
	APS_get_feedback_velocity_f(ElbowAxisId, &raw_arm_vel);
	APS_get_position_f(ShoulderAxisId, &raw_shoulder_vel);
	buffer[0] = raw_shoulder_vel * Unit_Convert;
	buffer[1] = raw_arm_vel * Unit_Convert;
}

void ControlCard::Close() {
	APS_close();
}
