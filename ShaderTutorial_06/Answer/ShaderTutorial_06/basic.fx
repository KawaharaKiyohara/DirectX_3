/*!
 *@brief	簡単なディフューズライティング+スペキュラのシェーダー。
 */


float4x4 g_worldMatrix;			//ワールド行列。
float4x4 g_viewMatrix;			//ビュー行列。
float4x4 g_projectionMatrix;	//プロジェクション行列。
float4x4 g_rotationMatrix;		//回転行列。法線を回転させるために必要になる。ライティングするなら必須。
float3	 g_eyePos;				//視点。

#define DIFFUSE_LIGHT_NUM	4		//ディフューズライトの数。
float4	g_diffuseLightDirection[DIFFUSE_LIGHT_NUM];	//ディフューズライトの方向。
float4	g_diffuseLightColor[DIFFUSE_LIGHT_NUM];		//ディフューズライトのカラー。
float4	g_ambientLight;								//環境光。

texture g_diffuseTexture;		//ディフューズテクスチャ。
sampler g_diffuseTextureSampler = 
sampler_state
{
	Texture = <g_diffuseTexture>;
    MipFilter = NONE;
    MinFilter = NONE;
    MagFilter = NONE;
    AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_INPUT{
	float4	pos			: POSITION;
	float4	color		: COLOR0;
	float3	normal		: NORMAL0;
	float2	uv			: TEXCOORD0;
};

struct VS_OUTPUT{
	float4	pos			: POSITION;
	float4	color		: COLOR0;
	float2	uv			: TEXCOORD0;
	float3	normal		: TEXCOORD1;
	float4	worldPos 	: TEXCOORD2;		//ワールド空間での頂点座標。
};

/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	float4 pos; 
	pos = mul( In.pos, g_worldMatrix );		//モデルのローカル空間からワールド空間に変換。
	Out.worldPos = pos;
	pos = mul( pos, g_viewMatrix );			//ワールド空間からビュー空間に変換。
	pos = mul( pos, g_projectionMatrix );	//ビュー空間から射影空間に変換。
	Out.pos = pos;
	Out.color = In.color;
	Out.uv = In.uv;
	Out.normal = mul( In.normal, g_rotationMatrix );	//法線を回す。
	return Out;
}
/*!
 *@brief	ディフューズライトを計算。
 *@param[in]	normal	法線。
 */
float3 CalcDiffuse( float3 normal )
{
	float3 diff = 0.0f;
	for( int i = 0; i < DIFFUSE_LIGHT_NUM; i++ ){
		diff += max( 0.0f, dot(normal.xyz, -g_diffuseLightDirection[i].xyz) ) 
				* g_diffuseLightColor[i].xyz;
	}
	return diff;
}
/*!
 *@brief	スペキュラライトを計算。
 *@param[in]	worldPos		ワールド座標系での頂点座標。
 *@param[in]	n				法線。
 */
float3 CalcSpecular( float3 worldPos, float3 normal )
{
	float3 spec = 0.0f;

	float3 toEyeDir = normalize(g_eyePos - worldPos);
	float3 R = -toEyeDir + 2.0f * dot(normal, toEyeDir) * normal;
	
	for( int i = 0; i < DIFFUSE_LIGHT_NUM; i++ ){
		//スペキュラ成分を計算する。
		//反射ベクトルを計算。
		float3 L = -g_diffuseLightDirection[i].xyz;
		spec += g_diffuseLightColor[i] * pow(max(0.0f, dot(L,R)), 5) * g_diffuseLightColor[i].w;	//スペキュラ強度。
	}
	return spec;
}
/*!
 *@brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	//ライトを計算。
	float4 lig = 0.0f;
	float3 normal = normalize(In.normal);				//法線が線形補間されているので、大きさ1ではなくなっていてマッハバンドが目立つので正規化する。
	lig.xyz = CalcDiffuse( In.normal );					//ディフューズライトを計算。
	lig.xyz += CalcSpecular( In.worldPos, normal );		//スペキュラライトを計算。
	
	lig += g_ambientLight;
	float4 color = tex2D( g_diffuseTextureSampler, In.uv );
	color.xyz *= lig;
	return color;
}

technique SkinModel
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}