#ifndef GUI_2DWPU_H
#define GUI_2DWPU_H

#include <QtGui/QMainWindow>
#include <qtimer.h>
#include <qgraphicsscene.h>
#include <qthread.h>
#include <qinputdialog.h>
#include <qfiledialog.h>

#include "ui_gui_2dwpu.h"
#include "simplelcd.h"

#include "2DWPU.h"
#include "Global.h"
#include "2DASM.h"
#include "2DWPUthread.h"

// Other dialogs
#include "BasicInstructionSetAnalyzer.h"

struct ProgramBlockViewData
{
	reg24 BS;
};

class GUI_2DWPU : public QMainWindow
{
	Q_OBJECT

public:
	GUI_2DWPU(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GUI_2DWPU();

public slots:
	void Open_BasicInstructionSetAnalyzer() {
		win_BasicInstructionSetAnalyzer->show(); }

	void Update(bool force_full = false);	// update whole interface
	void UpdateFreq();

	// Simulation related
	void SimStep();
	void SimStop();
	void SimMax();
	void Sim30Hz();
	void SimReset();
	void SimSetCores(uint cores);

	void StatRun();

	void SimSetBreakpoint();
	void SimClearBreakpoints();

	void ConfigureCoreNumber();

	// Opening and saving stuff
	void Open2DASM();

	// TEMP
	void DispMemArea()
	{
		int base = QInputDialog::getInt(this, "", "", 0, 0);
		QImage img(ram->data+base, 416, 256, QImage::Format_RGB32);
		win_SimpleLCD->Draw(&img);
	}
	void StatSimpleLCDSave()
	{
		if(ui.actionSimpleLCD_Save->isChecked())
			win_SimpleLCD->SaveStart(
				QFileDialog::getExistingDirectory());
		else
			win_SimpleLCD->SaveStop();
	}
		// ENDTEMP

private:
	Ui::GUI_2DWPUClass ui;

	// Visualisation elements
	QString GetGlobalInfo();
	QString GetCoreInfo(uint core);
	void UpdateCoreTextInfo();

	QTimer *timerFreq;

	// ProgramBlockView
	ProgramBlockViewData programBlockView;
	void UpdateProgramBlockView(bool force_full);
	void DecodeProgramBlockTable();

	// Statistics
	void UpdateStats();

	// Other dialogs
	BasicInstructionSetAnalyzer *win_BasicInstructionSetAnalyzer;

	SimpleLCD *win_SimpleLCD;
	QGraphicsScene *img_SimpleLCD;

	// 2DWPU simulation
	WPU2D::Core::WPU2Dcore *wpu;
	float wpuFreq;
	bool wpuFreqMax;
	ull wpuPrevCycles;
	WPU2Dthread *wpuThread;
	Memory *ram;	// RAM of the WPU
};

#endif // GUI_2DWPU_H
