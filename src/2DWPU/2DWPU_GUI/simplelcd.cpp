#include "simplelcd.h"

SimpleLCD::SimpleLCD(QGraphicsScene *scene, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->scene = scene;
	ui.simpleLCD->setScene(scene);
	ui.simpleLCD->scale(2.0, 2.0);
}

SimpleLCD::~SimpleLCD()
{

}
