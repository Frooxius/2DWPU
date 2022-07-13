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
			reg32 ptrST32(reg32 add)   { return ptrRAM(globreg->ST+add); }
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
			void ExecuteInstruction(DecodedInstr instr);

			/*  ********************************
						Emulation flow
				*******************************/
			void InternCycle();	// perform one cycle

			void FlowReturn() { FlowReturn(reg.ARG); }
			void FlowReturn(reg32 val);
			void FlowQuery(SimpleDir dir);
			void FlowQuery(QueryDir dir);
			void FlowQuery();	// query without changing the PC
			void UpdatePC(QueryDir dir);

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

			// Initialization
			void Initialize(Memory *RAM, IOinterface *io,
				uint instr_stack_size, uint arg_stack_size,
				bool secondary, GlobalRegisters *globreg);
						
		public:
			// Constructors
			Simple2DWPUcore(Memory *RAM, IOinterface *io, bool secondary, GlobalRegisters *globreg);	// default stacks size
			Simple2DWPUcore(Memory *RAM, IOinterface *io,
				uint instr_stack_size, uint arg_stack_size,
				bool secondary, GlobalRegisters *globreg);

			void Activate(bool activate) { reg.P_SW.AC(activate); }

			// Paralellism - output
			bool C_FREE();
			bool C_RQST();
			ushort C_xPC();
			ushort C_yPC();
			uint C_ARG();

			// Paralelism input
			void C_RQST(bool set);
			void C_xPC(ushort set);
			void C_yPC(ushort set);
			void C_ARG(uint set);

			// performs a single cycle of the individual core
			void Cycle() { InternCycle(); }

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