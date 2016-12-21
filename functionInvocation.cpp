void InvocationNode::codeGen() const {

	// determine stack pointer and push it..
	// this should be done during initialization ?
	bool isInt, isFloat, isStr;

	Instruction* storeIns;
	Instruction* subIns;

	string returnLabel = Instruction::getLabel(); 
	// remember getLabel() must be a static method
	// this method generates a new label randomly

	const Vector<ExprNode* >* pars = params();

	int reg = get_stck_ptr_reg(); // this is the stack pointer register

	if(pars != nullptr){
		for (vector<ExprNode* >::const_reverse_iterator i = pars->rbegin(); i != pars->rend(); ++i) {
			(*i)->codeGen();
			if(Type::isIntegral((*i)->type()->tag()) || Type::isBool((*i)->type()->tag())){
				isInt = true;
			}
			if(Type::isFloat((*i)->type()->tag())){
				isFloat = true;
			}
			if(Type::isString((*i)->type()->tag())){
				isStr = true;
			}
			if(isInt){
				storeIns = new MovIns(MovIns::MovInsType::STI);
				storeIns->operand1((*i)->getReg(), NULL);

				storeIns->operandDest(reg, NULL);
			}
			if(isFloat){
				storeIns = new MovIns(MovIns::MovInsType::STF);
				storeIns->operand1((*i)->getReg(), NULL);

				storeIns->operandDest(reg, NULL);
			}

			// add statements for subtracting stack pointer now 
			subIns = new ArithIns(ArithIns::ArithInsType::SUB);

			// always subtract 1 from the current value of the stack pointer ?
			const Value* decrement = new Value(1, Type::TypeTag::INT);

			subIns->operand1(reg, NULL);
			subIns->operand2(-1, decrement);

			subIns->operandDest(reg, NULL);
			// print the two instructions
			storeIns->print();
			subIns->print();
		}
	}
	// movelabel instruction needed now 

	Instruction* moveLabel = new MovIns(MovIns::MovInsType::MOVL);

	const Value* label = new Value(returnLabel, Type::TypeTag::STRING);
	moveLabel -> operand1(-1, label);  // -1 indicates that no register is needed
	int nextReg = intReg(); // fetch next integer register
	moveLabel -> operandDest(nextReg, NULL);
	moveLabel->print();

	storeIns = new MovIns(MovIns::MovInsType::STI);

	storeIns->operand1(nextReg, NULL);
	storeIns->operandDest(reg, NULL);  // store value of return label to stack pointer register
	storeIns->print();

	subIns = new ArithIns(ArithIns::ArithInsType::SUB);

	const Value* decrement = new Value(1, Type::TypeTag::INT);

	subIns->operand1(reg, NULL);
	subIns->operand2(-1, decrement);

	subIns->operandDest(reg, NULL); // return address pushed.
	// need to update base pointer also ? --> yes !
	subIns->print();

	// The below instruction updates base pointer
	storeIns = new MovIns(MovIns::MovInsType::STI);
	storeIns->operand1(get_base_ptr_reg(), NULL);
	storeIns->operandDest(reg, NULL);
	storeIns->print();

	// implement jump instructions to function label now
	Instruction* jumpFunction = new JumpIns(JumpIns::JumpInsType::JMP); // normal jump
	jumpFunction->funcLabel(symTabEntry()); 
	// get me function label -- I need to store it somewhere right ? -- // must store in symbol table

	jumpFunction->print();
	// TODO : generate a labelled instruction here 

}
