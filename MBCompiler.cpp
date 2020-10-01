#include "MBCompiler.h"

MBCompiler::MBCompiler() {
	_var_table_check = new int[TOTAL_VAR];
	_var_table = new string[TOTAL_VAR];
	for (int i = 0; i < TOTAL_VAR; i++)
		_var_table_check[i] = 0, _var_table[i] = pvar(i);
}
MBCompiler::~MBCompiler() {

}

void MBCompiler::createRules() {
	vector<Terminal> LINE_ERROR = { Terminal(error, 1) };
	grammar.resize(TOTAL_TERM);
	for (int i = 0; i < TOTAL_TERM; i++) {
		if (i == termLines) grammar[i].resize(19, LINE_ERROR);
		else grammar[i].resize(19);
		
	}
	
	vector<Terminal> LET = { Terminal(lexem, tLet), Terminal(expr, termExpr), Terminal(atom, aLet), Terminal(terminal, termLines) };
	vector<Terminal> GOTO = { Terminal(lexem, tGoto), Terminal(atom, aGoto), Terminal(terminal, termLines) };
	vector<Terminal> IF = { Terminal(lexem, tIf), Terminal(expr, termExpr), Terminal(lexem, tRelator), Terminal(expr, termExpr),
							Terminal(lexem, tGoto), Terminal(atom, aIfGo), Terminal(terminal, termLines) };
	vector<Terminal> GOSUB = { Terminal(lexem, tGosub), Terminal(atom, aGosub), Terminal(terminal, termLines) };
	vector<Terminal> RETURN = { Terminal(lexem, tReturn), Terminal(atom, aReturn), Terminal(terminal, termLines) };
	vector<Terminal> FOR = { Terminal(lexem, tFor), Terminal(expr, termExpr), Terminal(atom, aLet), Terminal(lexem, tTo), Terminal(expr, termExpr),
							 Terminal(atom, aStore), Terminal(terminal, termStep), Terminal(atom, aMark), Terminal(atom, aCheck),
							Terminal(terminal, termLines), Terminal(lexem, tNext), Terminal(atom, aGetNext),
							Terminal(atom, aGoto), Terminal(atom, aMark), Terminal(terminal, termLines) };
	vector<Terminal> REM = { Terminal(lexem, tRem), Terminal(terminal, termLines) };
	grammar[termOperand][tLet] = LET;
	grammar[termOperand][tFor] = FOR;
	grammar[termOperand][tGoto] = GOTO;
	grammar[termOperand][tGosub] = GOSUB;
	grammar[termOperand][tIf] = IF;
	grammar[termOperand][tReturn] = RETURN;
	grammar[termOperand][tRem] = REM;
	
	vector<Terminal> LINES = { Terminal(lexem, tNewLine), Terminal(atom, aNumberLine), Terminal(terminal, termOperand) };
	grammar[termLines][tNewLine] = LINES;

	vector<Terminal> STEP = { Terminal(lexem, tStep), Terminal(expr, termExpr), Terminal(atom, aStore) };
	vector<Terminal> FICTSTEP = { Terminal(atom, aFictStep) };
	for (int i = 0; i < grammar[termStep].size(); i++) grammar[termStep][i] = FICTSTEP;
	grammar[termStep][tStep] = STEP;


	vector<Terminal> TOKENERROR = { Terminal(lexem, tError) };
	for (int i = 0; i < grammar.size(); i++) grammar[i][tError] = TOKENERROR;

}

void MBCompiler::readingProgramm(string filename) {
	file.open(filename+".txt");
	this->filename = filename;
}
void MBCompiler::compile() {
	tokenization();
	parsing();
	if(!_errors)codeGen();
}

string MBCompiler::pvar(int id) {
	string res;
	res.push_back((char)(id / 11 + 65));
	if (id % 11 != 0) res.push_back((char)(id % 11 + 47));
	return res;
}
/*Presentations*/
void MBCompiler::printTokens() {
	string des_relator[7] = { "", "<", "=", ">", "<=", ">=", "<>" };
	string des_arifOp[6] = { "", "*", "+", "-", "/", "^" };
	string describe[19];
	describe[0] = "����� ������  | ";
	describe[1] = "�������           | ";
	describe[2] = "�������������� �� | ";
	describe[3] = "���� ���������    | ";
	describe[4] = "����� �����       | NEXT     | ";
	describe[5] = "���������         | LET      | ";
	describe[6] = "���               | FOR      | ";
	describe[7] = "�������           | GOTO     | ";
	describe[8] = "������������      | GOSUB    | ";
	describe[9] = "     (            | (";
	describe[10] = "    )             | )";
	describe[11] = "����              | IF";
	describe[12] = "�������           | RETURN";
	describe[13] = "�����             | END";
	describe[14] = "��                | TO";
	describe[15] = "���               | STEP";
	describe[16] = "�����������       | REM";
	describe[17] = "���������         | ";
	describe[18] = "������            | ";

	list<Token>::iterator it = _tokens.begin();
	while (it != _tokens.end()) {
		std::cout << describe[it->type];
		switch (it->type) {
		case tGoto:
		case tGosub:
			std::cout << it->value;
			break;
		case tRelator:
			std::cout << des_relator[it->value];
			break;
		case tArifOp:
			std::cout << des_arifOp[it->value];
			break;
		case tConstant:
			std::cout << _constants_table[it->value];
			break;
		case tNewLine:
			std::cout << _lines_table[it->value];
			break;
		case tVar:
		case tNext:
			cout << pvar(it->value);
			break;
		case tFor:
		case tLet:
			cout << pvar(it->value);
			std::cout << " | =" << std::endl;
			break;
		case tError:
			std::cout << it->value;
			break;
		default:
			break;
		}
		it++;
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
void MBCompiler::printTable() {
	std::cout << " ������� ���� |\n";
	std::cout << "--------------|\n"
		<< "  ����������  |\n"
		<< "--------------|\n";
	for (int i = 0; i < TOTAL_VAR; i++) {
		if (_var_table_check[i]) {
			std::cout << "     ";
			pvar(i);
			if (i < 26) std::cout << "        |";
			else 	   std::cout << "       |";
			cout << endl;
		}
	}
	std::cout << "--------------|\n"
		<< "  ���������   |\n"
		<< "--------------|\n";
	vector<string>::iterator it = _constants_table.begin();
	while (it != _constants_table.end()) {
		std::cout << "      " << *it << std::endl;
		it++;
	}
	std::cout << "--------------|\n"
		<< "    ������    |\n"
		<< "--------------|\n";
	for (int i = 0; i < _lines_table.size(); i++)
		std::cout << "      " << _lines_table[i] << std::endl;
}
void MBCompiler::printAtoms() {
	if (_errors) return;
	string des_relator[7] = { "", "<", "=", ">", "<=", ">=", "<>" };
	static const vector<string> at = {
		"���������",
		"����������������",
		"���������",
		"�������",
		"�����������",
		"�����������",
		"����������",
		"�������",
		"�����",
		"��������",
		"���������",
		"��������",
		"�������",
		"�����",
		"������",
		"�������"
		"",
		"���������",
		"",
		"���������"
	};
	list<Atom>::iterator it = _atoms.begin();
	while (it != _atoms.end()) {
		cout << at[it->type] << " ";
		for (int i = 0; i < it->attributes.size(); i++)
			cout << it->attributes[i] << " ";
		if (it->type == aIfGo) cout << des_relator[it->action];
		cout << endl;
		it++;
	}
}