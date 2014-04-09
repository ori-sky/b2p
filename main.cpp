#include <stdio.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <GdiPlus.h>
#include "FileStream.h"

using namespace Gdiplus;

#pragma comment(lib, "Gdiplus.lib")

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	GetImageEncodersSize(&num, &size);
	if(size == 0) return -1;

	ImageCodecInfo *info = (ImageCodecInfo *)malloc(size);
	if(!info) return -1;

	GetImageEncoders(num, size, info);

	for(UINT i=0; i<num; ++i)
	{
		if(wcscmp(info[i].MimeType, format) == 0)
		{
			*pClsid = info[i].Clsid;
			free(info);
			return i;
		}
	}
}

int main(int argc, char **argv)
{
	// startup
	ULONG_PTR gt;
	GdiplusStartupInput gsi;
	GdiplusStartup(&gt, &gsi, NULL);

	std::istreambuf_iterator<char> eos;
	std::string s = std::string(std::istreambuf_iterator<char>(std::cin), eos);
	const char *sz = s.c_str();

	PBITMAPFILEHEADER bf = (PBITMAPFILEHEADER)sz;
	PBITMAPINFOHEADER bi = (PBITMAPINFOHEADER)(sz + sizeof(BITMAPFILEHEADER));

	HBITMAP hBmp = CreateBitmap(bi->biWidth, bi->biHeight, bi->biPlanes, bi->biBitCount, sz + bf->bfOffBits);
	if(!hBmp) return 1;

	Bitmap *bitmap = Bitmap::FromHBITMAP(hBmp, NULL);
	if(!bitmap) return 2;

	bitmap->RotateFlip(RotateNoneFlipY);

	CLSID clsid;
	int res = GetEncoderClsid(L"image/png", &clsid);

	Status status = bitmap->Save(new FileStream(GetStdHandle(STD_OUTPUT_HANDLE)), &clsid);

	// cleanup
	delete bitmap;
	GdiplusShutdown(gt);
	return 0;
}