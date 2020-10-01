#pragma once

#include <iostream>
#include <fstream>

#include <algorithm>
#include <cmath>

#include <string>
#include <list>
#include <vector>
#include <stack>

#include "define code.h"
#include "Structs.h"

using std::string;
using std::list;
using std::vector;
using std::stack;
using std::size_t;
using std::ifstream;
using std::cout;
using std::endl;

class MBCompiler {
private:
	ifstream file;
	string filename;

	vector<symbolLexem> _sLexems;
	list<Token> _tokens;
	vector<Rules> grammar;
	list<Atom> _atoms;
	bool _errors = false;

	int* _var_table_check; 
	string* _var_table;
	vector<string> _constants_table;
	vector<int> _lines_table;	

	stack<string> memory_expr;
	stack<string> memory;

	void createRules();
	static string pvar(int id);

	/*Lexer's methods*/
	friend void transliterator(MBCompiler&, string&);
	void tokenization();
		int identifyNewLine(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
		int identifyKeyWord(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
		int identifyVariable(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
		int identifyConst(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
		int identifyRelator(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
		int endTokenization(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
			short indexVariable(size_t& sLexem_index);
			int newToken(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
			int createNewToken(short action_code, unsigned& line, size_t& sLexem_index, size_t value);
		int errorHandling(short action_code, unsigned& line, size_t& sLexem_index, size_t value);

	/*Parser's methods*/
	int parsing();
		int lexemHandling(stack<Terminal>& store, list<Token>::iterator& it, stack<int>& cycle);
			int findLineInTable(int line);
		int Exprparsing(stack<Terminal>& store, list<Token>::iterator& it);
			int oneAction(stack<string>& operands, stack<int>& actions);
			int calculate(stack<string>& operands, stack<int>& actions);
		int atomsHandling(int type, list<Token>::iterator& it);
			int createAtom();
		int skipError(stack<Terminal>& store, list<Token>::iterator& it, int errorID);
			int createMsgError(stack<Terminal>& store, list<Token>::iterator& it, int errorID);
	int pushRule(stack<Terminal>& store, int lexemType);

	/*Code Generator's methods*/
	int codeGen();
		void header();

public:
	MBCompiler();
	~MBCompiler();

	void readingProgramm(string filename);
	void compile();

	/*Presentation*/
	void printTokens();
	void printTable();
	void printAtoms();
};