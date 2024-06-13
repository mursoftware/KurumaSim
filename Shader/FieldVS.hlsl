#include "Common.hlsl"


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;

    //float4 position = float4(input.Position, 1.0f);
	float4 position = DecodePosition(input.Position);


    output.Position = mul(position, WVP);
    output.OldPosition = mul(position, OldWVP);
    output.WorldPosition.xyz = mul(position, World).xyz;
    output.WorldPosition.w = output.Position.z;

    //float4 normal = float4(input.Normal, 0.0f);
	float4 normal = DecodeNormal(input.Normal);
    
    output.Normal = mul(normal, World);
    
    //output.TexCoord = input.TexCoord;
	output.TexCoord = DecodeTexCoord(input.TexCoord);
    
	output.Color = DecodeColor(input.Position.w);
    //output.Occlusion = input.Occlusion;

    output.ShadowPosition[0] = mul(position, ShadowWVP[0]);
    output.ShadowPosition[1] = mul(position, ShadowWVP[1]);
    output.ShadowPosition[2] = mul(position, ShadowWVP[2]);

    
    return output;
}

