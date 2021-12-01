//============================================================================
// Name        : CodeGeneration.cpp
// Author      : 
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
			return 1;
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
	string toString();
	void execute(){
		if (p_expr->eval() == 1){
			var = p_expr->toString();
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
	ExprOutStmt();
	~ExprOutStmt();
	string toString();
	void execute();
};

class IfStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget;
public:
	IfStmt();
	~IfStmt();
	IfStmt(Expr* p, int e);
	string toString();
	void execute();
};
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
	void buildIf();
	void buildWhile();
	void buildStmt();
	void buildAssign();
	void buildInput();
	void buildOutput();
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
