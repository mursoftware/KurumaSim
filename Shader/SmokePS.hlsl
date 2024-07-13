


#include "Common.hlsl"

TextureCube<float4> textureEnv : register(t0);
//Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDepth : register(t1);
Texture2D<float4> textureDif : register(t2);
Texture2D<float4> textureIBLStatic : register(t3);
Texture2D<float> textureShadow[3] : register(t5);
SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);


//[earlydepthstencil]


PS_OUTPUT2 main(PS_INPUT input)
{
    PS_OUTPUT2 output;



    float4 baseColorTex = float4( 1.0, 1.0, 1.0, textureDif.Sample(sampler0, input.TexCoord).a);
    float4 baseColor = baseColorTex * Material.BaseColor;
    float3 normal = normalize(input.Normal.xyz);
    float3 position = input.WorldPosition.xyz;






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





    float3 directionalLight;
	directionalLight = saturate(dot(LightDirection, normal) * 0.5 + 0.5) * ScatteringLight * shadow / PI;




    float2 iblTexCoord;
    iblTexCoord.x = atan2(normal.x, normal.z) / (PI * 2);
    iblTexCoord.y = acos(normal.y) / PI;
	float3 iblLight = textureIBLStatic.Sample(sampler2, iblTexCoord).rgb;



    float3 diffuse = baseColor.rgb * (iblLight + directionalLight);




    float2 depthTexCoord;
    depthTexCoord = input.Position.xy / (SSBufferSize / 4);
    float n = 0.5; // camera z near
    float f = 400.0; // camera z far
	float smokeZ = (2.0 * n) / (f + n - input.Position.z * (f - n)) * f * 0.5;

	float alpha = 0.0;
    {
		float depth = textureDepth.SampleLevel(sampler1, depthTexCoord/* + float2(-4.0 / SSBufferSize.x, 4.0 / SSBufferSize.y)*/, 0).r;
		float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
		alpha += saturate((z - smokeZ + 0.5) * 0.5);
	}
 //   {
	//	float depth = textureDepth.SampleLevel(sampler1, depthTexCoord + float2(4.0 / SSBufferSize.x, 4.0 / SSBufferSize.y), 0).r;
	//	float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
	//	alpha += saturate((z - smokeZ + 0.5) * 0.5);
	//}
 //   {
	//	float depth = textureDepth.SampleLevel(sampler1, depthTexCoord + float2(-4.0 / SSBufferSize.x, -4.0 / SSBufferSize.y), 0).r;
	//	float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
	//	alpha += saturate((z - smokeZ + 0.5) * 0.5);
	//}
 //   {
	//	float depth = textureDepth.SampleLevel(sampler1, depthTexCoord + float2(4.0 / SSBufferSize.x, -4.0 / SSBufferSize.y), 0).r;
	//	float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;
	//	alpha += saturate((z - smokeZ + 0.5) * 0.5);
	//}
	//alpha /= 4;
    
	float4 color;
	color.rgb = diffuse * baseColor.a * alpha;
	color.a = baseColor.a * alpha;
    
    
    
   
    
 //   //fog
 //   {
	//	float3 eyeVector = position.xyz - CameraPosition.xyz;
	//	float len = length(eyeVector);
	//	eyeVector = normalize(eyeVector);
    
	//	float3 envLight;
 //       {
	//		float2 iblTexCoord;
	//		iblTexCoord.x = 0.0;
	//		iblTexCoord.y = 0.0;

	//		envLight = textureIBLStatic.Sample(sampler2, iblTexCoord).rgb;
	//	}


	//	float3 dirLight;
	//	dirLight = ScatteringLight / PI;


	//	float3 fogColor = float3(0.9, 0.9, 0.9) * 0.5;
	//	float fog = (1.0 - exp(-len * Fog)) * saturate(1.0 - eyeVector.y / (Fog * 100.0));

	//	output.Color.rgb = output.Color.rgb * (1.0 - fog) + fogColor * (envLight + dirLight) * fog;
	//}
    
    

    //color.rgb = depth.rgb;
    //color.a = 1.0;

	output.Color = color;



	return output;
}
