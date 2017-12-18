#ifndef TYPE_DEF_PID
  #define TYPE_DEF_PID

#include "type_def.h"

//PID control
typedef struct
{
	U16 Kp;
	U16 Kd;
	U16 Ki;
	U16 Kvff;
	U16 Kaff;
}
__CTRL_PID, *__PCTRL_PID;

typedef struct
{
	U16 KpShift;
	U16 KdShift;
	U16 KiShift;
	U16 KvffShift;
	U16 KaffShift;
	U16 PidShift;
}
__CTRL_SHIFT, *__PCTRL_SHIFT;

#define _MAX_FILTER_NUM		(2) //Biquad have 2 filter

typedef struct
{
	I16 Divider[_MAX_FILTER_NUM];
	I16 A1[_MAX_FILTER_NUM];
	I16 A2[_MAX_FILTER_NUM];
	I16 B0[_MAX_FILTER_NUM];
	I16 B1[_MAX_FILTER_NUM];
	I16 B2[_MAX_FILTER_NUM];
}
__CTRL_BIQUAD, *__PCTRL_BIQUAD;

typedef struct
{
	I32 Pos;
	I32 Acc;
	I32 Vel;
}
__CTRL_CMDINFO, *__PCTRL_CMDINFO;

typedef struct
{
	I32 ErrPos;
	I32 ErrAccum;
	I32 LastErrPos;
	I32 DiffErrPos;
	I32 PidResult;
	I32 KpResult;
	I32 KiResult;
	I32 KdResult;
	I32 KaffResult;
	I32 KvffResult;
	I32 Biquad0Result;
	I32 Biquad1Result;
}
__CTRL_CALC, *__PCTRL_CALC;

typedef struct
{
	F64 Kp;
	F64 Ki;
	F64 Kd;
}
__SID_PID, *__PSID_PID;

typedef struct
{
	F64 Gain;
	F64 Phase;
	F64 Frequency;
}
__SID_FREQ, *__PSID_FREQ;

typedef struct
{
	F64 Frequency;
	F64 OpenLoopGain;
	F64 CloseLoopGain;
	F64 PlantGainPos;
	F64 PlantGainVel;
	F64 GainReserved;
	F64 OpenLoopPhase ;
	F64 CloseLoopPhase ;
	F64 PlantPhasePos;
	F64 PlantPhaseVel;
	F64 PhaseReserved;
}
__SID_FREQ_ALL, *__PSID_FREQ_ALL;

typedef struct
{
	F64 Voltage;
	F64 Frequency;
	F64 Deviation;
}
__ID_PARA, *__PID_PARA;

#endif
