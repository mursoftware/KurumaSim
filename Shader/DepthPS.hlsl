
#include "Common.hlsl"



Texture2D<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);


//[earlydepthstencil]

void main(PS_INPUT input)
{

	clip(Material.BaseColor.a - 1.0);
	
    float baseColorTex = textureDif.Sample(sampler0, input.TexCoord).a;
	clip(baseColorTex - 1.0);
}
