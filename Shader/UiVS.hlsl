#include "Common.hlsl"


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    //float4 position = float4(input.Position, 1.0f);
	float4 position = DecodePosition(input.Position);

    output.Position = mul(position, WVP);
    output.WorldPosition.xyz = mul(position, World).xyz;
    
    //float4 normal = float4(input.Normal, 0.0f);
	float4 normal = DecodeNormal(input.Normal);
    
    output.Normal = mul(normal, World);
    
    //output.TexCoord = input.TexCoord * Param.zw + Param.xy;
	output.TexCoord.xy = DecodeTexCoord(input.TexCoord) * Param.zw + Param.xy;
    
    //output.Color = input.Color;
    //output.Occlusion = input.Occlusion;
    
    return output;
}

