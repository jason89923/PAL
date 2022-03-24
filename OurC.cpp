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

class Partially_Correct {
};

class QUIT {
};

enum TokenType {
    DIGIT = 0,
    DELIMITER = 1,
    IDENTIFIER = 2,
    BOOLEAN = 3
};

class TokenGetter {
   private:
    static map<string, int> sDelimiters;
    string mBufferedToken;
    char mBufferedChar;
    bool mFlag;

    static bool IsDigit(const string& str) {
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

    static bool IsBoolean(const string& str) {
        if (str == "true" || str == "false") {
            return true;
        }

        return false;
    }

    static bool IsLeagleIdentifier(const string& str) {
        if ((str[0] < 'a' || str[0] > 'z') && (str[0] < 'A' || str[0] > 'Z')) {
            return false;
        }

        for (int i = 0; i < str.length(); i++) {
            if ((str[i] < '0' || str[i] > '9') && (str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') && str[i] != '_') {
                return false;
            }
        }

        return true;
    }

   public:
    TokenGetter() {
        mFlag = false;
        mBufferedChar = '\0';
        string delimiters[] = {"+", "-", "*", "/", "=", ">", "<", "(", ")", ";", ":=", "<>", ">=", "<="};
        for (int i = 0; i < 14; i++) {
            sDelimiters[delimiters[i]] = i;
        }
    }

    void Flush() {
        mBufferedToken.clear();
        mBufferedChar = '\0';
        if (!mFlag) {
            char currentChar = cin.get();
            while (currentChar != '\n') {
                if (currentChar == EOF) {
                    throw END_OF_FILE();
                }
                currentChar = cin.get();
            }
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
            if (mBufferedChar != '\0') {
                currentChar = mBufferedChar;
                mBufferedChar = '\0';
            } else {
                currentChar = cin.get();
            }

            nextChar = cin.peek();
            while (((currentChar != ' ' && currentChar != '\n' && currentChar != '\t') || passingComment) && currentChar != EOF) {
                if (passingComment) {
                    if (currentChar == '\n')
                        passingComment = false;
                } else {
                    if (currentChar == '\n') {
                        mFlag = true;
                    } else {
                        mFlag = false;
                    }

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
                        } else {
                            if (IsDigit(token + currentString) || IsBoolean(token + currentString) || IsLeagleIdentifier(token + currentString)) {
                                token.push_back(currentChar);
                            } else {
                                if (token.length() > 0) {
                                    mBufferedChar = currentChar;
                                    return token;
                                } else {
                                    return currentString;
                                }
                            }
                        }
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
   public:
    TokenType mType;
    string mString;

    Token(const string& str) {
        mString = str;
        if (TokenGetter::sDelimiters.find(mString) != TokenGetter::sDelimiters.end()) {
            mType = DELIMITER;
        } else if (TokenGetter::IsDigit(mString)) {
            mType = DIGIT;
        } else if (TokenGetter::IsBoolean(mString)) {
            mType = BOOLEAN;
        } else if (TokenGetter::IsLeagleIdentifier(mString)) {
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

class GrammerChecker {
   private:
    map<string, string> mRuleMap;
    map<char, char> mPair;

    vector<string>* GetToken(const string& grammer) {
        if (grammer.length() == 0) {
            return NULL;
        }

        vector<string>* grammerQueue = new vector<string>();
        string buffer;
        int levelCounter = 0;
        bool isGroup = false;
        map<char, char>::iterator iter;
        char previousChar = ' ';
        for (int i = 0; i < grammer.length(); i++) {
            if (!isGroup) {
                if (previousChar != '\'') {
                    iter = mPair.find(grammer[i]);
                    if (iter != mPair.end()) {
                        isGroup = true;
                        levelCounter++;
                    }
                }
            } else {
                if (grammer[i] == iter->first) {
                    levelCounter++;
                } else if (grammer[i] == iter->second) {
                    levelCounter--;
                }

                if (levelCounter == 0) {
                    isGroup = false;
                }
            }

            if (grammer[i] == ' ' && levelCounter == 0) {
                if (buffer.length() > 0) {
                    grammerQueue->push_back(buffer);
                    buffer.clear();
                }
            } else {
                buffer.push_back(grammer[i]);
            }

            previousChar = grammer[i];
        }

        if (buffer.length() > 0) {
            grammerQueue->push_back(buffer);
        }

        return grammerQueue;
    }

    bool Compare(vector<Token>& instructionQueue, const string& grammer) {
        if (grammer[0] == '{' || grammer[0] == '[' || grammer[0] == '(' || grammer[0] == '<') {
            int previousSize = instructionQueue.size();
            bool ifSuccess = Check(instructionQueue, grammer.substr(1, grammer.length() - 2));
            if (grammer[0] == '(' || grammer[0] == '<') {
                mErrorToken = instructionQueue[0].mString;
                return ifSuccess;
            } else if (grammer[0] == '[') {
                mErrorToken = instructionQueue[0].mString;
                int currentSize = instructionQueue.size();
                return (ifSuccess || previousSize == currentSize);
            } else {
                while (ifSuccess) {
                    previousSize = instructionQueue.size();
                    ifSuccess = Check(instructionQueue, grammer.substr(1, grammer.length() - 2));
                }

                mErrorToken = instructionQueue[0].mString;
                int currentSize = instructionQueue.size();
                return previousSize == currentSize;
            }
        } else {
            if (instructionQueue.size() == 0) {
                throw Partially_Correct();
            }

            Token token = instructionQueue[0];
            string literal = "'" + token.mString + "'";
            if ((grammer == "IDENT" && token.mType == IDENTIFIER) || (grammer == "NUM" && token.mType == DIGIT) || (grammer == "SIGN" && (token.mString == "+" || token.mString == "-")) || (literal == grammer)) {
                instructionQueue.erase(instructionQueue.begin());
                return true;
            } else {
                return false;
            }
        }
    }

    bool Check(vector<Token>& instructionQueue, const string& grammer) {
        map<string, string>::iterator iter = mRuleMap.find(grammer);
        vector<string>* grammerQueue = NULL;
        if (iter != mRuleMap.end()) {
            grammerQueue = GetToken(iter->second);
        } else {
            grammerQueue = GetToken(grammer);
        }

        bool isORRelation = false;
        for (int i = 0; i < grammerQueue->size(); i++) {
            if (grammerQueue->at(i) == "|") {
                grammerQueue->erase(grammerQueue->begin() + i);
                isORRelation = true;
                i--;
            }
        }

        if (isORRelation) {
            for (int i = 0; i < grammerQueue->size(); i++) {
                int previousSize = instructionQueue.size();
                bool isCorrect = Compare(instructionQueue, grammerQueue->at(i));
                int currentSize = instructionQueue.size();
                if (isCorrect) {
                    return true;
                } else {
                    if (previousSize != currentSize) {
                        return false;
                    }
                }
            }

            return false;
        } else {
            for (int i = 0; i < grammerQueue->size(); i++) {
                if (!Compare(instructionQueue, grammerQueue->at(i))) {
                    return false;
                }
            }

            return true;
        }

        delete grammerQueue;
    }

   public:
    GrammerChecker() {
        mRuleMap["Command"] = "( IDENT ( ( ':=' <ArithExp> ) | <IDlessArithExpOrBexp> ) ';') | ( <NOT_ID_StartArithExpOrBexp> ';' )";
        mRuleMap["IDlessArithExpOrBexp"] = "{ ( '+' <Term> ) | ( '-' <Term> ) | ( '*' <Factor> ) | ( '/' <Factor> ) } [ <BooleanOperator> <ArithExp> ]";
        mRuleMap["BooleanOperator"] = "'=' | '<>' | '>' | '<' | '>=' | '<='";
        mRuleMap["NOT_ID_StartArithExpOrBexp"] = "<NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]";
        mRuleMap["NOT_ID_StartArithExp"] = "<NOT_ID_StartTerm> { ( '+' <Term> ) | ( '-' <Term> ) }";
        mRuleMap["NOT_ID_StartTerm"] = "<NOT_ID_StartFactor> { ( '*' <Factor> ) | ( '/' <actor> ) }";
        mRuleMap["NOT_ID_StartFactor"] = "( [ SIGN ] NUM ) | ( '(' <ArithExp> ')' )";
        mRuleMap["ArithExp"] = "<Term> { ( '+' <Term> ) | ( '-' <Term> ) }";
        mRuleMap["Term"] = "<Factor> { ( '*' <Factor> ) | ( '/' <Factor> ) }";
        mRuleMap["Factor"] = "IDENT | ( [ SIGN ] NUM ) | ( '(' <ArithExp> ')' )";
        mPair['{'] = '}';
        mPair['['] = ']';
        mPair['('] = ')';
        mPair['<'] = '>';
    }

    bool Check(const vector<Token>& statement) {
        vector<Token> instructionQueue = statement;
        try {
            return Check(instructionQueue, "Command");
        } catch (const Partially_Correct& e) {
            return true;
        }
    }

    string mErrorToken;
};

class Interpreter {
   public:
    Interpreter() {
        StatementAssembler mStatementAssembler;
        GrammerChecker mGrammerChecker;
        vector<Token> statement;
        try {
            while (true) {
                mStatementAssembler.GetNextStatement(statement);
                for (int i = 0; i < statement.size(); i++) {
                    cout << statement[i].mString << " ";
                }

                if (mGrammerChecker.Check(statement))
                    cout << " : Correct" << endl;
                else
                    cout << " : Error at " << mGrammerChecker.mErrorToken << endl;
            }
        } catch (const END_OF_FILE& err) {
            cout << "reached end of file" << endl;
        } catch (const QUIT& quit) {
            cout << "Program exits..." << endl;
        }
    }
};

int main() {
    ifstream input("file.txt", ios::in);
    cin.rdbuf(input.rdbuf());

    Interpreter interpreter;
}