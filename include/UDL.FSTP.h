#pragma once


#include <winnt.h>

#ifndef DLL_EXPORT_API
#define DLL_EXPORT_API extern "C" __declspec(dllexport)
#endif
//Base api of UDL
DLL_EXPORT_API HRESULT _stdcall OpenFSTP(long lDeviceIndex, CHAR* pstrDeviceConfig, long* hdlDevice);
DLL_EXPORT_API HRESULT _stdcall SetFSTPHandle(long lDeviceIndex, HANDLE hdlDevice, CHAR* pstrDeviceConfig);
DLL_EXPORT_API long    _stdcall ResetFSTP(long lDeviceIndex, long lSlotIndex, long lChIndex);
DLL_EXPORT_API void    _stdcall CloseFSTP(long lDeviceIndex);
DLL_EXPORT_API CHAR*   _stdcall GetFSTPLastErrorMessage(void);
DLL_EXPORT_API CHAR*   _stdcall GetFSTPInfo(long lDeviceIndex);
DLL_EXPORT_API CHAR*   _stdcall GetFSTPLibraryInfo();
//Expand api of FSTP 

/**************************************************
Set target reference path(full path) | type- 0:Read;1:Write;
*if you set read,fstp will reset the reference data;
*if you set read,please don't set fstpparam again;
*if you set write,you should do reference before execute sweep;
**************************************************/
DLL_EXPORT_API HRESULT _stdcall SetReferenceFile(long lDeviceIndex, CHAR* strReferencePath, int nReadorWrite);
//Scan without reference
DLL_EXPORT_API HRESULT _stdcall ExecuteILSingleSweep(long lDeviceIndex);
DLL_EXPORT_API HRESULT _stdcall ExecutePDLSingleSweep(long lDeviceIndex);

//Set scan range and step;
//If you use this API,the exist raw/reference data will be clear;
DLL_EXPORT_API HRESULT _stdcall SetFSTPParameters(long lDeviceIndex, double dblStartWL, double dbleStopWL, double dblWLStep);

//Set the scan param for each scan;
DLL_EXPORT_API HRESULT _stdcall SetAllPMParameters(long lDeviceIndex, double dblAvgTimeUS, double* pdblWavelength, double* pdblRange, long lFullRangeSweep, long lPMCount, long* lPMIndex);
DLL_EXPORT_API HRESULT _stdcall GetAllPMParameters(long lDeviceIndex, double* pdblAvgTimeUS, double* pdblWavelength, double* pdblRange, long* lFullRangeSweep, long lPMCount, long* lPMIndex);
DLL_EXPORT_API HRESULT _stdcall SetPMParameters(long lDeviceIndex, long lPMIndex, double dblAvgTimeUS, double dblWavelength, double dblRange, long lFullRangeSweep);
DLL_EXPORT_API HRESULT _stdcall GetPMParameters(long lDeviceIndex, long lPMIndex, double* pdblAvgTimeUS, double* pdblWavelength, double* pdblRange, long* lFullRangeSweep);
DLL_EXPORT_API HRESULT _stdcall ExecuteILCalibration(long lDeviceIndex);
DLL_EXPORT_API HRESULT _stdcall ExecutePDLCalibration(long lDeviceIndex);
DLL_EXPORT_API HRESULT _stdcall ExecuteILSweep(long lDeviceIndex);
DLL_EXPORT_API HRESULT _stdcall ExecutePDLSweep(long lDeviceIndex);
DLL_EXPORT_API HRESULT _stdcall GetSweepStatus(long lDeviceIndex, long* plSweepStatus, long* plEstWaitingTime);
DLL_EXPORT_API HRESULT _stdcall GetMeasureResult(long lDeviceIndex, long lPMIndex, double* pdblWL, double* pdblIL, double* pdblPDL, double* pdblTapIL, long* plDataCount);


