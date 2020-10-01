#include "MBCompiler.h"

void MBCompiler::tokenization() {
	unsigned line(0);
	size_t sLexem_index;
	short type;

	int (MBCompiler:: * transitions[10])(short, unsigned&, size_t&, size_t);
	transitions[0] = &MBCompiler::identifyNewLine;
	transitions[1] = &MBCompiler::identifyKeyWord;
	transitions[2] = &MBCompiler::identifyConst;
	transitions[3] = &MBCompiler::newToken; // Arifmetics op.
	transitions[4] = &MBCompiler::identifyRelator;
	transitions[5] = &MBCompiler::identifyConst; // Point
	transitions[6] = &MBCompiler::newToken; // (
	transitions[7] = &MBCompiler::newToken; // )
	transitions[8] = &MBCompiler::newToken; // Trash
	transitions[9] = &MBCompiler::endTokenization;

	string file_str;
	while (getline(file, file_str)) {
		transliterator(*this, file_str);
		sLexem_index = 0;
		while (_sLexems[sLexem_index].type != 9) {
			type = _sLexems[sLexem_index].type;
			(this->*transitions[type])(0, line, sLexem_index, _sLexems[sLexem_index].value);
		}
	}
}

void transliterator(MBCompiler& l, string& str) {
	unsigned index(1);
	l._sLexems.clear();
	l._sLexems.resize(str.size() + 2);
	l._sLexems[0] = symbolLexem(0);
		for (auto it: str) {
			if (std::isspace(it)) continue;
			/* LETTER */
			if (it >= 'A' && it <= 'Z')
				l._sLexems[index++] = symbolLexem(1, (int)it - 64);
			/* DIGIT */
			else if (it >= '0' && it <= '9')
				l._sLexems[index++] = symbolLexem(2, (int)it - (int)'0');
			/* ARIFMETIC */
			else if (it == '*')
				l._sLexems[index++] = symbolLexem(3, (int)it - 41);
			else if (it == '+')
				l._sLexems[index++] = symbolLexem(3, (int)it - 41);
			else if (it == '-')
				l._sLexems[index++] = symbolLexem(3, (int)it - 42);
			else if (it == '/')
				l._sLexems[index++] = symbolLexem(3, (int)it - 43);
			else if (it == '^')
				l._sLexems[index++] = symbolLexem(3, (int)it - 89);
			/* RELATIONSHIP */
			else if (it >= '<' && it <= '>')
				l._sLexems[index++] = symbolLexem(4, (int)it - 59);
			/* POINT */
			else if (it == '.')
				l._sLexems[index++] = symbolLexem(5);
			/* ( ) */
			else if (it == '(')
				l._sLexems[index++] = symbolLexem(6);
			else if (it == ')')
				l._sLexems[index++] = symbolLexem(7);

			/* TRASH */
			else l._sLexems[index++] = symbolLexem(8);
			l._sLexems[index - 1].symb = it;
		}
	l._sLexems[index].type = 9;
}

int MBCompiler::identifyNewLine(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	// action_code = 1 for GOTO or GOSUB
	if (action_code == 0) {
		line++;
		sLexem_index++;
	}
	/* Identify number new line in programm */
	int l(0);
	bool findDigit = (_sLexems[sLexem_index].type == numDigit) ? true : false;
	while (_sLexems[sLexem_index].type == numDigit)
		l = l * 10 + _sLexems[sLexem_index++].value;

	if (action_code == 1) {
		if (!findDigit) {
			errorHandling(6, line, sLexem_index, value);
			return -1;
		}
		return l;
	}
	/* Add identify number of line*/
	if (findDigit) {
		if (findLineInTable(l) != -1) {
			errorHandling(16, line, sLexem_index, value);
			return 1;
		}
		_lines_table.push_back(l);
		createNewToken(0, line, sLexem_index, _lines_table.size()-1);
	}
}
int MBCompiler::identifyKeyWord(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	int i = sLexem_index;
	const static short vec[26] = { -1, -1, -1, -1, 0, 2, 4, -1, 10, 		// A B C D E F G H I
								  -1, -1, 11, -1, 13, -1, -1, -1, 16, 	// J K L M N O P Q R  
								  22, 25, -1, -1, -1, -1, -1, -1 };  	// S T U V W X Y Z
	short res = vec[_sLexems[sLexem_index].value - 1];
	res = (res == -1) ? 26 : res;
	sLexem_index++;
	/* If can be keyWord but maybe variable */
	if (_sLexems[sLexem_index].type != numLetter) {
		identifyVariable(0, line, --sLexem_index, value);
		return 0;
	}
	/* first coln if equal; second coln if don't equal
	   26 it is error*/
	const static short tran[26][2] = { {1,26}, {26 + 13,26}, // E-ND
									  {3,26}, {26 + 6,26},   // F-OR 
									  {5,26}, {6,7}, {26 + 7,26}, // G-OTO (T or S)
									  {8,26}, {9,26}, {26 + 8,26}, // GO-SUB
									  {26 + 11,26}, // I-F
									  {12, 26}, {26 + 5,26}, // L-ET
									  {14,26}, {15, 26}, {26 + 4,26}, // N-EXT
									  {17,26}, {18,21}, {19,26}, {20,26}, {26 + 12, 26}, //R-ETURN (T or M)
									  {26 + 16, 26}, // RE-M
									  {23, 26}, {24, 26}, {26 + 15,26}, // S-TEP
									  {26 + 14, 26} }; // T-O
	const static short list_words[26] = { 14,4,15,18,15,20,15,19,21,2,6,5,20,5,24,20,5,20,21,18,14,13,20,5,16,15 };
	// N, D, O,R, O, T, O, S, U, B,F,E, T,E,X, T, E,T, U, R ,N, M, T, E, P,O
	int count = 0;
	while (res < 26) {
		count++;
		if (_sLexems[sLexem_index].value == list_words[res]) {
			res = tran[res][0];
			sLexem_index++;
		}
		else res = tran[res][1];
	}
	res -= 26;

	if (!res) {
		sLexem_index = i;
		if (_sLexems[sLexem_index + 1].type == numLetter) {
			if(vec[_sLexems[sLexem_index + 1].value-1] == -1) {
				while (_sLexems[sLexem_index].type == numLetter)
					sLexem_index++;
				errorHandling(3, line, sLexem_index, value);
				return 0;
			}
		}
		identifyVariable(0, line, sLexem_index, _sLexems[sLexem_index].value);
		return 0;
	}
	// NEXT/LET/FOR [varieble]
	if (res == 4 || res == 5 || res == 6) {
		short index = indexVariable(sLexem_index);
		if (index >= 286) errorHandling(4, line, sLexem_index, value);
		if (res == 5 || res == 6) {
			int _error = identifyRelator(1, line, sLexem_index, 0);
			if (_error != 0) {
				errorHandling(_error, line, sLexem_index, 0);
				return 0;
			}
		}
		createNewToken(res, line, sLexem_index, index);
		return 0;
	}
	// GOTO/GOSUB [number of line]
	else if (res == 7 || res == 8) {
		int index = identifyNewLine(1, line, sLexem_index, value);
		createNewToken(res, line, sLexem_index, index);
		return 0;
	}
	else if (res == 16)
		while (_sLexems[sLexem_index].type != numNewLine)
			sLexem_index++;

	createNewToken(res, line, sLexem_index, 0);
}
int MBCompiler::identifyVariable(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	short index = (_sLexems[sLexem_index].value - 1) * 11;
	if (_sLexems[sLexem_index + 1].type == numDigit) {
		index = (_sLexems[sLexem_index].value - 1) * 11 + _sLexems[sLexem_index + 1].value + 1;
		sLexem_index++;
	}

	sLexem_index++;
	createNewToken(tVar, line, sLexem_index, index);
	_var_table_check[index] = 1;
	return 0;

}
int MBCompiler::identifyConst(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	string cnst;
	while (_sLexems[sLexem_index].type == numDigit)
		cnst += _sLexems[sLexem_index++].symb;

	if (_sLexems[sLexem_index].type == numPoint) {
		cnst += _sLexems[sLexem_index++].symb;
		while (_sLexems[sLexem_index].type == numDigit)
			cnst += _sLexems[sLexem_index++].symb;
	}
	if (_sLexems[sLexem_index].type == numLetter) {
		if (_sLexems[sLexem_index].value == 'E' - 64) {
			cnst += _sLexems[sLexem_index++].symb;
			if (_sLexems[sLexem_index].type != numArifOp && _sLexems[sLexem_index].type != numDigit)
				errorHandling(7, line, sLexem_index, value);
			if (_sLexems[sLexem_index].type == numArifOp) {
				if (_sLexems[sLexem_index].value == 2 || _sLexems[sLexem_index].value == 3) {
					cnst += _sLexems[sLexem_index++].symb;
				}
				else errorHandling(7, line, sLexem_index, value);
			}
			while (_sLexems[sLexem_index].type == numDigit)
				cnst += _sLexems[sLexem_index++].symb;
		}
		else {
			_constants_table.push_back(cnst);
			createNewToken(17, line, sLexem_index, _constants_table.size()-1);
			return 0;
		}
	}
	_constants_table.push_back(cnst);
	createNewToken(17, line, sLexem_index, _constants_table.size() - 1);
}
int MBCompiler::identifyRelator(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	short j, i;
	if (_sLexems[sLexem_index].type == numRelator) {
		j = _sLexems[sLexem_index++].value;
		i = (_sLexems[sLexem_index].type == numRelator) ? _sLexems[sLexem_index++].value : 0;
	}
	else {
		j = i = 0;
		sLexem_index++;
	}
	static const short RelatorTable[4][4] =
	{ {0, 1, 2, 3},
	{0, 0, 0, 0},
	{0, 4, 0, 5},
	{0, 6, 0, 0} };

	size_t res(RelatorTable[i][j]);
	if (action_code == 1) {
		if (res == 2) return 0; // res = 2 it's =
		else return 8;
	}
	if (!res) errorHandling(5, line, sLexem_index, 0);
	else createNewToken(3, line, sLexem_index, res);
	return 0;
}

int MBCompiler::endTokenization(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	sLexem_index++;
	return 0;
}

short MBCompiler::indexVariable(size_t& sLexem_index) {
	if (_sLexems[sLexem_index].type != numLetter) return 286;


	short index = (_sLexems[sLexem_index].value - 1) * 11;
	if (_sLexems[sLexem_index + 1].type == numDigit) {
		index = (_sLexems[sLexem_index].value - 1) * 11 + _sLexems[sLexem_index + 1].value + 1;
		sLexem_index++;
	}
	sLexem_index++;
	return index;
}

int MBCompiler::newToken(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	switch (_sLexems[sLexem_index].type) {
	case 3:
		createNewToken(2, line, sLexem_index, value);
		break;
	case 6:
		createNewToken(9, line, sLexem_index, 0);
		break;
	case 7:
		createNewToken(10, line, sLexem_index, 0);
		break;
	case 8:
		createNewToken(18, line, sLexem_index, 14);
	}
	sLexem_index++;
	return 0;
}

int MBCompiler::createNewToken(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	_tokens.push_back(Token(action_code, value));
	return 0;
}

int MBCompiler::errorHandling(short action_code, unsigned& line, size_t& sLexem_index, size_t value) {
	createNewToken(18, line, sLexem_index, action_code);
	return 0;
}