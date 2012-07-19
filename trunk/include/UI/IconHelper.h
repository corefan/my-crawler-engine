#ifndef __ICON_HELPER_HPP
#define __ICON_HELPER_HPP

#include "GdiplusHelper.h"
#include "../Extend STL/UnicodeStl.h"



inline void InitBmpInfoHeader(HBITMAP Bitmap, BITMAPINFOHEADER& BI, WORD nBit)   
{   
	int Bytes		= 0;   
	DIBSECTION DS	= {0};   
	DS.dsBmih.biSize= 0;   

	Bytes = ::GetObject(Bitmap, sizeof(DS), &DS);   
	if ( Bytes >= sizeof(DS.dsBm) + sizeof(DS.dsBmih) && 
		DS.dsBmih.biSize >= sizeof(DS.dsBmih))   
	{
		::memcpy(&BI, &DS.dsBmih, sizeof(BITMAPINFOHEADER));   
	}
	else   
	{   
		::memset(&BI, 0, sizeof(BI));   
		BI.biSize = sizeof(BI);   
		BI.biWidth = DS.dsBm.bmWidth;   
		BI.biHeight = DS.dsBm.bmHeight;   
		BI.biBitCount = DS.dsBm.bmPlanes * DS.dsBm.bmBitsPixel;   
	}   

	if( nBit != 0 )   
		BI.biBitCount = nBit;   
	if( BI.biBitCount <= 8 )   
		BI.biClrUsed = 1 << BI.biBitCount;   

	BI.biPlanes = 1;   

	if( BI.biClrImportant > BI.biClrUsed )   
		BI.biClrImportant = BI.biClrUsed;   
	if( BI.biSizeImage == 0 )   
		BI.biSizeImage = ((BI.biWidth * BI.biBitCount + 31) / 32) * 4 * BI.biHeight;   
}   

inline void GetDIBSizes(HBITMAP Bitmap, DWORD& InfoSize, DWORD& ImageSize, WORD nBit)   
{   
	BITMAPINFOHEADER BI = {0};   

	InitBmpInfoHeader(Bitmap, BI, nBit);   
	InfoSize = sizeof(BITMAPINFOHEADER);

	if( BI.biBitCount > 8 )   
	{   
		if  ( BI.biCompression & BI_BITFIELDS)   
			InfoSize += 12;   
	}
	else
		InfoSize += sizeof(RGBQUAD) * (BI.biClrUsed !=0 ? BI.biClrUsed: (1 << BI.biBitCount)); 

	ImageSize   =   BI.biSizeImage;   
}   

inline void GetDIB(HBITMAP Bitmap, BITMAPINFO* BmpInfo, void* Bits, WORD nBit)
{   
	HDC dc = ::CreateCompatibleDC(NULL);   

	InitBmpInfoHeader(Bitmap, BmpInfo->bmiHeader, nBit);   
	GetDIBits(dc, Bitmap, 0, BmpInfo->bmiHeader.biHeight, Bits, BmpInfo, DIB_RGB_COLORS);

	::DeleteDC(dc);   
}   

struct TCursorOrIcon
{   
	WORD   Reserved;   
	WORD   wType;   
	WORD   Count;   
};   

struct TIconRec
{   
	BYTE   Width;   
	BYTE   Height;   
	WORD   Colors;   
	WORD   Reserved1;   
	WORD   Reserved2;   
	long   DIBSize;   
	long   DIBOffset;   
};   


inline void SaveIconToBuffer(HICON hIcon, BYTE *buffer, size_t& dwSize)
{
	if( hIcon == NULL )
		return;

	BYTE* pData = buffer;

	ICONINFO   IconInfo = {0}; 
	IconInfo.fIcon = TRUE;

	::GetIconInfo(hIcon,   &IconInfo);   

	BITMAPINFO *MonoInfo	= 0;
	void *MonoBits			= 0;
	DWORD MonoInfoSize		= 0, MonoBitsSize = 0;

	BITMAPINFO *ColorInfo	= 0;
	void *ColorBits			= 0;	
	DWORD ColorInfoSize		= 0, ColorBitsSize = 0;

	__try
	{
		if( IconInfo.hbmMask )
		{
			GetDIBSizes(IconInfo.hbmMask,   MonoInfoSize,   MonoBitsSize,   1);   
			MonoInfo = (BITMAPINFO*)new char[MonoInfoSize];   
			MonoBits = new char [MonoBitsSize];   
			GetDIB(IconInfo.hbmMask, MonoInfo, MonoBits, 1);   
		}


		if( IconInfo.hbmColor)  
		{
			GetDIBSizes(IconInfo.hbmColor,   ColorInfoSize,   ColorBitsSize,   32);   
			ColorInfo	=   (BITMAPINFO*)new char[ColorInfoSize];   
			ColorBits   =   new char [ColorBitsSize];   
			GetDIB(IconInfo.hbmColor,   ColorInfo,   ColorBits,   32);   
		}

		TCursorOrIcon CI = {0};   
		CI.wType	= 0x0001;   
		CI.Count	= 1;   

		::memcpy(pData, &CI, sizeof(CI));
		pData += sizeof(CI);

		TIconRec   List = {0};   
		List.Width   =   (BYTE)MonoInfo->bmiHeader.biWidth;   
		List.Height  =   (BYTE)MonoInfo->bmiHeader.biHeight;
		if (ColorInfo) 
		{
			List.Colors  =   ColorInfo->bmiHeader.biPlanes * ColorInfo->bmiHeader.biBitCount;   
			List.DIBSize =   ColorInfoSize + ColorBitsSize + MonoBitsSize;   
		} 
		else 
		{
			List.Colors  = MonoInfo->bmiHeader.biPlanes * MonoInfo->bmiHeader.biBitCount;   
			List.DIBSize = MonoInfoSize + MonoBitsSize;   
		}
		List.DIBOffset = sizeof(CI) + sizeof(List);   

		::memcpy(pData, &List, sizeof(List));
		pData += sizeof(List);

		if (ColorInfo)
		{
			ColorInfo->bmiHeader.biHeight   *=   2;   
			::memcpy(pData, ColorInfo, ColorInfoSize);
			pData += ColorInfoSize;
			::memcpy(pData, ColorBits, ColorBitsSize);
			pData += ColorBitsSize;
		}
		else 
		{
			::memcpy(pData, MonoInfo, MonoInfoSize);
			pData += MonoInfoSize;
		}

		::memcpy(pData, MonoBits, MonoBitsSize);
		pData += MonoBitsSize;

		if (IconInfo.hbmMask)
			::DeleteObject(IconInfo.hbmMask);   
		if (IconInfo.hbmColor)
			::DeleteObject(IconInfo.hbmColor);  

	}
	__finally
	{
		if( ColorInfo != NULL )
			delete []ColorInfo;

		if( MonoInfo != NULL )
			delete []MonoInfo;   

		if( ColorBits != NULL )
			delete []ColorBits;   

		if( MonoBits != NULL )
			delete []MonoBits;   
	}


	dwSize = pData - buffer;

}

inline HICON LoadIconFromFile(LPCTSTR lpszPath)
{
	HICON hIcon = NULL;

	static const stdex::tString extend = _T("ico");
	stdex::tString strExtend = lpszPath + (_tcslen(lpszPath) - extend.length());

	struct Euqal
	{
		bool operator()(TCHAR c1, TCHAR c2) const
		{ return ::tolower(c1) == ::tolower(c2); }
	};
	if( std::equal(extend.begin(), extend.end(), strExtend.begin(), Euqal()) )
		hIcon = reinterpret_cast<HICON>(::LoadImage(0, lpszPath, IMAGE_ICON, 
		0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE));
	else
		hIcon = ::ExtractIcon(::GetModuleHandle(NULL), lpszPath, 0);

	return hIcon;
}

inline HICON LoadIconFromBuffer(const unsigned char *buf, DWORD dwSize)
{
	/*std::auto_ptr<Bitmap> img(GdiPlusHelper::ImageFromMemory(buf, dwSize));

	HICON icon = 0;
	img->GetHICON(&icon);

	return icon;*/

	if( dwSize == 0 || buf == NULL )
		return NULL;

	TCHAR path[MAX_PATH]	= {0};
	HANDLE hFile			= INVALID_HANDLE_VALUE;
	DWORD dwWriteBytes		= 0;

	__try
	{
		::GetSystemDirectory(path, MAX_PATH);
		_tcscat_s(path, _T("\\i8Icon.ico"));
		::DeleteFile(path);

		hFile = ::CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
			__leave;


		::WriteFile(hFile, buf, dwSize, &dwWriteBytes, NULL);
		if( dwWriteBytes != dwSize )
			__leave;

	}
	__finally
	{
		if( hFile != INVALID_HANDLE_VALUE )
			::CloseHandle(hFile);
	}

	if( hFile == INVALID_HANDLE_VALUE || dwWriteBytes != dwSize )
		return NULL;

	return (HICON)::LoadImage(NULL, path, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);



	//HICON hIcon = NULL;     

	//// Ahhh, this is the magic API.     
	//PBYTE tmpBuf = (PBYTE)buf;
	//int offset = ::LookupIconIdFromDirectoryEx(tmpBuf, TRUE, dwSize, dwSize, LR_DEFAULTCOLOR);     

	//if (offset != 0)    
	//	hIcon = ::CreateIconFromResourceEx(tmpBuf + offset, 0, TRUE, 0x30000, dwSize, dwSize, LR_DEFAULTCOLOR);        

	//return hIcon;
}	


// 保存Icon到文件
inline bool SaveIconToFile(HICON hIconToSave, LPCTSTR sIconFileName)   
{   
	size_t dwSize = 0;
	BYTE buffer[10240] = {0};

	SaveIconToBuffer(hIconToSave, buffer, dwSize);

	HANDLE hFile = NULL;

	__try
	{
		hFile = ::CreateFile(sIconFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
			return false;

		DWORD dwWriteBytes = 0;
		::WriteFile(hFile, buffer, dwSize, &dwWriteBytes, NULL);
		if( dwWriteBytes != dwSize )
			return false;
	}
	__finally
	{
		if( hFile != NULL )
			::CloseHandle(hFile);
	}

	return true;
}  

#endif