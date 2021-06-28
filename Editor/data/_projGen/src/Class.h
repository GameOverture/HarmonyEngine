#ifndef %HY_CLASS%_h__
#define %HY_CLASS%_h__

#include "pch.h"

class %HY_CLASS% : public HyEntity2d
{
public:
	%HY_CLASS%(HyEntity2d *pParent);
	virtual ~%HY_CLASS%();

	virtual void OnUpdate() override;
};

#endif // %HY_CLASS%_h__
