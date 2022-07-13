#ifndef BASICINSTRUCTIONSETANALYZER_H
#define BASICINSTRUCTIONSETANALYZER_H

#include <QWidget>
#include "ui_BasicInstructionSetAnalyzer.h"

class BasicInstructionSetAnalyzer : public QWidget
{
	Q_OBJECT

public:
	BasicInstructionSetAnalyzer(QWidget *parent = 0);
	~BasicInstructionSetAnalyzer();

private:
	Ui::BasicInstructionSetAnalyzer ui;
};

#endif // BASICINSTRUCTIONSETANALYZER_H
