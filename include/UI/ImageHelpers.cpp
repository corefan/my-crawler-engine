#include "stdafx.h"
#include "ImageHelpers.h"

#define _WTL_NO_AUTOMATIC_NAMESPACE
#define _CSTRING_NS

#include <memory>
#include <functional>
#include <shlobj.h>
#include <memory>
#include <stdexcept>
#include <atlbase.h>
#include "wtl/atlapp.h"
#include "wtl/atlmisc.h"

#include "GdiplusHelper.h"


void ProcessImage(HBITMAP image)
{
	return;
	DIBSECTION dibsection = {0};
	int nBytes = 0;
	
	nBytes = ::GetObject( image, sizeof( DIBSECTION ), &dibsection );
	if( nBytes == sizeof( DIBSECTION ) )
	{
		size_t width = dibsection.dsBmih.biWidth;
		size_t height = abs( dibsection.dsBmih.biHeight );
		size_t BPP = dibsection.dsBmih.biBitCount;
		if( BPP != 32 )
			return;

		void *bits = dibsection.dsBm.bmBits;
		size_t pitch = (((width * BPP) + 31) / 32) * 4;
		for(size_t i = 0; i != width; ++i)
		{
			for(size_t j = 0; j < height; ++j)
			{
				LPBYTE pucColor = LPBYTE( bits )+(j*pitch)+((i*BPP)/8);
				pucColor[0] = pucColor[0] * pucColor[3] / 255;
				pucColor[1] = pucColor[1] * pucColor[3] / 255;
				pucColor[2] = pucColor[2] * pucColor[3] / 255;
			}
		}
	}
	else
		assert(0);
}

namespace detail
{
	template < typename FuncT >
	inline void LoadGdiplusImage(const void *mem, size_t len, const FuncT &func)
	{
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, len);
		void *pData = ::GlobalLock(hGlobal);
		memcpy(pData, mem, len);
		::GlobalUnlock(hGlobal);

		IStream *pStream = NULL;
		if( ::CreateStreamOnHGlobal(hGlobal,TRUE, &pStream) == S_OK )
		{
			std::auto_ptr<Bitmap> bm(Bitmap::FromStream(pStream));
			func(bm.get());

			pStream->Release();
		}

		::GlobalFree(hGlobal);
	}
}


HBITMAP AtlLoadGdiplusImage(const void *mem, size_t len)
{
	
	struct Covert
	{
		void Translate(HBITMAP &hBitmap, Bitmap *bitmap)
		{
			bitmap->GetHBITMAP(0, &hBitmap);
			assert(hBitmap != 0);
		}
	}covert;
		
	HBITMAP hBitmap = 0;
	detail::LoadGdiplusImage(mem, len, std::tr1::bind(&Covert::Translate, &covert, std::tr1::ref(hBitmap), std::tr1::placeholders::_1));

	return hBitmap;
}

HICON AtlLoadGdiplusIcon(const void *mem, size_t len)
{
	struct Covert
	{
		void Translate(HICON &icon, Bitmap *bitmap)
		{
			bitmap->GetHICON(&icon);
			assert(icon != 0);
		}
	}covert;

	HICON icon = 0;
	detail::LoadGdiplusImage(mem, len, std::tr1::bind(&Covert::Translate, &covert, std::tr1::ref(icon), std::tr1::placeholders::_1));

	return icon;
}


HBITMAP AtlLoadGdiplusImage(LPCTSTR lpszPath)
{
	std::auto_ptr<Bitmap> pBitmap(new Bitmap(CT2W(lpszPath))); 

	HBITMAP hBitmap = 0;
	pBitmap->GetHBITMAP(0, &hBitmap); 

	return hBitmap;
}

HBITMAP AtlLoadGdiplusImage(UINT id, LPCTSTR type)
{
	std::auto_ptr<Bitmap> pBitmap(GdiPlusHelper::LoadFromResource(id, type, AfxGetInstanceHandle()));
	HBITMAP hBitmap = 0;
	pBitmap->GetHBITMAP(0, &hBitmap); 

	return hBitmap;
}


HBITMAP GetBitmapThumbnail(HBITMAP bmp, size_t width, size_t height)
{
	/*HICON icon = Bitmap2Icon(bmp);
	std::auto_ptr<Bitmap> pBitmap(Bitmap::FromHICON(icon)); 
	std::auto_ptr<Image> src(pBitmap->GetThumbnailImage(width, height));

	void *outBuf = 0;
	size_t outLen = 0;
	GdiPlusHelper::ImageToMemory(src.get(), &outBuf, &outLen, L"image/bmp");

	std::auto_ptr<Bitmap> tmp(GdiPlusHelper::ImageFromMemory(outBuf, outLen));
	
	HBITMAP destBitmap = 0;
	tmp->GetHBITMAP(0, &destBitmap);
	delete []outBuf;*/

	return bmp;
}

void Bitmap2Buffer(HBITMAP bitmap, void **mem, size_t &size)
{
	std::auto_ptr<Bitmap> pBitmap(new Bitmap(bitmap, 0)); 
	GdiPlusHelper::ImageToMemory(pBitmap.get(), mem, &size, L"image/png");
}

HICON Bitmap2Icon(HBITMAP bitmap)
{
	/*BITMAP bitmapInfo = {0};
	assert(::GetObject(bitmap, sizeof(BITMAP), &bitmapInfo) != 0);

	CImageList imglist;
	imglist.Create(bitmapInfo.bmWidth, bitmapInfo.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 0);
	int nIndex = imglist.Add(CBitmap::FromHandle(bitmap), RGB(0, 0, 0));

	return imglist.ExtractIcon(nIndex);*/

	BITMAP bmp = {0};
    if (::GetObject(bitmap, sizeof(BITMAP), &bmp) == 0)
    {
        assert(0 && "´íÎóµÄÎ»Í¼¾ä±ú");
		return 0;
    }
	ICONINFO ii = {0};
	ii.fIcon	= TRUE;
	ii.xHotspot = bmp.bmWidth / 2;
	ii.yHotspot = bmp.bmHeight / 2;
	ii.hbmColor = bitmap;
	ii.hbmMask	= bitmap;

	HICON hIcon = ::CreateIconIndirect(&ii);

	return hIcon;
}

HBITMAP Icon2Bitmap(HICON icon)
{
	assert(icon != 0);

	std::auto_ptr<Bitmap> tmpBitmap(Bitmap::FromHICON(icon));
	HBITMAP bmp = 0;

	tmpBitmap->GetHBITMAP(0, &bmp);
	return bmp;
}

HBITMAP CopyBitmap(HBITMAP hSourcehBitmap)
{
	HBITMAP tmp = (HBITMAP)::CopyImage(hSourcehBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	ProcessImage(tmp);
	return tmp;
}


HBRUSH AtlGetBackgroundBrush(HWND hWnd, HWND hwndParent)
{
	ATL::CWindow wnd = hWnd;
	ATL::CWindow wndParent = hwndParent;
	WTL::CClientDC dcParent = wndParent;
	WTL::CRect rcParent;
	wndParent.GetWindowRect(&rcParent);
	WTL::CDC dcCompat1;
	dcCompat1.CreateCompatibleDC(dcParent);
	WTL::CBitmap bmpCompat1;
	bmpCompat1.CreateCompatibleBitmap(dcParent, rcParent.Width(), rcParent.Height());
	HBITMAP hOldBmp1 = dcCompat1.SelectBitmap(bmpCompat1);
	wndParent.SendMessage(WM_ERASEBKGND, (WPARAM) (HDC) dcCompat1, 0);
	wndParent.SendMessage(WM_PRINTCLIENT, (WPARAM) (HDC) dcCompat1, (LPARAM)(PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT));
	WTL::CRect rcWin;
	wnd.GetWindowRect(&rcWin);
	WTL::CDC dcCompat2;
	dcCompat2.CreateCompatibleDC();
	WTL::CBitmap bmpCompat2;
	bmpCompat2.CreateCompatibleBitmap(dcCompat1, rcWin.Width(), rcWin.Height());
	HBITMAP hOldBmp2 = dcCompat2.SelectBitmap(bmpCompat2);
	WTL::CRect rcSnap = rcWin;
	::MapWindowPoints(NULL, wndParent, (LPPOINT) (LPRECT) &rcSnap, 2);
	dcCompat2.BitBlt(0, 0, rcWin.Width(), rcWin.Height(), dcCompat1, rcSnap.left, rcSnap.top, SRCCOPY);
	HBRUSH hBrush = ::CreatePatternBrush(bmpCompat2);
	dcCompat1.SelectBitmap(hOldBmp1);
	dcCompat2.SelectBitmap(hOldBmp2);
	return hBrush;
}