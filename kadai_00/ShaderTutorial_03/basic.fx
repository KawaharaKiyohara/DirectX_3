/*!
 *@brief	シンプルな射影変換シェーダー。
 */

float4x4 mWorld;	//ワールド行列。
float4x4 mView;		//ビュー行列。
float4x4 mProj;		//プロジェクション行列。



struct VS_INPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
};

struct VS_OUTPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
};

/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	float4 pos = mul(In.pos, mWorld);
	pos = mul(pos, mView);
	pos = mul(pos, mProj);
	Out.pos = pos;
	Out.color = In.color;
	return Out;
}
/*!
 *@brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

technique SkinModel
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}