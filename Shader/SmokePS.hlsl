


#include "Common.hlsl"

TextureCube<float4> textureEnv : register(t0);
//Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDepth : register(t1);
Texture2D<float4> textureDif : register(t2);
Texture2D<float4> textureIBLStatic : register(t3);
Texture2D<float> textureShadow[3] : register(t5);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);



//EarlyZ有効時にはClip、Discardが使えない
//[earlydepthstencil]


PS_OUTPUT2 main(PS_INPUT input)
{
    PS_OUTPUT2 output;
    float PI = 3.141592653589;



    float4 baseColorTex = float4( 1.0, 1.0, 1.0, textureDif.Sample(sampler0, input.TexCoord).a);
    float4 baseColor = baseColorTex * Material.BaseColor;
    float3 normal = normalize(input.Normal.xyz);
    float3 position = input.WorldPosition.xyz;






    //シャドウ
    float shadow = 1.0;
    float2 shadowTexCoord;

    for (int i = 0; i < 3; i++)
    {
        shadowTexCoord.x = input.ShadowPosition[i].x * 0.5f + 0.5f;
        shadowTexCoord.y = -input.ShadowPosition[i].y * 0.5f + 0.5f;

        if (0.01 < shadowTexCoord.x && shadowTexCoord.x < 0.99 && 0.01 < shadowTexCoord.y && shadowTexCoord.y < 0.99)
        {
            //for (int j = 0; j < 4; j++)
            {
                float2 offset = PoissonSamples[(int)(Random(float4(position, TemporalFrame)) * 64)] * 0.004;
				float shadowColorTex = textureShadow[i].SampleLevel(sampler1, shadowTexCoord + offset, 0).r;
                if (shadowColorTex + 0.001 * (i + 1) < input.ShadowPosition[i].z)
                    shadow -= 1.0 / 1.0;
            }

            break;
        }
    }




    //レイリー散乱
	float3 sunLight = LightColor;
	float3 wavelength = float3(0.650, 0.570, 0.475);
	float3 wavelength4inv = 1.0 / pow(wavelength, 4);
	float atomDensityLight = 1.0 + pow(1.0 - LightDirection.y, 10) * 10.0;
	float3 scatteringLight = sunLight * exp(-atomDensityLight * atomDensityLight * wavelength4inv * 0.01);



    //ディレクショナルライト
    float3 directionalLight;
	directionalLight = saturate(dot(LightDirection, normal) * 0.25 + 0.75) * scatteringLight * shadow / PI;




    //IBL
    float2 iblTexCoord;
    iblTexCoord.x = atan2(normal.x, normal.z) / (PI * 2);
    iblTexCoord.y = acos(normal.y) / PI;
    float3 iblLight = textureIBLStatic.Sample(sampler0, iblTexCoord).rgb;



    float3 diffuse = baseColor.rgb * (iblLight + directionalLight);




    float2 depthTexCoord;
    depthTexCoord = input.Position.xy / (SSBufferSize / 4);
    float n = 0.5; // camera z near
    float f = 400.0; // camera z far
	float smokeZ = (2.0 * n) / (f + n - input.Position.z * (f - n)) * f * 0.5;

	float alpha = 0.0;
    {
		float depth = textureDepth.SampleLevel(sampler1, depthTexCoord + float2(-4.0 / SSBufferSize.x, 4.0 / SSBufferSize.y), 0).r;
		float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
		alpha += saturate((z - smokeZ + 0.5) * 0.5);
	}
    {
		float depth = textureDepth.SampleLevel(sampler1, depthTexCoord + float2(4.0 / SSBufferSize.x, 4.0 / SSBufferSize.y), 0).r;
		float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
		alpha += saturate((z - smokeZ + 0.5) * 0.5);
	}
    {
		float depth = textureDepth.SampleLevel(sampler1, depthTexCoord + float2(-4.0 / SSBufferSize.x, -4.0 / SSBufferSize.y), 0).r;
		float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
		alpha += saturate((z - smokeZ + 0.5) * 0.5);
	}
    {
		float depth = textureDepth.SampleLevel(sampler1, depthTexCoord + float2(4.0 / SSBufferSize.x, -4.0 / SSBufferSize.y), 0).r;
		float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
		alpha += saturate((z - smokeZ + 0.5) * 0.5);
	}
	alpha /= 4;
    
	float4 color;
	color.rgb = diffuse * baseColor.a * alpha;
	color.a = baseColor.a * alpha;

    //color.rgb = depth.rgb;
    //color.a = 1.0;

	output.Color = color;



	return output;
}
