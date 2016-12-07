#ifndef INSTRUCTION_H
#define INSTRUCTION_H

/********************* class definition of an instruction and what it contains ****************************/


class Instruction:
  public:
	enum class InstructionType {
		ARITH,
		FLOAT,
		RELOP,
		FLOATRELOP,
		PRINT,
		JMP,
		DATAMOV
			
	}; // all possible instruction types //
	Instruction(InstructionType it);
	/* An address can be a name, constant or a compiler-generated temporary */
	
	static int getLabel();
	
	/* Operand class to be defined here */
	
	class Operand{
        Operand();
        
        ~Operand();
        };
    
    
	
	
};
// The instruction types can have subclasses based on its type //
class ArithIns: public Instruction{
	public:
		enum class ArithInsType {
			ADD, SUB, DIV, MUL, 
			MOD, NEG, AND, OR, XOR 
			};
	public:
		ArithIns(ArithInsType at, const Operand* arg1, const Operand* arg2, const Operand* result);
	private:
        ArithInsType at_;
		const Operand* op1_;
		const Operand* op2_;
		const Operand* dest_;
};

class FloatArithIns: public Instruction{
	public:
		enum class FloatArithInsType {
			FADD, FSUB, FDIV, FMUL, FNEG

		};
	public: /* TODO constructor */
		FloatArithIns(FloatArithInsType fat, const Operand* arg1, const Operand* arg2, const Operand* result);
	private:
        FloatArithInsType fat_;
		const Operand* op1_;
		const Operand* op2_;
		const Operand* dest_;

};

class RelOpIns: public Instruction{
	public:
		enum class RelOpInsType {
			GT, GE, UGT, UGE, EQ, NE
		};
	public: /* TODO constructor */
		RelOpIns(RelOpInsType relop, const Operand* arg1, const Operand* arg2); /* No destination register */
	private:
        RelOpInsType relop_;
        const Operand* arg1_;
        const Operand* arg2_;
};

class FloatRelOpIns: public Instruction{
	public:
		enum class FloatRelOpInsType {
			FGT, FGE, FEQ, FNE
		};
	public: /*TODO constructor */
	    FloatRelOpIns(FloatRelOpIns frelop, const Operand* arg1, const Operand* arg2);
	private:
        FloatRelOpIns frelop_;
        const Operand* arg1;
        const Operand* arg2;
        
};

class PrintIns: public Instruction{
	public:
		enum class PrintInsType {
			PRTI, PRTS, PRTF
		};
	public: /* TODO constructor */
		PrintIns(PrintInsType pit, const Operand* arg1); /* Only one operand */
    private:
        PrintInsType pit_;
        const Operand* arg1;
};

class JumpIns: public Instruction{
	public:
		enum class JumpInsType {
			JMP, JMPC, JMPI, JMPCI
		};
	public: /* TODO constructor ---> how many operators */
		JumpIns(JumpInsType jit);
	private:
        JumpInsType jit_;
        
};

class MovIns: public Instruction{
	public:
		enum class MovInsType {
			MOVL, MOVS, MOVI, MOVF,
			MOVIF, LDI, LDF, STI,
			STF
		};
	public: /* TODO constructor */
		MovIns(MovInsType mit, const Operand* arg1, const Operand* arg2);  /* mov instructions have 2 operands */
		~MovIns();
	private:
        MovInsType mit_;
        const Operand* arg1_;
        const Operand* arg2_;
};	

#endif
