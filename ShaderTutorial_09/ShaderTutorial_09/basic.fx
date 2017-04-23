/*!
 *@brief	法線マップ
 */


float4x4 g_worldMatrix;			//ワールド行列。
float4x4 g_viewMatrix;			//ビュー行列。
float4x4 g_projectionMatrix;	//プロジェクション行列。
float4x4 g_rotationMatrix;		//!<回転行列。

#define DIFFUSE_LIGHT_NUM	4		//ディフューズライトの数。

float4	g_diffuseLightDirection[DIFFUSE_LIGHT_NUM];	//ディフューズライトの方向。
float4	g_diffuseLightColor[DIFFUSE_LIGHT_NUM];		//ディフューズライトのカラー。
float4	g_ambientLight;								//環境光。

struct VS_INPUT{
	float4	pos				: POSITION;
	float4	color			: COLOR0;
	float3	normal			: NORMAL;		//法線。
	float3	tangentNormal	: TANGENT;		//接ベクトル
	float2	uv				: TEXCOORD0;
};

struct VS_OUTPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
	float2	uv		: TEXCOORD0;
	float3	normal			: TEXCOORD1;		//法線。
	float3	tangentNormal	: TEXCOORD2;		//接ベクトル
};

texture g_diffuseMap;		//ディフューズマップ
sampler g_diffuseMapSampler = 
sampler_state
{
	Texture = <g_diffuseMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Wrap;
	AddressV = Wrap;
};

texture g_normalMap;		//法線マップ
sampler g_normalMapSampler = 
sampler_state
{
	Texture = <g_normalMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Wrap;
	AddressV = Wrap;
};


/*!
 * @brief	ライトを計算。
 */
float4 CalcLight( float3 normal )
{
	float4 lig = 0.0f;
	{
		for( int i = 0; i < DIFFUSE_LIGHT_NUM; i++ ){
			lig.xyz += max( 0.0f, dot(normal, -g_diffuseLightDirection[i].xyz) ) 
					* g_diffuseLightColor[i].xyz;
		}
		lig += g_ambientLight;
	}
	return lig;
}
/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	float4 pos; 
	pos = mul( In.pos, g_worldMatrix );		//モデルのローカル空間からワールド空間に変換。
	pos = mul( pos, g_viewMatrix );			//ワールド空間からビュー空間に変換。
	pos = mul( pos, g_projectionMatrix );	//ビュー空間から射影空間に変換。
	Out.pos = pos;
	Out.color = In.color;
	Out.uv = In.uv;
	//法線を回転。
	Out.normal = mul(In.normal, g_rotationMatrix);
	Out.tangentNormal = mul(In.tangentNormal, g_rotationMatrix);
	return Out;
}
/*!
 *@brief	頂点シェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	//法線マップからタンジェントスペース法線をフェッチ。
	float3 localNormal = tex2D( g_normalMapSampler, In.uv );
	//法線と接ベクトルの外積を求めて従法線を求める。
	float3 biNormal = normalize( cross( In.tangentNormal, In.normal) );
	//-1.0～1.0の範囲にマッピングする。
	localNormal = (localNormal * 2.0f)- 1.0f;
	//ワールドスペースの法線を求める。
	float3 worldNormal = In.tangentNormal * localNormal.x 
	                    + biNormal * localNormal.y 
	                    + In.normal * localNormal.z;
	                     
	//ワールドスペースの法線を使ってライティング。
	float4 lig = CalcLight(worldNormal);
	float4 diff = tex2D( g_diffuseMapSampler, In.uv );
	float4 color = diff* lig;
	return float4( color.xyz, 1.0f);
//	return float4(normal.xyz, 1.0f);
}

technique ColorPrim
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}