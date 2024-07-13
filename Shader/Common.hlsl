


cbuffer EnvConstantBuffer : register(b0)
{

    float		Fog;
    float3		LightDirection;

    float3		LightColor;
	float       Time;
    
	float3      ScatteringLight;
	float       CloudDensity;
	float       CloudHeight;

};



cbuffer CameraConstantBuffer : register(b1)
{
    float4      CameraPosition;
    
    float       Exposure;
    float       IBLWhiteBalance;
    float       WhiteBalance;
    float       Gamma;
    
	bool        AutoExposure;
    float       LensFlare;
    float		MotionBlur;
    float		Vignette;
    
    float		FocalLength;
    float		FocalDistance;
    float		FocalBlur;
    float		Dummy;

    float2		SSBufferSize;  
	int         AntiAliasing;
	float       TemporalRatio;
    
	int         MotionBlurCount;
	int         TemporalFrame;
	int         EnvBufferMipLevel;
};


cbuffer ObjectConstantBuffer : register(b2)
{
    float4x4 WVP;
    float4x4 OldWVP;
    float4x4 World;
	//float4x4 NormalWorld;
    float4x4 ShadowWVP[3];
	float4x4 InvVP;
    float4 Param;
};

cbuffer SubsetConstantBuffer : register(b3)
{
 /*   
    struct MATERIAL
    {
        float4 BaseColor;
        float4 EmissionColor;
        float Metallic;
        float Specular;
        float Roughness;
        float NormalWeight;
    } Material;
*/    
    

    struct MATERIAL
    {
		float4  ClearColor;
	    float   ClearCoat;
	    float   ClearSpecular;
	    float   ClearRoughness;
	    float   Dummy;

		float4  BaseColor;
	    float	BaseMetallic;
	    float	BaseSpecular;
	    float	BaseRoughness;
	    float	NormalWeight;

		float4 EmissionColor;
	} Material;

};

cbuffer BlobShadowBuffer : register(b4)
{
	float4x4 BodyW;
	float4x4 TireW[4];
};




float4 DecodePosition(int3 input)
{
	float4 position;
	position.xyz = input.xyz / (float)0x7fff;
	position.w = 1.0;
    
	return position;
}


float4 DecodeNormal(int2 input)
{
	float2 normal;
	normal.xy = input.xy / (float)0x7fff;

	float3 n = float3(normal.x, normal.y, 1.0 - abs(normal.x) - abs(normal.y));
	float t = max(-n.z, 0.0);
	n.x += n.x >= 0.0 ? -t : t;
	n.y += n.y >= 0.0 ? -t : t;
	return float4(normalize(n), 0.0);
}


float2 DecodeTexCoord(int2 input)
{
	float4 texCoord;
	texCoord.xy = input.xy / 50.0;
    
	return texCoord;
}

float4 DecodeColor(int input)
{
	float4 color;
	color.r = (float) (input & 0xf) / 0xf;
	color.g = (float) ((input & 0xf0) >> 4) / 0xf;   
	color.b = (float) ((input & 0xf00) >> 8) / 0xf;
	color.a = (float) ((input & 0xf000) >> 12) / 0xf;

	return color;
}


float4 DecodeAO(int input)
{
	float4 color;
	color.r = (float) input / 0x7fff;
	color.g = 1.0;
	color.b = 1.0;
	color.a = 1.0;

	return color;
}


struct VS_INPUT
{
	int4 Position : POSITION;
	int2 Normal : NORMAL;
	int2 TexCoord : TEXCOORD;
};

/*
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
    
    //float4 Color : COLOR;
    //float Occlusion : OCCLUSION;
};
*/



struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
    float4 WorldPosition : POSITION;
    float4 OldPosition : OLD_POSITION;
    float4 ShadowPosition[3] : SHADOW_POSITION0;

};

struct PS_OUTPUT
{
    float4 Color : SV_TARGET0;
    float4 Velocity : SV_TARGET1;
};

struct PS_OUTPUT2
{
    float4 Color : SV_TARGET0;
};



static const float2 PoissonSamples[64] =
{
    float2(-0.5119625f, -0.4827938f),
    float2(-0.2171264f, -0.4768726f),
    float2(-0.7552931f, -0.2426507f),
    float2(-0.7136765f, -0.4496614f),
    float2(-0.5938849f, -0.6895654f),
    float2(-0.3148003f, -0.7047654f),
    float2(-0.42215f, -0.2024607f),
    float2(-0.9466816f, -0.2014508f),
    float2(-0.8409063f, -0.03465778f),
    float2(-0.6517572f, -0.07476326f),
    float2(-0.1041822f, -0.02521214f),
    float2(-0.3042712f, -0.02195431f),
    float2(-0.5082307f, 0.1079806f),
    float2(-0.08429877f, -0.2316298f),
    float2(-0.9879128f, 0.1113683f),
    float2(-0.3859636f, 0.3363545f),
    float2(-0.1925334f, 0.1787288f),
    float2(0.003256182f, 0.138135f),
    float2(-0.8706837f, 0.3010679f),
    float2(-0.6982038f, 0.1904326f),
    float2(0.1975043f, 0.2221317f),
    float2(0.1507788f, 0.4204168f),
    float2(0.3514056f, 0.09865579f),
    float2(0.1558783f, -0.08460935f),
    float2(-0.0684978f, 0.4461993f),
    float2(0.3780522f, 0.3478679f),
    float2(0.3956799f, -0.1469177f),
    float2(0.5838975f, 0.1054943f),
    float2(0.6155105f, 0.3245716f),
    float2(0.3928624f, -0.4417621f),
    float2(0.1749884f, -0.4202175f),
    float2(0.6813727f, -0.2424808f),
    float2(-0.6707711f, 0.4912741f),
    float2(0.0005130528f, -0.8058334f),
    float2(0.02703013f, -0.6010728f),
    float2(-0.1658188f, -0.9695674f),
    float2(0.4060591f, -0.7100726f),
    float2(0.7713396f, -0.4713659f),
    float2(0.573212f, -0.51544f),
    float2(-0.3448896f, -0.9046497f),
    float2(0.1268544f, -0.9874692f),
    float2(0.7418533f, -0.6667366f),
    float2(0.3492522f, 0.5924662f),
    float2(0.5679897f, 0.5343465f),
    float2(0.5663417f, 0.7708698f),
    float2(0.7375497f, 0.6691415f),
    float2(0.2271994f, -0.6163502f),
    float2(0.2312844f, 0.8725659f),
    float2(0.4216993f, 0.9002838f),
    float2(0.4262091f, -0.9013284f),
    float2(0.2001408f, -0.808381f),
    float2(0.149394f, 0.6650763f),
    float2(-0.09640376f, 0.9843736f),
    float2(0.7682328f, -0.07273844f),
    float2(0.04146584f, 0.8313184f),
    float2(0.9705266f, -0.1143304f),
    float2(0.9670017f, 0.1293385f),
    float2(0.9015037f, -0.3306949f),
    float2(-0.5085648f, 0.7534177f),
    float2(0.9055501f, 0.3758393f),
    float2(0.7599946f, 0.1809109f),
    float2(-0.2483695f, 0.7942952f),
    float2(-0.4241052f, 0.5581087f),
    float2(-0.1020106f, 0.6724468f),
};


float Random(float4 Seed)
{
    float d = dot(Seed, float4(12.9898, 78.233, 32.241, 61.233));
    return frac(sin(d) * 43758.5453);
}


static float PI = 3.141592653589;




float3 Fresnel(float3 F0, float NdL, float Roughness)
{
	float3 fresnel;
    
	fresnel = F0 + (1.0 - F0) * pow(1.0 - NdL, 5.0);
	fresnel *= (1.0 - Roughness);
    
	return fresnel;
}
