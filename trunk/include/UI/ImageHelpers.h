#ifndef __IMAGE_HELPER_H
#define __IMAGE_HELPER_H


// ����ͼ��
void ProcessImage(HBITMAP image);
HBITMAP CopyBitmap(HBITMAP hSourcehBitmap);


// ����ͼƬ
HBITMAP AtlLoadGdiplusImage(const void *mem, size_t len);
HICON AtlLoadGdiplusIcon(const void *mem, size_t len);
HBITMAP AtlLoadGdiplusImage(LPCTSTR lpszPath);
HBITMAP AtlLoadGdiplusImage(UINT id, LPCTSTR type);

// ��ȡ����ͼ
HBITMAP GetBitmapThumbnail(HBITMAP bmp, size_t width, size_t height);

// ͼ����ת��
void Bitmap2Buffer(HBITMAP bitmap, void **mem, size_t &size);
HICON Bitmap2Icon(HBITMAP bitmap);
HBITMAP Icon2Bitmap(HICON icon);



// ��ȡ���ڱ���ɫ
HBRUSH AtlGetBackgroundBrush(HWND hWnd, HWND hwndParent);


#endif 

