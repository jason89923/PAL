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

enum RuleOperation {
    SEQUENTIAL = 0,
    OPTIONAL = 1
};

enum RuleNature {
    NORMAL = 0,
    REPEATABLE = 1,
    OMITTED = 2
};

class Rule {
   private:
    vector<Rule*> mRule;
    string basicRule;
    RuleOperation mOperation;
    RuleNature mRuleNature;

   public:
    Rule(string rule) {
        basicRule = rule;
        mOperation = SEQUENTIAL;
        mRuleNature = NORMAL;
    }

    Rule() {
        mOperation = SEQUENTIAL;
        mRuleNature = NORMAL;
    }

    void AddRule(Rule* rule) {
        mRule.push_back(rule);
    }

    void SetNature(RuleOperation operation, RuleNature ruleNature) {
        mOperation = operation;
        mRuleNature = ruleNature;
    }
};

class GrammerChecker {
   private:
    Rule* BuildRule1() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("[IDENT]"));
        Rule* subRule = new Rule();
        subRule->AddRule(new Rule(":="));
        subRule->AddRule(new Rule("<ArithExp>"));
        Rule* subRule2 = new Rule();
        subRule2->AddRule(subRule);
        subRule2->AddRule(new Rule("<IDlessArithExpOrBexp>"));
        subRule2->SetNature(OPTIONAL, NORMAL);
        rule->AddRule(subRule2);
        rule->AddRule(new Rule(";"));
        subRule2 = new Rule();
        subRule2->AddRule(new Rule("<NOT_IDStartArithExpOrBexp>"));
        subRule2->AddRule(new Rule(";"));
        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        parentRule->AddRule(subRule2);
        parentRule->SetNature(OPTIONAL, NORMAL);
        return parentRule;
    }

    Rule* BuildRule2() {
        Rule* rule = new Rule();
        // rule->AddRule(new Rule("[IDENT]"));
        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("+"));
        subRule->AddRule(new Rule("<Term>"));
        Rule* subRule2 = new Rule();
        subRule2->AddRule(new Rule("-"));
        subRule2->AddRule(new Rule("<Term>"));
        Rule* subRule3 = new Rule();
        subRule3->AddRule(new Rule("*"));
        subRule3->AddRule(new Rule("<Factor>"));
        Rule* subRule4 = new Rule();
        subRule4->AddRule(new Rule("/"));
        subRule4->AddRule(new Rule("<Factor>"));
        rule->SetNature(OPTIONAL, REPEATABLE);
        rule->AddRule(subRule);
        rule->AddRule(subRule2);
        rule->AddRule(subRule3);
        rule->AddRule(subRule4);

        subRule4 = new Rule();
        subRule4->AddRule(new Rule("<BooleanOperator>"));
        subRule4->AddRule(new Rule("<ArithExp>"));
        subRule4->SetNature(SEQUENTIAL, OMITTED);

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        parentRule->AddRule(subRule4);
        return parentRule;
    }

    Rule* BuildRule3() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("="));
        rule->AddRule(new Rule("<>"));
        rule->AddRule(new Rule(">"));
        rule->AddRule(new Rule("<"));
        rule->AddRule(new Rule(">="));
        rule->AddRule(new Rule("<="));

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        parentRule->SetNature(OPTIONAL, NORMAL);
        return parentRule;
    }

    Rule* BuildRule4() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("<NOT_ID_StartArithExp>"));

        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("<BooleanOperator>"));
        subRule->AddRule(new Rule("<ArithExp>"));
        subRule->SetNature(SEQUENTIAL, OMITTED);

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        parentRule->AddRule(subRule);
        return parentRule;
    }

    Rule* BuildRule5() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("<NOT_ID_StartTerm>"));

        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("+"));
        subRule->AddRule(new Rule("<Term>"));
        Rule* subRule2 = new Rule();
        subRule2->AddRule(new Rule("-"));
        subRule2->AddRule(new Rule("<Term>"));
        rule->SetNature(OPTIONAL, REPEATABLE);
        rule->AddRule(subRule);
        rule->AddRule(subRule2);

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        return parentRule;
    }

    Rule* BuildRule6() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("<NOT_ID_StartFactor>"));

        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("*"));
        subRule->AddRule(new Rule("<Factor>"));
        Rule* subRule2 = new Rule();
        subRule2->AddRule(new Rule("/"));
        subRule2->AddRule(new Rule("<Factor>"));
        rule->SetNature(OPTIONAL, REPEATABLE);
        rule->AddRule(subRule);
        rule->AddRule(subRule2);

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        return parentRule;
    }

    Rule* BuildRule7() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("SIGN"));
        rule->SetNature(SEQUENTIAL, OMITTED);

        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("NUM"));
        rule->AddRule(subRule);

        Rule* subRule2 = new Rule();
        subRule2->AddRule(new Rule("("));
        subRule2->AddRule(new Rule("<ArithExp>"));
        subRule2->AddRule(new Rule(")"));

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        parentRule->AddRule(subRule2);
        parentRule->SetNature(OPTIONAL, NORMAL);
        return parentRule;
    }

    Rule* BuildRule8() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("<Term>"));

        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("+"));
        subRule->AddRule(new Rule("<Term>"));
        Rule* subRule2 = new Rule();
        subRule2->AddRule(new Rule("-"));
        subRule2->AddRule(new Rule("<Term>"));
        rule->SetNature(OPTIONAL, REPEATABLE);
        rule->AddRule(subRule);
        rule->AddRule(subRule2);

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        return parentRule;
    }

    Rule* BuildRule9() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("<Factor>"));

        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("*"));
        subRule->AddRule(new Rule("<Factor>"));
        Rule* subRule2 = new Rule();
        subRule2->AddRule(new Rule("/"));
        subRule2->AddRule(new Rule("<Factor>"));
        rule->SetNature(OPTIONAL, REPEATABLE);
        rule->AddRule(subRule);
        rule->AddRule(subRule2);

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        return parentRule;
    }

    Rule* BuildRule10() {
        Rule* rule = new Rule();
        rule->AddRule(new Rule("IDENT"));

        Rule* subRule = new Rule();
        subRule->AddRule(new Rule("SIGN"));
        subRule->SetNature(SEQUENTIAL, OMITTED);

        Rule* subRule2 = new Rule();
        subRule2->AddRule(subRule);
        subRule2->AddRule(new Rule("NUM"));

        Rule* subRule3 = new Rule();
        subRule3->AddRule(new Rule("("));
        subRule3->AddRule(new Rule("<ArithExp>"));
        subRule3->AddRule(new Rule(")"));

        Rule* parentRule = new Rule();
        parentRule->AddRule(rule);
        parentRule->AddRule(subRule2);
        parentRule->AddRule(subRule3);
        parentRule->SetNature(OPTIONAL, NORMAL);
        return parentRule;
    }

   public:
    GrammerChecker() {
        BuildRule1();
        BuildRule10();
    }
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
    ifstream input("file.txt", ios::in);
    cin.rdbuf(input.rdbuf());

    Interpreter interpreter;
}