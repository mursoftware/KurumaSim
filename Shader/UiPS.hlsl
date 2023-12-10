
#include "Common.hlsl"



Texture2D<float4> textureColor : register(t0);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);





float4 main(PS_INPUT input) : SV_TARGET0
{
  
    float4 color;

    color = textureColor.Sample(sampler2, input.TexCoord) * Material.BaseColor;
 
    return color;
}
