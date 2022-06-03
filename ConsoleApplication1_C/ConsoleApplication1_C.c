#define	_CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>

void SaveBitmapAsFile(HDC hMemoryDC)
{
	const HBITMAP hbmScreen = GetCurrentObject(hMemoryDC, OBJ_BITMAP);
	if (hbmScreen) {
		BITMAP bmpScreen;
		if (GetObjectW(hbmScreen, sizeof bmpScreen, &bmpScreen)) {
			const WORD biBitCount = 24;
			const DWORD dwBmpSize = ((bmpScreen.bmWidth * biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
			char* const lpvBits = malloc(dwBmpSize);
			if (lpvBits) {
				BITMAPINFO info =
				{ /*BITMAPINFOHEADER    bmiHeader;*/
					{ /*DWORD      biSize         ;*/sizeof info.bmiHeader
					, /*LONG       biWidth        ;*/bmpScreen.bmWidth 
					, /*LONG       biHeight       ;*/bmpScreen.bmHeight
					, /*WORD       biPlanes       ;*/1         
					, /*WORD       biBitCount     ;*/biBitCount
					, /*DWORD      biCompression  ;*/BI_RGB
					, /*DWORD      biSizeImage    ;*/0
					, /*LONG       biXPelsPerMeter;*/0
					, /*LONG       biYPelsPerMeter;*/0
					, /*DWORD      biClrUsed      ;*/0
					, /*DWORD      biClrImportant ;*/0
					}
				, /*RGBQUAD             bmiColors[1]*/
					{ 0
					}
				};
				if (GetDIBits
				( /*_In_                                     HDC          hdc    */hMemoryDC
				, /*_In_                                     HBITMAP      hbm    */hbmScreen
				, /*_In_                                     UINT         start  */0
				, /*_In_                                     UINT         cLines */(UINT)bmpScreen.bmHeight
				, /*_Out_opt_                                LPVOID       lpvBits*/lpvBits
				, /*_At_((LPBITMAPINFOHEADER)lpbmi, _Inout_) LPBITMAPINFO lpbmi  */&info
				, /*_In_                                     UINT         usage  */DIB_RGB_COLORS
				)) {
					// A file is created, this is where we will save the screen capture.
					FILE* const fp = fopen("captureqwsx.bmp", "wb");
					if (fp) {
						// Add the size of the headers to the size of the bitmap to get the total file size.
						const BITMAPFILEHEADER   bmfHeader =
						{ /*WORD    bfType     ;*/MAKEWORD('B', 'M')
						, /*DWORD   bfSize     ;*/(DWORD)(sizeof bmfHeader + sizeof info.bmiHeader + dwBmpSize)
						, /*WORD    bfReserved1;*/0
						, /*WORD    bfReserved2;*/0
						, /*DWORD   bfOffBits  ;*/(DWORD)(sizeof bmfHeader + sizeof info.bmiHeader)
						};
						(void)fwrite(&bmfHeader     , sizeof bmfHeader     , 1, fp);
						(void)fwrite(&info.bmiHeader, sizeof info.bmiHeader, 1, fp);
						(void)fwrite(lpvBits        , dwBmpSize            , 1, fp);
						(void)fclose(fp);
					}
					else
						fprintf(stderr, "fopen function failed.\n");
				}
				free(lpvBits);
			}
			else
				fprintf(stderr, "malloc function failed.\n");
		}
		else
			fprintf(stderr, "GetObjectW function failed.\n");
	}
	else
		fprintf(stderr, "GetCurrentObject function failed.\n");
}

int main()
{
	const HDC hScreenDC = GetDC(NULL);
	if (hScreenDC) {
		const HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
		if (hMemoryDC) {
			const int cxScreen = GetDeviceCaps(hScreenDC, HORZRES);
			const int cyScreen = GetDeviceCaps(hScreenDC, VERTRES);
			const HBITMAP hBitmap = CreateCompatibleBitmap
			( hScreenDC
			, cxScreen
			, cyScreen
			);
			if (hBitmap) {
				const HGDIOBJ hPrevBitmap = SelectObject(hMemoryDC, hBitmap);
				if (BitBlt
				( /*_In_     HDC   hdc   */hMemoryDC
				, /*_In_     int   x     */0
				, /*_In_     int   y     */0
				, /*_In_     int   cx    */cxScreen
				, /*_In_     int   cy    */cyScreen
				, /*_In_opt_ HDC   hdcSrc*/hScreenDC
				, /*_In_     int   x1    */0
				, /*_In_     int   y1    */0
				, /*_In_     DWORD rop   */SRCCOPY
				))
					SaveBitmapAsFile(hMemoryDC);
				(void)SelectObject(hMemoryDC, hPrevBitmap);
				(void)DeleteObject(hBitmap);
			}
			(void)DeleteDC(hMemoryDC);
		}
		(void)ReleaseDC(NULL, hScreenDC);

	}
}
