/**************************************************************************
 *	WidgetRangedSlider.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETRANGEDSLIDER_H
#define WIDGETRANGEDSLIDER_H

#include <QWidget>

namespace Ui {
class WidgetRangedSlider;
}

class WidgetRangedSlider : public QWidget
{
	Q_OBJECT
	
	enum eType
	{
		TYPE_INT = 0,
		TYPE_DOUBLE
	};
	
	enum eSliderType
	{
		SLIDERTYPE_Ranged = 0,
		SLIDERTYPE_Single
	};
	
	int                         m_iPrevMin;
	int                         m_iPrevMax;
	double                      m_dPrevMin;
	double                      m_dPrevMax;
	
	int                         m_iPrevSingle;
	double                      m_dPrevSingle;
	
public:
	explicit WidgetRangedSlider(QWidget *parent = 0);
	~WidgetRangedSlider();
	
	void SetAsRangedSlider(bool bEnable);
	
	bool IsIntType();
	bool IsRangedType();
	
	void SetRange(int iMin, int iMax);
	void SetRange(double dMin, double dMax, int iNumDecimalPrecision);
	
	void SetValue(int iMin, int iMax);
	void SetValue(double dMin, double dMax);
	
	QVariant GetRangedValueMin();
	QVariant GetRangedValueMax();
	
	QVariant GetSingleValue();
	
Q_SIGNALS:
	void userChangedRangedValue(QVariant &oldMin, QVariant &oldMax, QVariant &newMin, QVariant &newMax);
	void userChangedSingleValue(QVariant &oldValue, QVariant &newValue);
	
private Q_SLOTS:
	void on_sbMin_Double_editingFinished();
	
	void on_sbMax_Double_editingFinished();
	
	void on_sbMin_Int_editingFinished();
	
	void on_sbMax_Int_editingFinished();
	
	void on_maxSlider_sliderMoved(int position);
	
	void on_minSlider_sliderMoved(int position);
	
	void on_sbSingle_Int_editingFinished();
	
	void on_sbSingle_Double_editingFinished();
	
	void on_singleSlider_sliderMoved(int position);
	
private:
	Ui::WidgetRangedSlider *ui;
	
	void OnDataCorrect();
};

#endif // WIDGETRANGEDSLIDER_H
