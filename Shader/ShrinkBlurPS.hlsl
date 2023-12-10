
#include "Common.hlsl"

Texture2D<float4> textureColor : register(t0);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);



float4 main(PS_INPUT input) : SV_TARGET0
{
  
    float4 color = 0.0;
/*
    float offset = 1.0;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(-offset / Param.x, offset / Param.y)).rgb;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(offset / Param.x, offset / Param.y)).rgb;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(-offset / Param.x, -offset / Param.y)).rgb;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(offset / Param.x, -offset / Param.y)).rgb;
    color.rgb += textureColor.Sample(sampler0, input.TexCoord).rgb;

    color.rgb *= 0.2;
*/

/*
    for (int x = -20; x <= 20; x++)
    {
        for (int y = -20; y <= 20; y++)
        {
            if(x != 0 || y != 0)
                color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(x / Param.x, y / Param.y)).rgb / (x * x + y * y);
        }
    }
    color.rgb += textureColor.Sample(sampler1, input.TexCoord).rgb;

    color.rgb *= 0.03;
*/


    float total = 1.0 + 0.5 * 4 +0.25 * 4;
    float offset = 1.0;

    color.rgb += textureColor.Sample(sampler1, input.TexCoord).rgb;

    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(-offset / Param.x, 0.0)).rgb * 0.5;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(offset / Param.x, 0.0)).rgb * 0.5;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(0.0, -offset / Param.y)).rgb * 0.5;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(0.0, offset / Param.y)).rgb * 0.5;

    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(-offset / Param.x, -offset / Param.y)).rgb * 0.25;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(offset / Param.x, -offset / Param.y)).rgb * 0.25;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(-offset / Param.x, offset / Param.y)).rgb * 0.25;
    color.rgb += textureColor.Sample(sampler1, input.TexCoord + float2(offset / Param.x, offset / Param.y)).rgb * 0.25;

    color.rgb /= total;



    //color.rgb = textureColor.Sample(sampler0, input.TexCoord).rgb;





    color.a = 1.0;


    
    return color;
}
