#include "Simple2DWPUcore.h"
#include "DecodedInstr.h"
#include <cmath>
#include <limits>

namespace WPU2D
{
	namespace Core
	{
		Simple2DWPUcore::Simple2DWPUcore(Memory *RAM, IOinterface *io, bool secondary,
			GlobalRegisters *globreg)
		{
			Initialize(RAM, io, DEFAULT_INSTR_STACK_SIZE,
				DEFAULT_ARG_STACK_SIZE,
				secondary, globreg);
		}

		Simple2DWPUcore::Simple2DWPUcore(Memory *RAM, IOinterface *io,
				uint instr_stack_size, uint arg_stack_size,
				bool secondary, GlobalRegisters *globreg)
		{
			Initialize(RAM, io, instr_stack_size, arg_stack_size, secondary, globreg);
		}

		// Constructor initializes the core
		void Simple2DWPUcore::Initialize(Memory *RAM, IOinterface *io,
				uint instr_stack_size, uint arg_stack_size,
				bool secondary, GlobalRegisters *globreg)
		{
			this->RAM = RAM;
			this->io = io;
			instr_stack = new MemoryStack<byte>(instr_stack_size);
			arg_stack = new MemoryStack<reg32>(arg_stack_size);
			this->globreg = globreg;

			Reset();

			// make it secondary if needed
			reg.P_SW.SC(secondary);
			Activate(false); // it's deactivated by default
		}

		// this will set the processor to its default state
		void Simple2DWPUcore::Reset()
		{
			reg.ARG = 0;
			reg.IS = 0;
			reg.PE = 0;
			reg.PO = 0;

			// preserve the secondary status
			bool secondary = reg.P_SW.SC();
			reg.P_SW.raw = 0;
			reg.P_SW.SC(secondary);

			parallel.rqst = false;

			reg.PC.xPC = 0;
			reg.PC.yPC = 0;
			reg.PC.BS = 0;
			ProgramBlockJump(reg.PC);
		}

		void Simple2DWPUcore::DecodeProgramBlockHeader()
		{
			// check if it's valid address first
			if(PROGRAM_BLOCK_HEADER_SIZE + reg.PC.BS >
				RAM->size)
			{
				// create an empty one
				pblock = ProgramBlockInfo();
				return;
			}

			// get the beginning address of the block
			byte *base = RAM->data + reg.PC.BS;

			pblock.LMB = *((uint*)(base + offLMB));
			pblock.UMB = *((uint*)(base + offUMB));
			pblock.LIB = *((ushort*)(base + offLIB));
			pblock.UIB = *((ushort*)(base + offUIB));
			pblock.LCB = *((uint*)(base + offLCB));
			pblock.UCB = *((uint*)(base + offUCB));

			// this requires a bit more work
			pblock.CRB.BS = *((uint*)(base + offCRB_BS));
			pblock.CRB.xPC = *((ushort*)(base + offCRB_XY));
			pblock.CRB.yPC = *((ushort*)(base + offCRB_XY)) >> 5;
		}

		reg32 Simple2DWPUcore::QueryRegAccess(uint regid)
		{
			switch(regid)
			{
			case queryRegARG:
				return reg.ARG;

			case queryRegPTR_ST:
				return ptrST32(0);

			case queryRegPTR_HE:
				// TO DO

			case queryReg_ST_PO:
				return ptrST32(reg.PO);
			case queryRegLI:
				return globreg->LI;
			case queryRegSQ:
				return globreg->SQ;
			case queryRegRE:
				return globreg->RE;
			case queryRegTR:
				return globreg->TR;
			case queryRegCI:
				return globreg->CI;
			case queryRegPE:
				return reg.PE;
			case queryRegHE:
				return globreg->HE;
			case queryRegST:
				return globreg->ST;
			case queryRegEL:
				return globreg->EL;
			case queryRegPO:
				return reg.PO;

			default:
				// TODO exception
				break;
			}

			// make compiler happy
			return 0;
		}

		bool Simple2DWPUcore::QueryBitAccess(uint bitid)
		{
			switch(bitid)
			{
				case queryBitUB0:
					return globreg->S_SW.UB0();
				case queryBitUB1:
					return globreg->S_SW.UB1();
				case queryBitUB2:
					return globreg->S_SW.UB2();
				case queryBitUB3:
					return globreg->S_SW.UB3();

				case queryBitTO0:
				case queryBitTO1:
				case queryBitTO2:
				case queryBitTO3:
					// TODO
					return false;
					break;

				case queryBitCB:
					return reg.P_SW.CB();

				case queryBitIQ:
					return false;
					// TODO

				default:
					return false;	// what else to do?
			}
		}

		// One cycle of the core
		void Simple2DWPUcore::InternCycle()
		{
			CheckParallelInvoke();

			parallel.rqst = false;	// drop it

			if(reg.P_SW.AC())
				ExecuteInstruction(DecodedInstr(fetch_instr));
		}

		void Simple2DWPUcore::CheckParallelInvoke()
		{
			// if it's a secondary core, check if there's some task waiting
			if(!reg.P_SW.AC() && reg.P_SW.SC() && parallel.rqst)
			{
				parallel.free = false;
				reg.ARG = parallel.parARG;
				reg.PC = parallel.parPC;
				fetch_instr = FetchInstruction(reg.PC);
				reg.P_SW.AC(true);
			}
		}

		void Simple2DWPUcore::FlowQuery(QueryDir dir)
		{
			dir = ProcessDir(dir);	// in case it's C

			// Update the PC
			UpdatePC(dir);

			reg.P_SW.PCB(false);	// reset PCB

			// fetch the new instruction
			fetch_instr = FetchInstruction(reg.PC);

			// Push the return direction
			InstrStackPush(InstructionFlowWord(
				InvertSimpleDir(dir.dir), dir.length, reg.IS) );

			// Always execute the mandatory part
			FlowQuery();
		}

		void Simple2DWPUcore::FlowQuery(SimpleDir dir)
		{
			FlowQuery(QueryDir(dir, InstrStackTop().RL()));
		}

		void Simple2DWPUcore::FlowQuery()
		{
			reg.IS = 0;	// clean it for the next instruction
		}

		void Simple2DWPUcore::FlowReturn(reg32 val)
		{
			try
			{
				reg.ARG = val;
				InstructionFlowWord ifw = InstrStackPop();
				reg.P_SW.PDF(ifw.PDF());
				reg.IS = ifw.IS();
				UpdatePC(QueryDir(ifw.RD(), ifw.RL()));
				fetch_instr = FetchInstruction(reg.PC);
			}
			catch(StackException err)
			{
				if(err.GetCode() == StackUnderflow)
				{
					// this is a legitimate situation and causes the block to return
					if(reg.P_SW.SC())
					{
						// if it's a secondary core, end the execution and return
						Activate(false);
						parallel.parARG = reg.ARG;	// the result
						parallel.free = true;
						Reset();
					}
					else
					{
						ProgramBlockJump(pblock.CRB);
					}
				}
				else
					throw err;
			}
		}

		void Simple2DWPUcore::UpdatePC(QueryDir dir)
		{
			if(!reg.P_SW.PCB())
				reg.PC.Move(dir);
		}

		reg16 Simple2DWPUcore::FetchInstruction(regPC addr)
		{
			return (reg16)ptrRAM( 
				addr.BS + PROGRAM_BLOCK_HEADER_SIZE
				+ addr.xPC*INSTRUCTION_WIDTH
				+ addr.yPC*PROGRAM_BLOCK_HEIGHT*INSTRUCTION_WIDTH);
		}

		void Simple2DWPUcore::ExecuteInstruction(DecodedInstr instr)
		{
			// following are used inside the switch, but can't be delacred inside it (compilation error)
			reg64 data;
			regPC newPC;

			switch(instr.baseType)
			{
			case insBaseQuery:
				PerfomQuery(instr);
				break;

			case insBaseOperation:
				PerformOperation(instr);
				break;

			case insBasePasstrough:
				PerformPasstrough(instr);
				break;

			case insBaseIndex:
				switch(instr.indexType)
				{
				case insIndexReturn:
					PerformReturn(instr);
					break;

				case insIndexJump:
					data = (reg64)IndexTable(instr.index) |
						(((reg64)instr.indexSubtype.jumpUpper2b) << 32);
					newPC.BS = (reg32)(data >> 10);
					newPC.xPC = (reg16)data;
					newPC.yPC = (reg16)(data >> 5);
					ProgramBlockJump(newPC);
					break;

				case insIndexDouble:
					PerformDouble(instr);
					break;

				case insIndexExtend:
					PerformExtended(instr);
					break;
				}
				break;

			default:
				// TODO exception!
				break;
			}
		}

		void Simple2DWPUcore::ProgramBlockJump(regPC destination)
		{
			reg.PC = destination;
			InstrStackReset();
			ArgStackReset();

			reg.IS = 0;

			DecodeProgramBlockHeader();

			fetch_instr = FetchInstruction(reg.PC);
			// TODO
		}

		// determines if the condition is true
		bool Simple2DWPUcore::QueryCondition(byte queryID)
		{
			if(Between((int)queryID, (int)QNZ_ARG, (int)QNZ_PO))
				return QueryRegAccess( queryID - QNZ_ARG ) != 0;
			if(Between((int)queryID, (int)QZ_ARG, (int)QZ_PO))
				return QueryRegAccess( queryID - QZ_ARG ) == 0;
			
			if(Between((int)queryID, (int)QB_UB0, (int)QNB_reserved1))
			{
				bool bit = QueryBitAccess( (queryID -
					( Between((int)queryID, (int)QB_UB0, (int)QNB_reserved1)
					?QB_UB0:QNB_UB0 ) ) );

				if( Between((int)queryID, 30, 41) )
					return bit;
				else
					return !bit;
			}

			switch(queryID)
			{
			case QEQ_PE:
				return reg.ARG == reg.PE;
			case QGT_PE:
				return reg.ARG > reg.PE;
			case QLT_PE:
				return reg.ARG < reg.PE;
			case QNN:
				return (reg.ARG & 0x80000000U) != 0;
			case QFEQ_PE:
				return FLOAT(reg.ARG) == FLOAT(reg.PE);
			case QFGT_PE:
				return FLOAT(reg.ARG) > FLOAT(reg.PE);
			case QFLT_PE:
				return FLOAT(reg.ARG) < FLOAT(reg.PE);
			case QFNAN:
				return FLOAT(reg.ARG) != FLOAT(reg.ARG);
			case QFINF:
				return isinf(FLOAT(reg.ARG));
			case QIF:
				// TODO
				return false;
			}

			// make compiler happy
			return false;
		}

		void Simple2DWPUcore::PerfomQuery(DecodedInstr instr)
		{
			switch(instr.index)
			{
				// QRY
			case 0x00:
				switch(reg.IS)
				{
				case 0:
					if(instr.SingleDirection())
						reg.IS = 2;	// skip to the end next time
					else
						reg.IS = 1;	// move to the next phase
					FlowQuery(instr.DecodeDirection(false));
					break;

				case 1:
					reg.IS = 2;	// move to the next phase
					FlowQuery(instr.DecodeDirection(true));
					break;

				case 2:
					FlowReturn();
					break;
				}
				break;

				// FRK
			case 0x01:
				// Not implemented clearly... yet
				break;

				// Everything else
			default:
				switch(reg.IS)
				{
				case 0:
					if(QueryCondition((byte)instr.index))
						FlowQuery(instr.DecodeDirection(false));
					else
					{
						if(instr.SingleDirection())
							FlowReturn();
						else
						{
							reg.IS = 1;
							FlowQuery(instr.DecodeDirection(true));
						}
					}
					break;
				case 1:
					FlowReturn();
					break;
				}
			}
		}

		QueryDir Simple2DWPUcore::ProcessDir(QueryDir dir)
		{
			if(dir.dir == sdir_C)
				dir.dir = InvertSimpleDir(InstrStackTop().RD());
			return dir;
		}

		void Simple2DWPUcore::PerformOperation(DecodedInstr instr)
		{
			switch(reg.IS)
			{
			case 0:
				if(instr.SingleDirection())
				{
					ArgStackPush(reg.ARG);
					reg.IS = 2;	// skip to the calculation phase
				}
				else
				{
					reg.IS = 1;
					// parallelism
					parallel.rqst = true;
					parallel.parARG = reg.ARG;
					// move the other way
					parallel.parPC = reg.PC;
					parallel.parPC.Move(ProcessDir(instr.DecodeDirection(true)));
				}
				FlowQuery(instr.DecodeDirection(false));
				break;

			case 1:
				if(!reg.P_SW.PDF())
				{
					// normal processing
					ArgStackPush(reg.ARG);	// save the value
					reg.IS = 2;
					FlowQuery(instr.DecodeDirection(true));	// get the second value
				}
				else
				{
					// fetch data from the parallelism manager
					if(parallel.redy)
						// but only if the data are ready
						FlowReturn( Operation(reg.ARG, parallel.parARG, (byte)instr.index) );
				}
				break;

			case 2:
				// perform calculation
				FlowReturn( Operation(ArgStackPop(), reg.ARG, (byte)instr.index) );
				break;
			}
		}

		reg32 Simple2DWPUcore::Operation(reg32 a, reg32 b, byte op_index)
		{
			reg64 r;	// can't be declared inside switch

			switch(op_index)
			{
			case ADD:
				return a+b;
			case ADC:
				return a+b + reg.P_SW.CB();
			case SUB:
				return a-b;
			case SBB:
				return a-b - reg.P_SW.CB();
			case MUL:
				return a*b;
			case MULH:
				r = ((reg64)a)*b;
				reg.PE = (reg32)(r >> 32);
				return (reg32)r;
			case DIV:
				return a/b;
			case MOD:
				return a%b;
			case AND:
				return a&b;
			case OR:
				return a|b;
			case XOR:
				return a^b;
			case NOT:
				return ~a;
			case LAND:
				return a&&b;
			case LOR:
				return a||b;
			case LXOR:
				return (a!=0)^(b!=0);
			case LNOT:
				return !a;
			case RL:
				return rotate(a, b);
			case RR:
				return rotate(a, -((int)b));
			case SL:
				return a << b;
			case SR:
				return a >> b;
			case UMIN:
				return min(a, b);
			case UMAX:
				return max(a, b);
			case SMIN:
				return min((int)a, (int)b);
			case SMAX:
				return max((int)a, (int)b);

				// FPU
			case FADD:
				return UINT(FLOAT(a)+FLOAT(b));
			case FSUB:
				return UINT(FLOAT(a)-FLOAT(b));
			case FMUL:
				return UINT(FLOAT(a)*FLOAT(b));
			case FDIV:
				return UINT(FLOAT(a)/FLOAT(b));
			case FSQR:
				return UINT(FLOAT(a)*FLOAT(a));
			case FSQRT:
				return UINT(sqrt(FLOAT(a)));
			case FPOW:
				return UINT(pow(FLOAT(a), FLOAT(b)));
			case FROOT:
				return UINT(pow(FLOAT(a), 1.0f/FLOAT(b)));
			case FSIN:
				return UINT(sin(FLOAT(a)));
			case FCOS:
				return UINT(cos(FLOAT(a)));
			case FTAN:
				return UINT(tan(FLOAT(a)));
			case FLOG2:
				return UINT(logN(FLOAT(a), 2.0f));
			case FLOG10:
				return UINT(log10(FLOAT(a)));
			case FLN:
				return UINT(log(FLOAT(a)));
			case FABS:
				return UINT(abs(FLOAT(a)));
			case FTOINT:
				return (int)FLOAT(a);
			case FFROMINT:
				return UINT((float)a);
			case FMIN:
				return UINT(min(FLOAT(a), FLOAT(b)));
			case FMAX:
				return UINT(max(FLOAT(a), FLOAT(b)));

			default:
				return 0;
			}
		}

		void Simple2DWPUcore::PerformPasstrough(DecodedInstr instr)
		{
			switch(reg.IS)
			{
			case 0:
				reg.IS = 1;
				if(instr.passType != insPassReturn)
					Action(instr.index);
				if(instr.passType == insPassImmediate)
					FlowReturn();
				else
					FlowQuery(instr.DecodeDirection(false));
				break;
			case 1:
				if(instr.passType != insPassEntry)
					Action(instr.index);
				FlowReturn();
				break;
			}
		}

		reg32 Simple2DWPUcore::PasstroughReg(uint regid)
		{
			switch(regid)
			{
			case passMOV_SW:
				return SW();
			case passMOV_BS:
				return reg.PC.BS;
			case passMOV_SI:
				return SI();
			case passMOV_SA:
				return SA();
			default:
				return *PasstroughDecodeReg(regid);
			}
		}

		void Simple2DWPUcore::PasstroughReg(uint regid, reg32 val)
		{
			regPC newPC;

			switch(regid)
			{
			case passMOV_SW:
				SW(val);
				break;
			case passMOV_BS:
				newPC.xPC = reg.PC.xPC;
				newPC.yPC = reg.PC.yPC;
				newPC.BS = val;
				ProgramBlockJump(newPC);
				break;
			case passMOV_SI:
				SI(val);
				break;
			case passMOV_SA:
				SA(val);
				break;
			default:
				*PasstroughDecodeReg(regid) = val;
			}
		}

		reg32 *Simple2DWPUcore::PasstroughDecodeReg(uint regid)
		{
			switch(regid)
			{
			case passMOV_LI:
				return &globreg->LI;
			case passMOV_SQ:
				return &globreg->SQ;
			case passMOV_RE:
				return &globreg->RE;
			case passMOV_TR:
				return &globreg->TR;
			case passMOV_CI:
				return &globreg->CI;
			case passMOV_PE:
				return &reg.PE;
			case passMOV_HE:
				return &globreg->HE;
			case passMOV_ST:
				return &globreg->ST;
			case passMOV_EL:
				return &globreg->EL;
			case passMOV_PO:
				return &reg.PO;
			case passMOV_IC:
				return &globreg->IC;
			case passMOV_IB:
				//return globreg->IB; TODO
			//case MOV_SW:	<< SKIPPED!
			//case passMOV_BS: << SKIPPED!
			//case passMOV_SI: << SKIPPED!
			//case passMOV_SA: << SKIPPED!
			case passMOV_TC:
				return &globreg->TC;
			case passMOV_TV0:
				return &globreg->TV[0];
			case passMOV_TV1:
				return &globreg->TV[1];
			case passMOV_TV2:
				return &globreg->TV[2];
			case passMOV_TV3:
				return &globreg->TV[3];
			case passMOV_TF0:
				return &globreg->TF[0];
			case passMOV_TF1:
				return &globreg->TF[1];
			case passMOV_TF2:
				return &globreg->TF[2];
			case passMOV_TF3:
				return &globreg->TF[3];
			}

			// make compiler happy
			return 0;
		}

		reg32 *Simple2DWPUcore::PasstroughDecodeSTACKReg(uint regid)
		{
			switch(regid)
			{
			case passSTACK_ARG:
				return &reg.ARG;
			// passSTACK_SW,
			case passSTACK_LI:
				return &globreg->LI;
			case passSTACK_SQ:
				return &globreg->SQ;
			case passSTACK_RE:
				return &globreg->RE;
			case passSTACK_TR:
				return &globreg->TR;
			case passSTACK_CI:
				return &globreg->CI;
			case passSTACK_EL:
				return &globreg->EL;
			case passSTACK_HE:
				return &globreg->HE;
			case passSTACK_ST:
				return &globreg->ST;
			case passSTACK_PE:
				return &reg.PE;
			case passSTACK_PO:
				return &reg.PO;
			//passSTACK_ptrST
			}

			// make compiler happy
			return &reg.ARG;
		}

		reg32 Simple2DWPUcore::PasstroughSTACKReg(uint regid)
		{
			switch(regid)
			{
			case passSTACK_SW:
				return SW();
			case passSTACK_ptrST:
				return ptrST32(0);
			default:
				return *PasstroughDecodeSTACKReg(regid);
			}
		}

		void Simple2DWPUcore::PasstroughSTACKReg(uint regid, reg32 val)
		{
			switch(regid)
			{
			case passSTACK_SW:
				SW(val);
				break;
			case passSTACK_ptrST:
				ptrST32(val, 0);
				break;
			default:
				*PasstroughDecodeSTACKReg(regid) = val;
			}
		}

		void Simple2DWPUcore::Action(uint index)
		{
			if(Between(index, (uint)MOVF_LI, (uint)MOVF_TF3))
			{
				reg.ARG = PasstroughReg(index - MOVF_LI);
				return;
			}

			if(Between(index, (uint)MOVT_LI, (uint)MOVT_TF3))
			{
				PasstroughReg(index - MOVT_LI, reg.ARG);
				return;
			}

			if(Between(index, (uint)MOVF32_ptrST, (uint)MOVT8_ptrHE_PO))
			{
				switch(index)
				{
				case MOVF32_ptrST:
					reg.ARG = ptrST32(0);
					break;
				case MOVT32_ptrST:
					ptrST32(reg.ARG, 0);
					break;
				case MOVF16_ptrST:
					reg.ARG = (reg32)ptrST16(0);
					break;
				case MOVT16_ptrST:
					ptrST16(reg.ARG, 0);
					break;
				case MOVF8_ptrST:
					reg.ARG = (reg32)ptrST8(0);
					break;
				case MOVT8_ptrST:
					ptrST8(reg.ARG, 0);

				case MOVF16_ptrHE:
					reg.ARG = (reg32)ptrHE16(0);
					break;
				case MOVT16_ptrHE:
					ptrHE16((reg16)reg.ARG, 0);
					break;
				case MOVF8_ptrHE:
					reg.ARG = (reg32)ptrHE8(0);
					break;
				case MOVT8_ptrHE:
					ptrHE8((reg8)reg.ARG, 0);
					break;

				case MOVF32_ptrST_PO:
					reg.ARG = ptrST32(reg.PO);
					break;
				case MOVT32_ptrST_PO:
					ptrST32(reg.ARG, reg.PO);
					break;
				case MOVF16_ptrST_PO:
					reg.ARG = (reg32)ptrST16(reg.PO);
					break;
				case MOVT16_ptrST_PO:
					ptrST16(reg.ARG, reg.PO);
					break;
				case MOVF8_ptrST_PO:
					reg.ARG = (reg32)ptrST8(reg.PO);
					break;
				case MOVT8_ptrST_PO:
					ptrST8(reg.ARG, reg.PO);

				case MOVF16_ptrHE_PO:
					reg.ARG = (reg32)ptrHE16(reg.PO);
					break;
				case MOVT16_ptrHE_PO:
					ptrHE16((reg16)reg.ARG, reg.PO);
					break;
				case MOVF8_ptrHE_PO:
					reg.ARG = (reg32)ptrHE8(reg.PO);
					break;
				case MOVT8_ptrHE_PO:
					ptrHE8((reg8)reg.ARG, reg.PO);
					break;
				}
				return;
			}

			if(Between(index, (uint)PUSH_ARG, (uint)PUSH_ptrST))
			{
				ArgStackPush(PasstroughSTACKReg(index - PUSH_ARG));
				return;
			}

			if(Between(index, (uint)POP_ARG, (uint)POP_ptrST))
			{
				PasstroughSTACKReg(index - POP_ARG,
					ArgStackPop());
				return;
			}

			switch(index)
			{
			case INC_ARG:
				reg.ARG++;
				break;
			case INC_LI:
				globreg->LI++;
				break;
			case INC_SQ:
				globreg->SQ++;
				break;
			case INC_HE:
				globreg->HE++;
				break;
			case INC_ST:
				globreg->ST++;
				break;
			case INC_PE:
				reg.PE++;
				break;
			case INC_PO:
				reg.PO++;
				break;

			case DEC_ARG:
				reg.ARG--;
				break;
			case DEC_LI:
				globreg->LI--;
				break;
			case DEC_SQ:
				globreg->SQ--;
				break;
			case DEC_HE:
				globreg->HE--;
				break;
			case DEC_ST:
				globreg->ST--;
				break;
			case DEC_PE:
				reg.PE--;
				break;
			case DEC_PO:
				reg.PO--;

			case SETB_UB0:
				globreg->S_SW.UB0(true);
				break;
			case SETB_UB1:
				globreg->S_SW.UB1(true);
				break;
			case SETB_UB2:
				globreg->S_SW.UB2(true);
				break;
			case SETB_UB3:
				globreg->S_SW.UB3(true);
				break;
			case SETB_IE:
				// TODO
			case SETB_IP:
				// TODO
			case SETB_CB:
				reg.P_SW.CB(true);
				break;

			case CLR_UB0:
				globreg->S_SW.UB0(false);
				break;
			case CLR_UB1:
				globreg->S_SW.UB1(false);
				break;
			case CLR_UB2:
				globreg->S_SW.UB2(false);
				break;
			case CLR_UB3:
				globreg->S_SW.UB3(false);
				break;
			case CLR_IE:
				//TODO
			case CLR_IP:
				//TODO
			case CLR_CB:
				reg.P_SW.CB(false);
				break;
			case RETI:
				//TODO
				break;
			}
		}

		void Simple2DWPUcore::PerformReturn(DecodedInstr instr)
		{
			switch(instr.indexSubtype.returnType)
			{
			case insIndexRetRegister:
				FlowReturn(ReturnRegister(instr.index));
				break;
			case insIndexRetValue:
				FlowReturn(IndexTable(instr.index));
				break;
			case insIndexRetMemory:
				FlowReturn(ptrRAM(IndexTable(instr.index)));
				break;
			case insIndexRetIO:
				FlowReturn(io->Read16(IndexTable(instr.index)));
				break;
			}
		}

		reg32 Simple2DWPUcore::ReturnRegister(uint regid)
		{
			// special case for the indirect addressing
			if(regid == (uint)-1)
				return 0;

			if(Between(regid, (uint)RET_LI, (uint)RET_TF3))
				switch(regid)
			{
				case RET_LI:
					return globreg->LI;
				case RET_SQ:
					return globreg->SQ;
				case RET_RE:
					return globreg->RE;
				case RET_TR:
					return globreg->TR;
				case RET_CI:
					return globreg->CI;
				case RET_PE:
					return reg.PE;
				case RET_HE:
					return globreg->HE;
				case RET_ST:
					return globreg->ST;
				case RET_EL:
					return globreg->EL;
				case RET_PO:
					return reg.PO;
				case RET_IC:
					return globreg->IC;
				case RET_IB:
					// TODO
				case RET_SW:
					return SW();
				case RET_BS:
					return (reg32)reg.PC.BS;
				case RET_SI:
					return SI();
				case RET_SA:
					return SA();
				case RET_TC:
					return globreg->TC;
				case RET_TV0:
					return globreg->TV[0];
				case RET_TV1:
					return globreg->TV[1];
				case RET_TV2:
					return globreg->TV[2];
				case RET_TV3:
					return globreg->TV[3];
				case RET_TF0:
					return globreg->TF[0];
				case RET_TF1:
					return globreg->TF[1];
				case RET_TF2:
					return globreg->TF[2];
				case RET_TF3:
					return globreg->TF[3];
			}

			/*
				The minus one is so this function is called with
				-1 if it's the first option is indirect addressing
				without any increment - this function returns 0 when
				called with -1
			*/

			// 32 bit

			if(Between(regid, (uint)RET_ptrST, (uint)RET_ptrST_PO))
				return ptrST32(ReturnRegister(regid - RET_ptrST - 1));

			if(Between(regid, (uint)RET_ptrHE, (uint)RET_ptrHE_PO))
				return ptrHE16(ReturnRegister(regid - RET_ptrHE - 1));

			// 16 bit

			if(Between(regid, (uint)RET16_ptrST, (uint)RET16_ptrST_PO))
				return ptrST16(ReturnRegister(regid - RET16_ptrST - 1));

			if(Between(regid, (uint)RET16_ptrHE, (uint)RET16_ptrHE_PO))
				return ptrHE16(ReturnRegister(regid - RET16_ptrHE - 1));

			// 8 bit

			if(Between(regid, (uint)RET8_ptrST, (uint)RET8_ptrST_PO))
				return ptrST8(ReturnRegister(regid - RET8_ptrST - 1));

			if(Between(regid, (uint)RET8_ptrHE, (uint)RET8_ptrHE_PO))
				return ptrHE8(ReturnRegister(regid - RET8_ptrHE - 1));

			// make compiler happy
			return 0;
		}

		void Simple2DWPUcore::PerformDouble(DecodedInstr instr)
		{
			switch(instr.indexSubtype.doubleType)
			{
			case insIndexDouble:
				switch(reg.IS)
				{
				case 0:
					switch(reg.P_SW.DS())
					{
					/*
					Phase 0 - instruction is first entered
					fetches and performs first call on the first instruction
					*/
					case doubleNONE:
						// phase 1 - calling first merged instruction
						reg.P_SW.DS(double_FirstCall);
						reg.P_SW.PCB(true);	// block fetching
						fetch_instr = (reg16)IndexTable(instr.index);

						// Argument Protect
						if(instr.AP)
							ArgStackPush(reg.ARG);

						FlowQuery();
						break;

					/*
						Phase 1 - executed upon return (or captured call) from the first call
						of the first instruction in the indexed data table
						If this instruction returned, then double instruction
						returns as well, otherwise, it fetches the second instruction
						and performs first call of the second instruction */
					case double_FirstReturn:
						/* in case that the first instruction already returns 
							the second won't be executed at all		*/
						if(reg.P_SW.DR())
						{
							reg.P_SW.DS(double_Cleanup); // last phase - return from the double instruction
							if(instr.AP)
								reg.ARG = ArgStackPop();
							FlowReturn();
						}
						else	// the query is capture and the second instruction is executed instead
						{
							reg.P_SW.PCB(true);
							// phase 3 - calling second instruction
							reg.P_SW.DS(double_SecondCall);
							// fetch the second instruction from the index table
							fetch_instr = (reg16)( IndexTable(instr.index) >> 16 );
							// execute it
							FlowQuery();
						}
						break;

					/*
						Phase 2 - executed upon return/captured-call from the first execution
						of the second instruction. In case that this instruction returns, then
						it calls  the first instruction again (which can cause another call
						to the second instruction, unless it returns as well)
						If second instruction calls, then it continues in its direction in
						the following phase
					*/
					case double_SecondReturn:
						// if the second instruction returns, return to the first one
						if(reg.P_SW.DR())
						{
							/* the second instruction returned, so the execution returns
							to the first one - if it returns as well, then whole double instruction
							will return. If it will call again, the second instruction gets executed
							again */
							reg.P_SW.DS(double_FirstCall);	// not a first call the the first instruction
							reg.P_SW.PCB(true);	// block fetching
							fetch_instr = (reg16)IndexTable(instr.index);
							FlowReturn();	// basically return to the first instruction
						}
						else
						{
							reg.IS++;	// next phase handles return from outside of this instruction
							// continue in the direction of the second instruction
							reg.P_SW.DS(doubleNONE);	// instructions around shouldn't even know about the double instruction

							// restore the ARG
							if(instr.AP)
								reg.ARG = ArgStackPop();

							FlowQuery(sdir_C);
						}
						break;
					}

					/*
						Phase 3 - execution flow returned here from the surroundings and
						thus, it must call the second instruction again (or rather "return"
						to it)		*/
					case 1:
						// now, "return" to the second instruction again
						reg.P_SW.DS(double_SecondCall);	// it's not the first call
						reg.P_SW.PCB(true);	// block fetching
						fetch_instr = (reg16)(IndexTable(instr.index) >> 16);

						if(instr.AP)
							ArgStackPush(reg.ARG);

						FlowReturn();	// basically return
						break;
					
				}
				break;

			case insIndexDoubleTwodir:
				switch(reg.IS)
				{
				/*
					Phase 0 - the instruction is first entered and calls the
					first instruction in the index table.
				*/
				case 0:
					switch(reg.P_SW.DS())
					{
					/*  instruction is first entered, so it calls the
						first instruction in the index table
					*/
					case doubleNONE:
						reg.P_SW.DS(double_FirstCall);
						reg.P_SW.PCB(true);	// block instruction fetching
						// fetch the first instructoin
						fetch_instr = (reg16)(IndexTable(instr.index));

						if(instr.AP)
							ArgStackPush(reg.ARG);

						FlowQuery();	// now call it
						break;

					/* First instruction returned, so continue in its direction
					   and also switch IS to 1, which handles the return from
					   the outside */
					case double_FirstReturn:
						if(reg.P_SW.DR())
						{
							// if the first instruction just returned, call the second one
							reg.P_SW.DS(double_SecondCall);
							reg.P_SW.PCB(true);
							// fetch the second instruction
							fetch_instr = (reg16)(IndexTable(instr.index) >> 16);
							FlowQuery();	// now call it
						}
						else
						{
							// continue outside
							reg.IS = 1;	// handles return from outside with first instruction
							reg.P_SW.DS(doubleNONE);

							if(instr.AP)
								ArgStackPush(reg.ARG);

							FlowQuery(sdir_C);
						}						
						break;

					/* Second returned, so either stop the execution, or continue outside */
					case double_SecondReturn:
						if(reg.P_SW.DR())
						{
							// end it
							reg.P_SW.DS(double_Cleanup);
							FlowReturn();
						}
						else
						{
							// continue outside
							reg.IS = 2;
							reg.P_SW.DS(doubleNONE);
							FlowQuery(sdir_C);
						}
						break;
					}
					break;

				// return from outside - first instruction
				case 1:
					// return to the first instruction
					reg.P_SW.DS(double_FirstCall);
					reg.P_SW.PCB(true);	// block instruction fetching
					// fetch the first instructoin
					fetch_instr = (reg16)(IndexTable(instr.index));
					FlowReturn();	// now return back to it
					break;

				// return from outside - second instruction
				case 2:
					// return to the second instruction
					reg.P_SW.DS(double_SecondCall);
					reg.P_SW.PCB(true);	// block instruction fetching
					// fetch the first instructoin
					fetch_instr = (reg16)(IndexTable(instr.index) >> 16);
					FlowReturn();	// now return back to it
					break;
				}
				break;
			}
		}

		void Simple2DWPUcore::PerformExtended(DecodedInstr instr)
		{
			// TODO
		}

		/* *****************************************************
							MEMORY ACCESS
		*******************************************************/

		reg32 Simple2DWPUcore::ptrRAM(reg32 addr)
		{
			if(addr > (RAM->size - sizeof(reg32)))
				return 0;

			return *((reg32*)(RAM->data+addr));
		}

		void Simple2DWPUcore::ptrRAM(reg32 addr, reg32 val)
		{
			if(addr > (RAM->size - sizeof(reg32)))
				return;

			*((reg32*)(RAM->data+addr)) = val;
		}

		void Simple2DWPUcore::ptrRAM(reg32 addr, reg16 val)
		{
			if(addr > (RAM->size - sizeof(reg32)))
				return;

			*((reg16*)(RAM->data+addr)) = val;
		}

		void Simple2DWPUcore::ptrRAM(reg32 addr, reg8 val)
		{
			if(addr > (RAM->size - sizeof(reg32)))
				return;

			*(RAM->data+addr) = val;
		}

		reg32 Simple2DWPUcore::IndexTable(reg16 addr)
		{
			addr &= 0x03FF;	// allow only up to 1023
			return ptrRAM( ((reg32)reg.PC.BS) +
				PROGRAM_BLOCK_HEADER_SIZE +
				(PROGRAM_BLOCK_HEIGHT*PROGRAM_BLOCK_WIDTH) *
				INSTRUCTION_WIDTH +
				addr * 4 );
		}

		void Simple2DWPUcore::InstrStackPush(InstructionFlowWord ifw)
		{
			instr_stack->Push((reg8)ifw);
		}

		InstructionFlowWord Simple2DWPUcore::InstrStackPop()
		{
			return InstructionFlowWord(instr_stack->Pop());
		}

		InstructionFlowWord Simple2DWPUcore::InstrStackTop()
		{
			return InstructionFlowWord(instr_stack->Top());
		}

		void Simple2DWPUcore::InstrStackReset()
		{
			instr_stack->Reset();
		}

		void Simple2DWPUcore::ArgStackPush(reg32 val)
		{
			arg_stack->Push(val);
		}

		reg32 Simple2DWPUcore::ArgStackPop()
		{
			return arg_stack->Pop();
		}

		void Simple2DWPUcore::ArgStackReset()
		{
			arg_stack->Reset();
		}

		reg16 Simple2DWPUcore::ptrIO16(reg16 addr)
		{
			return io->Read16(addr);
		}

		reg8 Simple2DWPUcore::ptrIO8(reg16 addr)
		{
			return io->Read8(addr);
		}

		void Simple2DWPUcore::ptrIO(reg16 addr, reg16 val)
		{
			return io->Write16(val, addr);
		}

		void Simple2DWPUcore::ptrIO(reg16 addr, reg8 val)
		{
			return io->Write8(val, addr);
		}

		// *******************************************
		//		PARALLELISM

		void Simple2DWPUcore::C_ACPT(bool set)
		{
			if(set)
			{
				// set the PDF bit
				InstructionFlowWord ifw = InstrStackPop();
				ifw.PDF(true);
				InstrStackPush(ifw);
			}
		}

	}
}