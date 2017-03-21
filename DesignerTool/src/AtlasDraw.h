#ifndef ATLASDRAW_H
#define ATLASDRAW_H

#include "IDraw.h"

class AtlasDraw : public IDraw
{
public:
    AtlasDraw(IHyApplication &hyApp);
    virtual ~AtlasDraw();
};

#endif // ATLASDRAW_H
