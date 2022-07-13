#include "simplelcd.h"

SimpleLCD::SimpleLCD(QGraphicsScene *scene, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->scene = scene;
	ui.simpleLCD->setScene(scene);
	ui.simpleLCD->scale(2.0, 2.0);
	updates = 0;
	frame = 0;

	timer = new QTimer(this);
	framesync = new QTime();
	connect(timer, SIGNAL(timeout()), this, SLOT(TimerUpdate()));
	timer->start(1000);
	framesync->start();
}

SimpleLCD::~SimpleLCD()
{
	delete timer;
}

void SimpleLCD::TimerUpdate()
{
	this->setCaption("SimpleLCD - " +
		QString::number(updates) + " FPS (" +
		QString::number(dropped) + " dropped)");
	updates = 0;
	dropped = 0;
}
