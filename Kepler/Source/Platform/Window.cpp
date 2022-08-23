#include "Window.h"

namespace Kepler
{
	TWindow::TWindow(i32 width, i32 height, const TString& title, const TWindowParams& params)
		:	Width(width), Height(height), Title(title)
	{
	}

	void TWindow::SetTitle(const TString& newTitle)
	{
		SetTitle_Impl(newTitle);
		Title = newTitle;
	}

	void TWindow::SetMaximized(bool bNewMaximized)
	{
		SetMaximized_Impl(bNewMaximized);
		Params.bMaximized = bNewMaximized;
	}

	void TWindow::SetDecorated(bool bNewDecorated)
	{
		SetDecorated_Impl(bNewDecorated);
		Params.bDecorated = bNewDecorated;
	}

	void TWindow::SetFullscreen(bool bNewFullscreen)
	{
		SetFullscreen_Impl(bNewFullscreen);
		Params.bFullscreen = bNewFullscreen;
	}

}