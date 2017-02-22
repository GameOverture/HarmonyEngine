#include "TextTest.h"


TextTest::TextTest(HarmonyInit &initStruct) : IHyApplication(initStruct)
{
}


TextTest::~TextTest()
{
}

/*virtual*/ bool TextTest::Initialize()
{
	return true;
}

/*virtual*/ bool TextTest::Update()
{
	return true;
}

/*virtual*/ void TextTest::Shutdown()
{
}
