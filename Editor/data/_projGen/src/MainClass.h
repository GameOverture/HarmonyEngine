#ifndef %HY_FILENAME%_h__
#define %HY_FILENAME%_h__

#include "pch.h"

class %HY_CLASS% : public HyEngine
{
	HyCamera2d *	m_pCamera;

	enum InputAction
	{
		INPUT_ExitGame = 0,
		
		INPUT_CameraUp,
		INPUT_CameraLeft,
		INPUT_CameraDown,
		INPUT_CameraRight
	};

public:
	%HY_CLASS%(HyInit &initStruct);
	virtual ~%HY_CLASS%();

	virtual bool OnUpdate() override;
};

#endif // %HY_FILENAME%_h__
