#include "AT.h"
#include "ATRender.h"
#include "ATRender_Internal.h"
#include "ATVector4.h"

#include <agile.h>
#include <wrl/client.h>

struct ATModelViewProjectionConstantBuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct ATDirect3D
{
	ATDirect3D()
	{
	}

	// Direct3D Objects.
	Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	ATModelViewProjectionConstantBuffer m_constantBufferData;

	// Cached renderer properties.
	D3D_FEATURE_LEVEL m_featureLevel;
	Windows::Foundation::Size m_renderTargetSize;
	Windows::Foundation::Rect m_windowBounds;
	Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
	Windows::Graphics::Display::DisplayOrientations m_orientation;

	// Transform used for display orientation.
	DirectX::XMFLOAT4X4 m_orientationTransform3D;
};

ATDirect3D* g_Renderer;

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;


static void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw Platform::Exception::CreateException(hr);
	}
}

static float ConvertDipsToPixels(float dips)
{
	static const float dipsPerInch = 96.0f;
	return floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
}

//ref class ATDirect3D
//{
//  void HandleDeviceLost()
//  {
//    // Reset these member variables to ensure that UpdateForWindowSizeChange recreates all resources.
//    m_windowBounds.Width = 0;
//    m_windowBounds.Height = 0;
//    m_swapChain = nullptr;
//
//    CreateDeviceResources();
//    UpdateForWindowSizeChange();
//  }
//
//  void UpdateForWindowSizeChange()
//  {
//    if (m_window->Bounds.Width  != m_windowBounds.Width ||
//      m_window->Bounds.Height != m_windowBounds.Height ||
//      m_orientation != DisplayProperties::CurrentOrientation)
//    {
//      ID3D11RenderTargetView* nullViews[] = {nullptr};
//      m_d3dContext->OMSetRenderTargets(ATARRAYSIZE(nullViews), nullViews, nullptr);
//      m_renderTargetView = nullptr;
//      m_depthStencilView = nullptr;
//      m_d3dContext->Flush();
//      CreateWindowSizeDependentResources();
//    }
//  }
//};

AT_API void ATRender_InitPlatformSpecific()
{
}

static void ATRender_CreateDeviceResources()
{
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, featureLevels, ATARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &g_Renderer->m_featureLevel, &context));

	// Get the Direct3D 11.1 API device and context interfaces.
	ThrowIfFailed(device.As(&g_Renderer->m_d3dDevice));
	ThrowIfFailed(context.As(&g_Renderer->m_d3dContext));

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ATModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
	ThrowIfFailed(device->CreateBuffer(&constantBufferDesc, nullptr, &g_Renderer->m_constantBuffer));
}

static void CreateWindowSizeDependentResources()
{
	// Store the window bounds so the next time we get a SizeChanged event we can
	// avoid rebuilding everything if the size is identical.
	g_Renderer->m_windowBounds = g_Renderer->m_window->Bounds;

	// Calculate the necessary swap chain and render target size in pixels.
	float windowWidth = ConvertDipsToPixels(g_Renderer->m_windowBounds.Width);
	float windowHeight = ConvertDipsToPixels(g_Renderer->m_windowBounds.Height);

	// The width and height of the swap chain must be based on the window's
	// landscape-oriented width and height. If the window is in a portrait
	// orientation, the dimensions must be reversed.
	g_Renderer->m_orientation = DisplayProperties::CurrentOrientation;
	bool swapDimensions =
		g_Renderer->m_orientation == DisplayOrientations::Portrait ||
		g_Renderer->m_orientation == DisplayOrientations::PortraitFlipped;
	g_Renderer->m_renderTargetSize.Width = swapDimensions ? windowHeight : windowWidth;
	g_Renderer->m_renderTargetSize.Height = swapDimensions ? windowWidth : windowHeight;

	if(g_Renderer->m_swapChain != nullptr)
	{
		// If the swap chain already exists, resize it.
		ThrowIfFailed(
			g_Renderer->m_swapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			static_cast<UINT>(g_Renderer->m_renderTargetSize.Width),
			static_cast<UINT>(g_Renderer->m_renderTargetSize.Height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
			)
			);
	}
	else
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
		swapChainDesc.Width = static_cast<UINT>(g_Renderer->m_renderTargetSize.Width); // Match the size of the window.
		swapChainDesc.Height = static_cast<UINT>(g_Renderer->m_renderTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
		swapChainDesc.Flags = 0;

		ComPtr<IDXGIDevice1>  dxgiDevice;
		ThrowIfFailed(
			g_Renderer->m_d3dDevice.As(&dxgiDevice)
			);

		ComPtr<IDXGIAdapter> dxgiAdapter;
		ThrowIfFailed(
			dxgiDevice->GetAdapter(&dxgiAdapter)
			);

		ComPtr<IDXGIFactory2> dxgiFactory;
		ThrowIfFailed(
			dxgiAdapter->GetParent(
			__uuidof(IDXGIFactory2), 
			&dxgiFactory
			)
			);

		Windows::UI::Core::CoreWindow^ window = g_Renderer->m_window.Get();
		ThrowIfFailed(
			dxgiFactory->CreateSwapChainForCoreWindow(
			g_Renderer->m_d3dDevice.Get(),
			reinterpret_cast<IUnknown*>(window),
			&swapChainDesc,
			nullptr, // Allow on all displays.
			&g_Renderer->m_swapChain
			)
			);

		// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
		// ensures that the application will only render after each VSync, minimizing power consumption.
		ThrowIfFailed(
			dxgiDevice->SetMaximumFrameLatency(1)
			);
	}

	// Set the proper orientation for the swap chain, and generate the
	// 3D matrix transformation for rendering to the rotated swap chain.
	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
	switch (g_Renderer->m_orientation)
	{
	case DisplayOrientations::Landscape:
		rotation = DXGI_MODE_ROTATION_IDENTITY;
		g_Renderer->m_orientationTransform3D = XMFLOAT4X4( // 0-degree Z-rotation
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
		break;

	case DisplayOrientations::Portrait:
		rotation = DXGI_MODE_ROTATION_ROTATE270;
		g_Renderer->m_orientationTransform3D = XMFLOAT4X4( // 90-degree Z-rotation
			0.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
		break;

	case DisplayOrientations::LandscapeFlipped:
		rotation = DXGI_MODE_ROTATION_ROTATE180;
		g_Renderer->m_orientationTransform3D = XMFLOAT4X4( // 180-degree Z-rotation
			-1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
		break;

	case DisplayOrientations::PortraitFlipped:
		rotation = DXGI_MODE_ROTATION_ROTATE90;
		g_Renderer->m_orientationTransform3D = XMFLOAT4X4( // 270-degree Z-rotation
			0.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
		break;

	default:
		throw ref new Platform::FailureException();
	}

	ThrowIfFailed(
		g_Renderer->m_swapChain->SetRotation(rotation)
		);

	// Create a render target view of the swap chain back buffer.
	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(
		g_Renderer->m_swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		&backBuffer
		)
		);

	ThrowIfFailed(
		g_Renderer->m_d3dDevice->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		&g_Renderer->m_renderTargetView
		)
		);

	// Create a depth stencil view.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT, 
		static_cast<UINT>(g_Renderer->m_renderTargetSize.Width),
		static_cast<UINT>(g_Renderer->m_renderTargetSize.Height),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
		);

	ComPtr<ID3D11Texture2D> depthStencil;
	ThrowIfFailed(
		g_Renderer->m_d3dDevice->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&depthStencil
		)
		);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	ThrowIfFailed(
		g_Renderer->m_d3dDevice->CreateDepthStencilView(
		depthStencil.Get(),
		&depthStencilViewDesc,
		&g_Renderer->m_depthStencilView
		)
		);

	// Set the rendering viewport to target the entire window.
	CD3D11_VIEWPORT viewport(
		0.0f,
		0.0f,
		g_Renderer->m_renderTargetSize.Width,
		g_Renderer->m_renderTargetSize.Height
		);

	g_Renderer->m_d3dContext->RSSetViewports(1, &viewport);

	float aspectRatio = g_Renderer->m_windowBounds.Width / g_Renderer->m_windowBounds.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// Note that the m_orientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.
	XMStoreFloat4x4(
		&g_Renderer->m_constantBufferData.projection,
		XMMatrixTranspose(
		XMMatrixMultiply(
		XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		),
		XMLoadFloat4x4(&g_Renderer->m_orientationTransform3D)
		)
		)
		);
}

AT_API void ATRender_InitialiseWindow(CoreWindow^ window)
{
	g_Renderer = ATNew(ATDirect3D);

	g_Renderer->m_window = window;

	ATRender_CreateDeviceResources();
	CreateWindowSizeDependentResources();
}

AT_API void ATRender_DeinitPlatformSpecific()
{
	ATDelete(g_Renderer);
}

AT_API void ATRender_SetOrthoView(float x, float y, float width, float height, float nearPlane, float farPlane)
{
	XMStoreFloat4x4(&g_Renderer->m_constantBufferData.projection, XMMatrixTranspose(XMMatrixMultiply(XMMatrixOrthographicOffCenterRH(x, x + width, y, y + height, nearPlane, farPlane), XMLoadFloat4x4(&g_Renderer->m_orientationTransform3D))));

	XMStoreFloat4x4(&g_Renderer->m_constantBufferData.view, XMMatrixIdentity());
	XMStoreFloat4x4(&g_Renderer->m_constantBufferData.model, XMMatrixIdentity());

	g_Renderer->m_d3dContext->UpdateSubresource(g_Renderer->m_constantBuffer.Get(), 0, NULL, &g_Renderer->m_constantBufferData, 0, 0);
	g_Renderer->m_d3dContext->VSSetConstantBuffers(0, 1, g_Renderer->m_constantBuffer.GetAddressOf());
	g_Renderer->m_d3dContext->OMSetRenderTargets(1, g_Renderer->m_renderTargetView.GetAddressOf(), g_Renderer->m_depthStencilView.Get());
}

AT_API void ATRender_Clear(ATClearFlags flags, const ATVector4& clearColor, float depth, int stencil)
{
	if (flags & ATCF_Color)
	{
		g_Renderer->m_d3dContext->ClearRenderTargetView(g_Renderer->m_renderTargetView.Get(), clearColor.f);
	}

	if (flags & (ATCF_Depth | ATCF_Stencil))
	{
		UINT clearFlags = ((flags & ATCF_Depth) ? D3D11_CLEAR_DEPTH : 0) | ((flags & ATCF_Stencil) ? D3D11_CLEAR_STENCIL : 0);
		g_Renderer->m_d3dContext->ClearDepthStencilView(g_Renderer->m_depthStencilView.Get(), clearFlags, depth, (UINT8)stencil);
	}
}

AT_API void ATRender_Present()
{
	// The application may optionally specify "dirty" or "scroll"
	// rects to improve efficiency in certain scenarios.
	DXGI_PRESENT_PARAMETERS parameters = {0};
	parameters.DirtyRectsCount = 0;
	parameters.pDirtyRects = nullptr;
	parameters.pScrollRect = nullptr;
	parameters.pScrollOffset = nullptr;

	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = g_Renderer->m_swapChain->Present1(1, 0, &parameters);

	// Discard the contents of the render target.
	// This is a valid operation only when the existing contents will be entirely
	// overwritten. If dirty or scroll rects are used, this call should be removed.
	g_Renderer->m_d3dContext->DiscardView(g_Renderer->m_renderTargetView.Get());

	// Discard the contents of the depth stencil.
	g_Renderer->m_d3dContext->DiscardView(g_Renderer->m_depthStencilView.Get());

	// If the device was removed either by a disconnect or a driver upgrade, we 
	// must recreate all device resources.
	if (hr == DXGI_ERROR_DEVICE_REMOVED)
	{
		//HandleDeviceLost();
	}
	else
	{
		ThrowIfFailed(hr);
	}
}

AT_API ID3D11Device1* ATRender_GetDevice()
{
	return g_Renderer->m_d3dDevice.Get();
}

AT_API ID3D11DeviceContext1* ATRender_GetContext()
{
	return g_Renderer->m_d3dContext.Get();
}
