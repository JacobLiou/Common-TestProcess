#pragma once
#define   TEST_MODULE_COUNT 4
#define   MAX_PM_COUNT 128

enum TPTaskItem
{
	RefCheck = 1,
	Circulator1_2Check = 2,
	Circulator2_3Check = 3,
	Product_SlotCheck = 10,
	Product_OpticalCheck =11,

	PixelMappingDW = 101,
	MFGInfoDW = 102,
	DefaultParamDW = 103,


	T30CCycle = 200,
	T30CAttenCal = 201,
	T30CAttenFineCal = 202,
	T30CCenterWLFineCal = 203,
	T30CAtteLutOptimization = 204,
	T30CILCal = 205,
	T30CBeamOffsetCal = 206,
	T30CBeamRelationCal = 207,

	T70CCycle = 700 ,
	T70CAttenCal = 701,
	T70CAttenFineCal = 702,
	T70CCenterWLFineCal = 703,
	T70CAtteLutOptimization = 704,
	T70CILCal = 705,
	T70CBeamOffsetCal = 706,
	T70CBeamRelationCal = 707,

	TF5CCycle=300,
	TF5CAttenCal=301,
	TF5CAttenFineCal=302,
	TF5CCenterWLFineCal=303,
	TF5CAtteLutOptimization=304,
	TF5CILCal=305,
	TF5CBeamOffsetCal=306,
	TF5CBeamRelationCal=307,

	T5CCycle=400,
	T5CAttenCal=401,
	T5CAttenFineCal=402,
	T5CCenterWLFineCal=403,
	T5CAtteLutOptimization=404,
	T5CILCal=405,
	T5CBeamOffsetCal=406,
	T5CBeamRelationCal=407,

	T15CCycle=500,
	T15CAttenCal=501,
	T15CAttenFineCal=502,
	T15CCenterWLFineCal=503,
	T15CAtteLutOptimization=504,
	T15CILCal=505,
	T15CBeamOffsetCal=506,
	T15CBeamRelationCal=507,

	T50CCycle = 600,
	T50CAttenCal = 601,
	T50CAttenFineCal =602,
	T50CCenterWLFineCal=603,
	T50CAtteLutOptimization=604,
	T50CILCal=605,
	T50CBeamOffsetCal=606,
	T50CBeamRelationCal=607,



	CenterWLLutDW=800,
	AttenLutDW=801,
	AttenOffsetDW=802,
	ILLutDW=803,
	BeamLutDW=804,


	T40Cycle = 1100,
	T40AttenVerify = 1101,
	T40FlatnessCurveVerify = 1102,
	T40WLShiftVerify = 1103,
	T400VDACScanVerify = 1104,

	T70Cycle=900,
	T70AttenVerify=901,
	T70FlatnessCurveVerify=902,
	T70WLShiftVerify=903,
	T700VDACScanVerify=904,

	T55Cycle=1000,
	T55AttenVerify=1001,
	T55FlatnessCurveVerify=1002,
	T55WLShiftVerify=1003,
	T550VDACScanVerify=1004,

	TF5Cycle=1200,
	TF5AttenVerify=1201,
	TF5FlatnessCurveVerify=1202,
	TF5WLShiftVerify=1203,
	TF50VDACScanVerify=1204,

	T25Cycle = 1300,
	T25AttenVerify = 1301,
	T25FlatnessCurveVerify = 1302,
	T25WLShiftVerify = 1303,
	T250VDACScanVerify = 1304,

};

//循环箱控制循环的结构体
typedef struct tagTestTemp
{
	int    nSetType;       //定标、验证温度设置类型，0 - 代表初测 1 - 代表终测， 类型不同对应的炉子最小最大温度不同
	double dbSetTemp;      //设置温度
	double dbDiffer;       //稳定温度与炉子环境温度的差异
	double dbMaxTemp;      //循环箱设置最大的温度
	double dbMinTemp;      //循环箱设置最小的温度
	double dbTolerance;    //循环箱达到平衡的范围
	double dbTempSlope;     //循环箱温度随时间的斜率
	double dbTempIntercept; //循环箱温度随时间的截距
}stSettingTemp, *pSettingTemp;


typedef struct tagScanParam
{
	BOOL    bDoPDL;
	BOOL    bCirculator;
	BOOL    bDoRef;
	int     nPMCount;
	double  dblStartWL;
	double  dblStopWL;
	double 	dblStepSize;
	double  dbPMLevel;
	DWORD	dwSampleCount;
} stFSTPScanParam, *pFSTPScanParam;

typedef struct tagScanResult
{
	BOOL bDoPDL;
	int nPMCount;
	double *pdbWLArry;
	double *pdbTapResult;
	double *pdbPMResult[MAX_PM_COUNT];
	double *pdbPDLResult[MAX_PM_COUNT];
}stFSTPScanResult,*pFSTPScanResult;


