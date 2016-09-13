#include "NewGame.h"


NewGame::NewGame(HarmonyInit &initStruct) : IHyApplication(initStruct),
											m_TestSprite("", "NewSprite"),
											m_TestQuad(1)
{
}


NewGame::~NewGame()
{
}

const char *szCUSTOM_FRAGMENTSHADER = "								\n\
#version 400																\n\
																			\n\
in vec4 Color;																\n\
out vec4 FragColor;															\n\
																			\n\
void main()																	\n\
{																			\n\
	FragColor = Color;														\n\
}";

/*virtual*/ bool NewGame::Initialize()
{
	m_pCam = Window().CreateCamera2d();

	m_TestSprite.Load();
	m_TestSprite.SetEnabled(true);

	m_TestQuad.Load();
	m_TestQuad.SetTextureSource(0, 100, 100, 500, 500);
	
	m_primBox.color.Set(0.0f, 0.0f, 1.0f, 1.0f);
	m_primBox.SetAsQuad(180.0f, 160.0f, false);
	m_primBox.SetDisplayOrder(0);
	m_primBox.pos.Set(-405.0f, 0.0f);

	glm::vec2 vLinePts[2];

	vLinePts[0].x = -2048.0f;
	vLinePts[0].y = 0.0f;
	vLinePts[1].x = 2048.0f;
	vLinePts[1].y = 0.0f;
	m_HorzLine.SetAsEdgeChain(vLinePts, 2, false);
	m_HorzLine.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
	m_HorzLine.Load();

	vLinePts[0].x = 0.0f;
	vLinePts[0].y = -2048.0f;
	vLinePts[1].x = 0.0f;
	vLinePts[1].y = 2048.0f;
	m_VertLine.SetAsEdgeChain(vLinePts, 2, false);
	m_VertLine.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
	m_VertLine.Load();


	IHyShader *pShader_Checkerboard = IHyRenderer::GetShader(10);
	pShader_Checkerboard->SetSourceCode(szCUSTOM_FRAGMENTSHADER, HYSHADER_Fragment);
	pShader_Checkerboard->SetVertexAttribute("position", HYSHADERVAR_vec4);
	pShader_Checkerboard->Finalize();

	m_primBox.SetCustomShader(pShader_Checkerboard);
	m_primBox.Load();

	return true;
}

/*virtual*/ bool NewGame::Update()
{
	return true;
}

/*virtual*/ void NewGame::Shutdown()
{
}
