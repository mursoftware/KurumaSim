


#include "Common.hlsl"


Texture2D<float3> textureColor : register(t0);
Texture2D<float2> textureVelocity : register(t1);
Texture2D<float> textureDepth : register(t2);
Texture2D<float4> textureShrink : register(t3);
Texture2D<float3> textureExposure : register(t4);
//SamplerState sampler0 : register(s0);
SamplerState sampler1 : register(s1);
SamplerState sampler3 : register(s3);



float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}



float3 gammacorrect(float3 x, float gamma)
{
	return pow(abs(x), 1.0 / gamma);
}



float3 ColorTemperatureToRGB(float temperatureInKelvins)
{
    float3 retColor;
	
    temperatureInKelvins = clamp(temperatureInKelvins, 1000.0, 40000.0) / 100.0;
    
    if (temperatureInKelvins <= 66.0)
    {
        retColor.r = 1.0;
        retColor.g = saturate(0.39008157876901960784 * log(temperatureInKelvins) - 0.63184144378862745098);
    }
    else
    {
		float t = abs(temperatureInKelvins - 60.0);
        retColor.r = saturate(1.29293618606274509804 * pow(t, -0.1332047592));
        retColor.g = saturate(1.12989086089529411765 * pow(t, -0.0755148492));
    }
    
    if (temperatureInKelvins >= 66.0)
        retColor.b = 1.0;
    else if (temperatureInKelvins <= 19.0)
        retColor.b = 0.0;
    else
        retColor.b = saturate(0.54320678911019607843 * log(temperatureInKelvins - 10.0) - 1.19625408914);

    return retColor;
}





float4 main(PS_INPUT input) : SV_TARGET0
{
    
    float2 texcoord = input.TexCoord;
   
    
    float4 color;  
    color.rgb = 0.0;
    color.a = 1.0;

	float depth = textureDepth.SampleLevel(sampler3, texcoord, 0).r;
    float n = 0.5; // camera z near
    float f = 400.0; // camera z far
    float z = (2.0 * n) / (f + n - depth * (f - n)) * f * 0.5;


	float4 projectionPosition = float4(texcoord * 2.0 - 1.0, depth, 1.0);
	projectionPosition.y = -projectionPosition.y;
	float4 worldPosition = mul(projectionPosition, InvVP);
	worldPosition /= worldPosition.w;
    
    
    float blur = FocalLength * abs(z - FocalDistance) / (z * (FocalDistance - FocalLength)) * FocalBlur;


	float offset = blur;//+log2(SSRatio) - 0.5;
	//offset = 0.0;


    //color.rgb = textureColor.SampleLevel(sampler1, texcoord, offset).rgb;

    
  
    //Motion blur for rotation
    {
		float count = 0.0;
		float2 blurPos = texcoord;
		float2 velocity = textureVelocity.SampleLevel(sampler3, blurPos, 0);      
      	float2 ov = velocity;

		for (int i = 0; i < MotionBlurCount-1; i++)
		{
			float a = saturate(1.0 - length(ov - velocity) * 100.0);
			color.rgb += textureColor.SampleLevel(sampler3, blurPos, offset).rgb * a;
			count += a;
            
			blurPos += -velocity * MotionBlur / MotionBlurCount;
			velocity = textureVelocity.SampleLevel(sampler3, blurPos, 0);
            
		}
        
		float a = saturate(1.0 - length(ov - velocity) * 100.0);
		color.rgb += textureColor.SampleLevel(sampler3, blurPos, offset).rgb * a;
		count += a;
        
  
		color.rgb /= count;
	}

 
    
   

    //flare
    {
        float r = LensFlare * 2.0;
        //color.rgb += textureColor.SampleLevel(sampler1, texcoord, 0 + offset).rgb * 1.0;
        color.rgb += textureColor.SampleLevel(sampler1, texcoord, 1.5 + offset).rgb / 2 * r;
        //color.rgb += textureColor.SampleLevel(sampler1, texcoord, 2 + offset).rgb * pow(r, 2);
        color.rgb += textureColor.SampleLevel(sampler1, texcoord, 3.5 + offset).rgb / 8 * r;
        //color.rgb += textureColor.SampleLevel(sampler1, texcoord, 4 + offset).rgb * pow(r, 4);
        color.rgb += textureColor.SampleLevel(sampler1, texcoord, 5.5 + offset).rgb / 32 * r;
        //color.rgb += textureColor.SampleLevel(sampler1, texcoord, 6 + offset).rgb * pow(r, 6);
        color.rgb += textureColor.SampleLevel(sampler1, texcoord, 7.5 + offset).rgb / 128 * r;
        //color.rgb += textureColor.SampleLevel(sampler1, texcoord, 8 + offset).rgb * pow(r, 8);
        //color.rgb += textureColor.SampleLevel(sampler1, texcoord, 9.5 + offset).rgb * pow(r, 9);
    }
 
    


    //Smoke Reduction Buffer Composition
    {
		float4 shrinkColor = textureShrink.SampleLevel(sampler1, texcoord, 0);
        
        //float2 pixelSize = 1.0 / SSBufferSize;
		//float shrinkAlpah = 0.0;
        
		//for (int i = 0; i < 1; i++)
		//{
		//	float2 offset = PoissonSamples[(int) (Random(float4(texcoord, 1, z)) * 64)] * pixelSize * 8.0;
		//	shrinkAlpah += textureShrink.SampleLevel(sampler1, texcoord + offset, 0).a;
		//}
		color.rgb = lerp(color.rgb, shrinkColor.rgb, shrinkColor.a);
		//color.rgb = lerp(color.rgb, shrinkColor.rgb, saturate(shrinkColor.r)); //Alpha not available in DXGI_FORMAT_R11G11B10_FLOAT
	}

    
    
        
   
 //   //fog
 //   {
 //		float3 eyeVector = worldPosition.xyz - CameraPosition.xyz;
	//	float len = length(eyeVector);
	//	eyeVector /= len;
        
	//	//if (len > 400.0)
	//	//	len = 1000.0;
        
	//		float3 dirLight;
	//	dirLight = ScatteringLight / PI;


	//	float3 fogColor = float3(0.9, 0.9, 0.9) * 0.15;
	//	float fog = (1.0 - exp(-len * Fog)) * saturate(1.0 - eyeVector.y / (Fog * 100.0));

	//	color.rgb = color.rgb * (1.0 - fog) + fogColor * dirLight * fog;
	//}
    
    

    
    
    //vignette
    {
		float2 pos;
		pos.x = texcoord.x * 2.0f - 1.0f;
		pos.y = texcoord.y * 2.0f - 1.0f;

		pos.x *= SSBufferSize.x / SSBufferSize.y;


		float len = length(pos);


		color.rgb *= saturate(1.0 - len * Vignette);
	}





    //Exposure 
	if (AutoExposure)
	{
		float3 exposure = textureExposure.SampleLevel(sampler1, float2(0.5, 0.5), 0);
		float lumi = exposure.r * 0.3 + exposure.g * 0.6 + exposure.g * 0.1;
		lumi *= 10000.0; //Luminance unit in shader is 1/10000 nit
        
		color.rgb *= Exposure / lumi;
	}
    else
	{
  		color.rgb *= Exposure;      
	}
    
    


    //tone mapping
    {
        //if (ACESFilmEnable)
			color.rgb = ACESFilm(color.rgb);
        //else
         //   color = saturate(color);
    }
    


    //white balance
    {
		color.rgb *= ColorTemperatureToRGB(WhiteBalance);// / ColorTemperatureToRGB(IBLWhiteBalance);
	}
  


    //gamma correction
    {
		color.rgb = gammacorrect(color.rgb, Gamma);
	}

    
    


	return color;

}
