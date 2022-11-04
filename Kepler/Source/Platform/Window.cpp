#include "Window.h"

namespace ke
{
	TWindow::TWindow(i32 width, i32 height, const String& title, const TWindowParams& params)
		:	m_Width(width), m_Height(height), m_Title(title), m_Name(title)
	{
	}

	void TWindow::SetTitle(const String& newTitle)
	{
		SetTitle_Impl(newTitle);
		m_Title = newTitle;
	}

	void TWindow::SetMaximized(bool bNewMaximized)
	{
		SetMaximized_Impl(bNewMaximized);
		m_Params.bMaximized = bNewMaximized;
	}

	void TWindow::SetDecorated(bool bNewDecorated)
	{
		SetDecorated_Impl(bNewDecorated);
		m_Params.bDecorated = bNewDecorated;
	}

	void TWindow::SetFullscreen(bool bNewFullscreen)
	{
		SetFullscreen_Impl(bNewFullscreen);
		m_Params.bFullscreen = bNewFullscreen;
	}

}