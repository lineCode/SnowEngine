// File: GameCode4_PS.hlsl - the pixel shader for GameCode4 (D3D11 renderer)

// Globals
cbuffer cbObjectColors : register( b0 )
{
	float4		g_vDiffuseObjectColor	: packoffset( c0 );
	float4		g_vAmbientObjectColor	: packoffset( c1 );
	bool		g_bHasTexture			: packoffset( c2.x );
};

//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------
Texture2D	g_txDiffuse : register( t0 );
SamplerState g_samLinear : register( s0 );

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 vDiffuse		: COLOR0;
	float2 vTexcoord	: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 GameCode4_PSMain( PS_INPUT Input ) : SV_TARGET
{
	float4 vOutputColor;

	if (g_bHasTexture)
		vOutputColor = g_txDiffuse.Sample( g_samLinear, Input.vTexcoord ) * Input.vDiffuse;
	else
		vOutputColor = Input.vDiffuse;
	
	return vOutputColor;
}

