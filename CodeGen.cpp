#pragma once

#include "MBCompiler.h"
#include <functional>

#define NL "\n"
#define TAB "	"

std::ofstream assembler;
string VAR;
bool isFirstExpr = true;

void MBCompiler::header() {
	assembler << "format pe" << NL;
	assembler << "entry _start" << NL;
	assembler << "_nametable:" << NL;
	for (int i = 0; i < TOTAL_VAR; i++) {
		if (_var_table_check[i] == 1)
			assembler << TAB << _var_table[i] << " rw " << 1 << NL
					  << TAB << "mov [" << _var_table[i] << "], " << 0 << NL;
		if (_var_table_check[i] == 2)
			assembler << TAB << _var_table[i] << " rw " << 1 << NL
					  << TAB << "mov [" << _var_table[i] << "], " << 0 << NL,
			assembler << TAB << "STP_" << _var_table[i] << " rw " << 1 << NL
					  << TAB << "mov [STP_" << _var_table[i] << "], " << 0 << NL,
			assembler << TAB << "TO_" << _var_table[i] << " rw " << 1 << NL
					  << TAB << "mov [TO_" << _var_table[i] << "], " << 0 << NL;
	}
	assembler << "_start:" << NL;
}
void end(const Atom& _atom) {
	assembler << TAB << "xor eax, eax" << NL;
	assembler << TAB << "mov eax, 1" << NL;
	assembler << TAB << "int 0x80" << NL;
}
void footer(const Atom& _atom) {
	assembler << "_exit:" << NL;
	end(_atom);
}

void clearRegisters() {
	assembler << TAB << "xor eax, aex" << NL;
	assembler << TAB << "xor ebx, ebx" << NL;
}
void check(const Atom& _atom) {
	assembler << TAB << "mov ax, " << _atom.attributes[0] << NL;
	assembler << TAB << "mov bx, " << "TO_" << _atom.attributes[0] << NL;
	assembler << TAB << "cmp ax, bx" << NL;
	assembler << TAB << "jge " << _atom.attributes[3] << NL;
}
void store(const Atom& _atom) {
	static bool isTo = true;
	string op = _atom.attributes[0];
	if (_atom.attributes[0][0] == 'T' && _atom.attributes[0].size() > 1) assembler << TAB << "pop ax" << NL, op = "ax";
	if (isTo) assembler << TAB << "mov [TO_" << VAR << "], " << op << NL, isTo = false;
	else assembler << TAB << "mov [STP_" << VAR << "], " << op << NL, isTo = true;
}
void nextStep(const Atom& _atom) {
	assembler << TAB << "mov ax, " << _atom.attributes[0] << NL;
	assembler << TAB << "mov bx, " << "STP_" << _atom.attributes[0] << NL;
	assembler << TAB << "add ax, bx" << NL;
	assembler << TAB << "mov [" << _atom.attributes[0] << "], ax" << NL;
}

void newLine(const Atom& _atom) {
	assembler << "L" << _atom.attributes[0] << ":" << NL;
}
string operand(const string& op, string reg) {
	return (op[0] == 'T' && op.size() > 1) ? ("pop "+reg) : ("mov " + reg + ", " + op);
}
void Expr(const Atom& _atom) {
	if (isFirstExpr) clearRegisters(), isFirstExpr = false;
	static const vector<string> operations = { "imul", "add", "sub", "idiv", "pow" };
	assembler << TAB << operand(_atom.attributes[0], "ax") << NL;
	assembler << TAB << operand(_atom.attributes[1], "bx") << NL;
	assembler << TAB << operations[_atom.action-1] << " ax, bx" << NL;
	assembler << TAB << "push ax" << NL;
}
void let(const Atom& _atom) {
	if (_atom.attributes[1][0] == 'T' && _atom.attributes[1].size() > 1)
		assembler << TAB << "pop ax" << NL << TAB << "mov [" << _atom.attributes[0] << "], ax" << NL;
	else
		assembler << TAB << "mov [" << _atom.attributes[0] << "], " << _atom.attributes[1] << NL;
	isFirstExpr = true;
	VAR = _atom.attributes[0];
}
void ifGoto(const Atom& _atom) {
	static const vector<string> jmp = { "jl", "je", "jg", "jle", "jge", "jne" };
	assembler << TAB << operand(_atom.attributes[0], "bx") << NL;
	assembler << TAB << operand(_atom.attributes[1], "ax") << NL;
	assembler << TAB << "cmp ax, bx" << NL;
	assembler << TAB << jmp[_atom.action - 1] << " " << _atom.attributes[2] << NL;
}
void Goto(const Atom& _atom) {
	assembler << TAB << "jmp " << _atom.attributes[0] << NL;
}
void loop(const Atom& _atom) {
	assembler << TAB << _atom.attributes[0] << ":" << NL;
}

int MBCompiler::codeGen() {
	assembler.open(filename + ".fasm");
	list<Atom>::const_iterator it = _atoms.begin();
	header();

	vector<std::function<void(const Atom&)> > command(TOTAL_ATOM);
	command[aNumberLine] = newLine;
	command[aLet] = let;
	command[aPlus] = command[aMinus] = command[aMult] = command[aDel] = command[aPow] = Expr;
	command[aIfGo] = ifGoto;
	command[aGoto] = command[aGosub] = Goto;
	command[aReturn] = end;
	command[aMark] = loop;
	command[aStore] = store;
	command[aCheck] = check;
	command[aGetNext] = nextStep;
	command[aEnd] = footer;

	for (const auto& it : _atoms)
		command[it.type](it);

	return 0;
}