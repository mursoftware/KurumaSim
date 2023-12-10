
#include "Common.hlsl"


Texture2D<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
SamplerState sampler0 : register(s0);




float4 main(PS_INPUT input) : SV_TARGET0
{
    float a;   
    a = textureDif.Sample(sampler0, input.TexCoord).r;
    a = pow(a, 2.2);
    a = 1 - a;
    a *= 1.2;
    
    a = saturate(a-0.2);
    
    return float4(0.0, 0.0, 0.0, a);
}
