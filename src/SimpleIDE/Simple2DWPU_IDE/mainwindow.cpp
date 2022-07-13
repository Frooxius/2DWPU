#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(NewFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(OpenFile()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(SaveFile()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::NewFile()
{
    ui->tableWidget->clear();
}

void MainWindow::OpenFile()
{
    NewFile();

    QString file = QFileDialog::getOpenFileName(this);

    QFile f(file);
    f.open(QFile::ReadOnly);

    f.readLine();   // skip the first

    QString line;
    for(int y = 0; y < 32; y++)
    {
        do { line = f.readLine(); }
        while( ! (f.atEnd() || line.contains('[')) );

        if(f.atEnd())
            break;

        for(int x = 0; x < 32; x++)
        {
            if(!line.contains('['))
                break;
            line = line.remove(0, line.indexOf('[')+1);
            QString temp = line;
            temp.truncate(line.indexOf(']'));
            if( temp != "HALT" || temp.isEmpty() )
                ui->tableWidget->setItem(y,x, new QTableWidgetItem(temp));
        }
    }

    f.close();
}

void MainWindow::SaveFile()
{
   QString file = QFileDialog::getSaveFileName(this);
   QString source;

   source += "static block ";
   source += file.remove(0, file.lastIndexOf("\\"));
   source += "\n";

   for(int y = 0; y < 32; y++)
   {
       for(int x = 0; x < 32; x++)
       {
           if(ui->tableWidget->item(y,x) != 0)
           {
               if(ui->tableWidget->item(y,x)->text().isEmpty())
                   source += "[HALT]\t";
               else
                   source += "[" + ui->tableWidget->item(y,x)->text() + "]\t";
           }
           else
               source += "[HALT]\t";
       }
       source += "\n";
   }

   source += "<ENDBLOCK>\n";

   QFile f(file);
   f.open(QFile::WriteOnly);
   f.write(source.toAscii());
}


