#include "STEClasses.h"
#include "Value.h"
#include "ParserUtil.h"

int countLocal=0;
int countGlobal=0;

void GlobalEntry::print(ostream& out, int indent) const
{
	// Add your code
	// To avoid printing declarations at the time of call-- do this--> call printST

	printST(out, indent, '\0' ,';');
	const vector<RuleNode*> rules_ = this->rules();
	RuleNode* rnm;
	// Loop over the rules and print them
	for (vector<RuleNode* >::const_iterator i = rules_.begin(); i!= rules_.end(); ++i){
		rnm = *i;
		rnm->print(out, indent);
		
	} 
}

void GlobalEntry::typePrint(ostream& out, int indent) const
{
	// Add your code
	
	const vector<RuleNode*> rules_ = this->rules();
	RuleNode* rnm;
	// Loop over the rules and print their types -- not defined yet :)
	for (vector<RuleNode* >::const_iterator i = rules_.begin(); i!= rules_.end(); ++i){
		rnm = *i;
		rnm->typePrint(out, indent);
		
	} 
}

void GlobalEntry::codeGen() const 
{
	Instruction* stckPtrIns;
	stckPtrIns = new MovIns(MovIns::MovInsType::MOVI);

	const Value* stckpointer = new Value(10000, Type::TypeTag::INT); // stack pointer points 10000
	stckPtrIns->operand1(-1, stckpointer);  // immediate value ,!register
	stckPtrIns->operandDest(get_stck_ptr_reg(), NULL);

	stckPtrIns->print();

	// base pointer points to stack pointer initially
	Instruction* basePtrIns;
	basePtrIns = new MovIns(MovIns::MovInsType::MOVI);

	basePtrIns->operand1(-1, stckpointer);
	basePtrIns->operandDest(get_base_ptr_reg(), NULL);

	basePtrIns->print();

	// initialize global area ?
	Instruction* globalPtrIns;
	globalPtrIns = new MovIns(MovIns::MOvInsType::MOVI);

	const Value* globalArea = new Value(1000, Type::TypeTag::INT); // global area points 1000? -> given somewhere ?
	globalPtrIns->operand1(-1, globalArea);

	globalPtrIns->operandDest(get_global_reg(), NULL);
	globalPtrIns->print();

	// perform the jump to begin now
	Instruction* jumpIns = new MovIns(JumpIns::JumpInsType::JMP);

	string newlabel = "begin";
	jumpIns->label(newlabel);

	jumpIns->print();

	// loop through all rules and generate code for them
	const vector<RuleNode* > allRules_ = rules();

	for(vector<RuleNode*>::const_iterator i = allRules_.begin(); i != allRules_.end(); ++i)
	{
		(*i)->codeGen();
	} 

	// end instructions now -->
	finalIns = new PrintIns(PrintIns::PrintInsType::PRTS);

	const Value* goodBye = new Value("*** DONE ***", Type::TypeTag::STRING);
	finalIns->operand1(-1, goodBye); // no register - only immediate value

	finalIns->print();
}


void EventEntry::print(ostream& out, int indent) const
{
	// Add your code
	out << "event ";
	out << name();
	const EventEntry* ee = this;
	out << '(';
	if(ee->symTab() != NULL){
		// obtain the map elements which are the formal parameters for the function
		const SymTab* map = ee->symTab();
		SymTab::const_iterator i = map->begin();
		while(i != map->end()){
			VariableEntry* v = (VariableEntry* )*i;
			if(v->varKind() == VariableEntry::VarKind::PARAM_VAR){
				v->print(out, indent); 
				out << ',';}
			else{	
				out << '\b';
				break;
				}
			++i;
			}
		}
	out << ");";
	out << endl;
}

void EventEntry::typePrint(ostream& out, int indent) const
{
	// Add your code
	out << "event ";
	out << name();
	const EventEntry* ee = this;
	out << '(';
	if(ee->symTab() != NULL){
		// obtain the map elements which are the formal parameters for the function
		const SymTab* map = ee->symTab();
		SymTab::const_iterator i = map->begin();
		while(i != map->end()){
			VariableEntry* v = (VariableEntry* )*i;
			if(v->varKind() == VariableEntry::VarKind::PARAM_VAR){
				v->typePrint(out, indent); 
				out << ',';}
			else{	
				out << '\b';
				break;
				}
			++i;
			}
		}
	out << ");";
	out << endl;
}

void EventEntry::codeGen() const
{
	// TODO:: Event codegen

}


void ClassEntry::print(ostream& out, int indent) const
{
	out << "class ";
	out << name() ;
	out << endl;
}

void ClassEntry::typePrint(ostream& out, int indent) const
{
	out << "class ";
	out << name() ;
	out << endl;
}

void ClassEntry::codeGen() const
{
	// nothing much to do
	return;

}

void VariableEntry::print(ostream& out, int indent) const
{	
	const Type* t1 = this->type();
	t1->print(out, indent);
	out << " ";
	out << name() ;
	const ExprNode* en = initVal();
	if(en != NULL){
		out << "=";
		en->print(out, indent);
	}
	 
}

void VariableEntry::typePrint(ostream& out, int indent) const
{	
	const Type* t1 = this->type();
	t1->print(out, indent);
	out << " ";
	out << name() ;
	const ExprNode* en = initVal();
	if(en != NULL){
		out << "=";
		en->typePrint(out, indent);
	}
	 
}

void VariableEntry::codeGen() const
{
	// again differentiate based on whether the variable is local/global/param based

	Instruction* addGlobalVar;
	Instruction* storeVarValue;

	Instruction* addLocalVar;
	Instruction* addParamVar;

	Instruction* getParamIns;

	bool isInt;

	// determine type of global var
	if(Type::isIntegral(type()->tag()) || Type::isBool(type()->tag())){
		isInt = true;
		setReg(intReg());  // sets integer register for this variable
	}

	if(Type::isFloat(type()->tag())){
		isInt = false;
		setReg(floatReg()); // sets floating point register for this variable
	}

	if(varKind() == VariableEntry::VarKind::GLOBAL_VAR){
		addGlobalVar = new ArithIns(ArithIns::ArithInsType::ADD);
		addGlobalVar->operand1(get_global_reg(), NULL); // get me global register

		const Value* offset = new Value(offSet(), Type::TypeTag::INT);
		addGlobalVar->operand2(-1, offSet);  // add offset value to this register

		
		intReg = intReg();  // give me an integer register
		addGlobalVar->operandDest(intReg, NULL);
		
		addGlobalVar->print();
		
		// if you have initial value--> store it in this register
		if(initVal() != NULL){
			if(isInt){
				initVal()->codeGen();  // call codeGen , have a register with it
				storeVarValue = new MovIns(MovIns::MovInsType::STI);
				
				storeVarValue->operand1(initVal()->getReg(), NULL); // get register associated with value
			}
			else{
				initVal()->codeGen();  // call codeGen , have a register with it
				storeVarValue = new MovIns(MovIns::MovInsType::STF);
				
				storeVarValue->operand1(initVal()->getReg(), NULL); // get register associated with value
			}
			storeVarValue->operandDest(intReg, NULL);

			storeVarValue->print();
		} 
	}
	else if(varKind() == VariableEntry::VarKind::LOCAL_VAR){
		addLocalVar = new ArithIns(ArithIns::ArithInsType::SUB); // subtract local variables from base pointer reg.

		addLocalVar->operand1(get_base_ptr_reg(), NULL); // get me base pointer register

		const Value* offset = new Value(offSet(), Type::TypeTag::INT);

		addLocalVar->operand2(-1, offSet);  // add offset value to this register

		
		intReg = intReg();  // give me an integer register
		addLocalVar->operandDest(intReg, NULL);
		
		addLocalVar->print();
		
		// if you have initial value--> store it in this register
		if(initVal() != NULL){
			if(isInt){
				initVal()->codeGen();  // call codeGen , have a register with it
				storeVarValue = new MovIns(MovIns::MovInsType::STI);
				
				storeVarValue->operand1(initVal()->getReg(), NULL); // get register associated with value
			}
			else{
				initVal()->codeGen();  // call codeGen , have a register with it
				storeVarValue = new MovIns(MovIns::MovInsType::STF);
				
				storeVarValue->operand1(initVal()->getReg(), NULL); // get register associated with value
			}
			storeVarValue->operandDest(intReg, NULL);

			storeVarValue->print();
		}
	}
	else{
			// PARAM VARS
			addParamVar = new ArithIns(ArithIns::ArithInsType::ADD); // add local variables from base pointer register.

			addParamVar->operand1(get_tmp_stck_ptr_reg(), NULL); // get me temporary stack pointer register-- as you saw in code
																	// for stack pointer 900-> I want 901

			const Value* offset = new Value(1, Type::TypeTag::INT);
			addParamVar->operand2(-1, offSet);  // add offset value to this register
			addParamVar->operandDest(get_tmp_stck_ptr_reg(), NULL); // ADD 901 1 901
		
			addParamVar->print();
		
			// 
			if(isInt){
				getParamIns = new MovIns(MovIns::MovInsType::LDI);  // load from memory to registers

			}
			else{
				getParamIns = new MovIns(MovIns::MovInsType::LDF);
			}
			getParamIns->operand1(get_tmp_stck_ptr_reg(), NULL);
			getParamIns->operandDest(getReg(), NULL); // LDI R901 R$$$

			getParamIns->print();
		} 
	}

}

/* simple memory allocation functions */

void VariableEntry::memAlloc()
{
	if(this->varKind()==VariableEntry::VarKind::GLOBAL_VAR){
		countGlobal += 1;
		this->offSet(countGlobal);
	}
	else if(this->varKind()==VariableEntry::VarKind::LOCAL_VAR || 
		this->varKind()==VariableEntry::VarKind::PARAM_VAR) {
		/* are you inside a function */
			
		/* stack pointer is stored within a register -- obtain the pointer from there */
		countLocal += 1;
		
		this->offSet(countLocal);
	}
}



void BlockEntry::print(ostream& out, int indent) const
{
	
	
}

void FunctionEntry::print(ostream& out, int indent) const
{
	const Type* t1 = this->type();
	t1->print(out, indent);
	out << " ";	

	out << name();
	const FunctionEntry* fe = this;
	out << '(';
	if(fe->symTab() != NULL){
		// obtain the map elements which are the formal parameters for the function
		const SymTab* map = fe->symTab();
		SymTab::const_iterator i = map->begin();
		while(i != map->end()){
			
			VariableEntry* v = (VariableEntry* )*i;
			if(v->varKind() == VariableEntry::VarKind::PARAM_VAR){
				v->print(out, indent); 
				out << ',';}
			else{
				break;
				}
			++i;
			}
		}
	out << ')';
	const CompoundStmtNode* csn = body();
	if(csn != NULL){
		csn->print(out, indent);
	}
	out << ';';
	out << endl;
}

void FunctionEntry::typePrint(ostream& out, int indent) const
{
	const Type* t1 = this->type();
	t1->print(out, indent);
	out << " ";	

	out << name();
	const FunctionEntry* fe = this;
	out << '(';
	if(fe->symTab() != NULL){
		// obtain the map elements which are the formal parameters for the function
		const SymTab* map = fe->symTab();
		SymTab::const_iterator i = map->begin();
		while(i != map->end()){
			VariableEntry* v = (VariableEntry* )*i;
			if(v->varKind() == VariableEntry::VarKind::PARAM_VAR){
				v->typePrint(out, indent); 
				out << ',';}
			else{
				break;
				}
			++i;
			}
		}
	out << ')';
	const CompoundStmtNode* csn = body();
	if(csn != NULL){
		csn->typePrint(out, indent);
	}
	out << ';';
	out << endl;
}

void FunctionEntry::codeGen() const
{
	int num_local_var = 0;
	int num_actual_param = 0;
	// many things happen when function entry happens
	// 1. label gets generated.
	Instruction* labelIns;
	labelIns = new LabelIns();
	labelIns->funLabel(name()); // function label 
	labelIns->print();

	//2. make base pointer equal to stack pointer
	Instruction* bpsp;
	bpsp = new MovIns(MovIns::MovInsType::MOVI);

	bpsp->operand1(get_stck_ptr_reg(), NULL); // stack 
	bpsp->operandDest(get_base_ptr_reg(), NULL);

	bpsp->print();

	//3. generate a temporary_stack_ptr register and add 1
	Instruction* tpsp;
	tpsp = new ArithIns(ArithIns::ArithInsType::ADD);
	tpsp->operand1(get_stck_ptr_reg(), NULL);

	const Value* v = new Value(1, Type::TypeTag::INT);
	tpsp->operand2(-1, v); // immediate value

	tpsp->operandDest(get_tmp_stck_ptr_reg(), NULL);
	tpsp->print();

	// 4. store return address in some register
	Instruction* returnIns;
	returnIns= new MovIns(MovIns::MovInsType::LDI);
	returnIns->operand1(get_tmp_stck_ptr_reg());

	int newReg = intReg(); // get me next integer register

	returnIns->operandDest(newReg, NULL);
	returnIns->print();

	//5. increment tmp stck by 1 so you can start storing parameters
	Instruction* tpsp1 = new ArithIns(ArithIns::ArithInsType::ADD);

	tpsp1->operand1(get_tmp_stck_ptr_reg(), NULL);
	tpsp1->operand2(-1, v);

	tpsp1->operandDest(get_tmp_stck_ptr_reg(), NULL);
	tpsp1->print();

	//6. start implementing codegen for parameters now
	// opt for accessing the symbol table map 
	SymTab* map = this->symTab();
	for(SymTab::iterator i = map->begin(); i != map->end(); ++i){
		VariableEntry* v = (VariableEntry*)(*i);
		if(v->varKind==VariableEntry::VarKind::PARAM_VAR){
			num_actual_param++;
			v->codeGen();
		}
	// do the same for local variables.. if present
	for(SymTab::iterator i = map->begin(); i != map->end(); ++i){
		VariableEntry* v = (VariableEntry*)(*i);
		if(v->varKind==VariableEntry::VarKind::LOCAL_VAR){
			num_local_var++ ;
			v->codeGen();

		}

	// codeGen for function body now

	if(body()){
		body()->codeGen();
	}
	// 7. update the base pointer through a sequence of statements
	const Value* valBase = new Value(num_local_var, Type::TypeTag::INT);

	Instruction* newBasePtIns = new ArithIns(ArithIns::ArithInsType::ADD);
	newBasePtIns->operand1(get_tmp_stck_ptr_reg(), NULL); // current value of temp stack ptr
	newBasePtIns->operand2(-1, valBase);

	newBasePtIns->operandDest(get_tmp_stck_ptr_reg(), NULL);

	newBasePtIns->print(); 

	// 8. now you can perform load instruction to a temp reg
	int anyReg = intReg();

	Instruction* newLoadIns = new MovIns(MovIns::MovInsType::LDI);
	newLoadIns->operand1(get_tmp_stck_ptr_reg(), NULL);
	newLoadIns->operandDest(anyReg, NULL);  // loaded the value of temp stack pointer
	newLoadIns->print();

	// 9. finally update base pointer
	Instruction* newBasePtrIns = new MovIns(MovIns::MovInsType::MOVI);
	newBasePtrIns->operand1(anyReg, NULL);
	newBasePtrIns->operandDest(get_tmp_stck_ptr_reg(), NULL);

	newBasePtrIns->print();

	// 9.a) before you update stack pointer -- please store its value in a register
	// we may need to use this value during return address ?
	regRetAddrLoad = intReg();
    Instruction* retAdrLdIns = new MovIns(MovIns::MovInsType::LDI);
    retAdrLdIns->operand1(get_stck_ptr_reg(), NULL);
    retAdrLdIns->operandDest(regRetAddrLoad, NULL);

	// 10. Update stack pointer also now :(
	// Stack pointer updates by no. of local vars + 1 + no. of parameters
	Instruction* newStackPtrIns = new ArithIns(ArithIns::ArithInsType::ADD);
	newStackPtrIns->operand1(get_stck_ptr_reg(), NULL);

	const Value* stackVal = new Value(num_actual_param + num_local_var + 1, Type::TypeTag::INT);

	newStackPtrIns->operand2(-1, stackVal);
	newStackPtrIns->operandDest(get_stck_ptr_reg(), NULL);

	newStackPtrIns->print();

	// Labels may not work !!?

	newFuncRetLabel = Instruction::get_label();
    setReturnLabel(newFuncRetLabel);

	funcRetLabel = new LabelIns();
    funcRetLabel->label(newFuncRetLabel);

    funcRetLabel->print();

    // 11. Now take the Jump !
    Instruction* jumpFinalIns = new JumpIns(JumpIns::JumpInsType::JMPI);
    jumpFinalIns->operandDest(regRetAddrLoad, NULL);

    jumpFinalIns->print();


}

void FunctionEntry::memAlloc() {

	/* set the offset counter to 0 */
	/* need to look after the activation record and stack pointer */
	
	countLocal = 0;

}

void EventEntry::memAlloc() {

	/* set the offset counter to 0 */
	countLocal = 0;

}
