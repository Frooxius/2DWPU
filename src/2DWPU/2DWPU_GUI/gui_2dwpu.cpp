#include "gui_2dwpu.h"
#include "Mnemonics.h"
#include "qdatetime.h"
#include "qfiledialog.h"
#include "FileLineReader.h"
#include "MemoryBinWriter.h"
#include "qinputdialog.h"
#include "2DASM_Exception.h"
#include "qmessagebox.h"

#include "IOsimpleLCD.h"
#include "IOrandgen.h"
#include "IOcount.h"

using namespace WPU2D::Core;
using namespace WPU2D::Shared;
using namespace WPU2D::ASM2D;

GUI_2DWPU::GUI_2DWPU(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	// Basic UI stuff
	connect(ui.BsimStep, SIGNAL(clicked()), this, SLOT(SimStep()));
	connect(ui.BsimMax, SIGNAL(clicked()), this, SLOT(SimMax()));
	connect(ui.BsimStop, SIGNAL(clicked()), this, SLOT(SimStop()));
	connect(ui.Bsim30Hz, SIGNAL(clicked()), this, SLOT(Sim30Hz()));
	connect(ui.BsetBreakpoint, SIGNAL(clicked()), this, SLOT(SimSetBreakpoint()));
	connect(ui.BclearBreakpoints, SIGNAL(clicked()), this, SLOT(SimClearBreakpoints()));

	// TEMP:
	connect(ui.actionSimpleLCD_Disp_Mem_TEMP, SIGNAL(triggered()), this, SLOT(DispMemArea()));
	connect(ui.actionSimpleLCD_Save, SIGNAL(triggered()), this, SLOT(StatSimpleLCDSave()));
	// END TEMP

	connect(ui.statRunB, SIGNAL(clicked()), this, SLOT(StatRun()));

	connect(ui.actionReset, SIGNAL(triggered()), this, SLOT(SimReset()));
	connect(ui.actionSet_Cores, SIGNAL(triggered()), this, SLOT(ConfigureCoreNumber()));


	timerFreq = new QTimer();
	connect(timerFreq, SIGNAL(timeout()), this, SLOT(UpdateFreq()));
	timerFreq->start(1000);

	// Opening and saving stuff
	connect(ui.actionOpen_2DASM, SIGNAL(triggered()), this, SLOT(Open2DASM()));

	// Configure other dialogs

	// Basic Instruction Set Analyzer
	win_BasicInstructionSetAnalyzer = new BasicInstructionSetAnalyzer();
	connect(ui.actionBasic_Instruction_Set_Analyzer, SIGNAL(triggered()),
		this, SLOT(Open_BasicInstructionSetAnalyzer()));

	// SimpleLCD
	img_SimpleLCD = new QGraphicsScene(0, 0, 400, 240, this);
	win_SimpleLCD = new SimpleLCD(img_SimpleLCD);
	connect(ui.actionSimpleLCD_400x240, SIGNAL(triggered()), win_SimpleLCD,
		SLOT(show()));

	// **************************************
	// Configure visualisation of the simulation
	// **************************************
	programBlockView.BS = 0xFFFFFFFFU;

	// **************************************
	// Configure the WPU for simulation
	// **************************************

	// Prepare memory first
	ram = new Memory(1024*1024*16);
	for(int i = 0; i < ram->size; ++i)
		*(ram->data+i) = rand();

	/*for(int i = 8000; i < 8000+128*24; i+=24)
	{
		*((reg32*)(ram->data+i)) = (i-8000)/24;
		*((reg32*)(ram->data+i+4)) = (i-8000)/24;
	}*/

	SimSetCores(1);	// creates the WPU

	// frequency and such
	wpuFreq = 0.0f;
	wpuFreqMax = false;
	wpuPrevCycles = 0;

	// setup the thread
	wpuThread = new WPU2Dthread(&wpu, &wpuFreq, &wpuFreqMax);
	connect(wpuThread, SIGNAL(UpdateUI()), this, SLOT(Update()));
	connect(wpuThread, SIGNAL(Halt()), this, SLOT(SimStop()));

	// Lastly, update the interface
	Update();
}

GUI_2DWPU::~GUI_2DWPU()
{

}

// Visualization

void GUI_2DWPU::Update(bool force_full)
{
	if(wpuFreqMax)
		return;
	UpdateProgramBlockView(force_full);
	UpdateCoreTextInfo();
	UpdateStats();
}

void GUI_2DWPU::UpdateCoreTextInfo()
{
	QString info;
	info = GetGlobalInfo();
	info += "<br/><br/>";
	for(int i = 0; i < wpu->GetCoreCount(); i++)
	{
		info += GetCoreInfo(i);
		info += "<br/>";
	}

	ui.coresInfoT->setText(info);
}

void GUI_2DWPU::UpdateProgramBlockView(bool force_full)
{
	// check if it shouldn't re-render
	if((wpu->GetPrivateReg(0)->PC.BS != programBlockView.BS)
		|| force_full)
		DecodeProgramBlockTable();

	ui.programBlockInstrTable->clearSelection();
	wpu->CheckParallelInvoke();
	for(int i = 0; i < wpu->GetCoreCount(); ++i)
	{
		if(wpu->GetPrivateReg(i)->P_SW.AC())
			ui.programBlockInstrTable->setCurrentCell(
				wpu->GetPrivateReg(i)->PC.yPC, wpu->GetPrivateReg(i)->PC.xPC,
				QItemSelectionModel::SelectionFlag::Select);
	}
}

void GUI_2DWPU::DecodeProgramBlockTable()
{
	for(int y = 0; y < 32; y++)
		for(int x = 0; x < 32; x++)
		{
			reg16 instr = *((reg16*)(ram->data +
				PROGRAM_BLOCK_HEADER_SIZE +
				x * INSTRUCTION_WIDTH +
				y * INSTRUCTION_WIDTH * PROGRAM_BLOCK_WIDTH));

			ui.programBlockInstrTable->setItem(y, x,
				new QTableWidgetItem( QString::fromStdString(
				Mnemonic(DecodedInstr(instr)).GetMnemonic()) ));
		}
}

void GUI_2DWPU::SimStep()
{
	wpu->Cycle(ui.stepsS->value());
	Update();
}

void GUI_2DWPU::SimStop()
{
	wpuFreq = 0.0f;
	wpuFreqMax = false;
	wpuThread->Stop();
	Update();
}

void GUI_2DWPU::SimMax()
{
	wpuFreqMax = true;
	wpuThread->StartIfNotRunning();
}

void GUI_2DWPU::SimReset()
{
	SimStop();
	wpu->Reset();
	Update();
}

void GUI_2DWPU::Sim30Hz()
{
	wpuFreq = 5.0f;
	wpuFreqMax = false;
	wpuThread->StartIfNotRunning();
}



QString GUI_2DWPU::GetGlobalInfo()
{
	QString info;
	info = "<b></u>Global:</b></u><br/>";
	info += "<b>LI: </b>" + QString::number(wpu->GetPublicReg()->LI, 16)
		+ "<b> SQ: </b>" + QString::number(wpu->GetPublicReg()->SQ, 16)
		+ "<br/>"
		+ "<b>RE: </b>" + QString::number(wpu->GetPublicReg()->RE, 16)
		+ "<b> TR: </b>" + QString::number(wpu->GetPublicReg()->TR, 16)
		+ "<br/>"
		+ "<b>CI: </b>" + QString::number(wpu->GetPublicReg()->CI, 16)
		+ "<b> HE: </b>" + QString::number(wpu->GetPublicReg()->HE, 16)
		+ "<br/>"
		+ "<b>ST: </b>" + QString::number(wpu->GetPublicReg()->ST, 16)
		+ "<b> EL: </b>" + QString::number(wpu->GetPublicReg()->EL, 16);

	info += "<br /><br />";

	info += "<b>*(ST+LI): </b>" + QString::number(wpu->GetMemVal(wpu->GetPublicReg()->LI), 16)
		+ "<br/>"
		+ "<b>*(ST+SQ): </b>" + QString::number(wpu->GetMemVal(wpu->GetPublicReg()->SQ), 16)
		+ "<br/>"
		+ "<b>*(ST+RE): </b>" + QString::number(wpu->GetMemVal(wpu->GetPublicReg()->RE), 16)
		+ "<br/>"
		+ "<b>*(ST+TR): </b>" + QString::number(wpu->GetMemVal(wpu->GetPublicReg()->TR), 16)
		+ "<br/>"
		+ "<b>*(ST+CI): </b>" + QString::number(wpu->GetMemVal(wpu->GetPublicReg()->CI), 16)
		+ "<br/>"
		+ "<b>*(ST+HE): </b>" + QString::number(wpu->GetMemVal(wpu->GetPublicReg()->HE), 16)
		+ "<br/>"
		+ "<b>*ST: </b>" + QString::number(wpu->GetMemVal(0), 16)
		+ "<br/>"
		+ "<b>*(ST+EL): </b>" + QString::number(wpu->GetMemVal(wpu->GetPublicReg()->EL), 16);

	return info;
}

QString GUI_2DWPU::GetCoreInfo(uint core)
{
	QString info;
	info = "<b><u>Core ";
	info += QString::number(core) + "</u></b><br/>";
	info += "<b>xPC:</b> " + QString::number(wpu->GetPrivateReg(core)->PC.xPC) +
		" <b>yPC:</b> " + QString::number(wpu->GetPrivateReg(core)->PC.yPC) +
		+ " <b>BS:</b> " + QString::number(wpu->GetPrivateReg(core)->PC.BS, 16);

	// Normal registers
	info += "<br/>";
	info += "<b>IS: </b>" + QString::number(wpu->GetPrivateReg(core)->IS)
		+ " <b>ARG: </b>" + QString::number(wpu->GetPrivateReg(core)->ARG, 16)
		+ "<br/><b>PO: </b>" + QString::number(wpu->GetPrivateReg(core)->PO, 16)
		+ "<b> PE: </b>" + QString::number(wpu->GetPrivateReg(core)->PE, 16)
		+ "<br/><b>*(ST+PO): </b>" + QString::number(wpu->GetMemVal(wpu->GetPrivateReg(core)->PO), 16)
		+ "<br/><b>*(ST+PE): </b>" + QString::number(wpu->GetMemVal(wpu->GetPrivateReg(core)->PE), 16)
		+ "<br/><b>SA: </b>" + QString::number(wpu->GetPrivateReg(core)->SA, 16)
		+ "<b> SI: </b>" + QString::number(wpu->GetPrivateReg(core)->SI, 16);
	info += "<br/>";
	return info;
}

void GUI_2DWPU::Open2DASM()
{
	FileLineReader *reader;

	try
	{
		QString fname = QFileDialog::getOpenFileName();
		reader = new FileLineReader(fname.toAscii().data());
		WPU2D::ASM2D::Assembler2D::Assembly(
			reader,
			new MemoryBinWriter(this->ram));
	}
	catch(WPU2D::ASM2D::ASM2D_Exception err)
	{
		QMessageBox::critical(this, "2DASM Error",
			err.GetMsg());
	}

	delete reader;
	Update(true);
	wpu->Reset();
}

void GUI_2DWPU::UpdateFreq()
{
    // detect real frequency
	ui.frequencyL->display((double)wpu->GetCycles()-wpuPrevCycles);
    wpuPrevCycles = wpu->GetCycles();
}

void GUI_2DWPU::SimSetCores(uint cores)
{
	wpu = new WPU2D::Core::Simple2DWPU(ram, cores);
	wpu->GetIOinterface()->InstallDevice(new WPU2D::Core::IOrandgen(), 0);
	wpu->GetIOinterface()->InstallDevice(new WPU2D::Core::IOsimpleLCD(
		this->win_SimpleLCD), 1);
	wpu->GetIOinterface()->InstallDevice(new WPU2D::Core::IOcounter(), 10);

	Update();
}

void GUI_2DWPU::ConfigureCoreNumber()
{
	SimSetCores(QInputDialog::getInt(this, "Change amount of cores",
		"Please enter amount of cores:", 1, 1, 128));
}

void GUI_2DWPU::UpdateStats()
{
	QString text;

	// calculate ips
	double ips = wpu->GetStats()->instructions/(double)wpu->GetStats()->cycles;

	text += "Total cycles: " + QString::number(wpu->GetStats()->cycles);
	text += "\t";
	text += "Total instructions: " + QString::number(wpu->GetStats()->instructions);
	text += "\t";
	text += "Average IPS: " + QString::number(ips);
	text += "\t";
	text += "Queries: " + QString::number(wpu->GetStats()->queries);
	text += "\t";
	text += "Returns: " + QString::number(wpu->GetStats()->returns);
	text += "\t";
	text += "<br />";

	text += "RAM reads: " + QString::number(wpu->GetStats()->memreads);
	text += "\t";
	text += "RAM writes: " + QString::number(wpu->GetStats()->memwrites);
	text += "\t";
	text += "IO reads: " + QString::number(wpu->GetStats()->ioreads);
	text += "\t";
	text += "IO writes: " + QString::number(wpu->GetStats()->iowrites);
	text += "<br />";

	text += "Parallel forks: " + QString::number(wpu->GetStats()->parallel_forks);
	text += "\t";
	text += "Parallel failed forks: " + QString::number(wpu->GetStats()->parallel_failed_forks);
	text += "\t";
	text += "Parallel joins: " + QString::number(wpu->GetStats()->parallel_joins);
	text += "\t";
	text += "Parallel failed joins: " + QString::number(wpu->GetStats()->parallel_failed_joins);

	ui.statsT->setText(text);
}

void GUI_2DWPU::SimSetBreakpoint()
{
	wpu->SetBreakpoint((reg5)ui.programBlockInstrTable->selectedItems()[0]->column(),
		(reg5)ui.programBlockInstrTable->selectedItems()[0]->row());
}

void GUI_2DWPU::SimClearBreakpoints()
{
	wpu->ClearBreakpoints();
}

void GUI_2DWPU::StatRun()
{
	uint samples = ui.statSamplesS->value();
	uint rate = ui.statSampleRateS->value();

	// allocate memory
	CoreStats *stats = new CoreStats[samples];

	// perform the measurements
	wpu->GetStats()->Reset();

	for(int i = 0; i < samples; ++i)
	{
		wpu->Cycle(rate);
		stats[i] = *wpu->GetStats();
		wpu->GetStats()->Reset();
	}

	QString dir = QFileDialog::getExistingDirectory() + "\\";

	// now write the stats down
	QFile out;

	out.setFileName(dir + "instructions.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].instructions) + "\n" );
	out.close();

	out.setFileName(dir + "ioreads.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].ioreads) + "\n" );
	out.close();

	out.setFileName(dir + "iowrites.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].iowrites) + "\n" );
	out.close();

	out.setFileName(dir + "memreads.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].memreads) + "\n" );
	out.close();

	out.setFileName(dir + "memwrites.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].memwrites) + "\n" );
	out.close();

	out.setFileName(dir + "queries.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].queries) + "\n" );
	out.close();

	out.setFileName(dir + "returns.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].returns) + "\n" );
	out.close();

	out.setFileName(dir + "ipc.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].instructions/(double)stats[i].cycles) + "\n" );
	out.close();

	out.setFileName(dir + "failed_forks.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].parallel_failed_forks) + "\n" );
	out.close();

	out.setFileName(dir + "forks.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].parallel_forks) + "\n" );
	out.close();

	out.setFileName(dir + "failed_joins.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].parallel_failed_joins) + "\n" );
	out.close();

	out.setFileName(dir + "joins.txt");
	out.open(QIODevice::OpenModeFlag::WriteOnly);
	for(int i = 0; i < samples; ++i)
		out.write( QString::number(stats[i].parallel_joins) + "\n" );
	out.close();
}
