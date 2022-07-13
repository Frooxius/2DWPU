#pragma once
#include "Global.h"
#include "Memory.h"
#include "CoreInfoHolders.h"
#include "Registers.h"
#include "IOinterface.h"
#include "DecodedInstr.h"

namespace WPU2D
{
	using namespace Shared;

	namespace Core
	{
		class Simple2DWPUcore
		{
			DEBUG_ALLPUBLIC

			// Statistics
			CoreStats *stats;

			// Required memories foinsBaseOperationr operation
			Memory *RAM;
			MemoryStack<byte> *instr_stack;
			MemoryStack<reg32> *arg_stack;

			reg16 fetch_instr;	// raw fetched instruction from the memory

			// The IO interface access
			IOinterface *io;

			reg16 ptrHE16(reg16 add) { return ptrIO16(globreg->HE+add); }
			reg8 ptrHE8(reg16 add) { return ptrIO8(globreg->HE+add); }

			void ptrHE16(reg16 val, reg16 add) { ptrIO(globreg->HE+add, val); }
			void ptrHE8(reg8 val, reg16 add) { ptrIO(globreg->HE+add, val); }

			reg8 ptrIO8(reg16 addr);
			reg16 ptrIO16(reg16 addr);
			void ptrIO(reg16 addr, reg16 val);
			void ptrIO(reg16 addr, reg8 val);

			// Indirect memory access
			reg32 ptrRAM(reg32 addr);
			void ptrRAM(reg32 addr, reg32 val);
			void ptrRAM(reg32 addr, reg16 val);
			void ptrRAM(reg32 addr, reg8 val);
			
			// Indirect memory access trough registers
		public:
			reg32 ptrST32(reg32 add)   { return ptrRAM(globreg->ST+add); }
		private:
			reg16 ptrST16(reg32 add)  { return (reg16)ptrRAM(globreg->ST+add); }
			reg8  ptrST8 (reg32 add)  { return (reg8)ptrRAM(globreg->ST+add); }

			void ptrST32(reg32 val, reg32 add) { ptrRAM(globreg->ST+add, val); }
			void ptrST16(reg16 val, reg32 add) { ptrRAM(globreg->ST+add, val); }
			void ptrST8 (reg8  val, reg32 add) { ptrRAM(globreg->ST+add, val); }

			// accessing the index table
			reg32 IndexTable(reg16 addr);

			// fetching instructions
			reg16 FetchInstruction(regPC addr);

			// Registers
			GlobalRegisters *globreg;
			PrivateRegisters reg; // has its own copy

			// Accessing special registers
			reg32 SW() { return reg.P_SW.raw | (((reg32)globreg->S_SW.raw)<<16); }
			void SW(reg32 val) { reg.P_SW.raw = val; globreg->S_SW.raw = val >> 16; }
			reg16 SI() { return instr_stack->GetAddr(); }
			void SI(reg16 val) { instr_stack->SetAddr(val); }
			reg16 SA() { return arg_stack->GetAddr(); }
			void SA(reg16 val) { arg_stack->SetAddr(val); }

			// Accessing the registers
			reg32 QueryRegAccess(uint regid);
			bool QueryBitAccess(uint bitid);

			reg32 *PasstroughDecodeReg(uint regid);
			reg32 PasstroughReg(uint regid);
			void PasstroughReg(uint regid, reg32 val);

			reg32 *PasstroughDecodeSTACKReg(uint regid);
			reg32 PasstroughSTACKReg(uint regid);
			void PasstroughSTACKReg(uint regid, reg32 val);

			reg32 ReturnRegister(uint regid);
			void TakeRegister(uint regid, reg32 val);

			// Holds info about the current program block
			ProgramBlockInfo pblock;

			// Program block decoding
			void DecodeProgramBlockHeader();

			// Operations with stacks
			void InstrStackPush(InstructionFlowWord ifw);
			InstructionFlowWord InstrStackPop();
			InstructionFlowWord InstrStackTop();	// get the top value without popping
			void InstrStackReset();

			void ArgStackPush(reg32 val);
			reg32 ArgStackPop();
			void ArgStackReset();

			// Executing instructions
			bool ExecuteInstruction(DecodedInstr instr);

			/*  ********************************
						Emulation flow
				*******************************/
			bool InternCycle();	// perform one cycle

			void FlowReturn() { FlowReturn(reg.ARG); }
			void FlowReturn(reg32 val);
			void FlowQuery(SimpleDir dir);
			void FlowQuery(QueryDir dir);
			void FlowQuery();	// query without changing the PC
			void UpdatePC(QueryDir dir);
			QueryDir ProcessDir(QueryDir dir);

			void ProgramBlockJump(regPC destination);

			// Determine if the condition is true or not
			bool QueryCondition(byte queryID);

			// Calculate result
			reg32 Operation(reg32 a, reg32 b, byte op_index);

			// perform action based in the passtrough instruction type
			void Action(uint index);

			void PerfomQuery(DecodedInstr instr);	// perform an query instruction
			void PerformOperation(DecodedInstr instr);	// perform operation instruction
			void PerformPasstrough(DecodedInstr instr);	// perform the passtrough instruction

			void PerformReturn(DecodedInstr instr);
			void PerformDouble(DecodedInstr instr);
			void PerformExtended(DecodedInstr instr);

			
			/*  ********************************
						Parallelism
				*******************************/
			struct
			{
				regPC parPC;
				bool rqst, free, take, redy;
				reg32 parARG;
				reg32 PO, PE;
			} parallel;

			// Initialization
			void Initialize(Memory *RAM, IOinterface *io,
				uint instr_stack_size, uint arg_stack_size,
				bool secondary, GlobalRegisters *globreg, CoreStats *stats);
						
		public:
			// Constructors
			Simple2DWPUcore(Memory *RAM, IOinterface *io, bool secondary,
				GlobalRegisters *globreg, CoreStats *stats);	// default stacks size
			Simple2DWPUcore(Memory *RAM, IOinterface *io,
				uint instr_stack_size, uint arg_stack_size,
				bool secondary, GlobalRegisters *globreg, CoreStats *stats);

			void Activate(bool activate) { reg.P_SW.AC(activate); }

			void CheckParallelInvoke();

			// Paralellism - output
			bool C_FTCH() { return reg.P_SW.PDF(); }
			bool C_FREE() { return !reg.P_SW.AC(); }
			bool C_RQST() { return parallel.rqst; }
			regPC C_PC()  { return parallel.parPC; }
			reg32 C_ARG() { return parallel.parARG; }
			reg32 C_PO() { return parallel.PO; }
			reg32 C_PE() { return parallel.PE; }

			// Paralelism input
			void C_REDY(bool set) { parallel.redy = set; }
			void C_ACPT(bool set);
			void C_RQST(bool set) { parallel.rqst = set; }
			void C_PC(regPC set)  { parallel.parPC = set; }
			void C_ARG(reg32 set)  { parallel.parARG = set; }

			// performs a single cycle of the individual core
			bool Cycle() { return InternCycle(); }

			void Reset();

			// Accessing internal data in a civilized manner
			PrivateRegisters *GetPrivateReg() {
				reg.SA = SA();
				reg.SI = SI();
				return &reg; 
			}
		};
	}
}