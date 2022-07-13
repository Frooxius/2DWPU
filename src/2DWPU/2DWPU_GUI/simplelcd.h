#ifndef SIMPLELCD_H
#define SIMPLELCD_H

#include <QWidget>
#include <qgraphicsscene.h>
#include "ui_simplelcd.h"

class SimpleLCD : public QWidget
{
	Q_OBJECT

	QGraphicsScene *scene;

public:
	SimpleLCD(QGraphicsScene *scene, QWidget *parent = 0);
	~SimpleLCD();

private:
	Ui::SimpleLCD ui;
};

#endif // SIMPLELCD_H
