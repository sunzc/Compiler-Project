void RefExprNode::codeGen() const{
	// codeGen functionality
	// TODO : check if some register is already allocated to the variable ??
	const VariableEntry* var = dynamic_cast<SymTabEntry* >(symTabEntry());
	// load instruction specification : Load Integer register from memory --
	// LDI / LDF <reg> <regormem>
	setReg(); // set register for variable
	// two types of instructions
	Instruction* addOffset ;  // add offset to register
	Instruction* loadMemory ;  // add value from memory location to 
												  // register for refexpr
	const Value* offsetVal;
	switch(var->varKind()){
		case: VariableEntry::VarKind::GLOBAL_VAR:
			addOffset = new ArithIns(ArithIns::ArithInsType::PLUS);
			addOffset->operand1(get_global_area(), NULL);
			offsetVal = Value(var->offSet(), Type::TypeTag::INT);

			addOffset->operand2(-1, offsetVal); // value based operand

			rePtr = intReg(); // fetch next integer register
			addOffset->operandDest(rePtr, NULL);

			addOffset->print();
			break;
		case: VariableEntry::VarKind::LOCAL_VAR:
			addOffset = new ArithIns(ArithIns::ArithInsType::PLUS);
			addOffset->operand1(get_base_ptr(), NULL);
			offsetVal = Value(-(var->offSet()), Type::TypeTag::INT); // is offset negative ? from the base pointer ->yes

			addOffset->operand2(-1, offsetVal);

			rePtr = intReg();
			addOffset->operandDest(rePtr, NULL);

			addOffset->print();
			break;
		case: VariableEntry::VarKind::PARAM_VAR:
			addOffset = new ArithIns(ArithIns::ArithInsType::PLUS);
			addOffset->operand1(get_base_ptr(), NULL);
			offsetVal = Value(var->offSet(), Type::TypeTag::INT);

			addOffset->operand2(-1, offsetVal);

			rePtr = intReg();
			addOffset->operandDest(rePtr, NULL);

			addOffset->print();
			break;
			
		default:
			//TODO --> nothing
	}

	// generate instruction for loading ino register
	if(intType){
		loadMemory = new MovIns(MovIns::MovInsType::LDI);	
	}
	else{
		loadMemory = new MovIns(MovIns::MovInsType::LDF);
	}
	loadMemory->operand1(rePtr, NULL);  // calculated address --> source operand
	loadMemory->operandDest(getReg()); // get register for this expression //
}

void IfNode::codeGen() const{

	if(!cond()) return;

	cond()->codeGen(); // generate code for the condition expression
	// cond expression generates statements for if statements ?
	// call individual codeGen functions for each component--but I need to generate labels ?
	// what you can do is associate a boolean parameter isLabel for each statement --> good idea ?
	then
	thenStmt()->codeGen();
	if(!elseStmt()) {
		elseStmt()->codeGen();
	}
}

void ReturnStmtNode::codeGen() const {
	Instruction* storeRetVal;
	Instruction* moveSP; // moves sp to point to bp
	Instruction* addOffset; // move stack pointer --> remove all parameters and return address
	Instruction* jmpIns; // jump to return address

	reg = get_stck_ptr_reg();
	int count = 0;
	// The below instruction stores the return value in a reg
	regPtr = intReg();
	regFPtr = floatReg(); 
    storeRetVal->operand_dest(regPtr, NULL);

    if(expr_->coercedType())
            isInt = (Type::isIntegral(expr_->coercedType()->tag()) || Type::isBool(expr_->coercedType()->tag()));
    else
            isInt = (Type::isIntegral(expr_->type()->tag()) || Type::isBool(expr_->type()->tag()));

    if(isInt){
    	storeRetval= new MovIns(MovIns::MovInsType::STI));
		storeRetVal->operandDest(regPtr, NULL);
    }
    else{
    	storeRetval= new MovIns(MovIns::MovInsType::STF));
		storeRetVal->operandDest(regFPtr, NULL);
    }
    

    storeRetVal->operand1(expr_->getReg(), NULL); // value from register containing expression
    storeRetVal->print();
	// The below instruction updates base pointer
	moveSP = new MovIns(MovIns::MovInsType::STI);
	moveSP->operand1(reg, NULL);
	moveSP->operandDest(get_base_ptr_reg(), NULL);
	moveSP->print();

	addOffset = new ArithIns(ArithIns::ArithInsType::ADD);
	FunctionEntry* fe = fun_ ;
	SymTab* map = fe->symTab();
	for(SymTab::iterator i = map->begin(); i != map->end(); ++i){
		VariableEntry* v = (VariableEntry*)(*i);
		if(v->varKind==VariableEntry::VarKind::PARAM_VAR){
			count++;  // counts no. of parameters ?
		}
	}
	const Value* toAdd = new Value(count+1, Type::TypeTag::INT); // include return address

	addOffset->operand1(reg, NULL);
	addOffset->operand2(-1, toAdd); // this is the value to be added to stack pointer

	addOffset->operandDest(reg, NULL);
	addOffset->print();

	setReg(); // to be implemented
	
}

void ValueNode::codeGen() const {
	// print the move instructions to register--> do not store in memory takes time
	// determine the types of value
	const Value* v = value();
	const Type* typ = v->type();

	Instruction* moveInstr;

	bool isInt = false;
	bool isFloat = false;
	bool isStr = false;

	if(Type::isBool(typ->tag()) || Type::isIntegral(typ->tag())){
		isInt = true;
		setReg(intReg);
	}
	else if(Type::isFloat(typ->tag())){
		isFloat = true;
		setReg(floatReg);
	}
	else if(Type::isString(typ->tag())){
		isStr = true;
		setReg(intReg);
	}
	if(isInt){
		moveInstr = new MovIns(MovIns::MovInsType::MOVI);
	}
	if(isFloat){
		moveInstr = new MovIns(MovIns::MovInsType::MOVF);
	}
	if(isStr){
		moveInstr = new MovIns(MovIns::MovInsType::MOVS);
	}

	moveInstr->operand1(-1, v); // no register, pure value
	moveInstr->operandDest(getReg(), NULL);

}


void OpNode::codeGen(){
	//handle simple arithmetic addition first

	Instruction* instr;

	bool intType = false;
	bool intSigned = false;

	if(coercedType() && (Type::isIntegral(coercedType()->tag()) || Type::isBool(coercedType()->tag()) || Type::isInt(coercedType()->tag()))
	{
		// if integer type set to TRUE, this helps us to access float or integer type registers
		intType = true;
	}

	if(intType && Type::isSigned(type()->tag())){

		intSigned = true;
	}

	// set integer or float registers for this expression using setReg() 
	// correspondingly get some integer/float register using getReg() for an expression

	int j = arity();
	for(i=0; i<j; i++){
		arg_[i]->codeGen();
	}

	switch(static_cast<int>(opCode()){
		// arithmetic operations done
		case static_cast<int>(OpNode::OpCode::PLUS):
			
			if(intType){
				instr = new ArithIns(ArithIns::ArithInsType::ADD);
				setReg(IntReg);  // set some integer register
			}
			else{
				instr = new ArithIns(ArithIns::ArithInsType::FADD);
				setReg(FloatReg);
			}
			instr->operand1(arg_[0]->getReg(), NULL);
			instr->operand2(arg_[1]->getReg(), NULL);
			break;
			
		case static_cast<int>(OpNode::OpCode::MINUS):
			if(intType){
				instr = new ArithIns(ArithIns::ArithInsType::SUB);
				setReg(IntReg);  // set some integer register
			}
			else{
				instr = new ArithIns(ArithIns::ArithInsType::FSUB);
				setReg(FloatReg);
			}
			instr->operand1(arg_[0]->getReg(), NULL);
			instr->operand2(arg_[1]->getReg(), NULL);
			break;
		

		case static_cast<int>(OpNode::OpCode::MULT):
			if(intType){
				instr = new ArithIns(ArithIns::ArithInsType::MUL);
				setReg(IntReg);
			}
			else{
				instr = new ArithIns(ArithIns::ArithInsType::FMUL);
				setReg(FloatReg);
			}
			instr->operand1(arg_[0]->getReg(), NULL);
			instr->operand2(arg_[1]->getReg(), NULL);
			break;

		case static_cast<int>(OpNode::OpCode::DIV):
			if(intType){
				instr = new ArithIns(ArithIns::ArithInsType::DIV);
				setReg(IntReg);
			}
			else{
				instr = new ArithIns(ArithIns::ArithInsType::FDIV);
				setReg(FloatReg);
			}
			instr->operand1(arg_[0]->getReg(), NULL);
			instr->operand2(arg_[1]->getReg(), NULL);
			break;

		case static_cast<int>(OpNode::OpCode::MOD):
			
			instr = new ArithIns(ArithIns::ArithInsType::MOD);
			setReg(IntReg);
			
			instr->operand1(arg_[0]->getReg(), NULL);
			instr->operand2(arg_[1]->getReg(), NULL);
			break;

		case static_cast<int>(OpNode::OpCode::UMINUS):
			if(intType){
				instr = new ArithIns(ArithIns::ArithInsType::NEG);
				setReg(IntReg);
			}
			else{
				instr = new ArithIns(ArithIns::ArithInsType::FNEG);
				setReg(FloatReg);
			}
			instr->operand1(arg_[0]->getReg(), NULL);
			// only 1 operand
			break;

		// RELATIONAL operations done here ALONG WITH CONDITIONAL JUMPS **********************************//

		case static_cast<int>(OpNode::OpCode::EQ):

			// need to maintain the value of the register for this expression
			jmpInstr = new JumpIns(JumpIns::JumpInsType::JMPC);
			// this jump instruction associated with a relational op
			int reg1 = arg_[0]->getReg();
			int reg2 = arg_[1]->getReg();
			jmpInstr->relOpType(RelOpIns::RelOpInsType::EQ);
			jmpInstr->operand1(reg1, NULL);
			jmpInstr->operand2(reg2, NULL);

			string newlabel = Instruction::getLabel(); // TODO

			jmpInstr->label(newLabel); 

			setReg(IntReg);

			jmpInstr->print();
			// more instructions may be needed. Hold on !
			return;

		case static_cast<int>(OpNode::OpCode::NE):
			// need to maintain the value of the register for this expression
			jmpInstr = new JumpIns(JumpIns::JumpInsType::JMPC);
			// this jump instruction associated with a relational op
			int reg1 = arg_[0]->getReg();
			int reg2 = arg_[1]->getReg();
			jmpInstr->relOpType(RelOpIns::RelOpInsType::NE);
			jmpInstr->operand1(reg1, NULL);
			jmpInstr->operand2(reg2, NULL);

			string newlabel = Instruction::getLabel(); // TODO

			jmpInstr->label(newLabel); // this method generates a new label randomly

			setReg(IntReg);

			jmpInstr->print();
			return;
			
		case static_cast<int>(OpNode::OpCode::GT):

			// need to maintain the value of the register for this expression
			jmpInstr = new JumpIns(JumpIns::JumpInsType::JMPC);
			// this jump instruction associated with a relational op
			int reg1 = arg_[0]->getReg();
			int reg2 = arg_[1]->getReg();
			if(intType && intSigned){
				jmpInstr->relOpType(RelOpIns::RelOpInsType::UGT);
			}
			else{
				jmpInstr->relOpType(RelOpIns::RelOpInsType::GT);
			}
			
			jmpInstr->operand1(reg1, NULL);
			jmpInstr->operand2(reg2, NULL);

			string newlabel = Instruction::getLabel(); // TODO

			jmpInstr->label(newLabel); // this method generates a new label randomly

			setReg(IntReg);

			jmpInstr->print();
			// more instructions may be needed. Hold on !
			return;

		case static_cast<int>(OpNode::OpCode::LT):

			// need to maintain the value of the register for this expression
			jmpInstr = new JumpIns(JumpIns::JumpInsType::JMPC);
			// this jump instruction associated with a relational op
			int reg1 = arg_[0]->getReg();
			int reg2 = arg_[1]->getReg();
			if(intType && intSigned){
				jmpInstr->relOpType(RelOpIns::RelOpInsType::ULT);
			}
			else{
				jmpInstr->relOpType(RelOpIns::RelOpInsType::LT);
			}
			jmpInstr->operand1(reg1, NULL);
			jmpInstr->operand2(reg2, NULL);

			string newlabel = Instruction::getLabel(); // TODO

			jmpInstr->label(newLabel); // this method generates a new label randomly

			setReg(IntReg);

			jmpInstr->print();
			// more instructions may be needed. Hold on !
			return;

		case static_cast<int>(OpNode::OpCode::GE):
			// need to maintain the value of the register for this expression
			jmpInstr = new JumpIns(JumpIns::JumpInsType::JMPC);
			// this jump instruction associated with a relational op
			int reg1 = arg_[0]->getReg();
			int reg2 = arg_[1]->getReg();
			if(intType && intSigned){
				jmpInstr->relOpType(RelOpIns::RelOpInsType::UGE);
			}
			else{
				jmpInstr->relOpType(RelOpIns::RelOpInsType::GE);
			}
			jmpInstr->operand1(reg1, NULL);
			jmpInstr->operand2(reg2, NULL);

			string newlabel = Instruction::getLabel(); // TODO

			jmpInstr->label(newLabel); // this method generates a new label randomly

			setReg(IntReg);

			jmpInstr->print();
			// more instructions may be needed. Hold on !
			return;

		case static_cast<int>(OpNode::OpCode::LE):
			// need to maintain the value of the register for this expression
			jmpInstr = new JumpIns(JumpIns::JumpInsType::JMPC);
			// this jump instruction associated with a relational op
			int reg1 = arg_[0]->getReg();
			int reg2 = arg_[1]->getReg();
			if(intType && intSigned){
				jmpInstr->relOpType(RelOpIns::RelOpInsType::ULE);
			}
			else{
				jmpInstr->relOpType(RelOpIns::RelOpInsType::LE);
			}
			jmpInstr->operand1(reg1, NULL);
			jmpInstr->operand2(reg2, NULL);

			string newlabel = Instruction::getLabel(); // TODO

			jmpInstr->label(newLabel); // this method generates a new label randomly

			setReg(IntReg);

			jmpInstr->print();
			// more instructions may be needed. Hold on !
			// make a label based instruction here ---> it is safe for statements !
			return;
		// 

		instr->operandDest(this->getReg());

		instr->print();

	}
}


