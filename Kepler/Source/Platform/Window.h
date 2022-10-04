#pragma once
#include "Core/Types.h"
#include <string>

namespace ke
{
	struct TWindowParams
	{
		ubyte bMaximized : 1 = false;
		ubyte bDecorated : 1 = true;
		ubyte bFullscreen : 1 = false;
	};

	class TWindow
	{
	public:
		TWindow(i32 width, i32 height, const TString& title, const TWindowParams& params = {});
		virtual ~TWindow() = default;

		inline bool IsCloseRequested() const { return m_bCloseRequested; }
		void SetTitle(const TString& newTitle);
		inline const TString& GetTitle() const { return m_Title; }
		inline const TString& GetName() const { return m_Name; }
		inline i32 GetWidth() const { return m_Width; }
		inline i32 GetHeight() const { return m_Height; }
		inline const TWindowParams& GetParams() const { return m_Params; }
		void SetMaximized(bool bNewMaximized);
		void SetDecorated(bool bNewDecorated);
		void SetFullscreen(bool bNewFullscreen);
		virtual void SetCursorPosition(float2 NewPosition) = 0;

		virtual void* GetNativeHandle() const = 0;

	protected:
		bool m_bCloseRequested = false;
		i32 m_Width{};
		i32 m_Height{};

		virtual void SetTitle_Impl(const TString& newTitle) = 0;
		virtual void SetMaximized_Impl(bool bNewMaximized) = 0;
		virtual void SetDecorated_Impl(bool bNewDecorated) = 0;
		virtual void SetFullscreen_Impl(bool bNewFullscreen) = 0;

	private:
		TString m_Name{};
		TString m_Title{};
		TWindowParams m_Params{};
	};
}