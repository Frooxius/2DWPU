#include "gui_2dwpu.h"
#include "Mnemonics.h"
#include "qdatetime.h"
#include "qfiledialog.h"
#include "FileLineReader.h"
#include "MemoryBinWriter.h"
#include "qinputdialog.h"

#include "IOsimpleLCD.h"
#include "IOrandgen.h"

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
		*(ram->data+i) = 0xFFU;

	SimSetCores(1);	// creates the WPU

	// frequency and such
	wpuFreq = 0.0f;
	wpuFreqMax = false;
	wpuPrevCycles = 0;

	// setup the thread
	wpuThread = new WPU2Dthread(&wpu, &wpuFreq, &wpuFreqMax);
	connect(wpuThread, SIGNAL(UpdateUI()), this, SLOT(Update()));

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
	wpu->Cycle();
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
		+ "<br/><b>SA: </b>" + QString::number(wpu->GetPrivateReg(core)->SA, 16)
		+ "<b> SI: </b>" + QString::number(wpu->GetPrivateReg(core)->SI, 16);
	info += "<br/>";
	return info;
}

void GUI_2DWPU::Open2DASM()
{
	QString fname = QFileDialog::getOpenFileName();
	FileLineReader *reader = new FileLineReader(fname.toAscii().data());
	WPU2D::ASM2D::Assembler2D::Assembly(
		reader,
		new MemoryBinWriter(this->ram));

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
		this->img_SimpleLCD), 1);

	Update();
}

void GUI_2DWPU::ConfigureCoreNumber()
{
	SimSetCores(QInputDialog::getInt(this, "Change amount of cores",
		"Please enter amount of cores:", 1, 1, 128));
}
