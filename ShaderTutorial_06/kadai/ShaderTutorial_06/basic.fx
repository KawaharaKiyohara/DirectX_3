/*!
 *@brief	簡単なディフューズライティング+スペキュラのシェーダー。
 */


float4x4 g_worldMatrix;			//ワールド行列。
float4x4 g_viewMatrix;			//ビュー行列。
float4x4 g_projectionMatrix;	//プロジェクション行列。
float4x4 g_rotationMatrix;		//回転行列。法線を回転させるために必要になる。ライティングするなら必須。

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
 *@brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	//ライトを計算。
	float4 lig = 0.0f;
	//ディフューズライトを計算。
	lig.xyz = CalcDiffuse( In.normal );					
	//ここにスペキュラライトの実装を記述しなさい。
	//視点のシェーダー定数は存在しないので、自分で追加を行うように。
	//モデルのワールド頂点座標はIn.worldPos、ワールド法線はIn.normal。
	//ライトの方向はg_diffuseLightDirection[0]とする。
	
	
	//アンビエントライトを加算。
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