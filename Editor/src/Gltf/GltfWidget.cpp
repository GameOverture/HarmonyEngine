#include "global.h"
#include "GltfWidget.h"

#include "ui_GltfWidget.h"

GltfWidget::GltfWidget(QWidget *pParent) :
	QWidget(pParent),
	m_pModel(nullptr),
	ui(new Ui::GltfWidget)
{
	ui->setupUi(this);

	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("GltfWidget::GltfWidget() invalid constructor used", LOGTYPE_Error);
}

GltfWidget::GltfWidget(GltfModel *pModel, QWidget *parent) :
	QWidget(parent),
	m_pModel(nullptr),
	ui(new Ui::GltfWidget)
{
	ui->setupUi(this);
}

GltfWidget::~GltfWidget()
{
	delete ui;
}
