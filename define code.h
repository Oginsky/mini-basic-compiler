#pragma once

/*Language's defines*/
#define TOTAL_VAR 286
#define TOTAL_TERM 4
#define TOTAL_ATOM 19
#define terminal 1
#define lexem 2
#define atom 3
#define error 4
#define expr 5

/* number class of symbol Lexems*/
#define numNewLine  	0
#define numLetter 		1
#define numDigit 		2
#define numArifOp 		3
#define numRelator 		4
#define numPoint 		5
#define numLBrac		6
#define numRbrac 		7
#define numTrash 		8
#define numLastLexem 	9

/* number class of Tokens */
#define tNewLine	  	0
#define tVar 			1
#define tArifOp 		2
#define tRelator	 	3
#define tNext	 		4
#define tLet	 		5
#define tFor			6
#define tGoto	 		7
#define tGosub	 		8
#define tLeftBrac	 	9
#define tRightBrac 		10
#define tIf	    		11
#define tReturn			12
#define tEnd			13
#define tTo				14
#define tStep			15
#define tRem			16
#define tConstant		17
#define tError			18

/* number value of Tokens */
#define tMult	1
#define tPlus	2
#define tMinus	3
#define tDel	4
#define tPow	5

/* number class of Atoma*/
#define aNumberLine		0
#define aFictStep		1 // deleted
#define aLet			2
#define aGoto			3
#define aGosub			4
#define aReturn			5
#define aIfGo			6
#define aStore			7
#define aMark			8
#define aCheck			9
#define aNext			10

#define aMult			11
#define aPlus			12
#define aMinus			13
#define aDel			14
#define aPow			15

#define aGetNext		16
#define aCut			17
#define aEnd			18

/*number of Terminals*/
#define termOperand		0
#define termLines		1
#define termStep		2
#define termExpr		3

