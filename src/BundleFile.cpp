#include "stdafx.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <WINSOCK2.H>
#include "BundleFile.h"
//#include "BundleFile_dll8.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* CRC32 table. */
const ULONG m_aulCRCTable[0x100] =
{
	0x0000000,  0x77073096, 0xEE0E612C, 0x990951BA,
	0x76DC419,  0x706AF48F, 0xE963A535, 0x9E6495A3,
	0xEDB8832,  0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	0x9B64C2B,  0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
	0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
	0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x1DB7106,  0x98D220BC, 0xEFD5102A,
	0x71B18589, 0x6B6B51F,  0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0xF00F934,  0x9609A88E, 0xE10E9818,
	0x7F6A0DBB, 0x86D3D2D,  0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
	0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
	0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
	0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x3B6E20C,  0x74B1D29A,
	0xEAD54739, 0x9DD277AF, 0x4DB2615,  0x73DC1683,
	0xE3630B12, 0x94643B84, 0xD6D6A3E,  0x7A6A5AA8,
	0xE40ECF0B, 0x9309FF9D, 0xA00AE27,  0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
	0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
	0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
	0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
	0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x26D930A,
	0x9C0906A9, 0xEB0E363F, 0x72076785, 0x5005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0xCB61B38,
	0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0xBDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
	0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
	0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
	0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

BundleFile::BundleFile()
{

}

BundleFile::~BundleFile()
{

}
BOOL BundleFile::ReadCfgFile(CString strFileAdd, int* piFileNum, LPSCF_IMG_INFO_ST pm_stImgInfoList, char* aszErrMsg)
{
	FILE* pfCfgFile = NULL;
	CString strMsgInfo;
	char szLineData[SCF_FILE_LINE_MAX_LEN] = { 0 };
	int iImgCount = 0;//count of file image
	char szImgName[SCF_MOD_NAME_LEN] = { 0 };
	char szPNVer[SCF_MOD_NAME_LEN] = { 0 };
	char szSNVer[SCF_MOD_NAME_LEN] = { 0 };
	int iRetVal = 0;	//return value
	UINT uiResult = 0; //file length to return
	ULONG ulBndLen = NULL_DWORD; //bundle length
	SCF_IMG_INFO_ST stImgInfo;//file information struct


	char  aszImageTag[SCF_ARRAY_LEN] = { 0 };         /* Tag of image. */
	int iImageType = 0;                                       /*Image Type of this image.*/
	int iImageHitless = 0;            /* Hitless flag. */
	int iStorageID = 0;            /*Storage ID of this image.*/
	int iImageIndex = 0;         /*Image Index of this image.*/
	char  aszImageVersion[SCF_IMG_VERSION_LEN] = { 0 };     /*Image Version of this image.*/
	DWORD dwImageBaseAddr = 0;       /*Image Base Addr of this image;*/
	int iSectorCnt = 0;       /* Sector Count of image in bundle. */
	//the line if ConfigFile is start with "[PN_TYPE]=" or  "[SN_TYPE]= or "[TAG]="
	char szCmpPN[SCF_CMPPN_SN] = SCF_PN_CFGFILE_DESCRP;
	char szCmpSN[SCF_CMPPN_SN] = SCF_SN_CFGFILE_DESCRP;
	char szCmpTAG[SCF_CMPPN_SN] = SCF_TAG_CFGFILE_DESCRP;
	char szCmpEND[SCF_CMPPN_SN] = SCF_END_CFGFILE_DESCRP;
	char  szCmpStart[SCF_CMPPN_SN] = SCF_START_CFGFILE_DESCRP;
	char szLineDataCopy[SCF_FILE_LINE_MAX_LEN] = { 0 };
	USHORT ulBndHdrSize = 0;
	int ImgVerFirstLen = 0;  //The value of the first byte of Image version
	int ImgVerSecndLen = 0;  //The value of the second byte of Image version
	int ImgVerThdLEN = 0;    //The value of the third and fourth byte of Image version
	char aszImgVerSplt[SCF_IMG_VERSION_LEN] = { 0 };
	BOOL isSelCfgFile = FALSE;
	CString strTemp1;
	int iFileIndex = 0;
	DWORD dwImageVersion = 0;
	//open the file
	pfCfgFile = fopen(strFileAdd, "r");
	if (pfCfgFile == NULL)
	{
		strMsgInfo.Format(_T("Failed to open the file:%s"), strFileAdd);
		memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		return FALSE;
	}
	//Get line of manifest
	ulBndLen = 0;
	while (NULL != fgets(szLineData, SCF_FILE_LINE_MAX_LEN, pfCfgFile))
	{
		//Don't parse remark
		strTemp1 = szLineData;
		strTemp1.TrimLeft();
		strTemp1.TrimRight();
		sprintf(szLineData, "%s", strTemp1);
		strcpy(szLineDataCopy, szLineData);
		if (FALSE == isSelCfgFile)
		{
			isSelCfgFile = TRUE;
			if (!strncmp(szLineDataCopy, szCmpStart, strlen(szCmpStart)))
			{
				continue;
			}
			else
			{
				strMsgInfo.Format(_T("The selected file is not a fit file:%s"), strFileAdd);
				memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				SAFE_FCLOSE(pfCfgFile);
				return FALSE;
			}
		}


		if (('#' == szLineData[0]) || (strlen(szLineData) == 0))
		{
			continue;
		}
		//get PN/SN/image tag/END value
		if (!strncmp(szLineDataCopy, szCmpPN, strlen(szCmpPN)))
		{
			if ((strlen(szLineDataCopy) - strlen(szCmpPN)) <= SCF_MOD_NAME_LEN)
			{
				strcpy(szPNVer, &szLineDataCopy[strlen(szCmpPN)]);
				szPNVer[strlen(szPNVer)] = 0;
				strTemp1 = szPNVer;
				strTemp1.TrimRight();
				strTemp1.TrimLeft();

				if (strTemp1.Find(' ') >= 0)
				{
					strMsgInfo.Format(_T("Invalid PN:%s"), szPNVer);
					memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
					SAFE_FCLOSE(pfCfgFile);
					return FALSE;
				}
				else
				{
					sprintf(szPNVer, "%s", strTemp1);
					continue;   // Jump out of the current loop,and turn into the next loop
				}
			}
			else
			{
				strMsgInfo.Format(_T("Invalid PN:%s"), szPNVer);
				memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				SAFE_FCLOSE(pfCfgFile);
				return FALSE;
			}
		}

		if (!strncmp(szLineDataCopy, szCmpSN, strlen(szCmpSN)))
		{
			if ((strlen(szLineDataCopy) - strlen(szCmpSN)) <= SCF_MOD_NAME_LEN)
			{
				strcpy(szSNVer, &szLineDataCopy[strlen(szCmpSN)]);
				szSNVer[strlen(szSNVer)] = 0;
				strTemp1 = szSNVer;
				strTemp1.TrimRight();
				strTemp1.TrimLeft();

				if (strTemp1.Find(' ') >= 0)
				{
					strMsgInfo.Format(_T("Invalid SN:%s"), szSNVer);
					memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
					SAFE_FCLOSE(pfCfgFile);
					return FALSE;
				}
				else
				{
					sprintf(szSNVer, "%s", strTemp1);
					continue;      // when the value of PN and SN  is obtained,turn into the next loop,
								 //and get the next line value.
				}
			}
			else
			{

				strMsgInfo.Format(_T("Invalid SN:%s"), szSNVer);
				memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				SAFE_FCLOSE(pfCfgFile);
				return FALSE;
			}
		}

		if (!strncmp(szLineDataCopy, szCmpTAG, strlen(szCmpTAG)))
		{
			if ((strlen(szLineDataCopy) - strlen(szCmpTAG)) <= SCF_ARRAY_LEN - 1)
			{
				strcpy(aszImageTag, &szLineDataCopy[strlen(szCmpTAG)]);

				aszImageTag[strlen(aszImageTag)] = 0;
				continue;

			}
			else
			{
				strMsgInfo.Format(_T("Invalid TAG:%s"), aszImageTag);
				memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				SAFE_FCLOSE(pfCfgFile);
				return FALSE;
			}

		}

		if (!strncmp(szLineDataCopy, szCmpEND, strlen(szCmpEND)))
		{
			break;
		}

		iRetVal = sscanf(szLineData, "%d %d %d %d %d %x %x %d %s", &iFileIndex, &iImageType, &iImageHitless, &iStorageID, &iImageIndex, &dwImageVersion, &dwImageBaseAddr, &iSectorCnt, szImgName);
		if ((EOF == iRetVal) || (0 == iRetVal))
		{
			strMsgInfo.Format(_T("Read this Line error,please check your selected file!"));
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pfCfgFile);
			return FALSE;
		}
		//Initialize the module information structure.
		memset((char*)&stImgInfo, 0x00, sizeof(stImgInfo));
		//check fileindex
		stImgInfo.iFileIndex = iFileIndex;

		//Check PN
		if ((strlen(szPNVer) <= 0) || (strlen(szPNVer)) >= SCF_MOD_NAME_LEN)
		{
			strMsgInfo.Format(_T("Invalid PN:%s"), szPNVer);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pfCfgFile);
			return FALSE;
		}
		else
		{

			memcpy(stImgInfo.aszPN, szPNVer, strlen(szPNVer));
		}
		//Check SN
		if ((strlen(szSNVer) <= 0) || (strlen(szSNVer)) >= SCF_MOD_NAME_LEN)
		{
			strMsgInfo.Format(_T("Invalid SN:%s"), szSNVer);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pfCfgFile);
			return FALSE;
		}
		else
		{

			memcpy(stImgInfo.aszSN, szSNVer, strlen(szSNVer));
		}

		//Get image tag

		strcpy(stImgInfo.aszImageTag, aszImageTag);
		//check image type
		if ((iImageType < SCF_IMGTYPE_MIN_VALUE) || (iImageType > SCF_ONE_BYTE_MAX_VALUE))
		{
			strMsgInfo.Format("Invalid image type:%d", iImageType);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pfCfgFile);
			return FALSE;
		}
		else
		{

			stImgInfo.uszImageType = (UCHAR)iImageType;
		}

		//Check hitless flag
		if ((1 != iImageHitless) && (0 != iImageHitless))
		{
			strMsgInfo.Format(_T("Invalid hitless:%d"), iImageHitless);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pfCfgFile);
			return FALSE;
		}
		else
		{

			stImgInfo.uszImageHitless = (UCHAR)iImageHitless;
		}
		//Get storage id
		if ((iStorageID < SCF_UNCHAR_MIN_VALUE) || (iStorageID) > SCF_ONE_BYTE_MAX_VALUE)
		{
			strMsgInfo.Format(_T("Invalid Storage ID:%d"), iStorageID);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pfCfgFile);
			return FALSE;
		}
		else
		{

			stImgInfo.uszStorageID = (UCHAR)iStorageID;
		}

		//Get image index
		if ((iImageIndex < SCF_UNCHAR_MIN_VALUE) || (iImageIndex > SCF_ONE_BYTE_MAX_VALUE))
		{
			strMsgInfo.Format(_T("Invalid Image index:%d"), iImageIndex);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		}
		else
		{

			stImgInfo.uszImageIndex = (UCHAR)iImageIndex;
		}

//16LSB格式的文件版本
		DWORD dwTemp = htonl(dwImageVersion);
		memcpy(&stImgInfo.aszImageVersion[0], (UCHAR*)&dwTemp, 4);

		//Get Image Base Addr
		stImgInfo.dwImageBaseAddr = dwImageBaseAddr;

		//Get Sector Count
		stImgInfo.uszSectorCnt = (UCHAR)iSectorCnt;
		//Get image file name
		if ((strlen(szImgName) <= 0) || (strlen(szImgName) >= SCF_MOD_NAME_LEN))
		{
			strMsgInfo.Format(_T("Invalid image name:%s"), szImgName);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pfCfgFile);
			return FALSE;
		}
		else
		{

			strcpy(stImgInfo.aszName, szImgName);
		}


		//Get image file length
		CString strFileName;
		strFileName.Format("./%s.bin", szImgName);
		FILE*pFile = NULL;
		pFile = fopen(strFileName, "rb");
		if (pFile == NULL)
		{
			stImgInfo.ulImgLen = 0;

		}
		else
		{
			uiResult = GetFileLength(strFileName, aszErrMsg);
			if (uiResult == 0)
			{
				SAFE_FCLOSE(pfCfgFile);
				return FALSE;
			}
			else
			{
				stImgInfo.ulImgLen = uiResult;
			}
			fclose(pFile);
			pFile = NULL;
		}


		//Record the module information
		memcpy(&pm_stImgInfoList[iImgCount], &stImgInfo, sizeof(stImgInfo));
		memset(&stImgInfo, 0x00, sizeof(stImgInfo));
		//Increase image count
		iImgCount++;
		memset(szLineData, 0x00, sizeof(szLineData));
	}
	SAFE_FCLOSE(pfCfgFile);



	*piFileNum = iImgCount;

	return TRUE;
}

BOOL BundleFile::BundleFiles(int iFileNum, ULONG ulBndLen, ULONG ulBndHdrLen, CString strBndVer, CString strProdType, BOOL b1KBndLoadMod, LPSCF_IMG_INFO_ST pm_stImgInfoList, PSCF_IMG_HEADER_INFO_ST pstImgHdrInfoList, char* aszErrMsg)
{
	UCHAR* uszBuffer = NULL;//buffer
	int iReturnValue = 0;//return value
	ULONG ulOffset = NULL_DWORD;//length offset
	ULONG ulCRC32 = NULL_DWORD;//CRC32 of the bundle
	ULONG ulFileLen = NULL_DWORD;//image file length
	CString strMsgInfo, strBndTime, strImgTime;//string information
	FILE* fpImgFile = NULL;//file point
	char aszImageTimeStamp[SCF_ARRAY_LEN] = { 0 };
	ULONG ulFrontalFileLen = 0;
	ULONG iImgHdrOff = ulBndHdrLen;//offset of image header of image file
	float fImgLen = 0.0;
	CString strMsgImgLen;
	ULONG ulGetCRC32 = 0x000000000;
	ulGetCRC32 ^= 0XFFFFFFFF;
	int itemp = 0;
	ULONG ulTemp = 0;
	//Allocate RAM for bundle buffer.
	uszBuffer = (UCHAR*)malloc(ulBndLen);
	if (NULL == uszBuffer)
	{
		strMsgInfo.Format(_T("Failed to allocate memory!"));
		if (aszErrMsg != NULL)
		{
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		}
		return FALSE;
	}
	else
	{
		memset(uszBuffer, 0, ulBndLen);
	}
	//Copy the information after m_BndHdrSize address.
	ulOffset = ulBndHdrLen;
	for (int i = 0;i < iFileNum;i++)
	{
		//Read image data into RAM.
		ulFileLen = pm_stImgInfoList[i].ulImgLen;
		//Open the image file.
		fpImgFile = fopen(pm_stImgInfoList[i].aszName, "rb");
		if (NULL == fpImgFile)
		{
			strMsgInfo.Format(_T("Failed to open the file:%s"), pm_stImgInfoList[i].aszName);
			if (aszErrMsg != NULL)
			{
				memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			}
			SAFE_FREE(uszBuffer);
			return FALSE;
		}
		if (pstImgHdrInfoList[i].iEveryImageHeaderCnt == 0)
		{

			ulOffset += SCF_MOD_NAME_LEN;
			iReturnValue = fread((UCHAR*)&uszBuffer[ulOffset], sizeof(UCHAR), ulFileLen, fpImgFile);
			if ((long)ulFileLen != iReturnValue)
			{
				strMsgInfo.Format(_T("Failed to read the file:%s"), pm_stImgInfoList[i].aszName);
				if (aszErrMsg != NULL)
				{
					memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				}
				SAFE_FCLOSE(fpImgFile);
				SAFE_FREE(uszBuffer);
				return FALSE;
			}

			//Get the current image time
			CTime currentTime = CTime::GetCurrentTime();
			pm_stImgInfoList[i].aszImageTimeStamp[0] = (CHAR)currentTime.GetMonth();
			pm_stImgInfoList[i].aszImageTimeStamp[1] = (CHAR)currentTime.GetDay();
			pm_stImgInfoList[i].aszImageTimeStamp[2] = (CHAR)currentTime.GetHour();
			pm_stImgInfoList[i].aszImageTimeStamp[3] = (CHAR)currentTime.GetMinute();
			pm_stImgInfoList[i].aszImageTimeStamp[4] = 0;
			//strcpy(pm_stImgInfoList[i].aszImageTimeStamp,aszImageTimeStamp);

			//get the CRC32 of each image file
			//Increase the image length.

			pm_stImgInfoList[i].ulCRC32 = CalcCRC32(ulGetCRC32, (UCHAR*)&uszBuffer[ulOffset], ulFileLen);
			pm_stImgInfoList[i].ulCRC32 ^= 0XFFFFFFFF;

			if (b1KBndLoadMod == TRUE)
			{
				ulFileLen += SCF_MOD_NAME_LEN;

				if (0 != (ulFileLen%SCF_XMD_PAGE_SIZE))
				{
					ulFileLen = (ulFileLen / SCF_XMD_PAGE_SIZE + 1)*SCF_XMD_PAGE_SIZE;
					itemp = pm_stImgInfoList[i].ulImgLen;
					if (pstImgHdrInfoList[i].bisFPGAFile == TRUE)//FPGA文件在1K模式下补全是用0XFF，因为用0X00补的话相当于FPGA默认是有值的
					{
						memset((UCHAR*)&uszBuffer[ulOffset + itemp], 0xFF, ulFileLen - itemp - SCF_MOD_NAME_LEN);
					}
					else//非FPGA文件补全是用0X00
					{
						memset((UCHAR*)&uszBuffer[ulOffset + itemp], 0x00, ulFileLen - itemp - SCF_MOD_NAME_LEN);
					}
					//改变文件头的CRC32
					pm_stImgInfoList[i].ulCRC32 = CalcCRC32(ulGetCRC32, (UCHAR*)&uszBuffer[ulOffset], ulFileLen - SCF_MOD_NAME_LEN);
					pm_stImgInfoList[i].ulCRC32 ^= 0XFFFFFFFF;
					//改变文件的长度
					pm_stImgInfoList[i].ulImgLen = ulFileLen - SCF_MOD_NAME_LEN;

				}

				ulOffset -= SCF_MOD_NAME_LEN;
			}

			ulOffset += ulFileLen;

			//SAFE_FCLOSE(fpImgFile);
			//Change order of all data of image header.
			pm_stImgInfoList[i].ulCRC32 = htonl(pm_stImgInfoList[i].ulCRC32);
			pm_stImgInfoList[i].ulImgLen = htonl(pm_stImgInfoList[i].ulImgLen);
			pm_stImgInfoList[i].dwImageBaseAddr = htonl(pm_stImgInfoList[i].dwImageBaseAddr);
		}
		else //带头的
		{
			iReturnValue = fread((UCHAR*)&uszBuffer[ulOffset], sizeof(UCHAR), ulFileLen, fpImgFile);
			if ((long)ulFileLen != iReturnValue)
			{
				strMsgInfo.Format(_T("Failed to read the file:%s"), pm_stImgInfoList[i].aszName);
				if (aszErrMsg != NULL)
				{
					memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				}
				SAFE_FCLOSE(fpImgFile);
				SAFE_FREE(uszBuffer);
				return FALSE;
			}


			if (b1KBndLoadMod == TRUE)
			{
				if (0 != (ulFileLen%SCF_XMD_PAGE_SIZE))
				{
					ulFileLen = (ulFileLen / SCF_XMD_PAGE_SIZE + 1)*SCF_XMD_PAGE_SIZE;
					itemp = pm_stImgInfoList[i].ulImgLen;

					//非1K模式下用0x00补全
					memset((UCHAR*)&uszBuffer[ulOffset + itemp], 0x00, ulFileLen - itemp);

					//改变文件头的CRC32
					pm_stImgInfoList[i].ulCRC32 = CalcCRC32(ulGetCRC32, (UCHAR*)&uszBuffer[ulOffset + SCF_MOD_NAME_LEN], ulFileLen - SCF_MOD_NAME_LEN);
					pm_stImgInfoList[i].ulCRC32 ^= 0XFFFFFFFF;
					ulTemp = htonl(pm_stImgInfoList[i].ulCRC32);
					memcpy(&pstImgHdrInfoList[i].szImageHeaderBuffer[0][20], (UCHAR*)&ulTemp, 4);//将文件的新的CRC32赋值给文件头部
					//改变文件头的长度
					ulTemp = htonl(ulFileLen - SCF_MOD_NAME_LEN);
					memcpy(&pstImgHdrInfoList[i].szImageHeaderBuffer[0][24], (UCHAR*)&ulTemp, 4);//将文件的新的数据区长度赋值给文件头部
					//用新的文件头改变原来的文件头
					memcpy((UCHAR*)&uszBuffer[ulOffset], &pstImgHdrInfoList[i].szImageHeaderBuffer[0], SCF_MOD_NAME_LEN);

				}

			}
			ulOffset += ulFileLen;
		}
		SAFE_FCLOSE(fpImgFile);

	}





	CTime currentTime = CTime::GetCurrentTime();
	strBndTime = currentTime.Format("%Y.%m.%d.%H.%M");
	//copy Bundle Header into RAM.
	//copy the Bundle Tag
	memcpy((UCHAR*)&uszBuffer[IDX_HDR_BNDTAG], SCF_BNDL_BNDTAG, strlen(SCF_BNDL_BNDTAG));
	//copy the Product Type
	memcpy((UCHAR*)&uszBuffer[IDX_HDR_PRODTYPE], strProdType, strlen(strProdType));
	//Set the bundle size
	ulTemp = htonl(ulBndLen);
	memcpy((UCHAR*)&(uszBuffer[IDX_HDR_BNDSIZE]), (UCHAR*)&ulTemp, sizeof(ulTemp));
	//copy the Bundle Header Size
	USHORT usTemp = htons(ulBndHdrLen);
	memcpy((UCHAR*)&uszBuffer[IDX_HDR_BNDHDRSIZE], (UCHAR*)&usTemp, sizeof(usTemp));
	//copy the Image Count
	usTemp = htons((USHORT)iFileNum);
	memcpy((UCHAR*)&uszBuffer[IDX_HDR_IMGCNT], (UCHAR*)&usTemp, sizeof(usTemp));
	//Copy the bundle version.
	memcpy((UCHAR*)&(uszBuffer[IDX_HDR_BNDVER]), strBndVer, strlen(strBndVer));
	//Copy the PN version
	memcpy((UCHAR*)&(uszBuffer[IDX_HDR_BNDPN]), (UCHAR*)&(pm_stImgInfoList[0].aszPN), SCF_MOD_NAME_LEN);
	//Copy the SN version
	memcpy((UCHAR*)&(uszBuffer[IDX_HDR_BNDSN]), (UCHAR*)&(pm_stImgInfoList[0].aszSN), SCF_MOD_NAME_LEN);
	//Copy the bundle time
	memcpy((UCHAR*)&(uszBuffer[IDX_HDR_BNDTIME]), strBndTime, strlen(strBndTime));
	//copy the image header into RAM
	int iImgOff = 0;
	int iHeadCount = 0;
	for (i = 0;i < iFileNum;i++)
	{
		//copy the image header into bundle header RAM
		if (pstImgHdrInfoList[i].iEveryImageHeaderCnt == 0)
		{
			//Copy the image tag
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGTAG]), (UCHAR*)&(pm_stImgInfoList[i].aszImageTag), strlen(pm_stImgInfoList[i].aszImageTag));
			//Copy the image type
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGTYPE]), (UCHAR*)&(pm_stImgInfoList[i].uszImageType), sizeof(pm_stImgInfoList[i].uszImageType));
			//Copy the image hitless
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGHITLESS]), (UCHAR*)&(pm_stImgInfoList[i].uszImageHitless), sizeof(pm_stImgInfoList[i].uszImageHitless));
			//Copy the storage id
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMASTORAGEID]), (UCHAR*)&(pm_stImgInfoList[i].uszStorageID), sizeof(pm_stImgInfoList[i].uszStorageID));
			//Copy the image index
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGINDEX]), (UCHAR*)&(pm_stImgInfoList[i].uszImageIndex), sizeof(pm_stImgInfoList[i].uszImageIndex));
			//Copy the image version
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGVER]), (UCHAR*)&(pm_stImgInfoList[i].aszImageVersion), SCF_ARRAY_LEN - 1);
			//Copy the image time stamp
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGTIME]), (UCHAR*)&(pm_stImgInfoList[i].aszImageTimeStamp), strlen(pm_stImgInfoList[i].aszImageTimeStamp));
			//Copy the image base_addr
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGADDR]), (UCHAR*)&(pm_stImgInfoList[i].dwImageBaseAddr), sizeof(pm_stImgInfoList[i].dwImageBaseAddr));
			//Copy the image CRC32
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGCRC32]), (UCHAR*)&(pm_stImgInfoList[i].ulCRC32), sizeof(pm_stImgInfoList[i].ulCRC32));
			//Copy the image length
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGLEN]), (UCHAR*)&pm_stImgInfoList[i].ulImgLen, sizeof(pm_stImgInfoList[i].ulImgLen));
			//Copy the sector count
			memcpy((UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_SECCNT]), (UCHAR*)&(pm_stImgInfoList[i].uszSectorCnt), sizeof(pm_stImgInfoList[i].uszSectorCnt));

			//copy the  image header into image file RAM
			memcpy((UCHAR*)&(uszBuffer[iImgHdrOff]), (UCHAR*)&(uszBuffer[iImgOff + IDX_HDR_IMGTAG]), SCF_MOD_NAME_LEN);
			//examine the length of frontal image is 16's multiple
			ulFrontalFileLen = ntohl(pm_stImgInfoList[i].ulImgLen);
			iImgHdrOff += SCF_MOD_NAME_LEN;
			if (b1KBndLoadMod == TRUE)
			{
				ulFrontalFileLen += SCF_MOD_NAME_LEN;
				if (0 != (ulFrontalFileLen%SCF_XMD_PAGE_SIZE))
				{
					ulFrontalFileLen = (ulFrontalFileLen / SCF_XMD_PAGE_SIZE + 1)*SCF_XMD_PAGE_SIZE;
				}
				ulFrontalFileLen -= SCF_MOD_NAME_LEN;

			}

			iImgHdrOff += ulFrontalFileLen;
			//iImgHdrOff+=SCF_MOD_NAME_LEN;
			iImgOff += SCF_BUDL_HDR_IMAGE;
		}
		else
		{
			for (int iHeadIndex = 0; iHeadIndex < pstImgHdrInfoList[i].iEveryImageHeaderCnt; iHeadIndex++)
			{

				memcpy(uszBuffer + iImgOff + IDX_HDR_IMGTAG, (UCHAR *)pstImgHdrInfoList[i].szImageHeaderBuffer[iHeadCount], SCF_BUDL_HDR_IMAGE);
				iImgOff += SCF_BUDL_HDR_IMAGE;
			}

			ulFrontalFileLen = pm_stImgInfoList[i].ulImgLen;

			iImgHdrOff += ulFrontalFileLen;
		}
	}
	//Set the CRC32 of bundle header
	ulCRC32 = CalcCRC32(ulGetCRC32, (UCHAR*)&uszBuffer[IDX_HDR_BNDSIZE], ulBndHdrLen - IDX_HDR_BNDSIZE);
	ulCRC32 ^= 0XFFFFFFFF;
	ulCRC32 = htonl(ulCRC32);
	memcpy((UCHAR*)&(uszBuffer[IDX_HDR_BNDCRC32]), (UCHAR*)&ulCRC32, sizeof(ulCRC32));
	//Save it into bundle file.
	fpImgFile = fopen(pm_stImgInfoList[0].aszName, "wb");
	if (NULL == fpImgFile)
	{
		strMsgInfo.Format(_T("Failed to open the file:%s"), pm_stImgInfoList[0].aszName);
		if (aszErrMsg != NULL)
		{
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		}
		SAFE_FREE(uszBuffer);
		return FALSE;
	}
	iReturnValue = fwrite(uszBuffer, sizeof(char), ulBndLen, fpImgFile);
	if (iReturnValue != (int)ulBndLen)
	{
		strMsgInfo.Format(_T("Failed to write the file:%s"), SCF_BNDL_FILE_NAME);
		if (aszErrMsg != NULL)
		{
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		}
		SAFE_FCLOSE(fpImgFile);
		SAFE_FREE(uszBuffer);
		return FALSE;
	}
	SAFE_FCLOSE(fpImgFile);
	SAFE_FREE(uszBuffer);
	return TRUE;
}

ULONG BundleFile::GetFileLength(const char *pszFileName, char* aszErrMsg)
{
	FILE* pFile = NULL;
	struct _stat stFileStat;
	UINT uiFileLen = NULL_DWORD;
	CString strMsgInfo;

	if (NULL == pszFileName)
	{
		strMsgInfo.Format(_T("The file name can't be null"));
		memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		return 0;
	}
	pFile = fopen(pszFileName, "rb");
	if (NULL == pFile)
	{
		strMsgInfo.Format(_T("Failed open the file:%s"), pszFileName);
		memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		return 0;
	}
	if (0 != _fstat(_fileno(pFile), &stFileStat))
	{
		strMsgInfo.Format(_T("Failed to get the information of the file:%s"), pszFileName);
		memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
		return 0;
	}
	uiFileLen = stFileStat.st_size;
	fclose(pFile);
	return uiFileLen;
}

ULONG BundleFile::CalcCRC32(ULONG ulInitCRC, UCHAR* uszDataBuf, ULONG ulDataLen)
{
	ULONG ulCRC32 = NULL_DWORD;
	ULONG ulCRCIndex = NULL_DWORD;
	//Record the CRC into CRC table.
	ulCRC32 = ulInitCRC;
	while (ulDataLen--)
	{
		ulCRCIndex = (ulCRC32 ^ (*uszDataBuf++)) & 0x000000FF;
		ulCRC32 = ((ulCRC32 >> 8) & 0x00FFFFFF) ^ m_aulCRCTable[ulCRCIndex];
	}
	return ulCRC32;


}

BOOL BundleFile::GetEveryImageFileHeader(int iFileNum, ULONG &ulBndLen, ULONG &ulBndHdrLen, BOOL b1KBndLoadMod, LPSCF_IMG_INFO_ST pm_stImgInfoList, PSCF_IMG_HEADER_INFO_ST pstImgHdrInfoList, char* aszErrMsg)
{
	FILE* pFile = NULL;
	//struct _stat stFileStat;
	//UINT uiFileLen = NULL_DWORD;
	CString strMsgInfo, strFileName;
	CHAR* uszBuffer = NULL;//buffer
	int iReturnValue = 0;
	ULONG ulFileLen = 0;
	int iEveryImgHdrCnt = 0;
	ULONG ulFileCountLen = 0;
	ULONG ulBundleLen = 0;
	ULONG  ulBundleHeaderLen = 0;
	UINT uiResult = 0;
	UINT uiTotalHeader = 0;
	//pm_stImgInfoList = new SCF_IMG_HEADER_INFO_ST[iFileNum];
	for (int i = 0;i < iFileNum;i++)
	{
		strFileName.Format("./%s.bin", pm_stImgInfoList[i].aszName);
		//ulFileLen=GetFileLength(strFileName);
		//uiResult = GetFileLength(strFileName);
		ulFileLen = pm_stImgInfoList[i].ulImgLen;

		uszBuffer = (CHAR*)malloc(ulFileLen);
		if (NULL == pm_stImgInfoList[i].aszName)
		{
			strMsgInfo.Format(_T("The file name can't be null"));
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			return 0;
		}
		pFile = fopen(strFileName, "rb");
		if (NULL == pFile)
		{
			strMsgInfo.Format(_T("Failed open the file:%s"), pm_stImgInfoList[i].aszName);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			return 0;
		}
		iReturnValue = fread(uszBuffer, sizeof(UCHAR), ulFileLen, pFile);
		if ((long)ulFileLen != iReturnValue)
		{
			strMsgInfo.Format(_T("Failed to read the file:%s!"), pm_stImgInfoList[i].aszName);
			memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
			SAFE_FCLOSE(pFile);
			SAFE_FREE(uszBuffer);
			return FALSE;
		}



		//判断image文件是否带头的，
		ULONG ulTemp = 0;
		if (!strncmp(&uszBuffer[ulFileCountLen], SCT_IBFH_DESCRP, SCF_FOUR_BYTE))
		{
			memcpy((UCHAR *)&ulTemp, (CHAR *)&uszBuffer[SCF_IMG_LEN_ADDR], SCF_FOUR_BYTE);
			ulTemp = ntohl(ulTemp);
			ulFileCountLen = ulTemp + SCF_MOD_NAME_LEN;
			if (ulFileCountLen == ulFileLen)
			{
				iEveryImgHdrCnt = 1;

			}
		}
		pstImgHdrInfoList[i].iEveryImageHeaderCnt = iEveryImgHdrCnt;

		//判断FPGA文件
		CHAR FPGAArray[10] = { 0 };
		FPGAArray[0] = (CHAR)0xFF;
		FPGAArray[1] = (CHAR)0xFF;
		FPGAArray[2] = (CHAR)0xFF;
		FPGAArray[3] = (CHAR)0xFF;
		FPGAArray[4] = (CHAR)0xAA;
		FPGAArray[5] = (CHAR)0x99;
		FPGAArray[6] = (CHAR)0x55;
		FPGAArray[7] = (CHAR)0x66;
		FPGAArray[8] = (CHAR)0x30;
		pstImgHdrInfoList[i].bisFPGAFile = FALSE;
		for (int j = 0;j < 119;j++)
		{
			if (!strncmp(&uszBuffer[j], FPGAArray, SCF_FPGA_FLAG_NUM))
			{
				pstImgHdrInfoList[i].bisFPGAFile = TRUE;
				break;
			}
			else
			{
				continue;
			}
		}


		//image文件不带头的
		if (iEveryImgHdrCnt == 0)
		{
			if ((pstImgHdrInfoList[i].bisFPGAFile == TRUE) && ((pm_stImgInfoList[i].uszImageType < 33) ||
				(pm_stImgInfoList[i].uszImageType > 48)))
			{
				strMsgInfo.Format(_T("%s是个FPGA文件，其文件类型配置有误,请重新配置！"), pm_stImgInfoList[i].aszName);
				memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				SAFE_FCLOSE(pFile);
				SAFE_FREE(uszBuffer);
				return FALSE;
			}
			if ((pstImgHdrInfoList[i].bisFPGAFile == TRUE) && (pm_stImgInfoList[i].uszSectorCnt != 1))
			{
				strMsgInfo.Format(_T("%s是个FPGA文件，其文件SectorCount配置有误,请重新配置！"), pm_stImgInfoList[i].aszName);
				memcpy(aszErrMsg, strMsgInfo, RETURNLENS(strMsgInfo));
				SAFE_FCLOSE(pFile);
				SAFE_FREE(uszBuffer);
				return FALSE;
			}

			ulFileLen += SCF_MOD_NAME_LEN;
			if (b1KBndLoadMod == TRUE)
			{
				if ((ulFileLen%SCF_XMD_PAGE_SIZE) != 0)
				{
					ulFileLen = (ulFileLen / SCF_XMD_PAGE_SIZE + 1)*SCF_XMD_PAGE_SIZE;
				}
			}

			//ulBundleLen+=SCF_MOD_NAME_LEN;

		}
		//image文件带头的
		if (iEveryImgHdrCnt == 1)
		{
			if (b1KBndLoadMod == TRUE)
			{
				if ((ulFileLen%SCF_XMD_PAGE_SIZE) != 0)
				{
					ulFileLen = (ulFileLen / SCF_XMD_PAGE_SIZE + 1)*SCF_XMD_PAGE_SIZE;

				}
			}
			memcpy(pstImgHdrInfoList[i].szImageHeaderBuffer[0], (CHAR*)uszBuffer, SCF_MOD_NAME_LEN);
		}

		ulBundleLen += ulFileLen;
		ulFileCountLen = 0;
		iEveryImgHdrCnt = 0;


		SAFE_FREE(uszBuffer);
		SAFE_FCLOSE(pFile);

	}

	ulBundleHeaderLen = SCF_MOD_NAME_LEN*iFileNum;
	ulBundleHeaderLen += SCF_BNDL_HDR_FIRST_PART;
	//包头为1K模式时
	if (b1KBndLoadMod == TRUE)
	{
		if (0 != (ulBundleHeaderLen % SCF_XMD_PAGE_SIZE))
		{
			ulBundleHeaderLen = ((ulBundleHeaderLen / SCF_XMD_PAGE_SIZE) + 1) * SCF_XMD_PAGE_SIZE;
		}
	}
	ulBndHdrLen = ulBundleHeaderLen;
	ulBundleLen += ulBundleHeaderLen;
	if (b1KBndLoadMod == TRUE)
	{
		if (0 != (ulBundleLen% SCF_XMD_PAGE_SIZE))
		{
			ulBundleLen = (ulBundleLen / SCF_XMD_PAGE_SIZE + 1)* SCF_XMD_PAGE_SIZE;
		}
	}
	ulBndLen = ulBundleLen;

	return TRUE;


}