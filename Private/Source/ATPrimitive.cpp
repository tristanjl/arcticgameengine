#include "AT.h"
#include "ATMatrix.h"
#include "ATRenderElement_Internal.h"

#ifdef WINDOWS_STORE
#include <agile.h>
#include <sys/stat.h>
#include <tchar.h>

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
#endif

const uint32 SCRATCH_BUFFER_VERTICIES = 1024;

struct ATScratchBuffer
{
#ifdef WINDOWS_STORE
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11InputLayout* m_inputLayout;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	void* vertexShaderData;
	void* pixelShaderData;
#endif
	ATVertex_PC verticies[SCRATCH_BUFFER_VERTICIES];
	uint32 vertexCount;
};

static ATScratchBuffer* s_ScratchBuffer;

AT_API void ATPrimitive_Init()
{
	s_ScratchBuffer = (ATScratchBuffer*)ATAlloc(sizeof(ATScratchBuffer));
	s_ScratchBuffer->vertexCount = 0;
#ifdef WINDOWS_STORE
	ID3D11Device1* device = ATRender_GetDevice();

	Windows::Storage::StorageFolder^ folder = Windows::ApplicationModel::Package::Current->InstalledLocation;
	//const wchar_t* installPath = folder->Path->Data();
	TCHAR vsFilename[MAX_PATH];
	TCHAR psFilename[MAX_PATH];
	//size_t convertedChars = 0;
	//CreateFile()
	_tcscpy(vsFilename, folder->Path->Data());
	//wcstombs_s(&convertedChars, vsFilename, sizeof(vsFilename), installPath, folder->Path->Length());
	_tcscpy(psFilename, vsFilename);
	_tcscat(vsFilename, L"\\ArcticGameEngine\\ATBasicVertexShader_Store.cso");
	_tcscat(psFilename, L"\\ArcticGameEngine\\ATBasicPixelShader_Store.cso");
    HANDLE vsFile = CreateFile2(vsFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	WIN32_FILE_ATTRIBUTE_DATA data;
	GetFileAttributesEx(vsFilename, GetFileExInfoStandard, &data);
	char* vShader = (char*)ATAlloc(data.nFileSizeLow);
	DWORD bytesRead;
    ReadFile(vsFile, vShader, data.nFileSizeLow, &bytesRead, nullptr);
    ThrowIfFailed(device->CreateVertexShader(vShader, data.nFileSizeLow, nullptr, &s_ScratchBuffer->m_vertexShader));
	const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ThrowIfFailed(device->CreateInputLayout(vertexDesc, ATARRAYSIZE(vertexDesc), vShader, data.nFileSizeLow, &s_ScratchBuffer->m_inputLayout));
	s_ScratchBuffer->vertexShaderData = vShader;
	CloseHandle(vsFile);
    HANDLE psFile = CreateFile2(psFilename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	GetFileAttributesEx(psFilename, GetFileExInfoStandard, &data);
	char* shader = (char*)ATAlloc(data.nFileSizeLow);
    ReadFile(psFile, shader, data.nFileSizeLow, &bytesRead, nullptr);
    ThrowIfFailed(device->CreatePixelShader(shader, data.nFileSizeLow, nullptr, &s_ScratchBuffer->m_pixelShader));
	CloseHandle(psFile);
	s_ScratchBuffer->pixelShaderData = shader;

	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(s_ScratchBuffer->verticies), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
	vertexBufferData.pSysMem = s_ScratchBuffer->verticies;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &s_ScratchBuffer->vertexBuffer);
#endif
}

AT_API void ATPrimitive_Deinit()
{
#ifdef WINDOWS_STORE
	s_ScratchBuffer->vertexBuffer->Release();
	s_ScratchBuffer->indexBuffer->Release();
	s_ScratchBuffer->m_vertexShader->Release();
	s_ScratchBuffer->m_pixelShader->Release();
	s_ScratchBuffer->m_inputLayout->Release();
	ATFree(s_ScratchBuffer->vertexShaderData);
	ATFree(s_ScratchBuffer->pixelShaderData);
#endif
	ATFree(s_ScratchBuffer);
}

AT_API uint32 ATPrimitive_VertexCount()
{
	return s_ScratchBuffer->vertexCount;
}

AT_API void ATPrimitive_Flush()
{
	ATRenderElement element;
#ifdef WINDOWS_STORE

    ATRender_GetContext()->UpdateSubresource(s_ScratchBuffer->vertexBuffer, 0, nullptr, &s_ScratchBuffer->verticies, 0, 0);
	element.vertexBuffer = s_ScratchBuffer->vertexBuffer;
	element.indexBuffer = s_ScratchBuffer->indexBuffer;
	element.vertexShader = s_ScratchBuffer->m_vertexShader;
	element.pixelShader = s_ScratchBuffer->m_pixelShader;
	element.inputLayout = s_ScratchBuffer->m_inputLayout;
#else
	element.verticies = s_ScratchBuffer->verticies;
#endif
	element.format = ATVF_PC;
	element.vertexCount = s_ScratchBuffer->vertexCount;
    element.texture = nullptr;
	element.transformation = &ATMatrix::Identity;
	ATRenderElement_Render(&element);
	s_ScratchBuffer->vertexCount = 0;
}

AT_API void ATPrimitive_DrawQuad(const ATVector4& topLeft, const ATVector4& bottomRight, const ATVector4& color)
{
	if (s_ScratchBuffer->vertexCount + 6 > SCRATCH_BUFFER_VERTICIES)
	{
		ATPrimitive_Flush();
	}

	uint32 writeVertex = s_ScratchBuffer->vertexCount;

	ATUNSED(topLeft);
	ATUNSED(bottomRight);
	ATUNSED(color);
#ifdef WINDOWS_STORE
	s_ScratchBuffer->verticies[writeVertex].position = *(DirectX::XMFLOAT4*)&topLeft.v;
	s_ScratchBuffer->verticies[writeVertex].color = *(DirectX::XMFLOAT4*)&color;
	if (writeVertex != 0)
	{
		++writeVertex;
		ATMemCopy(&s_ScratchBuffer->verticies[writeVertex], &s_ScratchBuffer->verticies[writeVertex - 1], sizeof(ATVertex_PC));
	}
	++writeVertex;
	s_ScratchBuffer->verticies[writeVertex].position.x = bottomRight.v.x;
	s_ScratchBuffer->verticies[writeVertex].position.y = topLeft.v.y;
	s_ScratchBuffer->verticies[writeVertex].position.z = topLeft.v.z;
	s_ScratchBuffer->verticies[writeVertex].position.w = topLeft.v.w;
	s_ScratchBuffer->verticies[writeVertex].color = *(DirectX::XMFLOAT4*)&color;
	++writeVertex;
	s_ScratchBuffer->verticies[writeVertex].position.x = topLeft.v.x;
	s_ScratchBuffer->verticies[writeVertex].position.y = bottomRight.v.y;
	s_ScratchBuffer->verticies[writeVertex].position.z = bottomRight.v.z;
	s_ScratchBuffer->verticies[writeVertex].position.w = bottomRight.v.w;
	s_ScratchBuffer->verticies[writeVertex].color = *(DirectX::XMFLOAT4*)&color;
	++writeVertex;
	s_ScratchBuffer->verticies[writeVertex].position = *(DirectX::XMFLOAT4*)&bottomRight.v;
	s_ScratchBuffer->verticies[writeVertex].color = *(DirectX::XMFLOAT4*)&color;
	++writeVertex;
	ATMemCopy(&s_ScratchBuffer->verticies[writeVertex], &s_ScratchBuffer->verticies[writeVertex - 1], sizeof(ATVertex_PC));
	s_ScratchBuffer->vertexCount = writeVertex + 1;
#else
	uint8 colorValue[4];
	colorValue[0] = (uint8)(color.v.x * 255.0f);
	colorValue[1] = (uint8)(color.v.y * 255.0f);
	colorValue[2] = (uint8)(color.v.z * 255.0f);
	colorValue[3] = (uint8)(color.v.w * 255.0f);

	ATMemCopy(s_ScratchBuffer->verticies[writeVertex].position, &topLeft.v.x, sizeof(s_ScratchBuffer->verticies->position));
	*((uint32 *)s_ScratchBuffer->verticies[writeVertex].color) = *((uint32 *)colorValue);
	if (writeVertex != 0)
	{
		++writeVertex;
		ATMemCopy(&s_ScratchBuffer->verticies[writeVertex], &s_ScratchBuffer->verticies[writeVertex - 1], sizeof(ATVertex_PC));
	}
	++writeVertex;
	s_ScratchBuffer->verticies[writeVertex].position[0] = bottomRight.v.x;
	s_ScratchBuffer->verticies[writeVertex].position[1] = topLeft.v.y;
	s_ScratchBuffer->verticies[writeVertex].position[2] = topLeft.v.z;
	*((uint32 *)s_ScratchBuffer->verticies[writeVertex].color) = *((uint32 *)colorValue);
	++writeVertex;
	s_ScratchBuffer->verticies[writeVertex].position[0] = topLeft.v.x;
	s_ScratchBuffer->verticies[writeVertex].position[1] = bottomRight.v.y;
	s_ScratchBuffer->verticies[writeVertex].position[2] = bottomRight.v.z;
	*((uint32 *)s_ScratchBuffer->verticies[writeVertex].color) = *((uint32 *)colorValue);
	++writeVertex;
	ATMemCopy(s_ScratchBuffer->verticies[writeVertex].position, &bottomRight.v.x, sizeof(s_ScratchBuffer->verticies->position));
	*((uint32 *)s_ScratchBuffer->verticies[writeVertex].color) = *((uint32 *)colorValue);
	++writeVertex;
	ATMemCopy(&s_ScratchBuffer->verticies[writeVertex], &s_ScratchBuffer->verticies[writeVertex - 1], sizeof(ATVertex_PC));
	s_ScratchBuffer->vertexCount = writeVertex + 1;
#endif
}
