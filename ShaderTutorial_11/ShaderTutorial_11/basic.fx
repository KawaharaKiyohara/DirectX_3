/*!
 *@brief	���e�V���h�E�B
 */

float4x4 g_worldMatrix;			//���[���h�s��B
float4x4 g_viewMatrix;			//�r���[�s��B
float4x4 g_projectionMatrix;	//�v���W�F�N�V�����s��B

int g_isShadowReciever;				//�V���h�E���V�[�o�[�H�P�Ȃ�V���h�E���V�[�o�[�B
float4x4 g_lightViewMatrix;			//���C�g�r���[�s��B
float4x4 g_lightProjectionMatrix;	//���C�g�v���W�F�N�V�����s��B

texture g_diffuseTexture;		//�f�B�t���[�Y�e�N�X�`���B
sampler g_diffuseTextureSampler = 
sampler_state
{
	Texture = <g_diffuseTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Wrap;
	AddressV = Wrap;
};

texture g_shadowMapTexture;		//�V���h�E�}�b�v�e�N�X�`���B
sampler g_shadowMapTextureSampler = 
sampler_state
{
	Texture = <g_shadowMapTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VS_INPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
	float2	uv		: TEXCOORD0;
};

struct VS_OUTPUT{
	float4	pos				: POSITION;	
	float4	color			: COLOR0;
	float2	uv				: TEXCOORD0;
	float4	lightViewPos 	: TEXCOORD1;		//���[���h���->���C�g�r���[���->���C�g�ˉe��Ԃɕϊ����ꂽ���W�B

};

/*!
 *@brief	���_�V�F�[�_�[�B
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4 worldPos; 
	worldPos = mul( In.pos, g_worldMatrix );		//���f���̃��[�J����Ԃ��烏�[���h��Ԃɕϊ��B
	Out.pos = mul( worldPos, g_viewMatrix );		//���[���h��Ԃ���r���[��Ԃɕϊ��B
	Out.pos = mul( Out.pos, g_projectionMatrix );	//�r���[��Ԃ���ˉe��Ԃɕϊ��B
	Out.color = In.color;
	Out.uv = In.uv;
	if(g_isShadowReciever == 1){
		//�V���h�E���V�[�o�[�B
		//���[���h���W�����C�g�J�������猩���ˉe��Ԃɕϊ�����B
		Out.lightViewPos = mul(worldPos, g_lightViewMatrix);
		Out.lightViewPos = mul(Out.lightViewPos, g_lightProjectionMatrix);
	}
	return Out;
}
/*!
 *@brief	�s�N�Z���V�F�[�_�[�B
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	float4 color = tex2D( g_diffuseTextureSampler, In.uv );
	if(g_isShadowReciever == 1){
		//�ˉe���(�X�N���[�����W�n)�ɕϊ����ꂽ���W��w�����Ŋ����Ă���(-1.0f�`1.0)�͈̔͂̐��K�����W�n�ɂȂ�B
		//�����UV���W�n(0.0�`1.0)�ɕϊ����āA�V���h�E�}�b�v���t�F�b�`���邽�߂�UV�Ƃ��Ċ��p����B
		float2 shadowMapUV = In.lightViewPos.xy / In.lightViewPos.w;	//���̌v�Z��(-1.0�`1.0)�͈̔͂ɂȂ�B
		shadowMapUV *= float2(0.5f, -0.5f);								//���̌v�Z��(-0.5�`0.5)�͈̔͂ɂȂ�B
		shadowMapUV += float2(0.5f, 0.5f);								//�����Ă����(0.0�`1.0)�͈̔͂ɂȂ��Ăt�u���W�n�ɕϊ��ł����B������ˁB
		float4 shadowVal = tex2D(g_shadowMapTextureSampler, shadowMapUV);	//�V���h�E�}�b�v�͉e�������Ă���Ƃ���̓O���[�X�P�[���ɂȂ��Ă���B
		color *= shadowVal;
	}
	return color;
}
/*!
 *@brief	�V���h�E�}�b�v�������ݗp�̃s�N�Z���V�F�[�_�[�B
 */
float4 PSRenderToShadowMapMain(VS_OUTPUT In) : COLOR
{
	return float4(0.5f, 0.5f, 0.5f, 1.0f);
}

/*!
 *@brief	�ʏ�`��p�̃e�N�j�b�N
 */
technique SkinModel
{
	pass p0
	{
		VertexShader 	= compile vs_3_0 VSMain();
		PixelShader 	= compile ps_3_0 PSMain();
	}
}
/*!
 *@brief	�V���h�E�}�b�v�������ݗp�̃e�N�j�b�N
 */
technique SkinModelRenderToShadowMap
{
	pass p0
	{
		VertexShader 	= compile vs_3_0 VSMain();
		PixelShader 	= compile ps_3_0 PSRenderToShadowMapMain(); 
	}
}