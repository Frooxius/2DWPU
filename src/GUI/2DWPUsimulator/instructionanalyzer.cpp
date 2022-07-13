#include "instructionanalyzer.h"
#include "ui_instructionanalyzer.h"

InstructionAnalyzer::InstructionAnalyzer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstructionAnalyzer)
{
    ui->setupUi(this);
}

InstructionAnalyzer::~InstructionAnalyzer()
{
    delete ui;
}
