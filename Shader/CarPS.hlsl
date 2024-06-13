
#include "Common.hlsl"

TextureCube<float4> textureEnv : register(t0);
Texture2D<float4> textureIBL : register(t1);
Texture2D<float4> textureDif : register(t2);
Texture2D<float4> textureIBLStatic : register(t3);

//Texture2D<float4> textureARM : register(t3);
//Texture2D<float4> textureNor : register(t4);
Texture2D<float> textureShadow[3] : register(t5);

SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler2 : register(s2);



[earlydepthstencil]


PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;

    

	//output.Color.rgb = Material.BaseColor.rgb * saturate(dot(float3(0.0, 1.0, 0.0), input.Normal.xyz));
	////output.Color.rgb = input.Normal.xyz;
	//output.Color.a = 1.0;
	//output.Velocity = 0.0;
	//return output;


    //float4 arm = textureARM.Sample(sampler0, input.TexCoord);
    float4 arm = 1.0;

    float4 baseColorTex = textureDif.Sample(sampler1, input.TexCoord);
    //baseColorTex = pow(baseColorTex, 2.2);
    //clip(baseColorTex.a - 1.0);

    float4 baseColor = baseColorTex * Material.BaseColor;
    float3 normalVtx = normalize(input.Normal.xyz);
    //float3 normalTex = textureNor.Sample(sampler0, input.TexCoord);
    //float3 normal = float3(normalTex.y * 2.0 - 1.0, normalTex.z, normalTex.x * 2.0 - 1.0) * Material.NormalWeight + normalVtx * (1.0 - Material.NormalWeight);
    float3 normal = normalVtx;
    float3 position = input.WorldPosition.xyz;
    float ao = input.Color.r * arm.x;
    //float ao = input.Occlusion * arm.x;
    ao = saturate(ao);



    float metallic = Material.Metallic * arm.z;
    float spec = Material.Specular;
    float roughness = Material.Roughness * arm.y;

    roughness = max(roughness, 0.1);




	float3 eyeVector = position.xyz - CameraPosition.xyz;
	float len = length(eyeVector);
	eyeVector = normalize(eyeVector);

    float3 eyeRefVector = reflect(eyeVector, normal.xyz);
    eyeRefVector = normalize(eyeRefVector);

    float ned = saturate(dot(normal.xyz, eyeRefVector));

    float3 halfVector = normalize(LightDirection - eyeVector);






    float shadow = 1.0;
    {
        float2 shadowTexCoord;
        shadowTexCoord.x = input.ShadowPosition[0].x * 0.5f + 0.5f;
        shadowTexCoord.y = -input.ShadowPosition[0].y * 0.5f + 0.5f;

        //for (int i = 0; i < 4; i++)
        {
			float2 offset = PoissonSamples[(int) (Random(float4(shadowTexCoord.xyy, /*i +*/ TemporalFrame)) * 64)] * 0.002;
			float shadowColorTex = textureShadow[0].SampleLevel(sampler1, shadowTexCoord + offset, 0).r;
            if (shadowColorTex + 0.001 < input.ShadowPosition[0].z)
				shadow -= 1.0;// / 4.0;
		}
    }


    float3 fresnel;
    {
        float3 f0 = lerp(0.08 * spec, baseColor.rgb, metallic);
		fresnel = f0 + (1.0 - f0) * pow(1.0 - ned, 5);
		fresnel *= (1.0 - roughness);
	}





    float3 diffuse;
    {


        float3 envLight = 0.0;
        {
            float2 iblTexCoord;
            iblTexCoord.x = atan2(normal.x, normal.z) / (PI * 2);
            iblTexCoord.y = acos(normal.y) / PI;

			envLight = textureIBL.Sample(sampler2, iblTexCoord).rgb * pow(ao, 2.2); //Normalized during IBL texture generation
		}


        float3 dirLight;
        {
			dirLight = saturate(dot(LightDirection, normal)) * ScatteringLight * shadow / PI;
		}


        diffuse = baseColor.rgb * (envLight + dirLight);
    }






    float3 specular = 0.0;
    {




        float3 envSpec = 0.0;
        {
            envSpec += textureEnv.SampleLevel(sampler1, eyeRefVector, roughness * 10.0).rgb;
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector + ddx(eyeRefVector) * 0.5, roughness * 10.0).rgb;
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector + ddy(eyeRefVector) * 0.5, roughness * 10.0).rgb;
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector + ddx(eyeRefVector) * 0.5 + ddy(eyeRefVector) * 0.5, roughness * 10.0).rgb;
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector + ddx(eyeRefVector) * 0.5 - ddy(eyeRefVector) * 0.5, roughness * 10.0).rgb;
           
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector - ddx(eyeRefVector) * 0.5, roughness * 10.0).rgb;
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector - ddy(eyeRefVector) * 0.5, roughness * 10.0).rgb;
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector - ddx(eyeRefVector) * 0.5 - ddy(eyeRefVector) * 0.5, roughness * 10.0).rgb;
			//envSpec += textureEnv.SampleLevel(sampler0, eyeRefVector - ddx(eyeRefVector) * 0.5 + ddy(eyeRefVector) * 0.5, roughness * 10.0).rgb;
          
			//envSpec /= 9.0;

            //Weaken specularity on surfaces with high curvature.
			float r = (length(ddx(normal) + ddy(normal))) * 5.0 + 1.0;
			envSpec /= r;
		}






        float3 dirSpec;
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



			dirSpec = (d * g) / (4.0 * NdotV * NdotL + 0.000001);

            
            //Weaken specularity on surfaces with high curvature
			float r = (length(ddx(normal) + ddy(normal))) * 5.0 + 1.0;
			dirSpec /= r;
            
            
			dirSpec *= ScatteringLight * shadow;
		}
     


		specular = envSpec + dirSpec;
	}





    float4 color;
    {
		color.rgb = diffuse * (1.0 - metallic) * baseColor.a * (1.0 - fresnel)
            + specular * fresnel * pow(ao, 4.4)
            + Material.EmissionColor.rgb;

		color.a = baseColor.a * (1.0 - fresnel.r) + 1.0 * fresnel.r;
        color.a *= baseColorTex.a;
		color.a = saturate(color.a);

        output.Color = color;
    }


   
    //fog
    {
		float3 envLight;
        {
			float2 iblTexCoord;
			iblTexCoord.x = 0.0;
			iblTexCoord.y = 0.0;

			envLight = textureIBLStatic.Sample(sampler2, iblTexCoord).rgb / 2.0;
		}


		float3 dirLight;
		dirLight = ScatteringLight / (2.0 * PI);


		float3 fogColor = float3(0.9, 0.9, 0.9) * 1.0;
		float fog = (1.0 - exp(-len * Fog)); // * saturate(1.0 - eyeVector.y / (Fog * 100.0));

		output.Color.rgb = output.Color.rgb * (1.0 - fog) + fogColor * (envLight + dirLight) * fog;
		output.Color.a += (1.0 - output.Color.a) * fog;

	}
    


    //velocity vector
    {
		float2 oldPosition = input.OldPosition.xy;
        oldPosition /= input.OldPosition.w;
        oldPosition.y = -oldPosition.y;
        oldPosition = (oldPosition + 1.0) * 0.5;

        float2 newPosition = input.Position.xy;
        newPosition.x /= SSBufferSize.x;
        newPosition.y /= SSBufferSize.y;

		float2 velocity = (newPosition - oldPosition);
        output.Velocity.xy = velocity * Param.w;

        //output.Velocity.z = 1.0;
		//output.Velocity.z = 1.0 - input.Position.z / input.Position.w;
		output.Velocity.a = 1.0;
		//output.Velocity.a = step(0.5, color.a);
	}



    return output;
}
