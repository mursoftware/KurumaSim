
#include "Common.hlsl"



Texture2D<float4> textureColor : register(t0);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);





float4 main(PS_INPUT input) : SV_TARGET0
{
  
    float4 color;

    color.rgb = textureColor.Sample(sampler1, input.TexCoord).rgb;
    color.a = Param.w;
 
    return color;
}
