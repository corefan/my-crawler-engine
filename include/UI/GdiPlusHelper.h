/****************************************************** 
*	Title		: GdiPlus Helper 
*	Description	: 利用单件模式，自动初始化和卸载GDIPlus
*	author		: 陈煜
*	date		: 2009/08/18
*	version		: 1.0 
*******************************************************/ 



#ifndef __GDIPLUS_HELPER_H
#define __GDIPLUS_HELPER_H

// fixed VC6
#define ULONG_PTR ULONG

#ifndef max
#define max std::max
#endif

#ifndef min
#define min std::min
#endif

#include <comdef.h>
#include <GdiPlus.h>
#include <cassert>
using namespace Gdiplus;

#pragma comment(lib, "GdiPlus")


#ifndef TRACE
#define TRACE //
#endif



/**
 *	Single Object
 */

class GdiPlusHelper
{
	// VC6 Bug!! 不支持单件模式
public:
	GdiPlusHelper()
	{
		Init();
	}

	~GdiPlusHelper()
	{
		Uninit();
	}


public:
	static GdiPlusHelper &GetInstance()
	{
		return gdiPlus;
	}

	//-------------------------------------------------------------------------
	// Function Name    :GetEncoderClsid
	// Parameter(s)     :const  WCHAR*  format			格式类型
	//					:CLSID*  pClsid					in/out 
	// Return           :bool							是否成功
	// Create			:2009-6-24 陈煜
	// Memo             :保存图片为指定格式
	//-------------------------------------------------------------------------
	static bool GetEncoderClsid(const  WCHAR*  format,  CLSID*  pClsid)  
	{  
		//-----------------设置EncoderParameters---------------------------------------
//		if(strType == L"image/jpeg")
//		{
//			*pValue = 75;   //jpg的压缩率1 - 100
//			pEncoderParameters->Count = 1; 
//			pEncoderParameters->Parameter[0].Guid = EncoderQuality; 
//			pEncoderParameters->Parameter[0].Type = EncoderParameterValueTypeLong;
//			pEncoderParameters->Parameter[0].NumberOfValues = 1; 
//			pEncoderParameters->Parameter[0].Value = pValue; 
//		}
//		else if(strType == L"image/bmp")
//		{
//			*pValue = 24;   //24位图
//			pEncoderParameters->Count = 1; 
//			pEncoderParameters->Parameter[0].Guid = EncoderColorDepth; 
//			pEncoderParameters->Parameter[0].Type = EncoderParameterValueTypeLong;
//			pEncoderParameters->Parameter[0].NumberOfValues = 1; 
//			pEncoderParameters->Parameter[0].Value = pValue; 
//		}


		UINT num = 0;	//  number  of  image  encoders  
		UINT size = 0;  //  size  of  the  image  encoder  array  in  bytes  

		ImageCodecInfo*  pImageCodecInfo  =  NULL;  

		GetImageEncodersSize(&num,  &size);  
		if( size  ==  0 )  
			return  false;    //  Failure  

		pImageCodecInfo  =  (ImageCodecInfo*)(malloc(size)); 
		
		if( pImageCodecInfo  ==  NULL )  
			return false;    //  Failure  

		GetImageEncoders(num,  size,  pImageCodecInfo);  

		for(UINT  j  =  0;  j  <  num;  ++j)  
		{  
			if(  wcscmp(pImageCodecInfo[j].MimeType,  format)  ==  0  )  
			{  
				*pClsid  =  pImageCodecInfo[j].Clsid;  
				free(pImageCodecInfo);  
				return  true;    //  Success  
			}          
		}  

		free(pImageCodecInfo);  
		return  false;    //  Failure  
	}  

	//-------------------------------------------------------------------------
	// Function Name    :ImageFromMemory
	// Parameter(s)     :const void *pBuf				内存缓冲区  
	//					:size_t size					缓冲区大小 
	// Return           :Image *						图片指针
	// Create			:2009-6-24 陈煜
	// Memo             :从内存读取数据转换为图片，需要调用delete image
	//-------------------------------------------------------------------------
	static Bitmap *ImageFromMemory(const void *pBuf, size_t size)
	{
		IStream *stream		= NULL;
		Bitmap *image		= NULL;
		HGLOBAL global		= NULL;

		__try
		{
			__try
			{
				global= ::GlobalAlloc(GMEM_FIXED, size );
				if( global == NULL )
				{
					__leave;
				}

				// get the IStream from the global object 
				if( ::CreateStreamOnHGlobal( global, TRUE, &stream ) != S_OK )
				{
					__leave;
				}

				// copy the buf content to the HGLOBAL 
				if( !_MemToGlobal( pBuf, size, global ) )
				{
					__leave;
				}
				
				// create the image from the stream 
				image = Bitmap::FromStream(stream);
			}
			__finally
			{
				if( stream != NULL )
					stream->Release();

				if( image == NULL )
				{
					::GlobalFree( global );
				}
				
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			TRACE("==Image From Memory Error : %d!==\n", GetExceptionCode());
		}

		return image;

		/* i suppose when the reference count for stream is 0, it will 
		GlobalFree automatically. The Image maintain the object also.*/	
		//::GlobalFree(global);
	}

	//-------------------------------------------------------------------------
	// Function Name    :ImageToMemory
	// Parameter(s)     :Image *pImage					需要转换图片指针
	//					:void **pOutBuf					返回转换后内存缓冲区
	//					:size_t *size					返回缓冲区大小
	//					:LPCWSTR pFormat				转换图像格式
	// Return           :Image *						图片指针
	// Create			:2009-6-24 陈煜
	// Memo             :把图片转换到内存，需要释放(*pOutBuf)
	//-------------------------------------------------------------------------
	static void *ImageToMemory(Image *pImage, void **pOutBuf, size_t *size, LPCWSTR pFormat, ULONG uQuality = 100)
	{
		IStream *stream = NULL;

		__try
		{
			__try
			{
				if( CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)&stream) != S_OK )
				{
	 				__leave;
				}

				CLSID clsid;
				GetEncoderClsid( pFormat/*L"image/jpeg"*/, &clsid );

				// Setup encoder parameters
				EncoderParameters encoderParameters;
				encoderParameters.Count = 1;
				encoderParameters.Parameter[0].Guid = EncoderQuality;
				encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
				encoderParameters.Parameter[0].NumberOfValues = 1;

				// setup compression level
				ULONG quality = uQuality;
				encoderParameters.Parameter[0].Value = &quality;

				Status SaveStatus = pImage->Save(stream, &clsid, &encoderParameters);
				if( SaveStatus != Ok ) 
				{
					__leave;
				}
				
				// get the size of the stream
				ULARGE_INTEGER ulnSize;
				LARGE_INTEGER lnOffset;
				lnOffset.QuadPart = 0;
				if( stream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize) != S_OK )
				{
					__leave;
				}

				// now move the pointer to the begining of the file
				if( stream->Seek(lnOffset, STREAM_SEEK_SET, NULL) != S_OK )
				{
					__leave;
				}
			
				*pOutBuf = static_cast<void *>(new size_t[(unsigned int)ulnSize.QuadPart]);
				//*pOutBuf = CCYMemoryPool::GetInstance().Allocate((size_t)ulnSize.QuadPart);
				if( *pOutBuf == NULL )
					__leave;

				ULONG ulBytesRead;
				if( stream->Read(*pOutBuf, (ULONG)ulnSize.QuadPart, &ulBytesRead) != S_OK )
				{
					__leave;
				}

				*size = (size_t)ulnSize.QuadPart;
			}
			__finally
			{
				if( stream != NULL )
					stream->Release();
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			TRACE("==Image To Memory Error : %d!==\n", GetExceptionCode());
		}

		return *pOutBuf;
	}

private:
	static bool _MemToGlobal(const void *buf, size_t size, HGLOBAL global)
	{
		__try
		{
			__try
			{
				void *dest = ::GlobalLock(global);
				if( dest == NULL )
				{
					__leave;
				}

				::memcpy(dest, buf, size);
			}
			__finally
			{
				::GlobalUnlock(global);
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			TRACE("==Gloabl Handle Exception!\n==");
		}

		return true;
	}

	//static bool _StreamToMem(IStream *stream, void **outbuf, size_t *size)
	//{
	//	ULARGE_INTEGER ulnSize;
	//	LARGE_INTEGER lnOffset;
	//	lnOffset.QuadPart = 0;

	//	/* get the stream size */
	//	if( stream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize) != S_OK )
	//	{
	//		return false;
	//	}

	//	if( stream->Seek(lnOffset, STREAM_SEEK_SET, NULL) != S_OK )
	//	{
	//		return false;
	//	}

	//	/* read it */
	//	*outbuf = malloc( (size_t)ulnSize.QuadPart );
	//	//*outbuf = CCYMemoryPool::GetInstance().Allocate((size_t)ulnSize.QuadPart);

	//	*size = (size_t) ulnSize.QuadPart;
	//	TRACE1("_StreamToMem size -- %d\n",*size);
	//	
	//
	//	ULONG bytesRead = 0;

	//	if( stream->Read(*outbuf, (ULONG)ulnSize.QuadPart, &bytesRead) != S_OK )
	//	{
	//		//CCYMemoryPool::GetInstance().Deallocate(*outbuf);
	//		free(*outbuf);
	//		return false;
	//	}

	//	if( ulnSize.QuadPart != bytesRead )
	//	{
	//		TRACE0("==Read Size not allow really size!==\n");
	//		return false;
	//	}

	//	return true;
	//}

public:
	//-------------------------------------------------------------------------
	// Function Name    :ExtractResourceToFile
	// Parameter(s)     :UINT nResID			资源ID
	//					:LPCTSTR lpszFilename	文件名
	//					:HMODULE hModule		模块名
	// Return           :BOOL					是否成功
	// Create			:2009-6-24 陈煜
	// Memo             :将资源释放到文件
	//-------------------------------------------------------------------------
	bool ExtractResourceToFile(LPCTSTR lpszType, UINT nResID, LPCTSTR lpszFilename, HMODULE hModule)
	{
		HRSRC hRes = ::FindResource( hModule, MAKEINTRESOURCE(nResID), lpszType);
		if( hRes == NULL )
		{
			assert(FALSE);
			return false;
		}

		DWORD dwSize = ::SizeofResource( hModule, hRes); 
		if( dwSize == 0 )
		{
			assert(FALSE);
			return false;
		}

		HGLOBAL hGlobal = ::LoadResource( hModule, hRes); 
		if( hGlobal == NULL )
		{
			assert(FALSE);
			return false;
		}

		LPVOID pBuffer = ::LockResource(hGlobal); 
		if( pBuffer == NULL )
		{
			assert(FALSE);
			::FreeResource(hGlobal); 
			return false;
		}

		HANDLE hFile = ::CreateFile( lpszFilename
			, GENERIC_WRITE
			, FILE_SHARE_WRITE | FILE_SHARE_READ
			, NULL
			, CREATE_ALWAYS
			, 0
			, NULL
			);
		if( hFile == NULL )
		{
			assert(FALSE);
			::FreeResource(hGlobal); 
			return false;
		}

		DWORD dwWritten = 0;
		::WriteFile( hFile, pBuffer, dwSize, &dwWritten, NULL);
		if( dwWritten != dwSize )
		{
			assert(FALSE);
			::FreeResource(hGlobal); 
			return false;
		}

		::FlushFileBuffers(hFile);
		::CloseHandle(hFile);
		::FreeResource(hGlobal); 

		return true;
	}// ExtractResourceToFile


	//-------------------------------------------------------------------------
	// Function Name    :LoadFromResource
	// Parameter(s)     :LPCTSTR pName			资源名称
	//					:LPCTSTR pType			资源类型名称
	//					:HMODULE hInst			模块名
	//					:Bitmap *pBitmap		图片
	// Return           :BOOL					是否成功
	// Create			:2009-6-24 陈煜
	// Memo             :将资源加载到GDIPLUS
	//-------------------------------------------------------------------------
	static Bitmap *LoadFromResource(UINT resID, LPCTSTR pType, HMODULE hInst)
	{
		LPCTSTR pName = MAKEINTRESOURCE(resID);

		HRSRC hResource = ::FindResource(hInst, pName, pType);
		if (!hResource)
			return false;

		DWORD imageSize = ::SizeofResource(hInst, hResource);
		if (!imageSize)
			return false;

		const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
		if (!pResourceData)
			return false;

		Bitmap *pBitmap = NULL;
		HGLOBAL hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
		if( hBuffer )
		{
			void* pBuffer = ::GlobalLock(hBuffer);
			if( pBuffer )
			{
				CopyMemory(pBuffer, pResourceData, imageSize);

				IStream* pStream = NULL;
				if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
				{
					pBitmap = Gdiplus::Bitmap::FromStream(pStream);
					pStream->Release();
					if( pBitmap )
					{ 
						if (pBitmap->GetLastStatus() != Gdiplus::Ok)
						{
							::GlobalUnlock(hBuffer);
							::GlobalFree(hBuffer);
							return NULL;
						}
					}
				}
				::GlobalUnlock(hBuffer);
			}
			::GlobalFree(hBuffer);
		}

		return pBitmap;
	}

private:
	void Init()
	{
		GdiplusStartupInput stGdiPlusStartupInput;
		GdiplusStartup(&m_ulGdiPlusToken, &stGdiPlusStartupInput, NULL);
	}

	void Uninit()
	{
		GdiplusShutdown(m_ulGdiPlusToken);
	}

private:
	static GdiPlusHelper gdiPlus;
	ULONG_PTR m_ulGdiPlusToken;
};




#endif