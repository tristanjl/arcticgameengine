cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 color : COLOR0;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float3 color : COLOR0;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 position = float4(input.position, 1.0f);

	// Transform the vertex positionition into projected space.
	position = mul(position, model);
	position = mul(position, view);
	position = mul(position, projection);
	output.position = position;

	// Pass through the color without modification.
	output.color = input.color;

	return output;
}
