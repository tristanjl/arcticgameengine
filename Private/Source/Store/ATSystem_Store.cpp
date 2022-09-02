#include "AT.h"
#include "ATSystem.h"
#include "ATSystem_Internal.h"
#include "ATInputCursor_Internal.h"

#include <ppltasks.h>

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

AT_API void ATRender_InitialiseWindow(CoreWindow^ window);

ref class ATApp sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
	ATApp() :
		m_windowClosed(false),
		m_windowVisible(true)
	{
	}

	// IFrameworkView Methods.
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView)
	{
		applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &ATApp::OnActivated);
		CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &ATApp::OnSuspending);
		CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &ATApp::OnResuming);
	}

	virtual void SetWindow(Windows::UI::Core::CoreWindow^ window)
	{
		window->SizeChanged += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &ATApp::OnWindowSizeChanged);
		window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &ATApp::OnVisibilityChanged);
		window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &ATApp::OnWindowClosed);
		window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
		window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ATApp::OnPointerPressed);
		window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ATApp::OnPointerReleased);
		window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ATApp::OnPointerMoved);
		window->PointerExited += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ATApp::OnPointerExited);
		window->PointerWheelChanged += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ATApp::OnPointerWheelChanged);


		ATRender_InitialiseWindow(window);

		ATSystem_InitialiseSystems();
	}

	virtual void Load(Platform::String^ entryPoint)
	{
	}

	virtual void Run()
	{
		while (!m_windowClosed)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
			if (m_windowVisible)
			{
				ATSystem_Update();
			}
		}
	}

	virtual void Uninitialize()
	{
		ATSystem_DeinitialiseSystems();
	}

protected:
	// Event Handlers.
	void OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
	{
		//m_renderer->UpdateForWindowSizeChange();
	}

	void OnLogicalDpiChanged(Platform::Object^ sender);
	void OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
	{
		CoreWindow::GetForCurrentThread()->Activate();
	}

	void OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
	{
		// Save app state asynchronously after requesting a deferral. Holding a deferral
		// indicates that the application is busy performing suspending operations. Be
		// aware that a deferral may not be held indefinitely. After about five seconds,
		// the app will be forced to exit.
		SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

		concurrency::create_task([this, deferral]()
		{
			// Insert your code here.

			deferral->Complete();
		});
	}

	void OnResuming(Platform::Object^ sender, Platform::Object^ args)
	{
		// Restore any data or state that was unloaded on suspend. By default, data
		// and state are persisted when resuming from suspend. Note that this event
		// does not occur if the app was previously terminated.
	}

	void OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
	{
		m_windowClosed = true;
	}

	void OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
	{
		m_windowVisible = args->Visible;
	}

	void OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
	{
		if (args->CurrentPoint->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_1, 0, args->CurrentPoint->Properties->IsLeftButtonPressed ? 1.0f : 0.0f);
			ATInputCursor_UpdateMouseState(ATIMI_Button_2, 0, args->CurrentPoint->Properties->IsRightButtonPressed ? 1.0f : 0.0f);
			ATInputCursor_UpdateMouseState(ATIMI_Button_3, 0, args->CurrentPoint->Properties->IsMiddleButtonPressed ? 1.0f : 0.0f);
			ATInputCursor_UpdateMouseState(ATIMI_Position_X, 0, args->CurrentPoint->Position.X);
			ATInputCursor_UpdateMouseState(ATIMI_Position_Y, 0, args->CurrentPoint->Position.Y);
		}
		else if (args->CurrentPoint->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Touch)
		{
			uint32 pointerId = args->CurrentPoint->PointerId;
			ATInputCursor_CreateTouchState(pointerId, args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
		}
	}

	void OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
	{
		if (args->CurrentPoint->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
		{
			ATInputCursor_UpdateMouseState(ATIMI_Button_1, 0, args->CurrentPoint->Properties->IsLeftButtonPressed ? 1.0f : 0.0f);
			ATInputCursor_UpdateMouseState(ATIMI_Button_2, 0, args->CurrentPoint->Properties->IsRightButtonPressed ? 1.0f : 0.0f);
			ATInputCursor_UpdateMouseState(ATIMI_Button_3, 0, args->CurrentPoint->Properties->IsMiddleButtonPressed ? 1.0f : 0.0f);
			ATInputCursor_UpdateMouseState(ATIMI_Position_X, 0, args->CurrentPoint->Position.X);
			ATInputCursor_UpdateMouseState(ATIMI_Position_Y, 0, args->CurrentPoint->Position.Y);
		}
		else if (args->CurrentPoint->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Touch)
		{
			uint32 pointerId = args->CurrentPoint->PointerId;
			ATInputCursor_UpdateTouchState(ATITI_Active, pointerId, 0.0f);
			ATInputCursor_UpdateTouchState(ATITI_Position_X, pointerId, args->CurrentPoint->Position.X);
			ATInputCursor_UpdateTouchState(ATITI_Position_Y, pointerId, args->CurrentPoint->Position.Y);
		}
	}

	void OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
	{
		if (args->CurrentPoint->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
		{
			ATInputCursor_UpdateMouseState(ATIMI_Position_X, 0, args->CurrentPoint->Position.X);
			ATInputCursor_UpdateMouseState(ATIMI_Position_Y, 0, args->CurrentPoint->Position.Y);
		}
		else if (args->CurrentPoint->PointerDevice->PointerDeviceType == Windows::Devices::Input::PointerDeviceType::Touch)
		{
			uint32 pointerId = args->CurrentPoint->PointerId;
			ATInputCursor_UpdateTouchState(ATITI_Position_X, pointerId, args->CurrentPoint->Position.X);
			ATInputCursor_UpdateTouchState(ATITI_Position_Y, pointerId, args->CurrentPoint->Position.Y);
		}
	}

	void OnPointerExited(CoreWindow^ sender, PointerEventArgs^ args)
	{
	}

	void OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ args)
	{
		ATInputCursor_UpdateMouseState(ATIMI_Mouse_Wheel, 0, (float)args->CurrentPoint->Properties->MouseWheelDelta);
	}

private:
	//CubeRenderer^ m_renderer;
	bool m_windowClosed;
	bool m_windowVisible;
};

ref class ATApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
	{
		return ref new ATApp();
	}
};

AT_API void ATSystem_InitPlatformSpecific(ATSystemInitData*)
{
}

AT_API void ATSystem_Main()
{
	auto applicationSource = ref new ATApplicationSource();
	Windows::ApplicationModel::Core::CoreApplication::Run(applicationSource);
}

AT_API void ATSystem_DisplayScreen()
{
	void ATRender_Present(); ATRender_Present();
}

AT_API void ATSystem_DeinitPlatformSpecific()
{
}
