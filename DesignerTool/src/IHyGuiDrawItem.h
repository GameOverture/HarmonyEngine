#ifndef IHYGUIDRAWITEM
#define IHYGUIDRAWITEM

class WidgetRenderer;

class IHyGuiDrawItem
{
protected:
    virtual void Hide() = 0;
    virtual void Show() = 0;
    virtual void Draw(WidgetRenderer &renderer) = 0;
};

#endif // IHYGUIDRAWITEM

