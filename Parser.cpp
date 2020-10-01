#pragma once

#include "MBCompiler.h"

int rule_size = 0;
int MBCompiler::pushRule(stack<Terminal>& store, int lexemType) {
	int terminalName = store.top().number;
	store.pop();

	if (grammar[terminalName][lexemType].empty())
		return 1;
	else rule_size = grammar[terminalName][lexemType].size();

	for(auto tr = grammar[terminalName][lexemType].rbegin(); tr != grammar[terminalName][lexemType].rend(); tr++)
		store.push(*tr);

	return 0;
}

int line = -1;
int relator = -1;
int MBCompiler::parsing() {
	createRules();
	list<Token>::iterator it = _tokens.begin();
	stack<Terminal> store;
	stack<int> cycle;
	store.push(Terminal(error, 13));
	store.push(Terminal(lexem, tEnd));
	store.push(Terminal(terminal, termLines));

	while (it != _tokens.end()) {
		rule_size--;
		int type = store.top().type;
		int number = store.top().number;
		switch (type) {
			case terminal:
				pushRule(store, it->type);
				break;
			case lexem:
				lexemHandling(store, it, cycle);
				store.pop();
				break;
			case atom:
				if(!_errors)atomsHandling(number,it);
				store.pop();
				break;
			case expr:
				Exprparsing(store, it);
				store.pop();
				break;
			case error:
				skipError(store, it, number);
				store.pop();
				break;
		}
	}
	if (store.top().type == lexem && store.top().number == tEnd) skipError(store, it, 15);
	_atoms.push_back(Atom(18));
	return 0;
}
	int MBCompiler::lexemHandling(stack<Terminal>& store, list<Token>::iterator& it, stack<int>& cycle) {
		static int loopID(1);
		if (it->type != store.top().number) {
			skipError(store, it, 10);
			return 1;
		}
		int index_line_table = -1;
		switch (store.top().number) {
			case tNewLine:
				line++;
				memory.push(std::to_string(_lines_table[it->value])); 
				break; 
			case tLet:
				memory.push(pvar(it->value)); 
				break;
			case tFor:
				memory.push("loop_e_" + std::to_string(loopID)); 
				memory.push("loop_s_"+std::to_string(loopID)); 
				memory.push(pvar(it->value)); 
				memory.push("loop_e_" + std::to_string(loopID)); 
				memory.push("loop_s_" + std::to_string(loopID++)); 
				memory.push(pvar(it->value)); 
				cycle.push(it->value); 
				_var_table_check[it->value] = 2; 
				break;
			case tNext:
				if (cycle.empty() || cycle.top() != it->value) { 
					skipError(store, it, 12);
					cycle.pop();
					return 1;
				}
				cycle.pop();
				break;
			case tGoto:
			case tGosub:
				index_line_table = findLineInTable(it->value);
				if (index_line_table == -1) {
					skipError(store, it, 11);
					return 1;
				}
				memory.push("L"+std::to_string(_lines_table[index_line_table])); 
				break;
			case tRelator:
				relator = it->value;
				break;
			case tError:
				skipError(store, it, it->value);
				return 0;
			case tTo:
				break;
			case tStep:
				break;
			default:
				break;
		}
		it++;
	}
	int MBCompiler::findLineInTable(int line) {
		size_t size = _lines_table.size();
		for (size_t i(0); i < size; i++)
			if (_lines_table[i] == line)
				return i;
		return -1;
	}


int MBCompiler::atomsHandling(int type, list<Token>::iterator& it) {
	Atom _atom;
	_atom.type = type;
	string expr1;
	string expr2;
	string exprStep;
	string loop_exit;
	switch (type) {
		case aNumberLine:
			_atom.attributes.push_back(memory.top());
			memory.pop();
			break;
		case aFictStep:
			memory_expr.push("0");
			return 0;
			break;
		case aLet: 
			_atom.attributes.push_back(memory.top());
			memory.pop();
			_atom.attributes.push_back(memory_expr.top());
			memory_expr.pop();
			break;
		case aGoto:
		case aGosub:
			_atom.attributes.push_back(memory.top());
			memory.pop();
			break;
		case aIfGo:
			expr2 = memory_expr.top();
			memory_expr.pop();
			expr1 = memory_expr.top();
			memory_expr.pop();
			_atom.attributes.push_back(expr1);
			_atom.attributes.push_back(expr2);
			_atom.action = relator;
			_atom.attributes.push_back(memory.top());
			memory.pop();
			break;
		case aStore:
			_atom.attributes.push_back(memory_expr.top());
			break;
		case aCheck:
			loop_exit = memory.top();
			memory.pop();
			_atom.attributes.push_back(memory.top());
			exprStep = memory_expr.top();
			memory_expr.pop();
			_atom.attributes.push_back(memory_expr.top());
			_atom.attributes.push_back(exprStep);
			memory_expr.pop();
			memory_expr.push(exprStep);
			_atom.attributes.push_back(loop_exit);
			break;
		case aGetNext:
			_atom.attributes.push_back(memory.top());
			memory.pop();
			_atom.attributes.push_back(memory_expr.top());
			memory_expr.pop();
			break;
		case aMark:
			_atom.attributes.push_back(memory.top());
			memory.pop();
			break;
	}
	_atoms.push_back(_atom);
	return 0;
}
int MBCompiler::createAtom() {

	return 0;
}
int MBCompiler::skipError(stack<Terminal>& store, list<Token>::iterator& it, int errorID) {
	createMsgError(store, it, errorID);
	_errors = true;
	do {
		it++;
	} while (it != _tokens.end() && it->type != tNewLine && it->type != tError);
	do {
		store.pop();
		rule_size--;
	} while (rule_size >= 0);
	store.push(Terminal(terminal, termLines));
	store.push(Terminal(terminal, termLines));
	return 0;
}
int MBCompiler::createMsgError(stack<Terminal>& store, list<Token>::iterator& it, int errorID) {
	string describe[19];
	describe[0] = "Новая строка";
	describe[1] = "Операнд ";
	describe[2] = "Арифметическая оп";
	describe[3] = "Знак отношения";
	describe[4] = "NEXT";
	describe[5] = "LET";
	describe[6] = "FOR";
	describe[7] = "GOTO";
	describe[8] = "GOSUB";
	describe[9] = "(";
	describe[10] = ")";
	describe[11] = "IFb";
	describe[12] = "RETURN";
	describe[13] = "END";
	describe[14] = "TO";
	describe[15] = "STEP";
	describe[16] = "REM";
	describe[17] = "Константа";
	describe[18] = "Ошибка";
	static const vector<string> er = {
		"Неопознанный символ в программе", //0
		"Отсутствует номер строки в начале", //1
		"Строка с повторяющимся номером", //2
		"Используется незарезервированное слово", //3
		"Отсутствует идентификатор переменной", //4
		"Несуществующий знак отношения", //5 
		"Отсутствует номер сроки после оператора ПЕРЕХОД", //6
		"Ошибка в записи константы", // 7
		"Отсутствует знак \"=\" после оперотора ПРИСВОИТЬ", // 8
		"Отсутсвует номер у следующий строки", // 9
		"Неожиданный вход", // 10
		"Отсутствует строка с номером указанным в операнде ПЕРЕХОД", //11
		"Ошибка закрытия цикла", //12
		"Программа после оператора END", //13
		"Неопознанный символ в программе", //14
		"Отсутствует END оператор в конце программы", //15
		"Строка с повторяющемся номером", //16
		"", //17
		"", //18
		"", //19
		"Нарешен баланс скобок в выражении", //20
		"Пропущен знак операции", //21
		"Пропущен операнд", //22
		"Два знака операции подряд", //23
		"Пустые скобки", //24
		"Отсутствует выражение" //25

	};
	if (line >= 0) cout << "Строка " << _lines_table[line] << ": ";
	cout << er[errorID];
	if (errorID == 10) cout  << " " << describe[it->type] << " вместо " << describe[store.top().number] << endl;
	else cout << endl;
	return 0;
}

int parts;
int checkExpr(list<Token>::iterator it) {
	if (it->type == tError) return it->value;
	else if (it->type == tArifOp) return 22;
	parts = -1;
	int  balans = 0;
	int prev = -1;
	bool is_expr = true;
	while (is_expr) {
		if (balans < 0) return 20;
		switch (it->type) {
			case tVar:
			case tConstant:
				if (prev == tVar || prev == tConstant || prev == tRightBrac) return 21;
				break;
			case tLeftBrac:
				balans++;
				break;
			case tRightBrac:
				if (prev == tArifOp) return 22;
				else if (prev == tLeftBrac) return 24;
				balans--;
				break;
			case tArifOp:
				if (prev == tLeftBrac) return 22;
				else if (prev == tArifOp) return 23;
				break;
			default:
				is_expr = false;
				break;
		}
		prev = (is_expr) ? it->type : prev;
		it++;
		parts++;
	}
	it--;
	if (prev == tArifOp) return 22;
	else if (balans > 0)return 20;
	else if (parts == 0) return 25;
	return 0;
}
int MBCompiler::oneAction(stack<string>& operands, stack<int>& actions) {
	static int tmpID(1);
	Atom nA;
	nA.type = 10 + actions.top();
	nA.action = actions.top();
	actions.pop();
	string op1 = operands.top();
	operands.pop();
	nA.attributes.push_back(operands.top());
	operands.pop();
	nA.attributes.push_back(op1);
	operands.push("TMP_" + std::to_string(tmpID++));
	nA.attributes.push_back(operands.top());
	_atoms.push_back(nA);
	return 0;
}
int MBCompiler::calculate(stack<string>& operands, stack<int>& actions) {
	while (actions.top() != tLeftBrac) oneAction(operands, actions);
	actions.pop();
	return 0;
}
int getOrder(int code) {
	static const int orders[6] = {0, 2, 1, 1, 2, 3 };
	return (code == tLeftBrac) ? 0 : orders[code];
}
int MBCompiler::Exprparsing(stack<Terminal>& store, list<Token>::iterator& it) {
	int chk = checkExpr(it);
	if (chk != 0) {
		skipError(store, it, chk);
		return 1;
	}
	stack<string> operands;
	stack<int> actions;
	int p;

	while (parts--) {
		switch (it->type) {
			case tConstant:
				operands.push(_constants_table[it->value]);
				break;
			case tVar:
				operands.push(pvar(it->value));
				break;
			case tArifOp:
				p = getOrder(it->value);
				if (actions.empty()) actions.push(it->value);
				else if (p > getOrder(actions.top())) actions.push(it->value);
				else {
					oneAction(operands, actions);
					actions.push(it->value);
				}
				break;
			case tLeftBrac:
				actions.push(tLeftBrac);
				break;
			case tRightBrac:
				calculate(operands, actions);
				break;
		}
		it++;
	}
	while (!actions.empty()) oneAction(operands, actions);
	memory_expr.push(operands.top());
	return 0;
}