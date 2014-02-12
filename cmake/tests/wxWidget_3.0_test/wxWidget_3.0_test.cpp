#include <wx/bitmap.h>
#include <wx/version.h>

#if !(wxMAJOR_VERSION==3 && wxMINOR_VERSION==0)
#  error "you need wxWidgets-3.0 installed"
#endif

int main()
{
	wxBitmap bitmap;
	return bitmap.GetSize().GetWidth();
}
