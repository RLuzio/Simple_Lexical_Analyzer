#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

							  //State table is 22 wide
							  //Accept = 0, Starting = 1, Letter = 2, Digit = 3, Separators = 4, Operators = 5, Comment = 6
				              //  L, D, (, ), {, }, [, ], ,, ., :, ;, *, +, -, =, /, >, <, %, !, \s\n\t
const int state_table[][22] = {	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },		//Accepting (skip spaces)
								{ 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 1 },		//Starting state (ignore spaces)
								{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		//Letters
								{ 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		//Digits
								{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		//Separators
								{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },		//Operators
								{ 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6 }		//Comment
};
//const char char_table[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
const string KEYW[] = { "int", "float", "bool", "True", "False", "if", "else", "then", "endif", "endelse", "while", "whileend", "do", "enddo", "for", "endfor", "STDinput", "STDoutput", "and", "or", "not" };
const char SEPARATORS[] = { ';', '(', ')', '{' , '}', '[' , ']', ',', '.', ':' };
const char OPS[] = { '*', '+', '-', '/', '=', '>', '<', '%' };

bool is_keyword(string tok) {
	for (int i = 0; i < sizeof(KEYW); ++i) {
		if (tok == KEYW[i]) {
			return true;
		}
	}
	return false;
}

bool is_number(string tok) {
	if (isdigit(tok[0])) {
		return true;
	}
	return false;
}

bool is_operator(char tok) {
	for (int i = 0; i < sizeof(OPS); ++i) {
		if (tok == OPS[i]) {
			return true;
		}
	}
	return false;
}

int is_separator(char tok) {
	if (isspace(tok)) {
		return true;
	}
	for (int i = 0; i < sizeof(SEPARATORS); ++i) {
		if (tok == SEPARATORS[i]) {
			return true;
		}
	}
	return false;
}

int update_col(char t) {
	if (t == '!')				{ return 20; }
	else if (isalpha(t))		{ return 0; }
	else if (isdigit(t))		{ return 1; }
	else if (is_operator(t))	{ return 14; }
	else if (is_separator(t))	{ return 3; }
}

int main(int argc, char* argv[]) {
	int row = 0;
	bool in_comment = 0;
	int state = 1, col = 21, prev_state = 1;

	vector<vector<string>> lex_table;
	vector<string> tmp;

	string tok, line, tmptok;

	ifstream fin;
	fin.open(argv[1]);

	if (!fin.is_open()) {
		printf("Failed to open file. Please restart\n");
		fin.close();
		return 0;
	}

	while (getline(fin, line)) {
		for (int cp = 0; cp < line.length(); ++cp) {

			col = update_col(line[cp]);
			prev_state = state;
			state = state_table[state][col];

			if (state != 6 && state != 0) {
				tok += line[cp];
			}

			if (state == 0 && !tok.empty()) {
				//=====================================================
				//Is Keyword
				if (is_keyword(tok)) {
					lex_table.push_back(tmp);
					lex_table[row].push_back("Keyword");
					lex_table[row++].push_back(tok);
				}

				//Is Number
				else if (prev_state == 3) {
					lex_table.push_back(tmp);
					lex_table[row].push_back("Number");
					lex_table[row++].push_back(tok);
				}

				//Is Separator
				else if (prev_state == 4 && !isspace(tok[0])) {
					lex_table.push_back(tmp);
					lex_table[row].push_back("Separator");
					lex_table[row++].push_back(tok);
				}

				//Is Operator
				else if (prev_state == 5) {
					lex_table.push_back(tmp);
					lex_table[row].push_back("Operator");
					lex_table[row++].push_back(tok);
				}

				//Otherwise, identifier
				else if (!isspace(tok[0])) {
					lex_table.push_back(tmp);
					lex_table[row].push_back("Identifier");
					lex_table[row++].push_back(tok);
				}
				tok.clear();
				state = 1;
				--cp;
			}
		}
	}

	for (row = 0; row < lex_table.size(); ++row) {
		std::cout << setw(15) << lex_table[row][0] << setw(10) << "=\t" << lex_table[row][1] << std::endl;
	}

	fin.close();
	return 0;
}
