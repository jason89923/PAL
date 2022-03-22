#include <stdio.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

class END_OF_FILE {
};

class QUIT {
};

enum TokenType {
    DIGIT = 0,
    DELIMITER = 1,
    IDENTIFIER = 2,
    BOOLEAN = 3
};

typedef vector<string> VS_vector;

class Rule {
   public:
    Rule(vector<VS_vector> rule) {
    }
};

class GrammerChecker {
   private:
    map<string, Rule*> mRuleMap;

    void BuildRule1(vector<VS_vector>& rule1) {
        vector<string> subRule;
        subRule.push_back("=");
        subRule.push_back("<>");
        subRule.push_back(">");
        subRule.push_back("<");
        subRule.push_back(">=");
        subRule.push_back("<=");
        rule1.push_back(subRule);
    }

   public:
    GrammerChecker() {
        vector<VS_vector> rule1;
        BuildRule1(rule1);
    }
};

class TokenGetter {
   private:
    static map<string, int> sDelimiters;
    string mBufferedToken;

   public:
    TokenGetter() {
        string delimiters[] = {"+", "-", "*", "/", "=", ">", "<", "(", ")", ";", ":=", "<>", ">=", "<="};
        for (int i = 0; i < 14; i++) {
            sDelimiters[delimiters[i]] = i;
        }
    }

    void Flush() {
        char currentChar = cin.get();
        while (currentChar != '\n') {
            if (currentChar == EOF) {
                throw END_OF_FILE();
            }
            currentChar = cin.get();
        }
    }

    string GetNextToken() {
        if (mBufferedToken.length() > 0) {
            string temp = mBufferedToken;
            mBufferedToken.clear();
            return temp;
        }
        string token;
        char currentChar = '\0', nextChar = '\0';
        bool passingComment = false;
        while (token.length() == 0) {
            currentChar = cin.get();
            nextChar = cin.peek();
            while (((currentChar != ' ' && currentChar != '\n' && currentChar != '\t') || passingComment) && currentChar != EOF) {
                if (passingComment) {
                    if (currentChar == '\n')
                        passingComment = false;
                } else {
                    if (currentChar == '/' && nextChar == '/')
                        passingComment = true;
                    else {
                        string currentString;
                        string nextString;
                        currentString.push_back(currentChar);
                        nextString.push_back(nextChar);
                        if (sDelimiters.find(currentString) != sDelimiters.end()) {
                            if (token.length() > 0) {
                                mBufferedToken = currentString;
                                return token;
                            } else
                                return currentString;
                        } else if (sDelimiters.find(currentString + nextString) != sDelimiters.end()) {
                            cin.get();
                            if (token.length() > 0) {
                                mBufferedToken = currentString + nextString;
                                return token;
                            } else
                                return currentString + nextString;
                        } else
                            token.push_back(currentChar);
                    }
                }

                currentChar = cin.get();
                nextChar = cin.peek();
            }

            if (currentChar == EOF) {
                throw END_OF_FILE();
            }
        }

        return token;
    }

    friend class Token;
};

map<string, int> TokenGetter::sDelimiters;

class Token {
   private:
    bool IsDigit(const string& str) {
        int numOfDecimalPoint = 0;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == '.') {
                numOfDecimalPoint++;
            }

            if (((str[i] < '0' || str[i] > '9') && str[i] != '.') || numOfDecimalPoint > 1) {
                return false;
            }
        }

        return true;
    }

    bool IsBoolean(const string& str) {
        if (str == "true" || str == "false") {
            return true;
        }

        return false;
    }

    bool IsLeagleIdentifier(const string& str) {
        if ((str[0] < 'a' || str[0] > 'z') && (str[0] < 'A' || str[0] > 'Z')) {
            return false;
        }

        for (int i = 0; i < str.length(); i++) {
            if ((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') && str[i] != '_') {
                return false;
            }
        }

        return true;
    }

   public:
    TokenType mType;
    string mString;

    Token(const string& str) {
        mString = str;
        if (TokenGetter::sDelimiters.find(mString) != TokenGetter::sDelimiters.end()) {
            mType = DELIMITER;
        } else if (IsDigit(mString)) {
            mType = DIGIT;
        } else if (IsBoolean(mString)) {
            mType = BOOLEAN;
        } else if (IsLeagleIdentifier(mString)) {
            mType = IDENTIFIER;
        } else {
            stringstream msg;
            msg << "Unrecognized token with first char : '" << mString[0] << "'";
            throw logic_error(msg.str().c_str());
        }
    }
};

class StatementAssembler {
   private:
    TokenGetter mTokenGetter;

   public:
    void GetNextStatement(vector<Token>& statement) {
        statement.clear();
        while (statement.size() == 0) {
            try {
                string token = mTokenGetter.GetNextToken();
                if (token == "quit") {
                    throw QUIT();
                }

                while (token != ";") {
                    Token temp = token;
                    statement.push_back(temp);
                    token = mTokenGetter.GetNextToken();
                }

                Token temp = token;
                statement.push_back(temp);
            } catch (const logic_error& err) {
                cout << err.what() << endl;
                statement.clear();
                mTokenGetter.Flush();
            }
        }
    }
};

class Interpreter {
   public:
    Interpreter() {
        StatementAssembler statementAssembler;
        vector<Token> statement;
        try {
            while (true) {
                statementAssembler.GetNextStatement(statement);
                for (int i = 0; i < statement.size(); i++) {
                    cout << statement[i].mString << " ";
                }

                cout << endl;
            }
        } catch (const END_OF_FILE& err) {
            cout << "reached end of file" << endl;
        } catch (const QUIT& quit) {
            cout << "Program exits..." << endl;
        }
    }
};

int main() {
    ifstream in("file.txt");
    cin.rdbuf(in.rdbuf());

    Interpreter interpreter;
}