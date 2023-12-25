


#include "Common.hlsl"

TextureCube<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
Texture2D<float4> textureIBLStatic : register(t3);
Texture2D<float4> textureSub : register(t4);
Texture2D<float> textureShadow[3] : register(t5);
Texture2D<float4> textureBlobShadowBody : register(t8);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);


//Clip and Discard are not available when EarlyZ is enabled.
//[earlydepthstencil]


PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    float PI = 3.141592653589;


    

    float4 baseColorTex = textureDif.Sample(sampler0, input.TexCoord);
    
	float threshold = Random(float4(input.TexCoord, 0.0, TemporalFrame)) * 0.5 + 0.5;
	clip(baseColorTex.a - threshold);

    float4 subColorTex = textureSub.Sample(sampler0, input.TexCoord);

    
    
    //AO
    float4 baseColor = baseColorTex * Material.BaseColor;
    float3 normal = normalize(input.Normal.xyz);
    float3 position = input.WorldPosition.xyz;
    float ao = input.Color.r;
    //float ao = input.Occlusion;
    ao = saturate(ao);
    ao = pow(ao, 2.2);
    
 
    //blob shadow
	float4 wp = float4(input.WorldPosition.xyz, 1.0);
	float4 localShadowPos = mul(wp, BodyW);
	float2 blobTexCoord = localShadowPos.xz;
	blobTexCoord.y /= 1.5;
	blobTexCoord += float2(-0.01, 0.04);
	blobTexCoord *= 0.171;
	blobTexCoord += 0.5;
    
	float4 blobShadowTex = textureBlobShadowBody.Sample(sampler1, blobTexCoord);
    
	ao *= pow(blobShadowTex.r, 1.6);
    
    
    
    

    baseColor.rgb = lerp(subColorTex.rgb, baseColor.rgb, input.Color.b) * input.Color.g;

    
    
    

    float metallic = Material.Metallic;
    float spec = Material.Specular;
    float smoothness = (1.0 - Material.Roughness) * pow(saturate(baseColor.r * 4.0), 1.0);
    float roughness = 1.0 - smoothness;

    roughness = max(roughness, 0.1);




    float3 eyeVector = position.xyz - CameraPosition.xyz;
	float len = length(eyeVector);
    eyeVector = normalize(eyeVector);

    float3 eyeRefVector = reflect(eyeVector, normal.xyz);
    eyeRefVector = normalize(eyeRefVector);

    float ned = saturate(dot(normal.xyz, eyeRefVector));

    float3 halfVector = normalize(LightDirection - eyeVector);







    float shadow = 1.0;
    float2 shadowTexCoord;
    
	float3 debugShadowColor[3] =
	{
		{ 1.0, 0.5, 0.5 },
		{ 0.5, 1.0, 0.5 },
		{ 0.5, 0.5, 1.0 }
	};

    for (int i = 0; i < 3; i++)
    {
        shadowTexCoord.x = input.ShadowPosition[i].x * 0.5f + 0.5f;
        shadowTexCoord.y = -input.ShadowPosition[i].y * 0.5f + 0.5f;

        if (0.01 < shadowTexCoord.x && shadowTexCoord.x < 0.99 && 0.01 < shadowTexCoord.y && shadowTexCoord.y < 0.99)
        {
            //for (int j = 0; j < 4; j++)
            {
                float2 offset = PoissonSamples[(int)(Random(float4(position, /*j + */TemporalFrame)) * 64)] * 0.002;
				float shadowColorTex = textureShadow[i].Sample(sampler1, shadowTexCoord + offset).r;
                if (shadowColorTex + 0.001 * (i + 1) < input.ShadowPosition[i].z)
                    shadow -= 1.0 / 1.0;
            }
            
			//baseColor.rgb *= debugShadowColor[i];

            break;
        }
    }

	//shadow = 1.0;





	float3 fresnel;
    {
        float3 f0 = lerp(0.08 * spec, baseColor.rgb, metallic);
		fresnel = f0 + (1.0 - f0) * pow(1.0 - ned, 5);
		fresnel *= (1.0 - roughness);
	}





    float3 diffuse;
    {


        float3 envLight;
        {
            float2 iblTexCoord;
            iblTexCoord.x = atan2(normal.x, normal.z) / (PI * 2);
            iblTexCoord.y = acos(normal.y) / PI;

			envLight = textureIBLStatic.Sample(sampler2, iblTexCoord).rgb; //Normalized during IBL texture generation
		}


        float3 dirLight;
        if (Material.NormalWeight > 0.5)
        {
			dirLight = saturate(dot(LightDirection, normal) * 0.5 + 0.5) * ScatteringLight * 0.0 / PI;
		}
        else
        {
			dirLight = saturate(dot(LightDirection, normal)) * ScatteringLight * shadow / PI;
		}



        diffuse = baseColor.rgb * (envLight * ao + dirLight);
    }






    float3 specular = 0.0;
    {




        float3 envSpec = 0.0;
        {
            envSpec = textureEnv.SampleLevel(sampler0, eyeRefVector, roughness * 10.0).rgb;
        }






        float3 dirSpec = 0.0;
        {
            float NdotH = saturate(dot(halfVector, normal.xyz));
            float NdotV = saturate(dot(normal.xyz, -eyeVector));
            float NdotL = saturate(dot(normal.xyz, LightDirection));
            float VdotH = saturate(dot(-eyeVector, halfVector));

            float d;
            {
                float alpha = roughness * roughness;
                float alpha2 = alpha * alpha;
                float t = ((NdotH * NdotH) * (alpha2 - 1.0) + 1.0);
                d = alpha2 / (PI * t * t);
            }

            float g;
            {

                float NH2 = 2.0 * NdotH;
                float g1 = (NH2 * NdotV) / VdotH;
                float g2 = (NH2 * NdotL) / VdotH;
                g = min(1.0, min(g1, g2));
            }



			dirSpec = (d * fresnel * g) / (4.0 * NdotV * NdotL + 0.000001);

            
			dirSpec *= ScatteringLight * shadow;
		}



        specular = envSpec + dirSpec;

        specular *= baseColor.rgb;
    }





    {
        float4 color;
		color.rgb = diffuse * (1.0 - metallic) * baseColor.a * (1.0 - fresnel)
            + specular * fresnel * pow(ao, 4.4)
            + Material.EmissionColor.rgb;

		color.a = baseColor.a * (1.0 - fresnel.r) + 1.0 * fresnel.r;
        color.a *= baseColorTex.a;

        output.Color = color;
    }


    
    
    //fog
    {
		output.Color.rgb = output.Color.rgb * input.Fog.a + input.Fog.rgb * (1.0 - input.Fog.a) * saturate(output.Color.g + 0.1);
	}
    
    
    

    //velocity
    {
		float2 oldPosition = input.OldPosition.xy;
		oldPosition /= input.OldPosition.w;
		oldPosition.y = -oldPosition.y;
		oldPosition = (oldPosition + 1.0) * 0.5;

		float2 newPosition = input.Position.xy;
		newPosition.x /= SSBufferSize.x;
		newPosition.y /= SSBufferSize.y;

		float2 velocity = (newPosition - oldPosition);
		output.Velocity.xy = velocity;


        //output.Velocity.z = 1.0;
	    //output.Velocity.z = 1.0 - input.Position.z / input.Position.w;
		output.Velocity.a = 1.0;
	}



	return output;
}
