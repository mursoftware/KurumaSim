
#include "Common.hlsl"



TextureCube<float4> textureEnv : register(t0);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);




float4 main(PS_INPUT input) : SV_TARGET0
{
     float PI = 3.141592653589;


    float4 color = 0.0;
    float2 texcoord = input.TexCoord;
  

    float3 destVec;
    destVec.x = sin(texcoord.y * PI) * cos(texcoord.x * PI * 2.0);
    destVec.z = sin(texcoord.y * PI) * sin(texcoord.x * PI * 2.0);
    destVec.y = cos(texcoord.y * PI);

    float dh = PI / 8.0;


    for (int y = 0; y < 8; y++)
    {

        for (int x = 0; x < 16; x++)
        {
            float3 srcPos;
            srcPos.x = x / 16.0;
            srcPos.y = y / 8.0;

            float dw = sin(srcPos.y * PI) * 2.0 * PI / 16.0;

            float3 srcVec;
            srcVec.x = sin(srcPos.y * PI) * cos(srcPos.x * PI * 2.0);
            srcVec.z = sin(srcPos.y * PI) * sin(srcPos.x * PI * 2.0);
            srcVec.y = cos(srcPos.y * PI);


			float weight = dot(destVec, srcVec) / PI * dh * dw; //Normalized Lambert

            if (weight > 0.0f)
            {
                color.rgb += textureEnv.SampleLevel(sampler2, srcVec, 4).rgb * weight;
            }

        }
    }

    color.a = 1.0;

    return color;
}
