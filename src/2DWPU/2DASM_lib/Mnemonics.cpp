#include "Mnemonics.h"
#include "2DASM_Exception.h"
#include "Global.h"

namespace WPU2D
{
	namespace ASM2D
	{
		void Mnemonic::ComposeInstruction()
		{
			// make empty instruction first
			instr = DecodedInstr();

			if(mnemonic.find("HALT") != mnemonic.npos)
			{
				instr.halt = true;
				return;
			}

			std::string base, direction, operand;
			DecomposeMnemonic(mnemonic, &base, &direction, &operand);

			DecodeBase(base);
			DecodeDirection(direction);
			
			// determine index and other vital stuff for the type
			switch(instr.baseType)
			{
			case insBaseQuery:
				// determine the specific index
				instr.index += GetQueryOperandOffset(instr.index, operand);
				break;

			case insBasePasstrough:
				// determine the execution time
				instr.passType = insPassNONE;
				for(int i = 0; PasstroughExectime[i]; ++i)
					if(*PasstroughExectime[i] == base[0])
						instr.passType = (InstrPasstrough)i;
				// check if it was found
				if(instr.passType == insPassNONE)
					throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_EXECUTION_TIME);

				// determine the specific index
				if(!PasstroughIndexIndirectMatch(base, operand))
					instr.index += GetPasstroughOperandOffset(instr.index, operand);

				break;

			case insBaseIndex:
				switch(instr.indexType)
				{
				case insIndexReturn:
					// determine index subtype
					bool val, mem, io;
					DecodeIndexReturnSubtype(mnemonic, &val, &mem, &io);
					if(val)
						instr.indexSubtype.returnType = insIndexRetValue;
					else if(mem)
						instr.indexSubtype.returnType = insIndexRetMemory;
					else if(io)
						instr.indexSubtype.returnType = insIndexRetIO;
					else
						instr.indexSubtype.returnType = insIndexRetRegister;

					if(val | io | mem)
						// determine the index for these three - value is in the index table
						instr.index = FromHEXString<ushort>(
							operand.substr(operand.find("@")+1, operand.npos));
					else
					{
						instr.index = -1;
						for(int i = 0; i < RET_TERMINATOR; ++i)
							if( GetReturnRegBaseName(i, instr.retTAK) == base &&
								ComposeReturnRegister(i) == operand)
							{
								instr.index = i;
								break;
							}
						if(instr.index == -1)
							throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, mnemonic); 
					}
					break;

				case insIndexJump:
					
				case insIndexDouble:

				case insIndexExtend:
					// nothing to do really...
					break;
				}
				break;
			}
		}

		void Mnemonic::DecodeBase(std::string base)
		{
			// check if it's index instruction first
			if((base.find("RET") != base.npos))
			{
				instr.baseType = insBaseIndex;
				instr.indexType = insIndexReturn;
				return;
			}
			if((base.find("TAK") != base.npos))
			{
				instr.baseType = insBaseIndex;
				instr.indexType = insIndexReturn;
				instr.retTAK = true;
				return;
			}
			if(base.find("JMP") != base.npos)
			{
				instr.baseType = insBaseIndex;
				instr.indexType = insIndexJump;
				return;
			}
			if((base.find("<") != base.npos) && (base.find(">") != base.npos))
			{
				instr.baseType = insBaseIndex;
				instr.indexType = insIndexDouble;
				return;
			}
			if(base.find("$") != base.npos)
			{
				instr.baseType = insBaseIndex;
				instr.indexType = insIndexExtend;
				return;
			}

			// perform query lookup
			for(int i = 0; i < 64; ++i)
				if(base == GetQueryBaseName(i))
				{
					// match was found
					instr.baseType = insBaseQuery;
					instr.index = i;	// base index - needs to be further processed
					return;
				}

			// perform operation lookup
			for(int i = 0; i < 64; ++i)
				if(base == GetOperationBaseName(i))
				{
					// match was found
					instr.baseType = insBaseOperation;
					// it's also possible to immediatelly determine index
					instr.index = i;
					return;
				}

			// perform passtrough lookup
			base = base.substr(1, base.npos);	// skip the first symbol (execution time)
			for(int i = 0; i < 128; ++i)
			{
				if(base == GetPasstroughBaseName(i))
				{
					instr.baseType = insBasePasstrough;
					instr.index = i;	// base index - needs to be further processed
					return;
				}
			}

			// if it doesn't match anything, it's not a known instruction
	
			throw ASM2D_Exception(ERR_ASM2D_UNKNOWN_INSTRUCTION, "\n" + base);
		}

		void Mnemonic::DecodeIndexReturnSubtype(std::string ins,
			bool *val, bool *mem, bool *io)
		{
			*val = true;
				for(int i = 3; i < ins.length(); ++i)
				{
					// skip spaces
					if(isspace(ins[i]))
						continue;
					// skip minus symbol
					if(ins[i] == '-')
						continue;
					// plus is okay too
					if(ins[i] == '+')
						continue;
					// skip this as well
					if(ins[i] == '@')
						continue;
					// must be all digits
					if(!isxdigit(ins[i]))
						*val = false;
				}
				*mem = (ins.find("*") != ins.npos) && (ins.find("*(") == ins.npos);
				*io = (ins.find("#") != ins.npos) && (ins.find("#(") == ins.npos);
		}

		void Mnemonic::DecomposeMnemonic(
				std::string mnemonic,
				std::string *base,
				std::string *direction,
				std::string *operand)
		{
			std::string temp_operand;

			// first, get the base
			if(mnemonic.find("_") == mnemonic.npos)
				*base = substrBetween(mnemonic, 0, mnemonic.find(' '));
			else
			{
				*base = substrBetween(mnemonic, 0, mnemonic.find('_'));
				// also the direction
				*direction = substrBetween(mnemonic, mnemonic.find('_')+1,
					mnemonic.find(' '));
			}

			if(mnemonic.find(' ') != mnemonic.npos)
				temp_operand = substrBetween(mnemonic, mnemonic.find(" ")+1,
					mnemonic.npos);

			// strip all spaces from the operand
			*operand = "";
			for(int i = 0; i < temp_operand.length(); ++i)
				if(!isspace(temp_operand[i]))
					*operand += temp_operand[i];
		}

		void Mnemonic::DecodeDirection(std::string direction)
		{
			// if it's empty, there's nothing to do
			if(direction == "")
				return;

			if(direction.length() > 3)
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_LENGTH);

			bool first = true, ocontinue;

			instr.direction = 0;
			for(int i = 0; i < direction.length(); ++i)
			{
				ocontinue = false;
				for(int n = 0; SimpleDirNames[n]; ++n)
					if(direction[i] == *SimpleDirNames[n])
					{
						if(first)
							instr.direction += n;
						else
							instr.direction += n*5;
						first = false;
						ocontinue = true;
						break;
					}
				if(ocontinue) continue;
				// if it doesn't match, check if it's length
				for(int n = 1; DirLength[n]; ++n)
					if(direction[i] == *DirLength[n])
					{
						// only one length is allowed
						if(instr.dir_length != 0)
							throw ASM2D_Exception(ERR_ASM2D_TWO_LENGTHS); 
						instr.dir_length = n;
						instr.long_dir_second = !first;
						ocontinue = true;
						break;
					}
				if(ocontinue) continue;

				// if it got there, then there's something not right with the syntax
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_LENGTH);
			}

			// now check if it's a single direction
			if(direction.length() == (2-(instr.dir_length==0)))
				instr.direction += 25;
		}

		int Mnemonic::GetPasstroughOperandOffset(reg16 index, std::string operand)
		{
			if(	Between(index, (reg16)MOVF_LI, (reg16)MOVF_TF3) ||
				Between(index, (reg16)MOVT_LI, (reg16)MOVT_TF3))
			{
				for(int i = 0; PasstroughMOVRegisters[i]; ++i)
					if(operand == PasstroughMOVRegisters[i])
						return i;
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
			}

			if( Between(index, (reg16)PUSH_ARG, (reg16)PUSH_ptrST) ||
				Between(index, (reg16)POP_ARG, (reg16)POP_ptrST))
			{
				for(int i = 0; PasstroughSTACKRegisters[i]; ++i)
					if(operand == PasstroughSTACKRegisters[i])
						return i;
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
			}

			if( Between(index, (reg16)INC_ARG, (reg16)INC_PO) ||
				Between(index, (reg16)DEC_ARG, (reg16)DEC_PO))
			{
				for(int i = 0; PasstroughINCDECRegisters[i]; ++i)
					if(operand == PasstroughINCDECRegisters[i])
						return i;
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
			}

			if( Between(index, (reg16)SETB_UB0, (reg16)SETB_CB) ||
				Between(index, (reg16)CLR_UB0, (reg16)CLR_CB))
			{
				for(int i = 0; PasstroughBits[i]; ++i)
					if(operand == PasstroughBits[i])
						return i;
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
			}

			if(index == RETI)
				return 0;

			// if nothing fits..
			throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
		}

		bool Mnemonic::PasstroughIndexIndirectMatch(std::string base, std::string operand)
		{
			base = base.substr(1, base.npos);	// remove the exec-time

			for(int i = MOVF32_ptrST; i <= MOVT8_ptrHE_PO; ++i)
				if( base == GetPasstroughBaseName(i) &&
					operand == GetPasstroughOperandName(i) )
				{
					instr.index = i;
					return true;
				}

			// nothing was found
			return false;
		}

		// ---------------------------------------------

		void Mnemonic::ComposeMnemonic()
		{
			mnemonic = "";	// make an empty string

			if(instr.halt)
			{
				mnemonic = "HALT";
				return;
			}

			switch(instr.baseType)
			{
			case insBaseQuery:
				mnemonic += GetQueryBaseName(instr.index);
				mnemonic += "_";
				mnemonic += ComposeDirection();
				mnemonic += " ";
				mnemonic += GetQueryOperandName(instr.index);
				break;

			case insBaseOperation:
				mnemonic += GetOperationBaseName(instr.index);
				mnemonic += "_";
				mnemonic += ComposeDirection();
				break;

			case insBasePasstrough:
				mnemonic += PasstroughExectime[instr.passType];
				mnemonic += GetPasstroughBaseName(instr.index);
				mnemonic += "_";
				mnemonic += ComposeDirection();
				mnemonic += " ";
				mnemonic += GetPasstroughOperandName(instr.index);
				break;

			case insBaseIndex:
				switch(instr.indexType)
				{
				case insIndexReturn:
					if(instr.indexSubtype.returnType == insIndexRetRegister)
						mnemonic += GetReturnRegBaseName(instr.index, instr.retTAK);
					else
						mnemonic += "RET";
					mnemonic += " ";
					switch(instr.indexSubtype.returnType)
					{
					case insIndexRetRegister:
						mnemonic += ComposeReturnRegister(instr.index);
						break;

					case insIndexRetValue:
						mnemonic += "@" + ToStringHEX(instr.index, 4);
						break;

					case insIndexRetMemory:
						mnemonic += "*(@" + ToStringHEX(instr.index, 4) + ")";
						break;

					case insIndexRetIO:
						mnemonic += "#(@" + ToStringHEX(instr.index, 4) + ")";
						break;
					}
					break;

				case insIndexJump:
					mnemonic += "JMP " + ToStringHEX((int)instr.indexSubtype.jumpUpper2b, 1)
						+ "@" + ToStringHEX(instr.index, 4);
					break;

				case insIndexDouble:
					if(instr.AP)
						mnemonic += "~";
					if(instr.indexSubtype.doubleType == insIndexDoubleMerge)
						mnemonic += ">";
					else
						mnemonic += "<";
					mnemonic += "@" + ToStringHEX(instr.index, 4);
					if(instr.indexSubtype.doubleType == insIndexDoubleMerge)
						mnemonic += "<";
					else
						mnemonic += ">";
					break;

				case insIndexExtend:
					mnemonic += "$@" + ToStringHEX(instr.index, 4);
					break;
				}
				break;
			}
		}

		std::string Mnemonic::ComposeDirection()
		{
			std::string direction;
			QueryDir dir;

			dir = instr.DecodeDirection(false);
			direction += DirLength[dir.length];
			direction += SimpleDirNames[dir.dir];

			if(!instr.SingleDirection())
			{
				dir = instr.DecodeDirection(true);
				direction += DirLength[dir.length];
				direction += SimpleDirNames[dir.dir];
			}

			return direction;
		}

		int Mnemonic::GetQueryOperandOffset(reg16 index, std::string operand)
		{
			switch(index)
			{
			case QNN:
			case QFNAN:
			case QFINF:
				// it can be ARG too
				if(operand == "ARG")
					operand = "";

				// let it fall trough
			case QRY:
			case FRK:
			case QIF:
				if(operand == "")
					return 0;
				else
					throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);

			case QEQ_PE:
			case QGT_PE:
			case QLT_PE:
			case QFEQ_PE:
			case QFGT_PE:
			case QFLT_PE:
				if(operand == "PE")
					return 0;
				else
					throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
			}

			// Register operands
			if(	Between(index, (reg16)QNZ_ARG, (reg16)QNZ_PO) ||
				Between(index, (reg16)QZ_ARG, (reg16)QZ_PO))
			{
				for(int i = 0; QueryRegisterOperands[i]; ++i)
					if(QueryRegisterOperands[i] == operand)
						return i;
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
			}

			if(	Between(index, (reg16)QB_UB0, (reg16)QB_reserved1) ||
				Between(index, (reg16)QNB_UB0, (reg16)QNB_reserved1) )
			{
				for(int i = 0; QueryBitOperands[i]; ++i)
					if(QueryBitOperands[i] == operand)
						return i;
				throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
			}

			// if nothing matches...
			throw ASM2D_Exception(ERR_ASM2D_SYNTAXERR_WRONG_OPERAND, this->mnemonic);
		}

		// ---------------------------------------------

		char *Mnemonic::GetQueryBaseName(reg16 index)
		{
			switch(index)
			{
			case QRY:
				return "QRY";
			case FRK:
				return "FRK";

			case QEQ_PE:
				return "QEQ";
			case QGT_PE:
				return "QGT";
			case QLT_PE:
				return "QLT";
			case QNN:
				return "QNN";
			case QFEQ_PE:
				return "QFEQ";
			case QFGT_PE:
				return "QFGT";
			case QFLT_PE:
				return "QFLT";
			case QFNAN:
				return "QFNAN";
			case QFINF:
				return "QFINF";
			case QIF:
				return "QIF";
			}

			if(Between(index, (reg16)QNZ_ARG, (reg16)QNZ_PO))
				return "QNZ";

			if(Between(index, (reg16)QZ_ARG, (reg16)QZ_PO))
				return "QZ";

			if(Between(index, (reg16)QB_UB0, (reg16)QB_reserved1))
				return "QB";

			if(Between(index, (reg16)QNB_UB0, (reg16)QNB_reserved1))
				return "QNB";
		}

		char *Mnemonic::GetQueryOperandName(reg16 index)
		{
			switch(index)
			{
			case QRY:
				return "";
			case FRK:
				return "";

			case QEQ_PE:
				return "PE";
			case QGT_PE:
				return "PE";
			case QLT_PE:
				return "PE";
			case QNN:
				return "ARG";
			case QFEQ_PE:
				return "PE";
			case QFGT_PE:
				return "PE";
			case QFLT_PE:
				return "PE";
			case QFNAN:
				return "ARG";
			case QFINF:
				return "ARG";
			case QIF:
				return "";
			}

			if(Between(index, (reg16)QNZ_ARG, (reg16)QNZ_PO))
				return QueryRegisterOperands[index-QNZ_ARG];

			if(Between(index, (reg16)QZ_ARG, (reg16)QZ_PO))
				return QueryRegisterOperands[index-QZ_ARG];

			if(Between(index, (reg16)QB_UB0, (reg16)QB_reserved1))
				return QueryBitOperands[index-QB_UB0];

			if(Between(index, (reg16)QNB_UB0, (reg16)QNB_reserved1))
				return QueryBitOperands[index-QNB_UB0];
		}

		
		char *Mnemonic::GetOperationBaseName(reg16 index)
		{
			if(Between(index, (reg16)ADD, (reg16)SMUL))
				return OperationALUNames[index];

			if(Between(index, (reg16)FADD, (reg16)FFRAC))
				return OperationFPUNames[index-FADD];

			return "?OP?";	// unknown operation
		}

		char *Mnemonic::GetPasstroughBaseName(reg16 index)
		{
			if(Between(index, (reg16)MOVF_LI, (reg16)MOVF_TF3))
				return "MOVF";

			if(Between(index, (reg16)MOVT_LI, (reg16)MOVT_TF3))
				return "MOVT";

			switch(index)
			{
			case MOVF32_ptrST:
			case MOVF32_ptrST_PO:
			case MOVF16_ptrHE:
			case MOVF16_ptrHE_PO:
				return "MOVF";
			case MOVT32_ptrST:
			case MOVT32_ptrST_PO:
			case MOVT16_ptrHE:
			case MOVT16_ptrHE_PO:
				return "MOVT";
			case MOVF16_ptrST:
			case MOVF16_ptrST_PO:
				return "MOVF16";
			case MOVT16_ptrST:
			case MOVT16_ptrST_PO:
				return "MOVT16";
			case MOVF8_ptrST:
			case MOVF8_ptrST_PO:
			case MOVF8_ptrHE:
			case MOVF8_ptrHE_PO:
				return "MOVF8";
			case MOVT8_ptrST:
			case MOVT8_ptrST_PO:
			case MOVT8_ptrHE:
			case MOVT8_ptrHE_PO:
				return "MOVT8";
			}

			if(Between(index, (reg16)PUSH_ARG, (reg16)PUSH_ptrST))
				return "PUSH";

			if(Between(index, (reg16)POP_ARG, (reg16)POP_ptrST))
				return "POP";

			if(Between(index, (reg16)INC_ARG, (reg16)INC_PO))
				return "INC";

			if(Between(index, (reg16)DEC_ARG, (reg16)DEC_PO))
				return "DEC";

			if(Between(index, (reg16)SETB_UB0, (reg16)SETB_CB))
				return "SETB";

			if(Between(index, (reg16)CLR_UB0, (reg16)CLR_CB))
				return "CLR";

			if(index == RETI)
				return "RETI";

			// if nothing fits..
			return "?ACT?";
		}

		char *Mnemonic::GetPasstroughOperandName(reg16 index)
		{
			if(Between(index, (reg16)MOVF_LI, (reg16)MOVF_TF3))
				return PasstroughMOVRegisters[index];

			if(Between(index, (reg16)MOVT_LI, (reg16)MOVT_TF3))
				return PasstroughMOVRegisters[index-MOVT_LI];

			switch(index)
			{
			case MOVF32_ptrST:
			case MOVT32_ptrST:
			case MOVF16_ptrST:
			case MOVT16_ptrST:
			case MOVF8_ptrST:
			case MOVT8_ptrST:
				return "*ST";

			case MOVF32_ptrST_PO:
			case MOVT32_ptrST_PO:			
			case MOVF16_ptrST_PO:			
			case MOVT16_ptrST_PO:
			case MOVF8_ptrST_PO:
			case MOVT8_ptrST_PO:
				return "*(ST+PO)";
			
			case MOVF16_ptrHE:
			case MOVF8_ptrHE:
			case MOVT8_ptrHE:
			case MOVT16_ptrHE:
				return "*HE";

			case MOVF16_ptrHE_PO:		
			case MOVF8_ptrHE_PO:
			case MOVT8_ptrHE_PO:
			case MOVT16_ptrHE_PO:
				return "*(HE+PO)";
			}

			if(Between(index, (reg16)PUSH_ARG, (reg16)PUSH_ptrST))
				return PasstroughSTACKRegisters[index-PUSH_ARG];

			if(Between(index, (reg16)POP_ARG, (reg16)POP_ptrST))
				return PasstroughSTACKRegisters[index-POP_ARG];

			if(Between(index, (reg16)INC_ARG, (reg16)INC_PO))
				return PasstroughINCDECRegisters[index-INC_ARG];

			if(Between(index, (reg16)DEC_ARG, (reg16)DEC_PO))
				return PasstroughINCDECRegisters[index-DEC_ARG];

			if(Between(index, (reg16)SETB_UB0, (reg16)SETB_CB))
				return PasstroughBits[index-SETB_UB0];

			if(Between(index, (reg16)CLR_UB0, (reg16)CLR_CB))
				return PasstroughBits[index-CLR_UB0];

			if(index == RETI)
				return "";

			// if nothing fits..
			return "??";
		}

		char *Mnemonic::GetReturnRegBaseName(reg16 index, bool tak)
		{
			if(!tak)
			{
				if(index < RET_ptrHE_PO)
					return "RET";
				if(index < RET16_ptrHE_PO)
					return "RET16";
				if(index < RET8_ptrHE_PO)
					return "RET8";

				if(Between(index, (reg16)RET_ptrST_m128, (reg16)RET_ptrST_p128))
					return "RET";

				return "RET?";
			}
			else
			{
				if(index < RET_ptrHE_PO)
					return "TAK";
				if(index < RET16_ptrHE_PO)
					return "TAK16";
				if(index < RET8_ptrHE_PO)
					return "TAK8";

				if(Between(index, (reg16)RET_ptrST_m128, (reg16)RET_ptrST_p128))
					return "TAK";

				return "TAK?";
			}
		}

		std::string Mnemonic::ComposeReturnRegister(reg16 index)
		{
			if(index == (reg16)-1)
				return "";

			if(Between(index, (reg16)RET_LI, (reg16)RET_TF3))
				return ReturnRegisters[index];

			if(Between(index, (reg16)RET_ptrST, (reg16)RET8_ptrHE_PO))
			{
				char *base;
				if( (index-RET_ptrST)/(RET_ptrHE_PO-RET_ptrST)&1 )
					base = "HE";
				else
					base = "ST";

				// limit it, because it's repetitive
				index -= RET_ptrST;
				index %= (RET_ptrST_PO-RET_ptrST)+1;

				std::string reg;
				reg  = ComposeReturnRegister(index-1);

				if(reg.empty())
					return (std::string("*") + base);

				reg = "*(" + std::string(base) + "+" + reg + ")";

				return reg;
			}

			if(Between(index, (reg16)RET_ptrST_m128, (reg16)RET_ptrST_p128))
			{
				int num = (index-RET_ptrST_m128)-128;
				std::string strnum = ToString(num);
				if(num >= 0)
					strnum = "+" + strnum;
				return "*(ST" + strnum + ")";
			}

			return "??";
		}

		char *OperationALUNames[] =
		{
			"ADD",
			"ADC",
			"SUB",
			"SBB",
			"MUL",
			"MULH",
			"DIV",
			"MOD",
			"AND",
			"OR",
			"XOR",
			"NOT",
			"LAND",
			"LOR",
			"LXOR",
			"LNOT",
			"RL",
			"RR",
			"SL",
			"SR",
			"UMIN",
			"UMAX",
			"SMIN",
			"SMAX",
			"SDIV",
			"SMUL"
		};

		char *OperationFPUNames[] =
		{
			 "FADD",
			 "FSUB",
			 "FMUL",
			 "FDIV",
			 "FSQR",
			 "FSQRT",
			 "FPOW",
			 "FROOT",
			 "FSIN",
			 "FCOS",
			 "FTAN",
			 "FLOG2",
			 "FLOG10",
			 "FLN",
			 "FABS",
			 "FTOINT",
			 "FFROMINT",
			 "FMIN",
			 "FMAX",
			 "FFRAC"
		};

		char *QueryRegisterOperands[] =
		{
			"ARG",
			"*ST",
			"*HE",
			"*(ST+PO)",
			"LI",
			"SQ",
			"RE",
			"TR",
			"CI",
			"PE",
			"HE",
			"ST",
			"EL",
			"PO",

			NULL
		};

		char *QueryBitOperands[] =
		{
			"UB0",
			"UB1",
			"UB2",
			"UB3",
			"TO0",
			"TO1",
			"TO2",
			"TO3",
			"CB",
			"IQ",
			"res_0",
			"res_1",

			NULL
		};

		char *SimpleDirNames[] =
		{
			"U",
			"R",
			"D",
			"L",
			"C",
			NULL
		};

		char *DirLength[] =
		{
			"",
			"2",
			"3",
			"4",
			NULL
		};

		char *PasstroughExectime[] =
		{
			">",
			"<",
			"=",
			"|",
			NULL
		};

		char *PasstroughMOVRegisters[] =
		{
			"LI",
			"SQ",
			"RE", 
			"TR",
			"CI",
			"PE",
			"HE",
			"ST",
			"EL",
			"PO",
			"IC",
			"IB",
			"SW",
			"BS",
			"SI",
			"SA",
			"TC",
			"TV0",
			"TV1",
			"TV2",
			"TV3",
			"TF0",
			"TF1",
			"TF2",
			"TF3",

			NULL
		};

		char *PasstroughSTACKRegisters[] =
		{
			"ARG", 
			"SW",
			"LI",
			"SQ",
			"RE",
			"TR",
			"CI",
			"EL",
			"HE",
			"ST",
			"PE",
			"PO",
			"*ST",

			NULL
		};

		char *PasstroughINCDECRegisters[] =
		{
			"ARG",
			"LI",
			"SQ",
			"HE",
			"ST",
			"PE",
			"PO",

			NULL
		};

		char *PasstroughBits[] =
		{
			"UB0",
			"UB1",
			"UB2",
			"UB3",
			"IE",
			"IP",
			"CB",

			NULL
		};

		char *ReturnRegisters[] =
		{
			"LI",
			"SQ",
			"RE", 
			"TR",
			"CI",
			"PE",
			"HE",
			"ST",
			"EL",
			"PO",
			"IC",
			"IB",
			"SW",
			"BS",
			"SI",
			"SA",
			"TC",
			"TV0",
			"TV1",
			"TV2",
			"TV3",
			"TF0",
			"TF1",
			"TF2",
			"TF3",

			NULL
		};
	}
}