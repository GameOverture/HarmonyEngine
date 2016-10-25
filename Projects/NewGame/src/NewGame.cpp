#include "NewGame.h"


NewGame::NewGame(HarmonyInit &initStruct) : IHyApplication(initStruct),
											m_TestSprite("", "NewSprite"),
											m_TestQuad(1),
											m_TestText("", "NewFont")
{
}


NewGame::~NewGame()
{
}

const char *szCUSTOM_VERTEXSHADER = "									\n\
#version 400																\n\
																			\n\
layout(location = 0) in vec4 position;										\n\
																			\n\
out vec2 coordinate;															\n\
																			\n\
uniform mat4 transformMtx;													\n\
uniform mat4 mtxWorldToCamera;												\n\
uniform mat4 mtxCameraToClip;												\n\
																			\n\
void main()																	\n\
{																			\n\
	coordinate.x = position.x;												\n\
	coordinate.y = position.y;												\n\
																			\n\
	vec4 temp = transformMtx * position;									\n\
	temp = mtxWorldToCamera * temp;											\n\
	gl_Position = mtxCameraToClip * temp;									\n\
}";


const char *szCUSTOM_FRAGMENTSHADER = "								\n\
#version 400																		\n\
																					\n\
in vec2 coordinate;																	\n\
out vec4 FragColor;																	\n\
																					\n\
uniform float uGridSize;															\n\
uniform vec2 uResolution;															\n\
uniform vec4 gridColor1;															\n\
uniform vec4 gridColor2;															\n\
																					\n\
void main()																			\n\
{																					\n\
	vec2 screenCoords = (gl_FragCoord.xy - (uResolution * 0.5f)) / uGridSize;		\n\
	FragColor = mix(gridColor1, gridColor2, step((float(int(floor(screenCoords.x) + floor(screenCoords.y)) & 1)), 0.9));		\n\
}";

glm::vec2 textBoxSize;

/*virtual*/ bool NewGame::Initialize()
{
	m_pCam = Window().CreateCamera2d();

	textBoxSize.x = 100.0f;
	textBoxSize.y = 200.0f;

	m_TestSprite.Load();
	m_TestSprite.SetEnabled(true);
	m_TestSprite.topColor.Set(0.0f, 1.0f, 0.0f, 1.0f);
	m_TestSprite.botColor.Set(1.0f, 0.0f, 0.0f, 1.0f);
	m_TestSprite.SetTransparency(0.5f);

	m_TestText.Load();
	m_TestText.TextSet("Oh my goodness, oh my damn. Oh my goodness, they going ham.");
	m_TestText.SetAsBox(textBoxSize.x, textBoxSize.y, false, false, true);
	m_TestText.pos.Set(0.0f, -100.0f);
	//m_TestText.rot.Set(45.0f);
	//m_TestText.scale.Set(0.5f, 2.0f);
	
	m_primBox.SetTint(0.0f, 0.0f, 1.0f);
	m_primBox.SetAsQuad(textBoxSize.x, textBoxSize.y, true);
	m_primBox.SetDisplayOrder(0);
	m_primBox.pos.Set(m_TestText.pos.X(), m_TestText.pos.Y() - textBoxSize.y);
	m_primBox.Load();

	m_TestQuad.Load();
	m_TestQuad.SetTextureSource(0, 100, 100, 500, 500);

	glm::vec2 vLinePts[2];

	vLinePts[0].x = -2048.0f;
	vLinePts[0].y = 0.0f;
	vLinePts[1].x = 2048.0f;
	vLinePts[1].y = 0.0f;
	m_HorzLine.SetAsEdgeChain(vLinePts, 2, false);
	m_HorzLine.SetTint(1.0f, 0.0f, 0.0f);
	m_HorzLine.Load();

	vLinePts[0].x = 0.0f;
	vLinePts[0].y = -2048.0f;
	vLinePts[1].x = 0.0f;
	vLinePts[1].y = 2048.0f;
	m_VertLine.SetAsEdgeChain(vLinePts, 2, false);
	m_VertLine.SetTint(1.0f, 0.0f, 0.0f);
	m_VertLine.Load();

	IHyShader *pShader_Checkerboard = IHyRenderer::MakeCustomShader();
	pShader_Checkerboard->SetSourceCode(szCUSTOM_VERTEXSHADER, HYSHADER_Vertex);
	pShader_Checkerboard->SetVertexAttribute("position", HYSHADERVAR_vec4);
	pShader_Checkerboard->SetSourceCode(szCUSTOM_FRAGMENTSHADER, HYSHADER_Fragment);
	pShader_Checkerboard->Finalize(HYSHADERPROG_Primitive);
	
	m_TestGrid.SetResolution(Window().GetResolution().x, Window().GetResolution().y);
	m_TestGrid.SetDisplayOrder(-100);
	m_TestGrid.pos.Set(Window().GetResolution().x * -0.5f, Window().GetResolution().y * -0.5f);
	m_TestGrid.SetCustomShader(pShader_Checkerboard);
	m_TestGrid.Load();


	return true;
}

/*virtual*/ bool NewGame::Update()
{
	return true;
}

/*virtual*/ void NewGame::Shutdown()
{
}
