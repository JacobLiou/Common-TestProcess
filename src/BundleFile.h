// CreateFileDlg.h : header file
//

#if !defined(AFX_CREATEFILEDLG_H__AD2F304D_B3E4_4C1F_ABAF_742C1709C981__INCLUDED_)
#define AFX_CREATEFILEDLG_H__AD2F304D_B3E4_4C1F_ABAF_742C1709C981__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		COLOR_DARKGRAY			(RGB(190,190,190))
/////////////////////////////////////////////////////////////////////////////
#define		SCF_BNDL_FILE_NAME		("./module_bundle.bin")
#define		SCF_FILE_LINE_MAX_LEN   (512)
#define		NULL_DWORD				(0xFFFFFFFF)
#define		SCF_MOD_VER_LEN			(16)   

#define		SCF_MOD_NAME_LEN		(32) 
#define		SCF_MOD_NAME_LEN_2		(256)  
#define		MAX_LIST_LINES			(256)  
#define		SCF_BUDL_HDR_IMAGE		(32) 
#define		SCF_BUDL_HDR_PNSNSEL	(0)    

//
#define     SCF_ARRAY_LEN        (5)
#define     SCF_CMPPN_SN             (15)
#define     SCF_BNDL_HDR_FIRST_PART  (128)
#define     SCF_IMG_VERSION_LEN      (14) 
#define     SCF_ONE_BYTE_MAX_VALUE   (255)
#define     SCF_TWO_BYTE_MAX_VALUE   (65535)  
#define     SCF_IMG_VER_LAST_TWO_BYTE  (2)   
#define     SCF_UNCHAR_MIN_VALUE       (0)
#define     SCF_IMGTYPE_MIN_VALUE    (1)  
#define     SCF_IMG_LEN_ADDR         (24)   
#define     SCF_FOUR_BYTE           (4) 
//#define		POLYNOMIAL              0x04C11DB7L
#define     SCF_FPGA_FLAG_NUM       (9)


/* Xmodem page size. */
#define		SCF_XMD_PAGE_SIZE       (1024)  
#define		SCF_MAX_IMG_CNT			(1000)  

/*The BundleTag of bundle                                                    */
/***DEF-**********************************************************************/
#define   SCF_BNDL_BNDTAG           ("OPLINK")
#define   SCF_PN_CFGFILE_DESCRP     ("[PN_TYPE]=")
#define   SCF_SN_CFGFILE_DESCRP      ("[SN_TYPE]=")
#define   SCF_TAG_CFGFILE_DESCRP      ("[TAG]=")
#define   SCF_END_CFGFILE_DESCRP      ("#END")
#define   SCF_START_CFGFILE_DESCRP     ("#START")
#define    SCT_IBFH_DESCRP           ("IBFH")
/* Release resource.                                                         */
/***DEF-**********************************************************************/
#define SAFE_FREE(ptr)        do{free(ptr); (ptr) = NULL;}while(0)
#define SAFE_FCLOSE(ptr)      do{fclose(ptr); (ptr) = NULL;}while(0)
#define	RETURNLENS(m_str)		(strlen(m_str)+1)
/***STRU+*********************************************************************/
/* The image information.                                                    */
/***STRU-*********************************************************************/
typedef struct SCF_IMG_INFO_st {
	ULONG ulImgLen;                /* Length of image in bundle. */    
    ULONG ulCRC32;                 /* CRC32 of image. */    
	char  aszName[256];//[SCF_MOD_NAME_LEN];  /* Name of this image. */
	char  aszPN[SCF_MOD_NAME_LEN];	/* PN */
	char  aszSN[SCF_MOD_NAME_LEN];	/* SN */	         
    char  aszImageTag[SCF_ARRAY_LEN];         /* Tag of image. */
	UCHAR  uszImageType;                                       /*Image Tag of this image.*/
    UCHAR  uszImageHitless;            /* Hitless flag. */
	UCHAR  uszStorageID;            /*Storage ID of this image.*/
	UCHAR  uszImageIndex;         /*Image Index of this image.*/
	UCHAR  aszImageVersion[SCF_ARRAY_LEN];      /*Image Version of this image.*/
	char  aszImageTimeStamp[SCF_ARRAY_LEN];
	DWORD dwImageBaseAddr;       /*Image Base Addr of this image;*/
	unsigned char  uszSectorCnt;       /* Sector Count of image in bundle. */ 
	int iFileIndex;         
} SCF_IMG_INFO_ST,*LPSCF_IMG_INFO_ST;

typedef struct SCF_IMG_HEADER_INFO_st
{
	int iEveryImageHeaderCnt;
	BOOL bisFPGAFile;
	char szImageHeaderBuffer[SCF_MAX_IMG_CNT][SCF_MOD_NAME_LEN];
}SCF_IMG_HEADER_INFO_ST, *PSCF_IMG_HEADER_INFO_ST;

//the first 1K RAM information
typedef enum {
	IDX_HDR_BNDTAG=0,
    IDX_HDR_PRODTYPE=8,
    IDX_HDR_BNDCRC32=16,
	IDX_HDR_BNDSIZE=20,
	IDX_HDR_BNDHDRSIZE=24,
	IDX_HDR_IMGCNT=26,
	IDX_HDR_BNDVER=32,
	IDX_HDR_BNDPN=48,
	IDX_HDR_BNDSN=80,
	IDX_HDR_BNDTIME=112,
	IDX_HDR_IMGTAG=128,
    IDX_HDR_IMGTYPE=132,
	IDX_HDR_IMGHITLESS=133,
	IDX_HDR_IMASTORAGEID=134,
	IDX_HDR_IMGINDEX=135,
	IDX_HDR_IMGVER=136,
	IDX_HDR_IMGTIME=140,
	IDX_HDR_IMGADDR=144,
	IDX_HDR_IMGCRC32=148,
	IDX_HDR_IMGLEN=152,
	IDX_HDR_SECCNT=156,
    IDX_HDR_END

   
} IDX_HDR_EN;


class BundleFile 
{
public:
/*************************************************
	 1.  函数名：ReadCfgFile
		函数功能：读配置文件
		形参：	strFileAdd：配置文件路径
		piFileNum：返回文件数量
		pstImgInfoList：返回从读配置文件得到的信息
		aszErrMsg：返回错误信息
*************************************************/
		/*************************************************
		2.函数名：GetEveryImageFileHeader
		函数功能：求image file header的个数
		形参：       iFileNum 文件数量
		ulBndLen 返回包的大小
		ulBndHdrLen 返回包头的大小
		b1KBndLoadMod：TRUE为1K的下载模式，FALSE为非1K的下载模式，人为设置
		pstImgInfoList：文件信息，从读配置文件得到的信息
		pstImgHdrInfoList：返回文件头信息
		aszErrMsg：返回错误信息
		*************************************************/
		
		/*************************************************
		3.函数名：BundleFiles
		函数功能：打包文件
		形参：    iFileNum 文件数量
		ulBndLen 包的大小
		ulBndHdrLen 包头的大小
		strBndVer：打包文件的版本号
		strProdType：产品类型
		b1KBndLoadMod：TRUE为1K的下载模式，FALSE为非1K的下载模式，人为设置
		pstImgInfoList：文件信息，从读配置文件得到的信息
		pstImgHdrInfoList：返回文件头信息
		aszErrMsg：返回错误信息
	*************************************************/
	BOOL GetEveryImageFileHeader(int iFileNum, ULONG &ulBndLen,ULONG &ulBndHdrLen,BOOL b1KBndLoadMod,LPSCF_IMG_INFO_ST pstImgInfoList,PSCF_IMG_HEADER_INFO_ST pstImgHdrInfoList,char* aszErrMsg);
	ULONG CalcCRC32(ULONG ulInitCRC,UCHAR* uszDataBuf,ULONG ulDataLen);
	ULONG GetFileLength(const char* pszFileName,char* aszErrMsg);
	BOOL ReadCfgFile(CString strFileAdd,int* piFileNum,LPSCF_IMG_INFO_ST pstImgInfoList,char* aszErrMsg);
	BOOL BundleFiles(int iFileNum,ULONG ulBndLen,ULONG ulBndHdrLen,CString strBndVer,CString strProdType,BOOL b1KBndLoadMod,LPSCF_IMG_INFO_ST pstImgInfoList,PSCF_IMG_HEADER_INFO_ST pstImgHdrInfoList, char* aszErrMsg);
	BundleFile();
	virtual ~BundleFile();
};
#endif // !defined(AFX_BUNDLEFILE_H__F9114A79_BCBC_4DB5_8968_67C1680724B4__INCLUDED_)