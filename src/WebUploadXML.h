//
// sproxy.exe generated file
// do not modify this file
//
// Created: 08/04/2011@08:38:27
//

#pragma once


#if !defined(_WIN32_WINDOWS) && !defined(_WIN32_WINNT) && !defined(_WIN32_WCE)
#pragma message("warning: defining _WIN32_WINDOWS = 0x0410")
#define _WIN32_WINDOWS 0x0410
#endif

#include <atlsoap.h>

namespace Atd_UploadMessage
{

template <typename TClient = CSoapSocketClientT<> >
class CAtd_UploadMessageT : 
	public TClient, 
	public CSoapRootHandler
{
protected:

	const _soapmap ** GetFunctionMap();
	const _soapmap ** GetHeaderMap();
	void * GetHeaderValue();
	const wchar_t * GetNamespaceUri();
	const char * GetServiceName();
	const char * GetNamespaceUriA();
	HRESULT CallFunction(
		void *pvParam, 
		const wchar_t *wszLocalName, int cchLocalName,
		size_t nItem);
	HRESULT GetClientReader(ISAXXMLReader **ppReader);

public:

	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv)
	{
		if (ppv == NULL)
		{
			return E_POINTER;
		}

		*ppv = NULL;

		if (InlineIsEqualGUID(riid, IID_IUnknown) ||
			InlineIsEqualGUID(riid, IID_ISAXContentHandler))
		{
			*ppv = static_cast<ISAXContentHandler *>(this);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	ULONG __stdcall AddRef()
	{
		return 1;
	}

	ULONG __stdcall Release()
	{
		return 1;
	}

	CAtd_UploadMessageT(ISAXXMLReader *pReader = NULL)
		:TClient(_T("http://172.18.1.101/amts/atd_uploadmessage.asmx"))
	{
		SetClient(true);
		SetReader(pReader);
	}
	
	~CAtd_UploadMessageT()
	{
		Uninitialize();
	}
	
	void Uninitialize()
	{
		UninitializeSOAP();
	}	

	HRESULT Upload(
		BSTR xmlContent, 
		BSTR* UploadResult
	);
};

typedef CAtd_UploadMessageT<> CAtd_UploadMessage;

struct __CAtd_UploadMessage_Upload_struct
{
	BSTR xmlContent;
	BSTR UploadResult;
};

extern __declspec(selectany) const _soapmapentry __CAtd_UploadMessage_Upload_entries[] =
{

	{
		0x5A2857CC, 
		"xmlContent", 
		L"xmlContent", 
		sizeof("xmlContent")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_IN | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CAtd_UploadMessage_Upload_struct, xmlContent),
		NULL,
		NULL,
		-1,
	},
	{
		0x2D24A9C4, 
		"UploadResult", 
		L"UploadResult", 
		sizeof("UploadResult")-1, 
		SOAPTYPE_STRING, 
		SOAPFLAG_NONE | SOAPFLAG_OUT | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL | SOAPFLAG_NULLABLE,
		offsetof(__CAtd_UploadMessage_Upload_struct, UploadResult),
		NULL,
		NULL,
		-1,
	},
	{ 0x00000000 }
};

extern __declspec(selectany) const _soapmap __CAtd_UploadMessage_Upload_map =
{
	0x089D4B14,
	"Upload",
	L"UploadResponse",
	sizeof("Upload")-1,
	sizeof("UploadResponse")-1,
	SOAPMAP_FUNC,
	__CAtd_UploadMessage_Upload_entries,
	sizeof(__CAtd_UploadMessage_Upload_struct),
	1,
	-1,
	SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
	0xC2E575C3,
	"http://tempuri.org/",
	L"http://tempuri.org/",
	sizeof("http://tempuri.org/")-1
};

extern __declspec(selectany) const _soapmap * __CAtd_UploadMessage_funcs[] =
{
	&__CAtd_UploadMessage_Upload_map,
	NULL
};

template <typename TClient>
inline HRESULT CAtd_UploadMessageT<TClient>::Upload(
		BSTR xmlContent, 
		BSTR* UploadResult
	)
{
    if ( UploadResult == NULL )
		return E_POINTER;

	HRESULT __atlsoap_hr = InitializeSOAP(NULL);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_INITIALIZE_ERROR);
		return __atlsoap_hr;
	}
	
	CleanupClient();

	CComPtr<IStream> __atlsoap_spReadStream;
	__CAtd_UploadMessage_Upload_struct __params;
	memset(&__params, 0x00, sizeof(__params));
	__params.xmlContent = xmlContent;

	__atlsoap_hr = SetClientStruct(&__params, 0);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_OUTOFMEMORY);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = GenerateResponse(GetWriteStream());
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_GENERATE_ERROR);
		goto __skip_cleanup;
	}
	
	__atlsoap_hr = SendRequest(_T("SOAPAction: \"http://tempuri.org/Upload\"\r\n"));
	if (FAILED(__atlsoap_hr))
	{
		goto __skip_cleanup;
	}
	__atlsoap_hr = GetReadStream(&__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_READ_ERROR);
		goto __skip_cleanup;
	}
	
	// cleanup any in/out-params and out-headers from previous calls
	Cleanup();
	__atlsoap_hr = BeginParse(__atlsoap_spReadStream);
	if (FAILED(__atlsoap_hr))
	{
		SetClientError(SOAPCLIENT_PARSE_ERROR);
		goto __cleanup;
	}

	*UploadResult = __params.UploadResult;
	goto __skip_cleanup;
	
__cleanup:
	Cleanup();
__skip_cleanup:
	ResetClientState(true);
	memset(&__params, 0x00, sizeof(__params));
	return __atlsoap_hr;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CAtd_UploadMessageT<TClient>::GetFunctionMap()
{
	return __CAtd_UploadMessage_funcs;
}

template <typename TClient>
ATL_NOINLINE inline const _soapmap ** CAtd_UploadMessageT<TClient>::GetHeaderMap()
{
	static const _soapmapentry __CAtd_UploadMessage_Upload_atlsoapheader_entries[] =
	{
		{ 0x00000000 }
	};

	static const _soapmap __CAtd_UploadMessage_Upload_atlsoapheader_map = 
	{
		0x089D4B14,
		"Upload",
		L"UploadResponse",
		sizeof("Upload")-1,
		sizeof("UploadResponse")-1,
		SOAPMAP_HEADER,
		__CAtd_UploadMessage_Upload_atlsoapheader_entries,
		0,
		0,
		-1,
		SOAPFLAG_NONE | SOAPFLAG_PID | SOAPFLAG_DOCUMENT | SOAPFLAG_LITERAL,
		0xC2E575C3,
		"http://tempuri.org/",
		L"http://tempuri.org/",
		sizeof("http://tempuri.org/")-1
	};


	static const _soapmap * __CAtd_UploadMessage_headers[] =
	{
		&__CAtd_UploadMessage_Upload_atlsoapheader_map,
		NULL
	};
	
	return __CAtd_UploadMessage_headers;
}

template <typename TClient>
ATL_NOINLINE inline void * CAtd_UploadMessageT<TClient>::GetHeaderValue()
{
	return this;
}

template <typename TClient>
ATL_NOINLINE inline const wchar_t * CAtd_UploadMessageT<TClient>::GetNamespaceUri()
{
	return L"http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline const char * CAtd_UploadMessageT<TClient>::GetServiceName()
{
	return NULL;
}

template <typename TClient>
ATL_NOINLINE inline const char * CAtd_UploadMessageT<TClient>::GetNamespaceUriA()
{
	return "http://tempuri.org/";
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CAtd_UploadMessageT<TClient>::CallFunction(
	void *, 
	const wchar_t *, int,
	size_t)
{
	return E_NOTIMPL;
}

template <typename TClient>
ATL_NOINLINE inline HRESULT CAtd_UploadMessageT<TClient>::GetClientReader(ISAXXMLReader **ppReader)
{
	if (ppReader == NULL)
	{
		return E_INVALIDARG;
	}
	
	CComPtr<ISAXXMLReader> spReader = GetReader();
	if (spReader.p != NULL)
	{
		*ppReader = spReader.Detach();
		return S_OK;
	}
	return TClient::GetClientReader(ppReader);
}

} // namespace Atd_UploadMessage
