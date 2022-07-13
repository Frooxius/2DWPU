#ifndef SIMPLELCD_H
#define SIMPLELCD_H

#include <QWidget>
#include <qgraphicsscene.h>
#include <qimage.h>
#include "ui_simplelcd.h"
#include <qtimer.h>
#include "qdatetime.h"

class SimpleLCD : public QWidget
{
	Q_OBJECT

	QGraphicsScene *scene;
	int updates;	// how many updates there were
	int dropped;	// how many of them were dropped

	QString savedir;
	uint frame;

public:
	SimpleLCD(QGraphicsScene *scene, QWidget *parent = 0);
	~SimpleLCD();

	void Draw(QImage *img)
	{
		if(savedir.length())
			img->save(savedir + "\\" + QString::number(frame++) + ".png",
					"PNG");

		if(framesync->elapsed() >= 16)
		{
			scene->clear();
			scene->addPixmap(QPixmap::fromImage(*img));
			scene->update();
			framesync->restart();
			updates++;
		}
		else 
			dropped++;
	}
	
	void SaveStart(QString dir) { savedir = dir; }
	void SaveStop() { savedir = ""; }

private:
	Ui::SimpleLCD ui;

	QTimer *timer;
	QTime *framesync;

private slots:
	void TimerUpdate();
};

#endif // SIMPLELCD_H
