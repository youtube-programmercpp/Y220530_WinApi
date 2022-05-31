#include <Windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")
#include <stdexcept>
#include <iostream>
#include <memory>
namespace Gdiplus {
	namespace Encoder {
		class __declspec(uuid("{557cf400-1a04-11d3-9a73-0000f81ef32e}")) BMP;
		class __declspec(uuid("{557cf401-1a04-11d3-9a73-0000f81ef32e}")) JPG;
		class __declspec(uuid("{557cf402-1a04-11d3-9a73-0000f81ef32e}")) GIF;
		class __declspec(uuid("{557cf405-1a04-11d3-9a73-0000f81ef32e}")) TIF;
		class __declspec(uuid("{557cf406-1a04-11d3-9a73-0000f81ef32e}")) PNG;
	}
}
void Screenshot()
{
	if (const std::unique_ptr<std::remove_pointer_t<HDC>, void(*)(HDC)>  hScreenDC
		{ GetDC(nullptr)
		,  [](HDC hDC)
			{
				(void)ReleaseDC(nullptr, hDC);
			}
		}) {
		Gdiplus::Bitmap bitmap
		{ GetDeviceCaps(hScreenDC.get(), HORZRES)
		, GetDeviceCaps(hScreenDC.get(), VERTRES)
		};
		Gdiplus::Graphics graphics(&bitmap);
		const auto hdc = graphics.GetHDC();
		const auto succeeded = BitBlt
		( /*_In_     HDC   hdc   */hdc
		, /*_In_     int   x     */0
		, /*_In_     int   y     */0
		, /*_In_     int   cx    */bitmap.GetWidth ()
		, /*_In_     int   cy    */bitmap.GetHeight()
		, /*_In_opt_ HDC   hdcSrc*/hScreenDC.get()
		, /*_In_     int   x1    */0
		, /*_In_     int   y1    */0
		, /*_In_     DWORD rop   */SRCCOPY
		);
		graphics.ReleaseHDC(hdc);
		if (succeeded) {
			if (bitmap.Save(L"Screen.jpg", &__uuidof(Gdiplus::Encoder::JPG)) != Gdiplus::Ok)
				throw std::runtime_error("Bitmap::Save function failed.");
		}
		else
			throw std::runtime_error("BitBlt function failed.");
	}
	else
		throw std::runtime_error("GetDC function failed.");
}
int main()
{
	using namespace Gdiplus;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	try {
		Screenshot();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	GdiplusShutdown(gdiplusToken);
}
