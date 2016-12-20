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

void VariableEntry::codeGen(){





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
