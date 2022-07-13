#pragma once
#include "Global.h"

namespace WPU2D
{
	namespace Shared
	{
		/*	**************************************
				**** Instruction info ****
			*************************************/

		enum InstrBase
		{
			insBaseQuery,
			insBaseOperation,
			insBasePasstrough,
			insBaseIndex,

			insBaseNONE	// error
		};

		enum InstrPasstrough
		{
			insPassEntry,
			insPassReturn,
			insPassBidir,
			insPassImmediate,

			insPassNONE	// error if used
		};

		enum InstrIndex
		{
			insIndexReturn,
			insIndexJump,
			insIndexDouble,
			insIndexExtend,
			
			insIndexNONE
		};

		enum InstrIndexReturn
		{
			insIndexRetRegister,
			insIndexRetValue,
			insIndexRetMemory,
			insIndexRetIO
		};

		enum InstrIndexDouble
		{
			insIndexDoubleMerge,
			insIndexDoubleTwodir
		};

		union InstrIndexSub
		{
			InstrIndexReturn returnType;
			InstrIndexDouble doubleType;
			byte jumpUpper2b;
			byte raw;	// easy access no matter the type
		};

		/*	**************************************
					**** Directions  ****
			*************************************/

		enum SimpleDir
		{
			sdir_U,
			sdir_R,
			sdir_D,
			sdir_L,
			sdir_C
		};

		inline SimpleDir InvertSimpleDir(SimpleDir dir)
		{
			switch(dir)
			{
			case sdir_U:
				return sdir_D;
			case sdir_R:
				return sdir_L;
			case sdir_D:
				return sdir_U;
			case sdir_L:
				return sdir_R;
			default:
				return dir;
			}
		}

		struct QueryDir
		{
			SimpleDir dir;
			byte length;

			QueryDir()
			{
				// default??
				dir = sdir_U;
				length = 0;
			}

			QueryDir(SimpleDir dir, byte length)
			{
				this->dir = dir;
				this->length = length;
			}
		};

		/*	**************************************
				  **** Instruction set  ****
			*************************************/

		enum QueryInstructions
		{
			QRY,
			FRK,

			QNZ_ARG,
			QNZ_ptrST,
			QNZ_ptrHE,
			QNZ_ptrST_PO,
			QNZ_LI,
			QNZ_SQ,
			QNZ_RE,
			QNZ_TR,
			QNZ_CI,
			QNZ_PE,
			QNZ_HE,
			QNZ_ST,
			QNZ_EL,
			QNZ_PO,

			QZ_ARG,
			QZ_ptrST,
			QZ_ptrHE,
			QZ_ptrST_PO,
			QZ_LI,
			QZ_SQ,
			QZ_RE,
			QZ_TR,
			QZ_CI,
			QZ_PE,
			QZ_HE,
			QZ_ST,
			QZ_EL,
			QZ_PO,

			QB_UB0,
			QB_UB1,
			QB_UB2,
			QB_UB3,
			QB_TO0,
			QB_TO1,
			QB_TO2,
			QB_TO3,
			QB_CB,
			QB_IQ,
			QB_reserved0,
			QB_reserved1,

			QNB_UB0,
			QNB_UB1,
			QNB_UB2,
			QNB_UB3,
			QNB_TO0,
			QNB_TO1,
			QNB_TO2,
			QNB_TO3,
			QNB_CB,
			QNB_IQ,
			QNB_reserved0,
			QNB_reserved1,

			QEQ_PE,
			QGT_PE,
			QLT_PE,
			QNN,
			QFEQ_PE,
			QFGT_PE,
			QFLT_PE,
			QFNAN,
			QFINF,
			QIF
		};

		enum OperationInstructions
		{
			ADD,
			ADC,
			SUB,
			SBB,
			MUL,
			MULH,
			DIV,
			MOD,
			AND,
			OR,
			XOR,
			NOT,
			LAND,
			LOR,
			LXOR,
			LNOT,
			RL,
			RR,
			SL,
			SR,
			UMIN,
			UMAX,
			SMIN,
			SMAX,
			
			FADD = 32,
			FSUB,
			FMUL,
			FDIV,
			FSQR,
			FSQRT,
			FPOW,
			FROOT,
			FSIN,
			FCOS,
			FTAN,
			FLOG2,
			FLOG10,
			FLN,
			FABS,
			FTOINT,
			FFROMINT,
			FMIN,
			FMAX,
			FFRAC			
		};

		enum PasstroughInstructions
		{
			MOVF_LI,
			MOVF_SQ,
			MOVF_RE,
			MOVF_TR,
			MOVF_CI,
			MOVF_PE,
			MOVF_HE,
			MOVF_ST,
			MOVF_EL,
			MOVF_PO,
			MOVF_IC,
			MOVF_IB,
			MOVF_SW,
			MOVF_BS,
			MOVF_SI,
			MOVF_SA,
			MOVF_TC,
			MOVF_TV0,
			MOVF_TV1,
			MOVF_TV2,
			MOVF_TV3,
			MOVF_TF0,
			MOVF_TF1,
			MOVF_TF2,
			MOVF_TF3,

			MOVT_LI,
			MOVT_SQ,
			MOVT_RE,
			MOVT_TR,
			MOVT_CI,
			MOVT_PE,
			MOVT_HE,
			MOVT_ST,
			MOVT_EL,
			MOVT_PO,
			MOVT_IC,
			MOVT_IB,
			MOVT_SW,
			MOVT_BS,
			MOVT_SI,
			MOVT_SA,
			MOVT_TC,
			MOVT_TV0,
			MOVT_TV1,
			MOVT_TV2,
			MOVT_TV3,
			MOVT_TF0,
			MOVT_TF1,
			MOVT_TF2,
			MOVT_TF3,

			MOVF32_ptrST,
			MOVT32_ptrST,
			MOVF16_ptrST,
			MOVT16_ptrST,
			MOVF8_ptrST,
			MOVT8_ptrST,

			MOVF16_ptrHE,
			MOVT16_ptrHE,
			MOVF8_ptrHE,
			MOVT8_ptrHE,

			MOVF32_ptrST_PO,
			MOVT32_ptrST_PO,
			MOVF16_ptrST_PO,
			MOVT16_ptrST_PO,
			MOVF8_ptrST_PO,
			MOVT8_ptrST_PO,

			MOVF16_ptrHE_PO,
			MOVT16_ptrHE_PO,
			MOVF8_ptrHE_PO,
			MOVT8_ptrHE_PO,

			PUSH_ARG,
			PUSH_SW,
			PUSH_LI,
			PUSH_SQ,
			PUSH_RE,
			PUSH_TR,
			PUSH_CI,
			PUSH_EL,
			PUSH_HE,
			PUSH_ST,
			PUSH_PE,
			PUSH_PO,
			PUSH_ptrST,

			POP_ARG,
			POP_SW,
			POP_LI,
			POP_SQ,
			POP_RE,
			POP_TR,
			POP_CI,
			POP_EL,
			POP_HE,
			POP_ST,
			POP_PE,
			POP_PO,
			POP_ptrST,

			INC_ARG,
			INC_LI,
			INC_SQ,
			INC_HE,
			INC_ST,
			INC_PE,
			INC_PO,

			DEC_ARG,
			DEC_LI,
			DEC_SQ,
			DEC_HE,
			DEC_ST,
			DEC_PE,
			DEC_PO,

			SETB_UB0 = 113,
			SETB_UB1,
			SETB_UB2,
			SETB_UB3,
			SETB_IE,
			SETB_IP,
			SETB_CB,

			CLR_UB0,
			CLR_UB1,
			CLR_UB2,
			CLR_UB3,
			CLR_IE,
			CLR_IP,
			CLR_CB,

			RETI
		};

		enum ReturnRegisterInstructions
		{
			RET_LI,
			RET_SQ,
			RET_RE,
			RET_TR,
			RET_CI,
			RET_PE,
			RET_HE,
			RET_ST,
			RET_EL,
			RET_PO,
			RET_IC,
			RET_IB,
			RET_SW,
			RET_BS,
			RET_SI,
			RET_SA,
			RET_TC,
			RET_TV0,
			RET_TV1,
			RET_TV2,
			RET_TV3,
			RET_TF0,
			RET_TF1,
			RET_TF2,
			RET_TF3,

			RET_ptrST,
			RET_ptrST_LI,
			RET_ptrST_SQ,
			RET_ptrST_RE,
			RET_ptrST_TR,
			RET_ptrST_CI,
			RET_ptrST_PE,
			RET_ptrST_HE,
			RET_ptrST_ST,
			RET_ptrST_EL,
			RET_ptrST_PO,

			RET_ptrHE,
			RET_ptrHE_LI,
			RET_ptrHE_SQ,
			RET_ptrHE_RE,
			RET_ptrHE_TR,
			RET_ptrHE_CI,
			RET_ptrHE_PE,
			RET_ptrHE_HE,
			RET_ptrHE_ST,
			RET_ptrHE_EL,
			RET_ptrHE_PO,

			RET16_ptrST,
			RET16_ptrST_LI,
			RET16_ptrST_SQ,
			RET16_ptrST_RE,
			RET16_ptrST_TR,
			RET16_ptrST_CI,
			RET16_ptrST_PE,
			RET16_ptrST_HE,
			RET16_ptrST_ST,
			RET16_ptrST_EL,
			RET16_ptrST_PO,

			RET16_ptrHE,
			RET16_ptrHE_LI,
			RET16_ptrHE_SQ,
			RET16_ptrHE_RE,
			RET16_ptrHE_TR,
			RET16_ptrHE_CI,
			RET16_ptrHE_PE,
			RET16_ptrHE_HE,
			RET16_ptrHE_ST,
			RET16_ptrHE_EL,
			RET16_ptrHE_PO,

			RET8_ptrST,
			RET8_ptrST_LI,
			RET8_ptrST_SQ,
			RET8_ptrST_RE,
			RET8_ptrST_TR,
			RET8_ptrST_CI,
			RET8_ptrST_PE,
			RET8_ptrST_HE,
			RET8_ptrST_ST,
			RET8_ptrST_EL,
			RET8_ptrST_PO,

			RET8_ptrHE,
			RET8_ptrHE_LI,
			RET8_ptrHE_SQ,
			RET8_ptrHE_RE,
			RET8_ptrHE_TR,
			RET8_ptrHE_CI,
			RET8_ptrHE_PE,
			RET8_ptrHE_HE,
			RET8_ptrHE_ST,
			RET8_ptrHE_EL,
			RET8_ptrHE_PO
		};


	}
}