#include "contrlCard.h"
#include<math.h>
#include <iostream>
contrlCard::contrlCard()
{
	v_card_name = 0;
	v_board_id = 0;
	v_channel = 0;
	v_total_axis = 0;
	v_is_card_initialed = 0;
	Is_thread_creat = 0;
    AxisStatus=OFF;
    ClutchStatu=OFF;
}
contrlCard::~contrlCard()
{

}
bool contrlCard::IsCardInitial(void)
{
	if (v_is_card_initialed == 0) {
		return false;
	} else {
		return true;
	}
}

int contrlCard::Initial()
{
	I32 boardID_InBits = 0;
	I32 mode = 0;
	I32 ret = 0;
	I32 i = 0;
	I32 card_name = 0;
	I32 tamp = 0;
	I32 StartAxisID = 0;
	I32 TotalAxisNum = 0;
	// Card(Board) initial
	ret = APS_initial(&boardID_InBits, mode);
	if (ret == ERR_NoError)
	{
		for (i = 0; i < 16; i++)
		{
			tamp = (boardID_InBits >> i) & 1;

			if (tamp == 1)
			{
				ret = APS_get_card_name(i, &card_name);

				if (card_name == DEVICE_NAME_PCI_8258
					|| card_name == DEVICE_NAME_AMP_82548)
				{
					ret = APS_get_first_axisId(i, &StartAxisID, &TotalAxisNum);

					//----------------------------------------------------
					v_card_name = card_name;
					v_board_id = i;
					v_total_axis = TotalAxisNum;
					v_is_card_initialed = true;
					//电机轴参数归零
					SetParamZero();
					//从flash中load设置的参数
					APS_load_parameter_from_flash(0);

					if (v_total_axis == 4) v_channel = 2;
					else if (v_total_axis == 8) v_channel = 4;
					//----------------------------------------------------

					Is_thread_creat = false;
					break;
				}
			}
		}

		if (v_board_id == -1)
		{
			v_is_card_initialed = false;
		}
		return 1;
	}
	else
	{
		v_is_card_initialed = false;
		return 0;
	}
}
void contrlCard::Robot_Pos(bool Optoelc_Switch[2][2])
{
	I32 DI_Group = 0; // If DI channel less than 32
	I32 DI_Data = 0; // Di data
	I32 di_ch[__MAX_DI_CH];
	I32 returnCode = 0; // Function return code
	returnCode = APS_read_d_input(0, DI_Group, &DI_Data);
	for (int i = 0; i < __MAX_DI_CH; i++)
		di_ch[i] = ((DI_Data >> i) & 1);

    Optoelc_Switch[0][0]=di_ch[16];//0号电机ORG信号
    Optoelc_Switch[0][1]=di_ch[17];//0号电机MEL信号

    Optoelc_Switch[1][0]=di_ch[18];//1号电机ORG信号
    Optoelc_Switch[1][1]=di_ch[19];//1号电机MEL信号
}
//开关离合器 true-离合器闭合，false-离合器断开
void contrlCard::SetClutch(bool onOroff)
{
	bool do_ch[__MAX_DO_CH] = { 0 };
	I32 digital_output_value = 0;
	I32 returnCode = 0;

	do_ch[8] = onOroff;
	do_ch[9] = onOroff;
	for (int i = 0; i < __MAX_DO_CH; i++)
		digital_output_value |= (do_ch[i] << i);

	returnCode = APS_write_d_output(0
		, 0                     // I32 DO_Group
		, digital_output_value  // I32 DO_Data
		);
	if (returnCode == 0)
	{
		ClutchStatu = onOroff;
	}
}
//使得各电机参数归零
void contrlCard::SetParamZero()
{
	for (I32 Axis_ID = 0; Axis_ID < 4; Axis_ID++)
	{
		APS_set_command_f(Axis_ID, 0.0);//设置命令位置为0
		APS_set_position_f(Axis_ID, 0.0);//设置反馈位置为0
	}
}
void contrlCard::ServeTheMotor(bool onOroff )
{
    for(int i=shoudlerAxisId;i<=elbowAxisId;i++)
      {
            APS_set_axis_param_f( i, PRA_CURVE,      0.5 );//Set acceleration rate
            APS_set_axis_param_f( i, PRA_ACC,    300000.0 ); //Set acceleration rate
            APS_set_axis_param_f( i, PRA_DEC,    300000.0 ); //Set deceleration rate
            APS_set_axis_param_f( i, PRA_STP_DEC, 10000.0 );
            APS_set_servo_on(i, onOroff);
			//Sleep(500);
      }
    AxisStatus=onOroff;
}

void contrlCard::SetVelocityCmd(short AxisId, double Vel)
{	
    //单位转换
    double afterConvert=(fabs(Vel)/Unit_Convert);
	//速度保护
	if (afterConvert > 8 / Unit_Convert) {
		afterConvert = 8 / Unit_Convert;
	}
    //如果电机没开，打开电机
    if(!AxisStatus)
        ServeTheMotor(ON);
    //如果离合器没开，打开离合器
    if(!ClutchStatu)
        SetClutch(CON);

    if(Vel>0)
        APS_vel(AxisId,1,afterConvert,0);
    else
        APS_vel(AxisId,0,afterConvert,0);

}
bool contrlCard::getAixsServeStatu(short AxisId)
{
    I32  MotionIO=APS_motion_io_status(AxisId);
    bool statu=((MotionIO>>MIO_SVON)&1);
    return statu;
}
void contrlCard::MotionMove(short AxisId,double Vel,bool switchData[2])
{
    //正常工作区，直接运动
    if((!switchData[0])&&(!switchData[1]))
    {
        SetVelocityCmd(AxisId, Vel);
    }
    //下极限，正向有效，反向无效
    else if(switchData[0]){
        if(Vel>0)
            SetVelocityCmd(AxisId, Vel);
        else
            APS_stop_move(AxisId);
    }
    //上极限，反向有效，正向无效
    else if(switchData[1]){
        if(Vel>0)
            APS_stop_move(AxisId);
        else
            SetVelocityCmd(AxisId, Vel);
    }
}

void contrlCard::getEncoderData(double EncoderData[2])
{
	int ret = 0;
	double raw_arm = 0;
	double  raw_shoulder = 0;
	ret = APS_get_position_f(elbowAxisId, &raw_arm);
	ret = APS_get_position_f(shoudlerAxisId, &raw_shoulder);
	EncoderData[0] = raw_shoulder*Unit_Convert;
	EncoderData[1] = raw_arm*Unit_Convert;
}

void contrlCard::Set_hWnd(HWND hWnd) {
	m_hWnd = hWnd;
}

