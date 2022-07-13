#ifndef INSTRUCTIONANALYZER_H
#define INSTRUCTIONANALYZER_H

#include <QDialog>

namespace Ui {
class InstructionAnalyzer;
}

class InstructionAnalyzer : public QDialog
{
    Q_OBJECT
    
public:
    explicit InstructionAnalyzer(QWidget *parent = 0);
    ~InstructionAnalyzer();
    
private:
    Ui::InstructionAnalyzer *ui;
};

#endif // INSTRUCTIONANALYZER_H
