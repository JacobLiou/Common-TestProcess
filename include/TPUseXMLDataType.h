#ifndef	_MIMS_XMLDATATYPE_INCLUDED_
#define _MIMS_XMLDATATYPE_INCLUDED_
//// HFileV   1.1.0.5
/// DLL File Version 1.1.0.5
// 2021/9/30 1.1.0.5
/////XML 解析库导出函数原型
///BOOL DllParseXMLStr(BSTR bstrInString , PVOID POutData)
///BOOL DllCreateGraphDataXMLStr(PVOID pInData ,double* pdbGraphX,double* pdbGraphY,int nDataCount, BSTR* pbstrGraphOutString)
///BOOL DllCreateXMLStr(PVOID pInData , BSTR* pbstrOutString)
///BOOL DllGetLastErrorStr(BSTR* pbstrErrString)
///
/// 20210930 增加如下功能
/// 1，增加base info 节点数量扩展产品信息量
/// 2，增加OPLINK 员工信息查询接口
/// 3，增加人员技能校验接口
/// 4，增加测试数据存放目录接口
/// 5，增加FW 标准发布路径信息获取
/////消息命令类型 v1.3.0.3
////// 20230518 增加功能如下// 
// 1，增加位置信息（工厂位置）
// 2，增加通用功能信息接口
//////////////////
// V1.3.0.4
//////20230905
// 扩展通用接口内存长度
#define  MSG_COMMAND_RUNTASK				1
#define  MSG_COMMAND_RUNDEVICE				2
#define  MSG_COMMAND_STOPRUN				3
#define  MSG_COMMAND_DEBUGMODE				4
#define  MSG_COMMAND_RUNDONE				5
#define	 MSG_COMMAND_RUNCOMMAND				6
#define	 MSG_COMMAND_RUNINIT				7
#define  MSG_COMMAND_QUIT					8

#define	 MSG_MSG_BASEINFO					11
#define	 MSG_MSG_TASKINFO					12
#define	 MSG_MSG_OLDRATE					13
#define	 MSG_MSG_TASKDESCRIPTION			14
#define	 MSG_MSG_MSGBOX						15
#define	 MSG_MSG_LOG						16
#define	 MSG_MSG_COMMANDINFO				17
#define	 MSG_MSG_CONFIGPATH					30
#define  MSG_MSG_TEMPLATE					31
#define  MSG_MSG_TASKLIST					32
#define		MSG_MSG_UPLOADTESTDATA				33

#define		MSG_MSG_CONVPN						34
#define		MSG_MSG_CONVPROCESS					35
#define		MSG_MSG_STATIONINFO					36
#define		MSG_MSG_TESTRESULTOPC				37
#define		MSG_MSG_MOVEIN						38
#define		MSG_MSG_MOVEOUT						39
#define		MSG_MSG_USERLOGINVERIFY				40
#define		MSG_MSG_STATIONVERIFY				41

#define		MSG_MSG_OPLINKUSERINFO				42
#define		MSG_MSG_USERSKILLVERIFY				43
#define		MSG_MSG_CREATESNDIR					44
#define		MSG_MSG_FWSTANDARDPATH				45

#define		MSG_MSG_LOCATIONINFO				46
#define		MSG_MSG_GENERICFUNC					47

#define	 MSG_TESTDATA_PROCESS				21
#define	 MSG_TESTDATA_PROGRESS				22
#define	 MSG_TESTDATA_GRAPHINIT				23
#define	 MSG_TESTDATA_GRAPHDATAHEAD				24







////// 消息对应项的长度/////////////////////////////
//////  command///////////////////////////////
#define	COMMAND_RUNINIT_USER_LEN			10*2
#define	COMMAND_RUNINIT_SN_LEN				20*8
#define	COMMAND_RUNINIT_PN_LEN				128*2
//////  COMMAND///////////////////////////////
#define	COMMAND_RUNTASK_INDEX_LEN			10*2
#define	COMMAND_RUNTASK_USER_LEN			20*2
#define	COMMAND_RUNTASK_TITLE_LEN			128*2

//////  COMMAND///////////////////////////////
#define	COMMAND_RUNCOMMAND_INDEX_LEN			10*2
#define	COMMAND_RUNCOMMAND_USER_LEN			20*2
#define	COMMAND_RUNCOMMAND_DATA_LEN			2048*2

/// 任务执行结果反馈
#define	COMMAND_RUNDONE_INDEX_LEN			10*2
#define	COMMAND_RUNDONE_USER_LEN			20*2
#define	COMMAND_RUNDONE_TITLE_LEN			10*2


#define	COMMAND_RUNDEVICE_INDEX_LEN			10*2
#define	COMMAND_RUNDEVICE_USER_LEN			20*2
#define	COMMAND_RUNDEVICE_TITLE_LEN			128*2

#define	COMMAND_STOPRUN_USER_LEN			20*2


#define	COMMAND_DEBUGMODE_USER_LEN			20*2
#define	COMMAND_DEBUGMODE_PASSWORD_LEN			20*2
#define	COMMAND_DEBUGMODE_DEBUG_LEN			128*2

///////msg//////////////////////////////////////////////
///baseinfo
#define MSG_BASEINFO_OPERATE_LEN			10*2
#define	MSG_BASEINFO_PROCESSTYPE_LEN		20*2
#define MSG_BASEINFO_AUTHOR_LEN				20*2
#define MSG_BASEINFO_VERSION_LEN			15*2
#define MSG_BASEINFO_SPEC_LEN				15*2
#define MSG_BASEINFO_PNNUMBER_LEN			30*2
#define MSG_BASEINFO_DATE_LEN				30*2
#define	MSG_CONFIGPATH_DIR_LEN				512*2
// add 20210928
#define	MSG_BASEINFO_TESTSPEC_LEN		10*2
#define MSG_BASEINFO_TESTMODE_LEN		10*2
#define MSG_BASEINFO_WORKORDER_LEN		30*2
#define MSG_BASEINFO_INPROCESS_LEN		10*2
#define MSG_BASEINFO_INHOLD_LEN			10*2
#define MSG_BASEINFO_INREWORK_LEN		10*2
#define MSG_BASEINFO_PRODFAMILY_LEN		20*2
#define MSG_BASEINFO_ALIASNAME_LEN		512*2
#define MSG_BASEINFO_MTOPN_LEN			30*2
#define MSG_BASEINFO_OPLINKPN_LEN		30*2

///task info
#define MSG_TASKINFO_OPERATE_LEN			10*2
#define MSG_TASKINFO_INDEX_LEN				2048*200
#define MSG_TASKINFO_TITLE_LEN				2048*200
///command info
#define MSG_COMMANDINFO_OPERATE_LEN			10*2
#define MSG_COMMANDINFO_INDEX_LEN				10*2
#define	MSG_COMMANDINFO_TITLE_LEN				256*2
#define MSG_COMMANDINFO_EXAMPLE_LEN				2048*2
////old rate
#define MSG_OLDRATE_OPERATE_LEN				10*2
#define MSG_OLDRATE_INDEX_LEN				2048*2
#define MSG_OLDRATE_RATE_LEN				512*2
///task description
#define MSG_TASKDESCRIPTION_OPERATE_LEN			10*2
#define MSG_TASKDESCRIPTION_INDEX_LEN				10*2
#define MSG_TASKDESCRIPTION_DESCRIPTION_LEN				2048*2
///// msg box
#define MSG_MSGBOX_OPERATE_LEN					10*2
#define MSG_MSGBOX_MSGID_LEN					10*2
#define MSG_MSGBOX_MSGTITLE_LEN					128*2
#define MSG_MSGBOX_DESCRIPTION_LEN				256*2
#define MSG_MSGBOX_BUTTONID_LEN					128*2
#define MSG_MSGBOX_BUTTONTITLE_LEN				512*2

//MSG CONVPN
#define MSG_CONVPN_ORIGINPN_LEN						30*2
#define MSG_CONVPN_CONVTYPE_LEN						10*2
#define MSG_CONVPN_RESULT_LEN						10*2
#define MSG_CONVPN_RESULTPN_LEN						30*2
#define MSG_CONVPN_NOTEMSG_LEN						2048*2

//MSG CONVPROCESS
#define MSG_CONVPROCESS_PRODPN_LEN					30*2
#define MSG_CONVPROCESS_ORIGINPROCESS_LEN			30*2
#define MSG_CONVPROCESS_CONVTYPE_LEN				10*2
#define MSG_CONVPROCESS_RESULT_LEN					10*2
#define MSG_CONVPROCESS_RESULTPROCESS_LEN			30*2
#define MSG_CONVPROCESS_NOTEMSG_LEN					2048*2

// MSG station info
#define MSG_STATIONINFO_PCNAME_LEN					30*2
#define MSG_STATIONINFO_RESULT_LEN					10*2
#define MSG_STATIONINFO_STATIONID_LEN				30*2
#define MSG_STATIONINFO_STATIONNAME_LEN				60*2
#define MSG_STATIONINFO_NOTEMSG_LEN					2048*2

//MSG RESULT TO OPC
#define MSG_RESULTTOOPC_SN_LEN						30*2
#define MSG_RESULTTOOPC_RESULT_LEN						10*2
#define MSG_RESULTTOOPC_NOTEMSG_LEN					2048*2

// msg MOVE OUT
#define MSG_MOVEOUT_SN_LEN							30*2
#define MSG_MOVEOUT_RESULT_LEN						10*2
#define MSG_MOVEOUT_NOTEMSG_LEN						2048*2

//MSG MOVE IN
#define MSG_MOVEIN_SN_LEN							30*2
#define MSG_MOVEIN_RESULT_LEN						10*2
#define MSG_MOVEIN_NOTEMSG_LEN						2048*2
// MSG LOGIN VERIFY
#define MSG_LOGINVERIFY_USERNAME_LEN				30*2
#define MSG_LOGINVERIFY_PASSWORD_LEN				10*2
#define MSG_LOGINVERIFY_LOGINMODE_LEN				10*2
#define MSG_LOGINVERIFY_RESULT_LEN					10*2
#define MSG_LOGINVERIFY_NOTMSG_LEN					2048*2

// MSG STATION VERIFY
#define MSG_STATIONVERIFY_STATIONID_LEN				30*2
#define MSG_STATIONVERIFY_USERID_LEN				30*2
#define MSG_STATIONVERIFY_TYPE_LEN					10*2
#define MSG_STATIONVERIFY_RESULT_LEN				10*2
#define MSG_STATIONVERIFY_NOTMSG_LEN				2048*2

// add 20210909 OplinkUserInfo

#define MSG_OPLKUSERINFO_OPERATE_LEN				10*2
#define MSG_OPLKUSERINFO_OPLINKID_LEN				10*2
#define MSG_OPLKUSERINFO_USERMAME_LEN				128*2
#define MSG_OPLKUSERINFO_DPTNAME_LEN				256*2
#define MSG_OPLKUSERINFO_POSNAME_LEN				128*2
#define MSG_OPLKUSERINFO_OPCID_LEN					10*2
#define MSG_OPLKUSERINFO_RESULT_LEN					10*2
#define MSG_OPLKUSERINFO_NOTEMSG_LEN				2048*2

// add 20210929 UserSkillVerify
#define MSG_USERSKILLVERIFY_OPERATE_LEN				10*2
#define MSG_USERSKILLVERIFY_OPCID_LEN				10*2
#define MSG_USERSKILLVERIFY_PROCESSTYPE_LEN			30*2
#define MSG_USERSKILLVERIFY_RESULT_LEN				10*2
#define MSG_USERSKILLVERIFY_NOTEMSG_LEN				2048*2

// add 20210929 CreateSNDir

#define	MSG_CREATEDSNDIR_OPERATE_LEN				10*2
#define	MSG_CREATEDSNDIR_SN_LEN						20*2
#define	MSG_CREATEDSNDIR_ROOTDIR_LEN				1024*2
#define	MSG_CREATEDSNDIR_RESULT_LEN					10*2
#define	MSG_CREATEDSNDIR_NOTEMSG_LEN				2048*2

// add 20210929 fwinfo
#define MSG_FWSTANDARDPATH_OPERATE_LEN						10*2
#define MSG_FWSTANDARDPATH_DUTPN_LEN						20*2
#define MSG_FWSTANDARDPATH_RESULT_LEN						10*2
#define MSG_FWSTANDARDPATH_FWPN_LEN							20*2
#define MSG_FWSTANDARDPATH_NOTEMSG_LEN						2048*2

// add 20230518 // Location info
#define	MSG_LOCATION_OPERATE_LEN							10*2
#define	MSG_LOCATION_LANGUAGE_LEN							10*2
#define	MSG_LOCATION_LOCATION_LEN							10*2
#define	MSG_LOCATION_FACTORY_LEN							10*2
#define	MSG_LOCATION_RESULT_LEN								10*2
#define MSG_LOCATION_NOTEMSG_LEN							2048*2

// add 20230518  Generasl func
#define MSG_GENERIC_PAR_MAX									10
#define	MSG_GENERIC_FUNCNAME_LEN							50*2
#define	MSG_GENERIC_OPERATE_LEN								10*2
#define	MSG_GENERIC_INPUTCOUNT_LEN							10*2
#define	MSG_GENERIC_OUTPUTCOUNT_LEN							10*2
#define	MSG_GENERIC_OUTPUT_LEN								512*2
#define	MSG_GENERIC_INPUT_LEN								512
#define	MSG_GENERIC_RESULT_LEN								10*2
#define MSG_GENERIC_NOTEMSG_LEN								2048*2




////log

#define MSG_LOG_OPERATE_LEN							10*2
#define MSG_LOG_LEVEL_LEN							10*2
#define MSG_LOG_DESCRIPTION_LEN						512*2
////////////////////////////////////TestData/////////////////////////////
//// process

#define		TESTDATA_PROCESS_ID_LEN					10*2
#define		TESTDATA_PROCESS_OPLINE_LEN				128*2
#define		TESTDATA_PROCESS_DESCRIPTION_LEN			128*2
#define		TESTDATA_PROCESS_RESULT_LEN				10*2
#define		TESTDATA_PROCESS_THRESHOLD_LEN			128*2
#define		TESTDATA_PROCESS_PASSFAIL_LEN			10*2
#define		TESTDATA_PROCESS_DATE_LEN				128*2

////// progress

#define		TESTDATA_PROGRESS_FULLSTEP_LEN			10*2
#define		TESTDATA_PROGRESS_STEP_LEN				10*2
#define		TESTDATA_PROGRESS_DATE_LEN				128*2

///// graphinit

#define		TESTDATA_GRAPHINIT_TITLE_LEN			128*2
#define		TESTDATA_GRAPHINIT_GRAPHINDEX_LEN		10*2
#define		TESTDATA_GRAPHINIT_X_LEN                128*2
#define		TESTDATA_GRAPHINIT_Y1_LEN				128*2
#define		TESTDATA_GRAPHINIT_Y2_LEN				128*2

///// GraphDataHead 

#define		TESTDATA_GRAPHDATAHEAD_OPERATE_LEN		20*2
#define		TESTDATA_GRAPHDATAHEAD_GRAPHINDEX_LEN	10*2
#define		TESTDATA_GRAPHDATAHEAD_PLOTINDEX_LEN	10*2
#define		TESTDATA_GRAPHDATAHEAD_PLOTTITLE_LEN	128*2
#define		TESTDATA_GRAPHDATAHEAD_COLOR_LEN		20*2
#define		TESTDATA_GRAPHDATAHEAD_YINDEX_LEN		10*2


struct Command_RunInit 
{
	char szUser[COMMAND_RUNINIT_USER_LEN];
	char szSN[COMMAND_RUNINIT_SN_LEN];
	char szPN[COMMAND_RUNINIT_PN_LEN];
};


struct Command_RunTask 
{
	char szIndex[COMMAND_RUNTASK_INDEX_LEN];
	char szUser[COMMAND_RUNTASK_USER_LEN];
	char szTitle[COMMAND_RUNTASK_TITLE_LEN];
	char szStepType[COMMAND_RUNTASK_INDEX_LEN];
};
struct Command_RunCommand 
{
	char szIndex[COMMAND_RUNCOMMAND_INDEX_LEN];
	char szUser[COMMAND_RUNCOMMAND_USER_LEN];
	char szData[COMMAND_RUNCOMMAND_DATA_LEN];
};
struct Command_RunDone 
{
	char szIndex[COMMAND_RUNDONE_INDEX_LEN];
	char szUser[COMMAND_RUNDONE_USER_LEN];
	char szResult[COMMAND_RUNDONE_TITLE_LEN];
};

struct Command_RunDevice 
{
	char szIndex[COMMAND_RUNDEVICE_INDEX_LEN];
	char szUser[COMMAND_RUNDEVICE_USER_LEN];
	char szTitle[COMMAND_RUNDEVICE_TITLE_LEN];
};
struct Command_StopRun 
{
	char szUser[COMMAND_STOPRUN_USER_LEN];
};
struct Command_DebugMode 
{
	char szUser[COMMAND_DEBUGMODE_USER_LEN];
	char szPassword[COMMAND_DEBUGMODE_PASSWORD_LEN];
	char szDebug[COMMAND_DEBUGMODE_DEBUG_LEN];
};
////基础信息
struct Msg_BaseInfo
{
	char szOperate[MSG_BASEINFO_OPERATE_LEN];
	char szProcessType[MSG_BASEINFO_PROCESSTYPE_LEN];
	char szAuthor[MSG_BASEINFO_AUTHOR_LEN];
	char szVersion[MSG_BASEINFO_VERSION_LEN];
	char szSpec[MSG_BASEINFO_SPEC_LEN];
	char szPnNumber[MSG_BASEINFO_PNNUMBER_LEN];
	char szDate[MSG_BASEINFO_DATE_LEN];
	// ADD 2021
	char szIsTestSpec[MSG_BASEINFO_TESTSPEC_LEN];
	char szTestMode[MSG_BASEINFO_TESTMODE_LEN];
	char szMFGOrder[MSG_BASEINFO_WORKORDER_LEN];
	char szInProcess[MSG_BASEINFO_INPROCESS_LEN];
	char szInHold[MSG_BASEINFO_INHOLD_LEN];
	char szInRework[MSG_BASEINFO_INREWORK_LEN];
	char szProductFamily[MSG_BASEINFO_PRODFAMILY_LEN];
	char szAliasName1[MSG_BASEINFO_ALIASNAME_LEN];
	char szAliasName2[MSG_BASEINFO_ALIASNAME_LEN];
	char szAliasName3[MSG_BASEINFO_ALIASNAME_LEN];
	char szMTOPN[MSG_BASEINFO_MTOPN_LEN];
	char szOplinkPN[MSG_BASEINFO_OPLINKPN_LEN];
};
// add 2020 配置路径
struct Msg_ConfigPath
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szTestDataPath[MSG_CONFIGPATH_DIR_LEN];
	char szConfigPath[MSG_CONFIGPATH_DIR_LEN];
	char szFWFilePath[MSG_CONFIGPATH_DIR_LEN];
};
//获取测试模板
struct Msg_Template
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szProdSN[COMMAND_RUNINIT_SN_LEN];
	char szPorcess[MSG_BASEINFO_PROCESSTYPE_LEN];
	char szSpec[MSG_BASEINFO_SPEC_LEN];
	char szWO[MSG_BASEINFO_SPEC_LEN];
	char szKeyData[MSG_MSGBOX_MSGID_LEN];
	char szNoteMsg[MSG_CONFIGPATH_DIR_LEN];
};
// 工单表头
struct Msg_TaskList
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szProdSN[COMMAND_RUNINIT_SN_LEN];
	char szGroup[COMMAND_RUNINIT_SN_LEN];
	char szPosition[MSG_CONFIGPATH_DIR_LEN];
	char szResult[MSG_MSGBOX_MSGID_LEN];
	char szNoteMsg[MSG_CONFIGPATH_DIR_LEN];
	char szSubSN[MSG_CONFIGPATH_DIR_LEN];
};
// 上传数据
struct Msg_UploadData
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szProdSN[COMMAND_RUNINIT_SN_LEN];
	char szFileName[MSG_CONFIGPATH_DIR_LEN];
	char szResult[MSG_MSGBOX_MSGID_LEN];
	char szNoteMsg[MSG_CONFIGPATH_DIR_LEN];
};
// end 2020

// add 2021
struct Msg_ConvPN
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szOriginPN[MSG_CONVPN_ORIGINPN_LEN];
	char szConvTyoe[MSG_CONVPN_CONVTYPE_LEN];
	char szResult[MSG_CONVPN_RESULT_LEN];
	char szResultPN[MSG_CONVPN_RESULTPN_LEN];
	char szNoteMsg[MSG_CONVPN_NOTEMSG_LEN];
};

struct Msg_ConvProcess
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szProdPN[MSG_CONVPROCESS_PRODPN_LEN];
	char szOriginProcess[MSG_CONVPROCESS_ORIGINPROCESS_LEN];
	char szConvTyoe[MSG_CONVPROCESS_CONVTYPE_LEN];
	char szResult[MSG_CONVPROCESS_RESULT_LEN];
	char szReslutProcess[MSG_CONVPROCESS_RESULTPROCESS_LEN];
	char szNoteMsg[MSG_CONVPROCESS_NOTEMSG_LEN];
};

struct Msg_StationInfo
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szPCName[MSG_STATIONINFO_PCNAME_LEN];
	char szResult[MSG_STATIONINFO_RESULT_LEN];
	char szStationID[MSG_STATIONINFO_STATIONID_LEN];
	char szStationName[MSG_STATIONINFO_STATIONNAME_LEN];
	char szNoteMsg[MSG_STATIONINFO_NOTEMSG_LEN];
};
struct Msg_ResultToOPC
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szSN[MSG_RESULTTOOPC_SN_LEN];
	char szResult[MSG_RESULTTOOPC_RESULT_LEN];
	char szNoteMsg[MSG_RESULTTOOPC_NOTEMSG_LEN];
};

struct Msg_MoveOut
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szSN[MSG_MOVEOUT_SN_LEN];
	char szResult[MSG_MOVEOUT_RESULT_LEN];
	char szNoteMsg[MSG_MOVEOUT_NOTEMSG_LEN];
};

struct Msg_MoveIn
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szSN[MSG_MOVEIN_SN_LEN];
	char szResult[MSG_MOVEIN_RESULT_LEN];
	char szNoteMsg[MSG_MOVEIN_NOTEMSG_LEN];
};

struct Msg_LoginVerify
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szUserName[MSG_LOGINVERIFY_USERNAME_LEN];
	char szPassWord[MSG_LOGINVERIFY_PASSWORD_LEN];
	char szLoginMode[MSG_LOGINVERIFY_LOGINMODE_LEN];
	char szResult[MSG_LOGINVERIFY_RESULT_LEN];
	char szNoteMsg[MSG_LOGINVERIFY_NOTMSG_LEN];
};

struct Msg_StationVerify
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szStationID[MSG_STATIONVERIFY_STATIONID_LEN];
	char szUserID[MSG_STATIONVERIFY_USERID_LEN];
	char szType[MSG_STATIONVERIFY_TYPE_LEN];
	char szResult[MSG_STATIONVERIFY_RESULT_LEN];
	char szNoteMsg[MSG_STATIONVERIFY_NOTMSG_LEN];
};

struct Msg_OplinkUserInfo
{
	char szOperate[MSG_OPLKUSERINFO_OPERATE_LEN];
	char szOplinkID[MSG_OPLKUSERINFO_OPLINKID_LEN];
	char szUserName[MSG_OPLKUSERINFO_USERMAME_LEN];
	char szDptName[MSG_OPLKUSERINFO_DPTNAME_LEN];
	char szPosName[MSG_OPLKUSERINFO_POSNAME_LEN];
	char szOPCID[MSG_OPLKUSERINFO_OPCID_LEN];
	char szResult[MSG_OPLKUSERINFO_RESULT_LEN];
	char szNoteMsg[MSG_OPLKUSERINFO_NOTEMSG_LEN];
};

struct Msg_UserSkillVerify
{
	char szOperate[MSG_USERSKILLVERIFY_OPERATE_LEN];
	char szOPCID[MSG_USERSKILLVERIFY_OPCID_LEN];
	char szProcessType[MSG_USERSKILLVERIFY_PROCESSTYPE_LEN];
	char szResult[MSG_USERSKILLVERIFY_RESULT_LEN];
	char szNoteMsg[MSG_USERSKILLVERIFY_NOTEMSG_LEN];
};

struct Msg_CreateSNDir
{
	char szOperate[MSG_CREATEDSNDIR_OPERATE_LEN];
	char szSN[MSG_CREATEDSNDIR_SN_LEN];
	char szRootDir[MSG_CREATEDSNDIR_ROOTDIR_LEN];
	char szResult[MSG_CREATEDSNDIR_RESULT_LEN];
	char szNoteMsg[MSG_CREATEDSNDIR_NOTEMSG_LEN];
};

struct Msg_FWStandardPath
{
	char szOperate[MSG_FWSTANDARDPATH_OPERATE_LEN];
	char szDUTMTSPN[MSG_FWSTANDARDPATH_DUTPN_LEN];
	char szResult[MSG_FWSTANDARDPATH_RESULT_LEN];
	char szFWPN[MSG_FWSTANDARDPATH_FWPN_LEN];
	char szNoteMsg[MSG_FWSTANDARDPATH_NOTEMSG_LEN];
};
// end 2021

// add 2023 05 19
struct Msg_LocationInfo
{
	char szOperate[MSG_LOCATION_OPERATE_LEN];
	char szLanguage[MSG_LOCATION_LANGUAGE_LEN];
	char szLocation[MSG_LOCATION_LOCATION_LEN];
	char szFactory[MSG_LOCATION_FACTORY_LEN];
	char szResult[MSG_LOCATION_RESULT_LEN];
	char szNoteMsg[MSG_LOCATION_NOTEMSG_LEN];
};

struct Msg_GenericFunc
{
	char szFuncName[MSG_GENERIC_FUNCNAME_LEN];
	char szOperate[MSG_GENERIC_OPERATE_LEN];
	char szInputCount[MSG_GENERIC_INPUTCOUNT_LEN];
	char szOutputCount[MSG_GENERIC_OUTPUTCOUNT_LEN];
	char szInput[MSG_GENERIC_PAR_MAX][MSG_GENERIC_INPUT_LEN];
	char szOutput[MSG_GENERIC_PAR_MAX][MSG_GENERIC_OUTPUT_LEN];
	char szResult[MSG_GENERIC_RESULT_LEN];
	char szNoteMsg[MSG_GENERIC_NOTEMSG_LEN];
};



// end 2023
//// 任务信息，
struct Msg_TaskInfo 
{
	char szOperate[MSG_TASKINFO_OPERATE_LEN];
	char szIndex[MSG_TASKINFO_INDEX_LEN];
	char szTitle[MSG_TASKINFO_TITLE_LEN];
	char szStepType[MSG_TASKINFO_OPERATE_LEN];
};
//// 命令信息，
struct Msg_CommandInfo 
{
	char szOperate[MSG_COMMANDINFO_OPERATE_LEN];
	char szIndex[MSG_COMMANDINFO_INDEX_LEN];
	char szTitle[MSG_COMMANDINFO_TITLE_LEN];
	char szExample[MSG_COMMANDINFO_EXAMPLE_LEN];
};
//// 上次测试进度
struct Msg_OldRate
{
	char szOperate[MSG_OLDRATE_OPERATE_LEN];
	char szIndex[MSG_OLDRATE_INDEX_LEN];
	char szRate[MSG_OLDRATE_RATE_LEN];
};
////任务描述，用于描述显示，该结构只包含一个任务的描述信息
struct Msg_TaskDescription
{
	char szOperate[MSG_TASKDESCRIPTION_OPERATE_LEN];
	char szIndex[MSG_TASKDESCRIPTION_INDEX_LEN];
	char szDescription[MSG_TASKDESCRIPTION_DESCRIPTION_LEN];
};
//// 消息提示框
struct Msg_MsgBox 
{
	char szOperate[MSG_MSGBOX_OPERATE_LEN];
	char szMsgId[MSG_MSGBOX_MSGID_LEN];
	char szMsgTitle[MSG_MSGBOX_MSGTITLE_LEN];
	char szDescription[MSG_MSGBOX_DESCRIPTION_LEN];
	char szButtonId[MSG_MSGBOX_BUTTONID_LEN];
	char szButtonTitle[MSG_MSGBOX_BUTTONTITLE_LEN];
};
//// 日志信息
struct Msg_Log 
{
	char szOperate[MSG_LOG_OPERATE_LEN];
	char szLevel[MSG_LOG_LEVEL_LEN];
	char szDescription[MSG_LOG_DESCRIPTION_LEN];
};

// 测试过程数据
struct TestData_Process
{
	char	szId[TESTDATA_PROCESS_ID_LEN];
	char	szOpLine[TESTDATA_PROCESS_OPLINE_LEN];
	char	szDescription[TESTDATA_PROCESS_DESCRIPTION_LEN];
	char	szResult[TESTDATA_PROCESS_RESULT_LEN];
	char	szThreshold[TESTDATA_PROCESS_THRESHOLD_LEN];
	char	szPassFail[TESTDATA_PROCESS_PASSFAIL_LEN];
	char	szDate[TESTDATA_PROCESS_DATE_LEN];
};

struct TestDAta_Progress
{
	char	szFullStep[TESTDATA_PROGRESS_FULLSTEP_LEN];
	char	szStep[TESTDATA_PROGRESS_STEP_LEN];
	char	szDate[TESTDATA_PROGRESS_DATE_LEN];
};

struct TestData_GraphInit
{
	char	szTitle[TESTDATA_GRAPHINIT_TITLE_LEN];
	char	szGraphIndex[TESTDATA_GRAPHINIT_GRAPHINDEX_LEN];
	char	szX[TESTDATA_GRAPHINIT_X_LEN];
	char	szY1[TESTDATA_GRAPHINIT_Y1_LEN];
	char	szY2[TESTDATA_GRAPHINIT_Y2_LEN];
};

struct TestData_GraphDataHead
{
	char	szOperate[TESTDATA_GRAPHDATAHEAD_OPERATE_LEN];
	char	szGraphIndex[TESTDATA_GRAPHDATAHEAD_GRAPHINDEX_LEN];
	char	szPlotIndex[TESTDATA_GRAPHDATAHEAD_PLOTINDEX_LEN];
	char	szPlotTitle[TESTDATA_GRAPHDATAHEAD_PLOTTITLE_LEN];
	char	szColor[TESTDATA_GRAPHDATAHEAD_COLOR_LEN];
	char	szYIndex[TESTDATA_GRAPHDATAHEAD_YINDEX_LEN];
};
union MSG_StructUnion
{
	Command_RunInit		ComRunInitData;
	Command_RunTask		ComRunTaskData;
	Command_RunDone		ComRunDone;
	Command_RunDevice	ComRunDeviceData;
	Command_StopRun		ComStopRunData;
	Command_DebugMode	ComDebugModeData;
	Command_RunCommand	ComRunCommandData;

	Msg_BaseInfo		MsgBaseInfo;
	Msg_TaskInfo		MsgTaskInfo;
	Msg_OldRate			MsgOldRate;
	Msg_TaskDescription	MsgTaskDescription;
	Msg_MsgBox			MsgBox;
	Msg_Log				MsgLog;
	Msg_CommandInfo		MsgCommandInfo;
	Msg_ConfigPath		MsgConfigPath;
	Msg_Template		MsgTemplate;
	Msg_TaskList		MsgTaskList;
	Msg_UploadData		MsgUploadData;
	Msg_ConvPN			MsgConvPN;
	Msg_ConvProcess		MsgConvProcess;
	Msg_StationInfo		MsgStationInfo;
	Msg_ResultToOPC		MsgResultToOPC;
	Msg_MoveOut			MsgMoveOut;
	Msg_MoveIn			MsgMoveIn;
	Msg_LoginVerify		MsgLoginVerify;
	Msg_StationVerify	MsgStationVerify;
	Msg_OplinkUserInfo	MsgOplinkUserInfo;
	Msg_UserSkillVerify MsgUserSkillVerify;
	Msg_CreateSNDir		MsgCreateSNDir;
	Msg_FWStandardPath	MsgFWStandardPath;
	Msg_LocationInfo	MsgLocationInfo;
	Msg_GenericFunc		MsgGenericFunc;


	TestData_Process	TestDataProcess;
	TestDAta_Progress	TestDataProgress;
	TestData_GraphInit	TestDataGraphInit;
	TestData_GraphDataHead TestDataGraphHead;
	
	

};

struct ALL_MSG_DATA
{
	int					nDataType;
	MSG_StructUnion		unDataStruct;
};

#endif