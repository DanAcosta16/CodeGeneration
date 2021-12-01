//============================================================================
// Name        : CodeGeneration.cpp
// Author      : Daniel, Guillermo
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
using namespace std;

// You will need these forward references.
class Expr;
class Stmt;
//branch comment
// Runtime Global Variables
int pc;  				// program counter
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, int> vartable; 		// map of variables and their values
vector<Stmt *> insttable; 		// table of instructions
map<string, string> symboltable; 	// map of variables to datatype (i.e. sum t_integer)


// Runtime Global Methods
void dump(){
	for(int i = 0; i < tokens.size(); i++){
		cout << tokens[i] << endl;// prints vartable, instable, symboltable
	}
	for(auto itr = symboltable.begin(); itr != symboltable.end(); ++itr){
		cout << (*itr).first << " " << (*itr).second << endl;
	}
}

// You may need a few additional global methods to manipulate the global variables


// Classes Stmt and Expr
// It is assumed some methods in statement and expression objects will change and
// you may need to add a few new ones.


class Expr{ // expressions are evaluated!
public:
	virtual int eval() = 0;
	virtual string toString() = 0;
	virtual ~Expr(){}
};

class ConstExpr : public Expr{ //Dan
private:
	int value;
public:
	ConstExpr(int val); //Dan
	int eval(); //Dan
	string toString(); //Dan
	void setValue(int v){value = v;}
	int getValue(){return value;}
};
ConstExpr::ConstExpr(int value){
	setValue(value);
}
int ConstExpr::eval(){
	return value;
}
string ConstExpr::toString(){
	string str = to_string(value);
	return str;
}
class IdExpr : public Expr{//Guille
private:
	string id;
public:
	//Guille
	IdExpr(string s){
		id = s;
	}
	//Guille
	int eval(){
		map<string,string>:: iterator it = symboltable.find(id);
		
		if (it != symboltable.end()){
			return vartable[id];
		}
		else{
			return 0;
		}
	}
	//Guille
	string toString(){
		return id;
	}
};

class InFixExpr : public Expr{
private:
	vector<Expr *> exprs;
	vector<string> ops;  // tokens of operators
public:
	~InFixExpr();
	int eval();
	string toString();
};
void InFixExpr::addConst(int value){
	Expr* ptr = new ConstExpr(value);
	exprs.push_back(ptr);
}
void InFixExpr::addId(string id){
	Expr* ptr = new IdExpr(id);
	exprs.push_back(ptr);
}
int InFixExpr::eval(){
	vector<int> values;
	int index = 0;
	int result = 0;
	for(int i=0; i < exprs.size();i++){
		while(values.size() < 2 && index < exprs.size()){
			values.push_back(exprs[index]->eval());
			index++;
		}
		if(values.size() == 2){
			if (ops[i] == "+"){
				result = values[0] + values[1];
			}
			else if(ops[i] == "-"){
				result = values[0] - values[1];
			}
			else if(ops[i] == "*"){
				result = values[0] * values[1];
			}
			else if(ops[i] == "/"){
				result = values[0] / values[1];
			}
			else if(ops[i] == "%"){
				result = values[0] % values[1];
			}
			values[0] = result;
			values.pop_back();

		}
		else{
			result = values[0];
		}
	}
	return result;

}
class Stmt{ // statements are executed!
private:
	string name;
public:
	Stmt(){}
	Stmt(string name);
	virtual ~Stmt(){};
	virtual string toString() = 0;
	virtual void execute() = 0;
	void setName(string n){name = n;}
	string getName(){return  name;}
};
Stmt::Stmt(string name){
	setName(name);
}
class AssignStmt : public Stmt{//Guille
private:
	string var;
	Expr* p_expr;
public:
	//Guille
	AssignStmt(){
		setN("s_assign");
		setVar("");
		setExpr(nullptr);
	}
	AssignStmt(Expr* p, string v){
		setN("s_assign");
		p_expr = p;
		var = v;
	}
	~AssignStmt(){
		delete p_expr;
		
		p_expr = nullptr;
	}
	string toString(){
		return var;
	}
	void execute(){
		if (p_expr->eval() == 1){
			var = p_expr->toString();
			vartable[var] = p_expr->eval();
		}
		else{
			//Raise exception
		}
	}
	void setVar(string v){
		var = v;
	}
	string getVar(){
		return var;
	}
	void setExpr(Expr* p){
		p_expr = p;
	}
	Expr* getExpr(){
		return p_expr;
	}
	void setN(string name){
		setName(name);
	}
	
};

class InputStmt : public Stmt{ //Dan
private:
	string var;
public:
	InputStmt(); //Dan
	~InputStmt(); //Dan
	string toString(); //Dan
	void execute(); //Dan
	void setN(string n){setName(n);}
	void setVar(string v){var = v;}
};
InputStmt::InputStmt(){
	setN("t_input");
	setVar("");
}
void InputStmt::execute(){
	cout << "Enter a value: ";
	cin >> var;
}
class StrOutStmt : public Stmt{ //Dan
private:
	string value;
public:
	StrOutStmt(); //Dan
	~StrOutStmt(); //Dan
	string toString(); //Dan
	void execute(); //Dan
	void setN(string n){setName(n);}
	void setValue(string v){value = v;}
};
StrOutStmt::StrOutStmt(){
	setN("t_strout");
	setValue("");
}
void StrOutStmt::execute(){
	cout << value << endl;
}
class ExprOutStmt : public Stmt{
private:
	Expr* p_expr;
public:
	ExprOutStmt(){
		p_expr = nullptr;
	}
	ExprOutStmt(Expr* p){
		p_expr = p;
	}
	~ExprOutStmt(){
		delete p_expr;
		p_expr = nullptr;
	}
	string toString(){
		return p_expr->toString();
	}
	void execute(){
		cout << p_expr-eval()<< endl;
	}
};

class IfStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget;
public:
	IfStmt(){
		name  = "t_if";
		p_expr = nullptr;
		elsetarget = 0;
	}
	IfStmt(Expr* p,int e){
		p_expr = p;
		elsetarget = e;
	}
	~IfStmt();
<<<<<<< guillegonzalezf-patch-1-1
=======

>>>>>>> master
	string toString(){
		return p_expr->toString();
	}
	void execute(){
		if(p_expr->eval()){
			int index = -1;
			bool found = false;
			for(int i=0; i<insttable.size() && !found ;i++){
				if(insttable[i] == this){
					index = i;
					found = true;
				}
			}
			insttable[index+1]->execute();

		}
		else{
			insttable[elsetarget]->execute();
		}
	}
	void setExpr(Expr* p){
		p_expr = p;
	}
	Expr* getExpr(){
		return p_expr;
	}
	void setElseTarget(int elset){
		elsetarget = elset;
	}
	int getElseTarget(){
		return elsetarget;
	}
<<<<<<< guillegonzalezf-patch-1-1
};
=======

	IfStmt(Expr* p, int e);
	string toString();
	void execute();
>>>>>>> master

class WhileStmt : public Stmt{ //Dan
private:
	Expr* p_expr;
	int elsetarget;
public:
	WhileStmt(); //Dan
	WhileStmt(Expr* p, int elsetarget);
	~WhileStmt(); //Dan
	string toString(); //Dan
	void execute(); //Dan
	void setElseTarget(int elset){
		elsetarget = elset;
	}
	int getElseTarget(){
		return elsetarget;
	}
};
WhileStmt::WhileStmt(Expr* p, int e){
	p_expr = p;
	elsetarget = e;
}

class GoToStmt: public Stmt{ //Dan
private:
	int elsetarget;
public:
	GoToStmt(); //Dan
	~GoToStmt(); //Dan
	string toString(); //Dan
	void execute(); //Dan
};

class Compiler{
private:
	ConstExpr* lastReferenced;
	IfStmt* lastIf;
	WhileStmt* lastWhile;

	int instIndex = 1;
	void buildIf(){
		string token = *tokitr;
		string lexeme = *lexitr;

		if(lastIf != nullptr){
			lastIf->setElseTarget(instIndex);
			lastIf = nullptr;

			tokitr++;
			lexitr++;
		}
		else if(lastIf == nullptr){
			tokitr++;
			lexitr++;

			token = *tokitr;
			lexeme = *lexitr;

			vector<Expr*> expressions;
			vector<string> operators;

			while(token != "t_then" && tokitr != tokens.end()){
				if(token == "t_id"){
					expressions.push_back(new IdExpr(lexeme));
				}
				else if(token == "t_int"){
					expressions.push_back(new ConstExpr(atoi(lexeme.c_str())));
				}
				else if(token == "s_gt" || token == "s_lt" || token == "s_le" ||
						token == "s_ge" || token == "s_ne" || token == "s_eq"){
					operators.push_back(lexeme);
				}
				tokitr++;
				lexitr++;
			}
			IfStmt* ifStmt = new IfStmt(new InFixExpr(expressions,operators));
			lastIf = ifStmt;

			insttable.push_back(ifStmt);
			instIndex++;
		}
	}
	void buildWhile(){
		string token = *tokitr;
		string lexeme = *lexitr;

		if(token == "t_loop"  && lastWhile != nullptr){
			lastWhile->setElseTarget(instIndex);
			lastWhile = nullptr;

			tokitr++;
			lexitr++;
		}
		else if(token == "t_while"){
			tokitr++;
			lexitr++;
			token = *tokitr;
			lexeme = *lexitr;

			vector<Expr*> expressions;
			vector<string> operators;

			while (token != "t_loop" && tokitr != tokens.end()) {
				if (token == "t_id") {
					expressions.push_back(new IdExpr(lexeme));
				} else if (token == "t_int") {
					expressions.push_back(new ConstExpr(atoi(lexeme.c_str())));
				} else if (token == "s_gt" || token == "s_lt" || token == "s_le" ||
						   token == "s_ge" || token == "s_eq" || token == "s_ne") {
					operators.push_back(lexeme);
				}

				tokitr++;
				lexitr++;
			}
			tokitr++;
			lexitr++;
		}
		WhileStmt* whileStmt = new WhileStmt(new InFixExpr(expressions, operators));
		insttable.push_back(whileStmt);
		instIndex++;
		lastWhile = whileStmt;
	}
	void buildStmt();
	void buildAssign(){
		string token = *tokitr;
		string lexeme = *lexitr;

		if(token == "s_assign"){
			tokitr++;
			lexitr++;

			token = *tokitr;
			lexeme = *lexitr;

			vector<Expr*> expressions;
			vector<string> operators;

			while(token != "s_semi"){
				if(token == "t_int"){
					int value = atoi(lexeme.c_str());

					expressions.push_back(new ConstExpr(value));
				}
				else if(token == "t_id"){
					map<string,int>::iterator it = vartable.find(lexeme);
					if(it != vartable.end()){
						expressions.push_back(new ConstExpr(it->second));
					}
				}
				else if(token == "s_plus" || token == "s_minus" || token == "s_multi" || token == "s_div" || token == "s_mod"){
					operators.push_back(lexeme);
				}
				tokitr++;
				lexitr++;
			}

			InFixExpr* infix = new InFixExpr(expressions,operators);
			insttable.push_back(new AssignStmt(lastReferenced->toString(),infix));


		}

	}
	void buildInput();
	void buildOutput(){
		string token = *tokitr;
		string lexeme = *lexitr;

		if(token == "t_output"){
			tokitr++;
			lexitr++;
			while(token == "t_end"){
				tokitr++;
				lexitr++;

				if(token == "t_id"){
					insttable.push_back(new ExprOutStmt(IdExpr(lexeme)));
					instIndex++;
				}
				else if(token == "t_str"){
					insttable.push_back(new StrOutStmt(lexeme));
					instIndex++;
				}
			}
		}
	}
	// use one of the following buildExpr methods
	void buildExpr(Expr*&);      Expr* buildExpr();

	// headers for populate methods may not change
	void populateTokenLexemes(istream& infile);
	void populateSymbolTable(istream& infile);
public:
	// headers may not change
	Compiler(istream& source, istream& symbols){
		populateTokenLexemes(source);
		populateSymbolTable(symbols);
	}

	// The compile method is responsible for getting the instruction
	// table built.  It will call the appropriate build methods.
	bool compile();

	// The run method will execute the code in the instruction table
	void run(){}
};
Expr* Compiler::buildExpr(){
	InFixExpr infix;
	tokitr++, lexitr++;
	while(*tokitr != "s_rparen"){
		if(*tokitr == "t_id"){
			infix.addId(*lexitr);
			tokitr++, lexitr++;
		}
		else if(*tokitr == "t_int"){
			infix.addConst(stoi(*lexitr));
			tokitr++, lexitr++;
		}
		else{
			infix.addOperator(*lexitr);
			tokitr++, lexitr++;
		}
	}
	Expr* ptr = &infix;
	return ptr;
}
void Compiler::buildIf(){
	tokitr++, lexitr++; //iterate to left parantheses
	Expr* exptr = buildExpr();
	tokitr++, lexitr++; //iterate past then
	Stmt* ifptr = new IfStmt(exptr, 0);
	insttable.push_back(ifptr);
}
void Compiler::buildWhile(){
	tokitr++, lexitr++;
	Expr* exptr = buildExpr();
	tokitr++, lexitr++; //iterate past loop
	Stmt* whileptr = new WhileStmt(exptr, 0);
	insttable.push_back(whileptr);
}
void Compiler::buildAssign(){

}
bool Compiler::compile(){
	while(*tokitr != "t_begin"){ //loop from beginning until begin is found
				tokitr++, lexitr++;
			}
			tokitr++, lexitr++; //iterate to next token
			while(tokitr!=tokens.end()){
				if(*tokitr == "t_if"){
					buildIf();
				}
				else if(*tokitr == "t_while"){
					buildWhile();
				}
				else if(*tokitr == "t_id"){
					buildAssign();
				}
				else if(*tokitr == "t_input"){
					buildInput();
				}
				else if(*tokitr == "t_output"){
					buildOutput();
				}
			}
}
void Compiler::populateTokenLexemes(istream& infile){
    string line, tok, lex;
    int pos;
    getline(infile, line);
    bool valid = true;
    while(!infile.eof() && (valid)){
        pos = line.find(" ");
        tok = line.substr(0, pos);
        lex = line.substr(pos+1, line.length());
        //cout << pos << " " << tok << " " << lex << endl;
        tokens.push_back(tok);
        lexemes.push_back(lex);
        getline(infile, line);
    }
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}
void Compiler::populateSymbolTable(istream& infile){
    string line, tok, symbol;
    int pos;
    getline(infile, line);
    bool valid = true;
    while(!infile.eof() && (valid)){
        pos = line.find(" ");
        tok = line.substr(0, pos);
        symbol = line.substr(pos+1, line.length());
        cout << pos << " " << tok << " " << symbol << endl;
        symboltable[tok] = symbol;
        if(tok == "t_integer"){
        	vartable[symbol] = 0;
        }
        getline(infile, line);
    }
}
int main(){
	ifstream infile1("toklex.txt");
	ifstream infile2("symboltable.txt");
	if (!infile1 || !infile2) exit(-1);
	Compiler c(infile1, infile2);
	dump();
	c.compile();
	c.run();
	return 0;
}
