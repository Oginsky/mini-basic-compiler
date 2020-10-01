#pragma once

#include <vector>
#include <string>
using std::vector;
using std::string;

struct symbolLexem {
	short type;
	short value;
	char symb;

	symbolLexem() {
		type = -1;
		value = 0;
	}
	symbolLexem(short t, short v = 0) {
		type = t;
		value = v;
	}
};

struct Token {
	int type;
	size_t value;

	Token() {
		type = -1;
		value = 0;
	}
	Token(short t, size_t v) {
		type = t;
		value = v;
	}
};

struct Terminal {
	int type = -1;
	int number = -1;

	Terminal() {

	}

	Terminal(int t, int n) {
		type = t;
		number = n;
	}
};

typedef vector<Terminal> Rule;
typedef vector<Rule> Rules;

struct Atom {
	int type = -1;
	int action;
	vector<string> attributes;

	Atom() {

	}
	Atom(int type) {
		this->type = type;
	}

	Atom(int t, vector<string> attr) {
		type = t;
		attributes = attr;
	}
};