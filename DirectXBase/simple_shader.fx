//
// Simple shader. Does nothing fancy.
//

Texture2D tex2D;
SamplerState linearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


struct PointLight
{
	float3 Pos;
	float4 Color;
};

float3 lightDir = float3(0, 0, 10);
float4 lightColor = float4(0.9, 0.2, 0.2, 1.0);
float4 ambientLight = float4(0.2, 0.2, 0.2, 1.0);
float3 lightPosition = float3(0, 50, 10);

float3 lightDir2 = float3(0, 0, -10);

float lightIntensity = 1.9;

matrix World;
matrix View;
matrix Projection;
matrix WorldInverseTranspose;

PointLight light;

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD0;
	float2 Tex : TEXCOORD1;
	float3 LightDir : TEXCOORD2;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};



PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	float3 worldPos = mul(input.Pos, World).xyz;

	output.Pos = mul( input.Pos, World );
	output.LightDir = (light.Pos - worldPos);
    output.Pos = mul( output.Pos, View );    
    output.Pos = mul( output.Pos, Projection );
	output.Normal = mul(input.Normal, World);
	output.Tex = input.Tex;

	
    return output;  
}

float4 textured( PS_INPUT input ) : SV_Target
{
	float4 finalColor = 0;
	
	float3 l = input.LightDir;
	float attenuation = saturate(1.0 - dot(l, l));
	l = normalize(input.LightDir);
	float3 n = normalize(input.Normal);
	

	float lDotn = dot(l, n);

	float4 color = lDotn * light.Color;

	finalColor = tex2D.Sample(linearSampler, input.Tex) * color;
	finalColor.a = 1.0;

	return finalColor;
}

float4 noTexture( PS_INPUT input ) : SV_Target
{
	float4 finalColor = 0;
	
	finalColor = saturate(dot(lightDir, input.Normal) * lightColor);
	finalColor.a = 1.0;

	return finalColor;
}



RasterizerState DisableCulling
{
    CullMode = NONE;
};
DepthStencilState DepthEnabling
{
	DepthEnable = TRUE;
};


technique10 full
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, textured() ) );

		SetRasterizerState(DisableCulling);       
		SetDepthStencilState(DepthEnabling, 0);
    }
}

technique10 texturingDisabled
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, noTexture() ) );
    }
}