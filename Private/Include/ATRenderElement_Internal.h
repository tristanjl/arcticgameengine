#ifndef AT_RENDER_ELEMENT
#define AT_RENDER_ELEMENT

#include "ATTargets.h"
#include "ATTypes.h"

#ifdef WINDOWS_STORE
#include "ATRender_Internal.h"
#endif

struct ATTexture;
struct ATRenderState;
struct ATMatrix;

enum ATVertexFormat
{
	ATVF_PC,
	ATVF_PUC,
	ATVF_PUCN,
	ATVF_PNUCBT
};

struct ATVertex_PC
{
#ifdef WINDOWS_STORE
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 color;
#else
	uint8 color[4];
	float position[3];
#endif
};

struct ATVertex_PUC
{
#ifdef WINDOWS_STORE
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT4 color;
#else
	float uv[2];
	uint8 color[4];
	float position[3];
#endif
};

struct ATRenderElement
{
#ifdef WINDOWS_STORE
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
#else
	void* verticies;
#endif
	ATVertexFormat format;
	uint32 vertexCount;
	ATTexture* texture;
	const ATMatrix* transformation;
};

AT_API void ATRenderElement_Render(ATRenderElement* element);

#endif // AT_RENDER_ELEMENT
