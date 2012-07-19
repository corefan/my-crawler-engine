#ifndef __IMAGE_HELPER_H
#define __IMAGE_HELPER_H


// 处理图像
void ProcessImage(HBITMAP image);
HBITMAP CopyBitmap(HBITMAP hSourcehBitmap);


// 加载图片
HBITMAP AtlLoadGdiplusImage(const void *mem, size_t len);
HICON AtlLoadGdiplusIcon(const void *mem, size_t len);
HBITMAP AtlLoadGdiplusImage(LPCTSTR lpszPath);
HBITMAP AtlLoadGdiplusImage(UINT id, LPCTSTR type);

// 获取缩略图
HBITMAP GetBitmapThumbnail(HBITMAP bmp, size_t width, size_t height);

// 图像句柄转换
void Bitmap2Buffer(HBITMAP bitmap, void **mem, size_t &size);
HICON Bitmap2Icon(HBITMAP bitmap);
HBITMAP Icon2Bitmap(HICON icon);



// 获取窗口背景色
HBRUSH AtlGetBackgroundBrush(HWND hWnd, HWND hwndParent);


#endif 

