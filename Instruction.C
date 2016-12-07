#include "Instruction.h"
#include "ParserUtil.h"

ArithIns::ArithIns(ArithInsType at, const Operand* arg1, const Operand* arg2, const Operand* result)
        :Instruction(Instruction::InstructionType::ARITH){
            at_ = at;
            op1_ = arg1;
            op2_ = arg2;
            dest_ = result;
            }
            
FloatArithIns::FloatArithIns(FloatArithInsType fat, const Operand* arg1, const Operand* arg2, const Operand* result)
        :Instruction(Instruction::InstructionType::FLOAT){
            fat_ = fat;
            op1_ = op1;
            op2_ = op2;
            dest_ = result;
            
            }
            
RelOpIns::RelOpIns(RelOpInsType relop, const Operand* arg1, const Operand* arg2)
        :Instruction(Instruction::InstructionType::RELOP){
            relop_ = relop;
            arg1_ = arg1;
            arg2_ = arg2;
            }
            
FloatRelOpIns(FloatRelOpIns frelop, const Operand* arg1, const Operand* arg2)
        :Instruction(Instruction::InstructionType::FLOATRELOP){
            frelop_ = frelop;
            arg1_ = arg1;
            arg2_ = arg2;
            }
            
PrintIns(PrintInsType pit, const Operand* arg1)
        :Instruction(Instruction::InstructionType::PRINT){
            pit_ = pit;
            arg1_ = arg1;
            
            }
            
MovIns(MovInsType mit, const Operand* arg1, const Operand* arg2)
        :Instruction(Instruction::InstructionType::DATAMOV){
            mit_ = mit;
            arg1_ = arg1;
            arg2_ = arg2;
            
            }
            
/* TODO: OTHER methods to be implemented */
