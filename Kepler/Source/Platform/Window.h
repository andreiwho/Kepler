#pragma once
#include "Core/Types.h"
#include <string>

namespace Kepler
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

		inline bool IsCloseRequested() const { return bCloseRequested; }
		void SetTitle(const TString& newTitle);
		inline const TString& GetTitle() const { return Title; }
		inline const TString& GetName() const { return Name; }
		inline i32 GetWidth() const { return Width; }
		inline i32 GetHeight() const { return Height; }
		inline const TWindowParams& GetParams() const { return Params; }
		void SetMaximized(bool bNewMaximized);
		void SetDecorated(bool bNewDecorated);
		void SetFullscreen(bool bNewFullscreen);

		virtual void* GetNativeHandle() const = 0;

	protected:
		bool bCloseRequested = false;
		i32 Width{};
		i32 Height{};

		virtual void SetTitle_Impl(const TString& newTitle) = 0;
		virtual void SetMaximized_Impl(bool bNewMaximized) = 0;
		virtual void SetDecorated_Impl(bool bNewDecorated) = 0;
		virtual void SetFullscreen_Impl(bool bNewFullscreen) = 0;

	private:
		TString Name{};
		TString Title{};
		TWindowParams Params{};
	};
}