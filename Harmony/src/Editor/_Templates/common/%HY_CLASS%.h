#ifndef %HY_CLASS%_h__
#define %HY_CLASS%_h__

#include "Harmony/HyEngine.h"

class %HY_CLASS% : public HyEngine
{
	HyCamera2d *	m_pCamera;

public:
	%HY_CLASS%(HarmonyInit &initStruct);
	virtual ~%HY_CLASS%();

	virtual bool OnUpdate() override;
};

#endif // %HY_CLASS%_h__
