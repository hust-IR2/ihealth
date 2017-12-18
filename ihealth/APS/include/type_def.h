#ifndef TYPE_DEF
  #define TYPE_DEF

#include <windows.h>

typedef char		    I8;  
typedef unsigned char   U8;
typedef short           I16;
typedef unsigned short  U16;
typedef long            I32;
typedef unsigned long   U32;
typedef float           F32;
typedef double          F64;


typedef struct 
{
	I32 tick;
	I32 data[4]; //Total channel = 4
} STR_SAMP_DATA_4CH;

typedef struct 
{
	I32 tick;
	I32 data[8]; //Total channel = 8
} STR_SAMP_DATA_8CH;


typedef struct
{
	I16 i16_accType;	//Axis parameter
	I16 i16_decType;	//Axis parameter
	I32 i32_acc;		//Axis parameter
	I32 i32_dec;		//Axis parameter
	I32 i32_initSpeed;	//Axis parameter
	I32 i32_maxSpeed;	//Axis parameter
	I32 i32_endSpeed; 	//Axis parameter
} MOVE_PARA; //Speed pattern

typedef struct 
{
	I32 i32_pos;		// Position data (relative or absolute) (pulse)
	I16 i16_accType;	// Acceleration pattern 0: T-curve,  1: S-curve
	I16 i16_decType;	// Deceleration pattern 0: T-curve,  1: S-curve
	I32 i32_acc;		// Acceleration rate ( pulse / ss )
	I32 i32_dec;		// Deceleration rate ( pulse / ss )
	I32 i32_initSpeed;	// Start velocity	( pulse / s )
	I32 i32_maxSpeed;	// Maximum velocity  ( pulse / s )
	I32 i32_endSpeed; 	// End velocity		( pulse / s )
	I32 i32_angle;		// Arc move angle    ( degree, -360 ~ 360 )
	U32 u32_dwell;		// Dwell times       ( unit: ms )
	I32 i32_opt;    	// Option //0xABCD , D:0 absolute, 1:relative
} POINT_DATA;

typedef struct   
{ 
    I32 i32_pos;              //(Center)Position data (could be relative or absolute value) 
    I16 i16_accType;       //Acceleration pattern 0: T curve, 1:S curve   
    I16 i16_decType;       // Deceleration pattern 0: T curve, 1:S curve 
    I32 i32_acc;             //Acceleration rate ( pulse / sec 2 ) 
    I32 i32_dec;             //Deceleration rate ( pulse / sec 2  ) 
    I32 i32_initSpeed;     //Start velocity ( pulse / s ) 
    I32 i32_maxSpeed;  //Maximum velocity    ( pulse / s ) 
    I32 i32_endSpeed;   //End velocity  ( pulse / s )     
    I32 i32_angle;          //Arc move angle ( degree, -360 ~ 360 ) 
    U32 u32_dwell;         //dwell times ( unit: ms ) *Divided by system cycle time. 
    I32 i32_opt;              //Point move option. (*) 
    
    I32 i32_pitch;			// pitch for helical move
    I32 i32_totalheight;	// total hight
	I16 i16_cw;			// cw or ccw
    I16 i16_opt_ext;		// option extend
} POINT_DATA_EX; 

typedef struct 
{
	I32 i32_pos[16];	// Position data (relative or absolute) (pulse)
	I32 i32_initSpeed;	// Start velocity	( pulse / s ) 
	I32 i32_maxSpeed;	// Maximum velocity  ( pulse / s ) 
	I32 i32_angle;		// Arc move angle    ( degree, -360 ~ 360 ) 
	U32 u32_dwell;		// Dwell times       ( unit: ms ) 
	I32 i32_opt;    	// Option //0xABCD , D:0 absolute, 1:relative

} POINT_DATA2;

typedef struct
{
	I32 i32_pos[4];
	I32 i32_maxSpeed;
	I32 i32_endPos[2];
	I32 i32_dir;
	I32 i32_opt;
}POINT_DATA3;

typedef struct
{
	I16 i16_jogMode;	// Jog mode. 0:Free running mode, 1:Step mode
	I16 i16_dir;		// Jog direction. 0:positive, 1:negative direction
	I16 i16_accType;	// Acceleration pattern 0: T-curve,  1: S-curve
	I32 i32_acc;		// Acceleration rate ( pulse / ss )
	I32 i32_dec;		// Deceleration rate ( pulse / ss )
	I32 i32_maxSpeed;	// Positive value, maximum velocity  ( pulse / s )
	I32 i32_offset;		// Positive value, a step (pulse)
	I32 i32_delayTime;  // Delay time, ( range: 0 ~ 65535 millisecond, align by cycle time)
} JOG_DATA;

typedef struct
{
	U8 u8_homeMode;
	U8 u8_homeDir;
	U8 u8_curveType;

	I32 i32_orgOffset;
	I32 i32_acceleration;
	I32 i32_startVelocity;
	I32 i32_maxVelocity;
	I32 i32_OrgVelocity;
} HOME_PARA;

typedef struct _POS_DATA_2D
{
    U32 u32_opt;        // option, [0x00000000,0xFFFFFFFF]
    I32 i32_x;          // x-axis component (pulse), [-2147483648,2147484647]
    I32 i32_y;          // y-axis component (pulse), [-2147483648,2147484647]
    I32 i32_theta;      // x-y plane arc move angle (0.000001 degree), [-360000,360000]
}
POS_DATA_2D, *PPOS_DATA_2D;

typedef struct _PNT_DATA_2D
{
    U32 u32_opt;        // option, [0x00000000,0xFFFFFFFF]
    I32 i32_x;          // x-axis component (pulse), [-2147483648,2147484647]
    I32 i32_y;          // y-axis component (pulse), [-2147483648,2147484647]
    I32 i32_theta;      // x-y plane arc move angle (0.000001 degree), [-360000,360000]
    I32 i32_acc;        // acceleration rate (pulse/ss), [0,2147484647]
    I32 i32_dec;        // deceleration rate (pulse/ss), [0,2147484647]
    I32 i32_vi;         // initial velocity (pulse/s), [0,2147484647]
    I32 i32_vm;         // maximum velocity (pulse/s), [0,2147484647]
    I32 i32_ve;         // ending velocity (pulse/s), [0,2147484647]
}
PNT_DATA_2D, *PPNT_DATA_2D;

typedef struct _PNT_DATA_2D_F64
{
    U32 u32_opt;        // option, [0x00000000,0xFFFFFFFF]

    F64 f64_x;          // x-axis component (pulse), [-2147483648,2147484647]
    F64 f64_y;          // y-axis component (pulse), [-2147483648,2147484647]
    F64 f64_theta;      // x-y plane arc move angle (0.000001 degree), [-360000,360000]
    F64 f64_acc;        // acceleration rate (pulse/ss), [0,2147484647]
    F64 f64_dec;        // deceleration rate (pulse/ss), [0,2147484647]
    F64 f64_vi;         // initial velocity (pulse/s), [0,2147484647]
    F64 f64_vm;         // maximum velocity (pulse/s), [0,2147484647]
    F64 f64_ve;         // ending velocity (pulse/s), [0,2147484647]

	F64 f64_sf;			// s-factor [0.0 ~ 1.0]
}
PNT_DATA_2D_F64, *PPNT_DATA_2D_F64;

// Point table structure (Four dimension)
typedef struct _PNT_DATA_4DL
{
    U32 u32_opt;        // option, [0x00000000,0xFFFFFFFF]
    I32 i32_x;          // x-axis component (pulse), [-2147483648,2147484647]
    I32 i32_y;          // y-axis component (pulse), [-2147483648,2147484647]
    I32 i32_z;          // z-axis component (pulse), [-2147483648,2147484647]
    I32 i32_u;          // u-axis component (pulse), [-2147483648,2147484647]
    I32 i32_acc;        // acceleration rate (pulse/ss), [0,2147484647]
    I32 i32_dec;        // deceleration rate (pulse/ss), [0,2147484647]
    I32 i32_vi;         // initial velocity (pulse/s), [0,2147484647]
    I32 i32_vm;         // maximum velocity (pulse/s), [0,2147484647]
    I32 i32_ve;         // ending velocity (pulse/s), [0,2147484647]
}
PNT_DATA_4DL, *PPNT_DATA_4DL;


// Point table structure (One dimension)
typedef struct _PNT_DATA
{
    U32 u32_opt;        // option, [0x00000000,0xFFFFFFFF]
    I32 i32_x;          // x-axis component (pulse), [-2147483648,2147484647]
    I32 i32_theta;      // x-y plane arc move angle (0.001 degree), [-360000,360000]
    I32 i32_acc;        // acceleration rate (pulse/ss), [0,2147484647]
    I32 i32_dec;        // deceleration rate (pulse/ss), [0,2147484647]
    I32 i32_vi;         // initial velocity (pulse/s), [0,2147484647]
    I32 i32_vm;         // maximum velocity (pulse/s), [0,2147484647]
    I32 i32_ve;         // ending velocity (pulse/s), [0,2147484647]
}
PNT_DATA, *PPNT_DATA;

//Asynchronized call
typedef struct _ASYNCALL
{
	HANDLE	h_event; // 
    I32    		i32_ret;   // 
}
ASYNCALL, *PASYNCALL;

typedef struct _TSK_INFO
{
    U16 State;      	// 
    U16 RunTimeErr;     // 
    U16 IP;
    U16 SP;
    U16 BP;
	U16 MsgQueueSts;
}
TSK_INFO, *PTSK_INFO;

typedef struct _VAO_DATA
{
	//Param
	I32 outputType;	//Output type, [0, 3]
	I32 inputType;	//Input type, [0, 1]
	I32 config;			//PWM configuration according to output type
	I32 inputSrc;		//Input source by axis, [0, 0xf]

	//Mapping table
	I32 minVel;		//Minimum linear speed, [ positive ]
	I32 velInterval;	//Speed interval, [ positive ]
	I32 totalPoints;	//Total points, [1, 32]
	I32 mappingDataArr[32];	 //mapping data array
}
VAO_DATA, *PVAO_DATA;

#define MAX_SAMPL_CH	(8)
#define MAX_SAMPL_SRC	(2)

typedef struct _SAMP_PARAM
{
	I32 rate;							//Sampling rate
	I32 edge;							//Trigger edge
	I32 level;							//Trigger level
	I32 trigCh;							//Trigger channel
	I32 sourceByCh[MAX_SAMPL_CH][MAX_SAMPL_SRC];
	//Sampling source by channel, named sourceByCh[a][b], 
	//a: Channel
	//b: 0: Sampling source 1: Sampling axis
	//Sampling source: F64 data occupies two channel, I32 data occupies one channel.
} 
SAMP_PARAM, *PSAMP_PARAM;

#define MAX_PT_DIM			(6)

typedef struct
{
	I32 Dimension;
	I32 AxisArr[MAX_PT_DIM];

} PTINFO, *PPTINFO;

typedef struct 
{
	F64		DwTime; //Unit is ms

} PTDWL, *PPTDWL;

typedef struct 
{
	I32		Dim; 
	F64		Pos[MAX_PT_DIM];

} PTLINE, *PPTLINE;

#define MAXHEXLIX									( 0x0003 ) //Helix axes is 3.
#define MAXARC3										( 0x0003 ) //ARC3 axes is 3.
#define MAXARC2										( 0x0002 ) //ARC2 axes is 2.

typedef struct 
{
	U8		Index[MAXARC2]; //Index X,Y
	F64		Center[MAXARC2]; //Center Arr
	F64		Angle; //Angle

} PTA2CA, *PPTA2CA;

typedef struct 
{
	U8		Index[MAXARC2]; //Index X,Y
	F64		Center[MAXARC2]; // 
	F64		End[MAXARC2]; // 
	I16		Dir; //

} PTA2CE, *PPTA2CE;

typedef struct 
{
	U8		Index[MAXARC3]; //Index X,Y
	F64		Center[MAXARC3]; //Center Arr
	F64		Normal[MAXARC3]; //Normal Arr
	F64		Angle; //Angle

} PTA3CA, *PPTA3CA;

typedef struct 
{
	U8		Index[MAXARC3]; //Index X,Y
	F64		Center[MAXARC3]; //Center Arr
	F64		End[MAXARC3]; //End Arr
	I16		Dir; //

} PTA3CE, *PPTA3CE;

typedef struct 
{
	U8		Index[MAXHEXLIX]; //Index X,Y
	F64		Center[MAXHEXLIX]; //Center Arr
	F64		Normal[MAXHEXLIX]; //Normal Arr
	F64		Angle; //Angle
	F64		DeltaH;
	F64		FinalR;

} PTHCA, *PPTHCA;

typedef struct 
{
	U8		Index[MAXHEXLIX]; //Index X,Y
	F64		Center[MAXHEXLIX]; //Center Arr
	F64		Normal[MAXHEXLIX]; //Normal Arr
	F64		End[MAXHEXLIX]; //End Arr
	I16		Dir; //

} PTHCE, *PPTHCE;

typedef struct
{
	U16 BitSts;	//b0: Is PTB work? [1:working, 0:Stopped]
						//b1: Is point buffer full? [1:full, 0:not full]
						//b2: Is point buffer empty? [1:empty, 0:not empty]
						//b3, b4, b5: Reserved for future
						//b6~: Be always 0
	U16 PntBufFreeSpace; 
	U16 PntBufUsageSpace;
	U32 RunningCnt;

} PTSTS, *PPTSTS;

typedef struct
{
	U32 MotionLoopLoading;
	U32 HostLoopLoading;
	U32 MotionLoopLoadingMax;
	U32 HostLoopLoadingMax;

} LPSTS, *PLPSTS;

typedef struct
{
	U16 ServoOffCondition;	
	F64 DspCmdPos;		
	F64 DspFeedbackPos;		
	F64 FpgaCmdPos;		
	F64 FpgaFeedbackPos;		
	F64 FpgaOutputVoltage;		

} DEBUG_DATA;

typedef struct
{
	U16 AxisState;
	U16 GroupState;
	U16 AxisSuperState;

} DEBUG_STATE;


//New ADCNC structure define
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _POS_DATA_2D_F64		/* This structure extends original point data contents from "I32" to "F64" 
									   for internal computation. It's important to prevent data overflow. */
{
    U32 u32_opt;        // option, [0x00000000, 0xFFFFFFFF]
    F64 f64_x;          // x-axis component (pulse), [-9223372036854775808, 9223372036854775807]
    F64 f64_y;          // y-axis component (pulse), [-9223372036854775808, 9223372036854775807]
    F64 f64_theta;      // x-y plane arc move angle (0.000001 degree), [-360000, 360000]
}
POS_DATA_2D_F64, *PPOS_DATA_2D_F64;		// Added in 2013.1.22

typedef struct _POS_DATA_2D_RPS		/* This structure adds another variable to record what point was be saved */
{
    U32 u32_opt;        // option, [0x00000000, 0xFFFFFFFF]
 
	I32 i32_x;          // x-axis component (pulse), [-2147483648, 2147483647]
    I32 i32_y;          // y-axis component (pulse), [-2147483648, 2147483647]
    I32 i32_theta;      // x-y plane arc move angle (0.000001 degree), [-360000, 360000]

	U32 crpi;			// current reading point index
}
POS_DATA_2D_RPS, *PPOS_DATA_2D_RPS;		// Added in 2013.1.22



typedef struct _POS_DATA_2D_F64_RPS		/* This structure adds another variable to record what point was be saved */
{
    U32 u32_opt;        // option, [0x00000000, 0xFFFFFFFF]
 
	F64 f64_x;          // x-axis component (pulse), [-2147483648, 2147483647]
    F64 f64_y;          // y-axis component (pulse), [-2147483648, 2147483647]
    F64 f64_theta;      // x-y plane arc move angle (0.000001 degree), [-360000, 360000]

	U32 crpi;			// current reading point index
}
POS_DATA_2D_F64_RPS, *PPOS_DATA_2D_F64_RPS;		// Added in 2013.7.30


typedef struct _PNT_DATA_2D_EXT
{
    U32  u32_opt;        // option, [0x00000000,0xFFFFFFFF]
    F64  f64_x;          // x-axis component (pulse), [-2147483648,2147484647]
    F64  f64_y;          // y-axis component (pulse), [-2147483648,2147484647]
    F64  f64_theta;      // x-y plane arc move angle (0.000001 degree), [-360000,360000]
    
	F64  f64_acc[4];        // acceleration rate (pulse/ss), [0,2147484647]
    F64  f64_dec[4];        // deceleration rate (pulse/ss), [0,2147484647]

	I32  crossover;
	I32  Iboundary;		// initial boundary
    F64  f64_vi[4];      // initial velocity (pulse/s), [0,2147484647]
	U32  vi_cmpr;
    F64  f64_vm[4];      // maximum velocity (pulse/s), [0,2147484647]
	U32  vm_cmpr;
    F64  f64_ve[4];      // ending velocity (pulse/s), [0,2147484647]
	U32  ve_cmpr;
	I32  Eboundary;		// end boundary

	F64  f64_dist;		// point distance
	F64  f64_angle;		// path angle between previous & current point

	F64  f64_radius;		// point radiua (used in arc move)
	I32  i32_arcstate;
	U32  spt;			// speed profile type

	// unit time measured by DSP sampling period
	F64  t[4];

	// Horizontal & Vertical line flag
	I32  HorizontalFlag;
	I32  VerticalFlag;
}
PNT_DATA_2D_EXT, *PPNT_DATA_2D_EXT;	// Added in 2013.3.13
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
































#endif
