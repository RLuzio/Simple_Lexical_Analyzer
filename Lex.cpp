#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

							  //State table is 22 wide
						      //Accept = 0, Starting = 1, Letter = 2, Digit = 3, Separators = 4, Operators = 5, Comment = 6
							  //  L, D, (, ), {, }, [, ], ,, ., :, ;, *, +, -, =, /, >, <, %, !, \s\n\t, $, _
const int state_table[][24] = { { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },		//Accepting (skip spaces)
				{ 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 1, 999, 999 },		//Starting state (ignore spaces)
				{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2 },		//Letters
				{ 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 999, 999 },		//Digits
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 999, 999 },		//Separators
				{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 999, 999 },		//Operators
				{ 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 6, 999, 999 }		//Comment
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

bool is_separator(char tok) {
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
	int col = 100;

	if (t == '!') { col = 20; }
	else if (isalpha(t)) { col = 0; }
	else if (isdigit(t)) { col = 1; }
	else if (is_operator(t)) { col = 14; }
	else if (is_separator(t)) { col = 3; }
	else if (t == '$') { col = 22; }
	else if (t == '_') { col = 23;  }

	return col;
}

int main(/*int argc, char* argv[]*/) {
	int state = 1, row = 0, col = 21, prev_state = 1, table_size = 0;

	vector<vector<string>> lex_table;
	vector<string> tmp;

	string tok, line, file;

	cout << "Enter a file to lexically analyze: ";
	cin >> file;
		
	ifstream fin;
	fin.open(file);

	if (!fin.is_open()) {
		printf("Failed to open file. Please restart\n");
		fin.close();
		return 0;
	}

	while (getline(fin, line)) {
		//tok.clear();
		//state = 1;
		for (int cp = 0; cp < line.length(); ++cp) {
			col = update_col(line[cp]);
			if(col == 100) { 
				cout << "INVALID CHARACTER FOUND: " << line[cp] << endl;
				cout << "EXITING!" << endl;
				return 0;
			}

			prev_state = state;
			state = state_table[state][col];

			if (state == 999) {
				cout << "INVALID CHARACTER FOUND: " << line[cp] << endl;
				cout << "EXITING!" << endl;
				return 0;
			}

			else if (state != 6 && state != 0) {
				tok += line[cp];
			}

			if (state == 4) {
				state = 0;
				prev_state = 4;
			}
			else if (state == 5) {
				state = 0;
				prev_state = 5;
			}


			if (state == 0 && !tok.empty()) {
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

				if (!isspace(line[cp]) && prev_state != 4 && prev_state != 5) {
					--cp;
				}
			}
		}
	}

	for (int i = 0; i < lex_table.size(); ++i) {
		std::cout << setw(15) << lex_table[i][0] << setw(10) << "=\t" << lex_table[i][1] << std::endl;
	}

	fin.close();
	return 0;
}
