#include "AT.h"
#include "ATRenderElement_Internal.h"

AT_API void ATRenderElement_Render(ATRenderElement* element)
{
	ID3D11DeviceContext1* context = ATRender_GetContext();

	UINT stride = (element->format == ATVF_PC ? sizeof(ATVertex_PC) : sizeof(ATVertex_PUC));
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &element->vertexBuffer, &stride, &offset);
	//context->IASetIndexBuffer(&pElement->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(element->inputLayout);

	context->VSSetShader(element->vertexShader, NULL, 0);
	context->PSSetShader(element->pixelShader, NULL, 0);

	context->Draw(element->vertexCount, 0);
}
