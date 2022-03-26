# include <stdio.h>
# include <iomanip>
# include <iostream>
# include <map>
# include <sstream>
# include <stack>
# include <stdexcept>
# include <string>
# include <vector>

using namespace std ;

class END_OF_FILE {
}; // class END_OF_FILE


class QUIT {
}; // class QUIT


class Partially_Correct {
}; // class Partially_Correct


class Uncalculable {
}; // class Uncalculable


enum TokenType {
  INT = 0,
  FLOAT = 1,
  DELIMITER = 2,
  IDENTIFIER = 3,
  BOOLEAN = 4,
  ERROR = 5
}; // enum TokenType


class Token {

  public :
  TokenType mType ;
  string mString ;
  bool mNegative ;
  double mValue ;
  Token( const string & str, TokenType type ) {
    mString = str ;
    mType = type ;
    mNegative = false ;
    mValue = 0 ;
  } // Token()

  Token() {
    mString = "0" ;
    mType = INT ;
    mNegative = false ;
    mValue = 0 ;
  } // Token()

  void Transform() {
    if ( mType == INT || mType == FLOAT ) {
      stringstream ss ;
      ss << mString ;
      ss >> mValue ;
      if ( mNegative )
        mValue *= - 1 ;
    } // if
    else if ( mType == BOOLEAN ) {
      if ( mString == "false" )
        mValue = 0 ;
      else
        mValue = 1 ;
    } // else if
  } // Transform()

  void Add( const Token & token ) {
    mValue += token.mValue ;
    mType = Type( mType, token.mType ) ;
  } // Add()

  void Sub( const Token & token ) {
    mValue -= token.mValue ;
    mType = Type( mType, token.mType ) ;
  } // Sub()

  void Multiplication( const Token & token ) {
    mValue *= token.mValue ;
    mType = Type( mType, token.mType ) ;
  } // Multiplication()

  void Division( const Token & token ) {
    if ( token.mValue == 0 )
      throw logic_error( "Error" ) ;
    mValue /= token.mValue ;
    if ( Type( mValue ) == INT && Type( mType, token.mType ) == INT )
      mType = INT ;
    else
      mType = FLOAT ;
  } // Division()

  void GT( const Token & token ) {
    float value = mValue ;
    EQ( token ) ;
    if ( mValue == 1 )
      mValue = 0 ;
    else {
      if ( value > token.mValue )
        mValue = 1 ;
      else
        mValue = 0 ;
    } // else
  } // GT()

  void GEQ( const Token & token ) {
    LT( token ) ;
    if ( mValue == 1 )
      mValue = 0 ;
    else
      mValue = 1 ;
  } // GEQ()

  void LT( const Token & token ) {
    float value = mValue ;
    EQ( token ) ;
    if ( mValue == 1 )
      mValue = 0 ;
    else {
      if ( value < token.mValue )
        mValue = 1 ;
      else
        mValue = 0 ;
    } // else
  } // LT()

  void LEQ( const Token & token ) {
    GT( token ) ;
    if ( mValue == 1 )
      mValue = 0 ;
    else
      mValue = 1 ;
  } // LEQ()

  void EQ( const Token & token ) {
    mType = BOOLEAN ;
    float sub = mValue - token.mValue ;
    if ( sub < 0.0001 && sub > - 0.0001 )
      mValue = 1 ;
    else
      mValue = 0 ;
  } // EQ()

  void NEQ( const Token & token ) {
    EQ( token ) ;
    if ( mValue == 0 )
      mValue = 1 ;
    else
      mValue = 0 ;
  } // NEQ()


  private :
  TokenType Type( float number ) {
    if ( ( number - ( int ) number ) == 0 )
      return INT ;
    else
      return FLOAT ;
  } // Type()

  TokenType Type( const TokenType & type1, const TokenType & type2 ) {
    if ( type1 == FLOAT || type2 == FLOAT ) {
      return FLOAT ;
    } // if
    else {
      return INT ;
    } // else
  } // Type()
}; // class Token


class TokenGetter {

  private :
  map < string, int > mDelimiters ;
  bool IsDigit( const string & str ) {
    int numOfDecimalPoint = 0 ;
    for ( int i = 0 ; i < str.length() ; i++ ) {
      if ( str[ i ] == '.' ) {
        numOfDecimalPoint++ ;
      } // if

      if ( ( ( str[ i ] < '0' || str[ i ] > '9' ) && str[ i ] != '.' ) || numOfDecimalPoint > 1 ) {
        return false ;
      } // if
    } // for

    return true ;
  } // IsDigit()

  bool IsBoolean( const string & str ) {
    if ( str == "true" || str == "false" ) {
      return true ;
    } // if

    return false ;
  } // IsBoolean()

  bool IsLeagleIdentifier( const string & str ) {
    if ( ( str[ 0 ] < 'a' || str[ 0 ] > 'z' ) && ( str[ 0 ] < 'A' || str[ 0 ] > 'Z' ) ) {
      return false ;
    } // if

    for ( int i = 0 ; i < str.length() ; i++ ) {
      if ( ( str[ i ] < '0' || str[ i ] > '9' ) 
           && ( str[ i ] < 'a' || str[ i ] > 'z' ) 
           && ( str[ i ] < 'A' || str[ i ] > 'Z' ) 
           && str[ i ] != '_' ) {
        return false ;
      } // if
    } // for

    return true ;
  } // IsLeagleIdentifier()

  TokenType Type( const string & str ) {
    if ( IsDigit( str ) ) {
      if ( str.find( '.' ) == string::npos ) {
        return INT ;
      } // if
      else {
        return FLOAT ;
      } // else
    } // if
    else if ( IsBoolean( str ) ) {
      return BOOLEAN ;
    } // else if
    else if ( IsLeagleIdentifier( str ) ) {
      return IDENTIFIER ;
    } // else if
    else if ( mDelimiters.count( str ) > 0 ) {
      return DELIMITER ;
    } // else if
    else {
      return ERROR;
    } // else
  } // Type()

  void AddToken( vector < Token > * tokenList, const string & str ) {
    Token token( str, Type( str ) ) ;
    tokenList -> push_back( token ) ;
  } // AddToken()

  bool Contains( const string & str ) {
    map < string, int >::iterator iter = mDelimiters.find( str ) ;
    return iter != mDelimiters.end() ;
  } // Contains()


  public :
  static void GetLine( string & str ) {
    char ch = getchar() ;
    while ( ch != '\n' ) {
      str.push_back( ch ) ;
      ch = getchar() ;
    } // while
  } // GetLine()

  TokenGetter() {
    string delimiters[ ] = { "+", "-", "*", "/", "=", ">", "<", "(", ")", ";", ":=", "<>", ">=", "<=" } ;
    for ( int i = 0 ; i < 14 ; i++ ) {
      mDelimiters[ delimiters[ i ] ] = 0 ;
    } // for
  } // TokenGetter()

  vector < Token > * GetTokenList() {
    string line ;
    vector < Token > * tokenList = new vector < Token > ;
    GetLine( line ) ;
    string buffer ;
    try {
      string currentChar = " " ;
      string nextChar = " " ;
      for ( int i = 0 ; i < line.length() && ( currentChar != "/" || nextChar != "/" ) ; i++ ) {
        currentChar = line.substr( i, 1 ) ;
        if ( i < line.length() - 1 ) {
          nextChar = line.substr( i + 1, 1 ) ;
        } // if
        else {
          nextChar = " " ;
        } // else

        if ( currentChar != "/" || nextChar != "/" ) {
          if ( Contains( currentChar ) || Contains( currentChar + nextChar ) 
               || currentChar == " " || currentChar == "\t" ) {
            if ( buffer.length() > 0 ) {
              AddToken( tokenList, buffer ) ;
              buffer.clear() ;
            } // if

            if ( Contains( currentChar + nextChar ) ) {
              AddToken( tokenList, currentChar + nextChar ) ;
              i++ ;
            } // if
            else if ( Contains( currentChar ) ) {
              AddToken( tokenList, currentChar ) ;
            } // else if
          } // if
          else {
            if ( Type( buffer + currentChar ) == ERROR ) {
              if ( buffer.length() > 0 ) {
                AddToken( tokenList, buffer ) ;
                buffer.clear() ;
              } // if
            } // if

            buffer += currentChar ;
          } // else
        } // if
      } // for

      if ( buffer.length() > 0 ) {
        Token token( buffer, Type( buffer ) ) ;
        tokenList -> push_back( token ) ;
      } // if
    } // try

    catch( const logic_error & err ) {
      delete tokenList ;
      throw ;
    } // catch

    return tokenList ;
  } // GetTokenList()
}; // class TokenGetter


class GrammerChecker {

  private :
  map < string, string > mRuleMap ;
  map < char, char > mPair ;
  map < string, int > mIDTable ;
  string mErrorToken ;
  vector < string > * GetToken( const string & grammer ) {
    if ( grammer.length() == 0 ) {
      return NULL ;
    } // if

    vector < string > * grammerQueue = new vector < string >() ;
    string buffer ;
    int levelCounter = 0 ;
    bool isGroup = false ;
    map < char, char >::iterator iter ;
    char previousChar = ' ' ;
    for ( int i = 0 ; i < grammer.length() ; i++ ) {
      if ( !isGroup ) {
        if ( previousChar != '\'' ) {
          iter = mPair.find( grammer[ i ] ) ;
          if ( iter != mPair.end() ) {
            isGroup = true ;
            levelCounter++ ;
          } // if
        } // if
      } // if
      else {
        if ( grammer[ i ] == iter -> first )
          levelCounter++ ;
        else if ( grammer[ i ] == iter -> second )
          levelCounter-- ;
        if ( levelCounter == 0 )
          isGroup = false ;
      } // else

      if ( grammer[ i ] == ' ' && levelCounter == 0 ) {
        if ( buffer.length() > 0 ) {
          grammerQueue -> push_back( buffer ) ;
          buffer.clear() ;
        } // if
      } // if
      else {
        buffer.push_back( grammer[ i ] ) ;
      } // else

      previousChar = grammer[ i ] ;
    } // for

    if ( buffer.length() > 0 ) {
      grammerQueue -> push_back( buffer ) ;
    } // if

    return grammerQueue ;
  } // GetToken()

  bool Compare( vector < Token > & instructionQueue, const string & grammer ) {
    if ( grammer[ 0 ] == '{' || grammer[ 0 ] == '[' || grammer[ 0 ] == '(' || grammer[ 0 ] == '<' ) {
      int previousSize = instructionQueue.size() ;
      bool ifSuccess = Check( instructionQueue, grammer.substr( 1, grammer.length() - 2 ) ) ;
      if ( grammer[ 0 ] == '(' || grammer[ 0 ] == '<' ) {
        mErrorToken = instructionQueue[ 0 ].mString ;
        return ifSuccess ;
      } // if
      else if ( grammer[ 0 ] == '[' ) {
        mErrorToken = instructionQueue[ 0 ].mString ;
        int currentSize = instructionQueue.size() ;
        return ( ifSuccess || previousSize == currentSize ) ;
      } // else if
      else {
        while ( ifSuccess ) {
          previousSize = instructionQueue.size() ;
          ifSuccess = Check( instructionQueue, grammer.substr( 1, grammer.length() - 2 ) ) ;
        } // while

        mErrorToken = instructionQueue[ 0 ].mString ;
        int currentSize = instructionQueue.size() ;
        return previousSize == currentSize ;
      } // else
    } // if
    else {
      if ( instructionQueue.size() == 0 ) {
        throw Partially_Correct() ;
      } // if

      Token token = instructionQueue[ 0 ] ;
      string literal = "'" + token.mString + "'" ;
      if ( ( grammer == "IDENT" && token.mType == IDENTIFIER ) 
           || ( grammer == "NUM" && ( token.mType == INT || token.mType == FLOAT ) ) 
           || ( grammer == "SIGN" && ( token.mString == "+" || token.mString == "-" ) ) 
           || ( literal == grammer ) ) {
        instructionQueue.erase( instructionQueue.begin() ) ;
        return true ;
      } // if
      else {
        return false ;
      } // else
    } // else
  } // Compare()

  bool Check( vector < Token > & instructionQueue, const string & grammer ) {
    map < string, string >::iterator iter = mRuleMap.find( grammer ) ;
    vector < string > * grammerQueue = NULL ;
    if ( iter != mRuleMap.end() ) {
      grammerQueue = GetToken( iter -> second ) ;
    } // if
    else {
      grammerQueue = GetToken( grammer ) ;
    } // else

    bool isORRelation = false ;
    for ( int i = 0 ; i < grammerQueue -> size() ; i++ ) {
      if ( grammerQueue -> at( i ) == "|" ) {
        grammerQueue -> erase( grammerQueue -> begin() + i ) ;
        isORRelation = true ;
        i-- ;
      } // if
    } // for

    if ( isORRelation ) {
      for ( int i = 0 ; i < grammerQueue -> size() ; i++ ) {
        int previousSize = instructionQueue.size() ;
        bool isCorrect = Compare( instructionQueue, grammerQueue -> at( i ) ) ;
        int currentSize = instructionQueue.size() ;
        if ( isCorrect ) {
          return true ;
        } // if
        else {
          if ( previousSize != currentSize ) {
            return false ;
          } // if
        } // else
      } // for

      return false ;
    } // if
    else {
      for ( int i = 0 ; i < grammerQueue -> size() ; i++ ) {
        if ( !Compare( instructionQueue, grammerQueue -> at( i ) ) ) {
          return false ;
        } // if
      } // for

      return true ;
    } // else

    delete grammerQueue ;
  } // Check()

  bool LegalIdentifier( const vector < Token > & statement ) {
    int i = 0 ;
    if ( statement.size() >= 2 ) {
      if ( statement[ 1 ].mString == ":=" && statement[ 0 ].mType == IDENTIFIER ) {
        mIDTable[ statement[ 0 ].mString ] = 0 ;
        i = 2 ;
      } // if
    } // if
    else {
      return true ;
    } // else

    for ( ; i < statement.size() ; i++ ) {
      if ( statement[ i ].mType == IDENTIFIER ) {
        if ( mIDTable.find( statement[ i ].mString ) == mIDTable.end() ) {
          stringstream ss ;
          ss << "Undefined identifier : '" << statement[ i ].mString << "'" ;
          throw logic_error( ss.str() ) ;
        } // if
      } // if
    } // for

    return true ;
  } // LegalIdentifier()


  public :
  GrammerChecker() {
    string temp = "( IDENT ( ( ':=' <ArithExp> ) | ";
    temp += "<IDlessArithExpOrBexp> ) ';') | ( <NOT_ID_StartArithExpOrBexp> ';' )";
    mRuleMap[ "Command" ] = temp;
    temp = "{ ( '+' <Term> ) | ( '-' <Term> ) | ";
    temp += "( '*' <Factor> ) | ( '/' <Factor> ) } [ <BooleanOperator> <ArithExp> ]" ;
    mRuleMap[ "IDlessArithExpOrBexp" ] = temp;       
    mRuleMap[ "BooleanOperator" ] = "'=' | '<>' | '>' | '<' | '>=' | '<='" ;
    mRuleMap[ "NOT_ID_StartArithExpOrBexp" ] = "<NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]" ;
    mRuleMap[ "NOT_ID_StartArithExp" ] = "<NOT_ID_StartTerm> { ( '+' <Term> ) | ( '-' <Term> ) }" ;
    mRuleMap[ "NOT_ID_StartTerm" ] = "<NOT_ID_StartFactor> { ( '*' <Factor> ) | ( '/' <Factor> ) }" ;
    mRuleMap[ "NOT_ID_StartFactor" ] = "( [ SIGN ] NUM ) | ( '(' <ArithExp> ')' )" ;
    mRuleMap[ "ArithExp" ] = "<Term> { ( '+' <Term> ) | ( '-' <Term> ) }" ;
    mRuleMap[ "Term" ] = "<Factor> { ( '*' <Factor> ) | ( '/' <Factor> ) }" ;
    mRuleMap[ "Factor" ] = "IDENT | ( [ SIGN ] NUM ) | ( '(' <ArithExp> ')' )" ;
    mPair[ '{' ] = '}' ;
    mPair[ '[' ] = ']' ;
    mPair[ '(' ] = ')' ;
    mPair[ '<' ] = '>' ;
  } // GrammerChecker()

  void Check( const vector < Token > & statement ) {
    vector < Token > instructionQueue = statement ;
    try {
      if ( !Check( instructionQueue, "Command" ) ) {
        stringstream ss ;
        ss << "Unexpected token : '" << mErrorToken << "'" ;
        throw logic_error( ss.str() ) ;
      } // if
    } // try
    catch( const Partially_Correct & e ) {
    } // catch

    LegalIdentifier( statement ) ;
  } // Check()
}; // class GrammerChecker


class Calculator {

  private :
  map < string, Token > mIDTable ;
  int Priority( const Token & token ) {
    if ( token.mString == "(" )
      return 0 ;
    else if ( token.mString == ":=" )
      return 1 ;
    else if ( token.mString == "<" || token.mString == ">" || token.mString == "<=" 
              || token.mString == ">=" || token.mString == "<>" || token.mString == "=" )
      return 1 ;
    else if ( token.mString == "+" || token.mString == "-" )
      return 2 ;
    else if ( token.mString == "*" || token.mString == "/" )
      return 3 ;
    else
      throw Uncalculable() ;
  } // Priority()

  vector < Token > * PutSign( const vector < Token > & statement ) {
    vector < Token > * formula = new vector < Token >() ;
    string nextSign = "" ;
    Token nextToken( "123", INT ) ;
    for ( int i = 0 ; i < statement.size() ; i++ ) {
      if ( i < statement.size() - 1 )
        nextToken = statement[ i + 1 ] ;
      else
        nextToken.mType = INT ;
      if ( statement[ i ].mType == DELIMITER && nextToken.mType == DELIMITER 
           && statement[ i ].mString != ")" 
           && nextToken.mString != "(" && nextToken.mString != ")" )
        nextSign = nextToken.mString ;
      if ( statement[ i ].mType != DELIMITER || nextToken.mType == DELIMITER || nextSign == "" )
        formula -> push_back( statement[ i ] ) ;
      if ( nextSign != "" && ( statement[ i ].mType == INT || statement[ i ].mType == FLOAT ) ) {
        if ( nextSign == "-" )
          formula -> at( formula -> size() - 1 ).mNegative = true ;
        nextSign = "" ;
      } // if
    } // for

    if ( formula -> size() >= 2 ) {
      if ( formula -> at( 0 ).mString == "+" || formula -> at( 0 ).mString == "-" ) {
        if ( formula -> at( 0 ).mString == "-" )
          formula -> at( 1 ).mNegative = true ;
        formula -> erase( formula -> begin() ) ;
      } // if
    } // if

    return formula ;
  } // PutSign()

  vector < Token > * Postfix( vector < Token > * formula ) {
    vector < Token > * postfix = new vector < Token >() ;
    stack < Token > stack ;
    for ( int i = 0 ; i < formula -> size() ; i++ ) {
      if ( formula -> at( i ).mString != ";" ) {
        if ( formula -> at( i ).mType == DELIMITER ) {
          if ( formula -> at( i ).mString == "(" ) {
            stack.push( formula -> at( i ) ) ;
          } // if
          else if ( formula -> at( i ).mString == ")" ) {
            bool stop = false ;
            while ( !stack.empty() && !stop ) {
              if ( stack.top().mString != "(" )
                postfix -> push_back( stack.top() ) ;
              else
                stop = true ;
              stack.pop() ;
            } // while
          } // else if
          else {
            if ( !stack.empty() ) {
              while ( !stack.empty() && Priority( stack.top() ) >= Priority( formula -> at( i ) ) ) {
                postfix -> push_back( stack.top() ) ;
                stack.pop() ;
              } // while
            } // if

            stack.push( formula -> at( i ) ) ;
          } // else
        } // if
        else {
          postfix -> push_back( formula -> at( i ) ) ;
        } // else
      } // if
    } // for

    delete formula ;
    while ( !stack.empty() ) {
      if ( stack.top().mString == "(" ) {
        throw Uncalculable() ;
      } // if

      postfix -> push_back( stack.top() ) ;
      stack.pop() ;
    } // while

    return postfix ;
  } // Postfix()

  bool LegalOperation( const Token & token1, const Token & token2 ) {
    if ( token1.mType == IDENTIFIER || token2.mType == IDENTIFIER )
      return true ;
    if ( ( token1.mType == INT || token1.mType == FLOAT ) 
         && ( token2.mType == INT || token2.mType == FLOAT ) )
      return true ;
    else if ( token1.mType == BOOLEAN && token2.mType == BOOLEAN )
      return true ;
    return false ;
  } // LegalOperation()

  void TableLookup( Token & token ) {
    if ( token.mType == IDENTIFIER ) {
      map < string, Token >::iterator iter = mIDTable.find( token.mString ) ;
      if ( iter != mIDTable.end() ) {
        token.mType = iter -> second.mType ;
        token.mValue = iter -> second.mValue ;
      } // if
    } // if
  } // TableLookup()

  Token Calculate( vector < Token > * postfix ) {
    stack < Token > stack ;
    for ( int i = 0 ; i < postfix -> size() ; i++ ) {
      if ( postfix -> at( i ).mType != DELIMITER ) {
        Token token = postfix -> at( i ) ;
        token.Transform() ;
        TableLookup( token ) ;
        stack.push( token ) ;
      } // if
      else {
        if ( stack.empty() )
          throw Uncalculable() ;
        Token operand2 = stack.top() ;
        stack.pop() ;
        if ( stack.empty() )
          throw Uncalculable() ;
        Token operand1 = stack.top() ;
        stack.pop() ;
        if ( !LegalOperation( operand1, operand2 ) ) {
          throw logic_error( "Error" ) ;
        } // if

        if ( postfix -> at( i ).mString == "+" )
          operand1.Add( operand2 ) ;
        else if ( postfix -> at( i ).mString == "-" )
          operand1.Sub( operand2 ) ;
        else if ( postfix -> at( i ).mString == "*" )
          operand1.Multiplication( operand2 ) ;
        else if ( postfix -> at( i ).mString == "/" )
          operand1.Division( operand2 ) ;
        else if ( postfix -> at( i ).mString == ">" )
          operand1.GT( operand2 ) ;
        else if ( postfix -> at( i ).mString == ">=" )
          operand1.GEQ( operand2 ) ;
        else if ( postfix -> at( i ).mString == "<" )
          operand1.LT( operand2 ) ;
        else if ( postfix -> at( i ).mString == "<=" )
          operand1.LEQ( operand2 ) ;
        else if ( postfix -> at( i ).mString == "=" )
          operand1.EQ( operand2 ) ;
        else if ( postfix -> at( i ).mString == "<>" )
          operand1.NEQ( operand2 ) ;
        else if ( postfix -> at( i ).mString == ":=" ) {
          mIDTable[ operand1.mString ] = operand2 ;
          operand1 = operand2 ;
        } // else if

        stack.push( operand1 ) ;
      } // else
    } // for

    delete postfix ;
    if ( stack.size() > 1 )
      throw Uncalculable() ;
    return stack.top() ;
  } // Calculate()


  public :
  Token Calculate( const vector < Token > & statement ) {
    try {
      return Calculate( Postfix( PutSign( statement ) ) ) ;
    } // try

    catch( const Uncalculable & err ) {
      Token token ;
      return token ;
    } // catch
  } // Calculate()
}; // class Calculator


class StatementProcessor {

  private :
  TokenGetter mTokenGetter ;
  GrammerChecker mGrammerChecker ;
  vector < Token > mTokenQueue ;
  Calculator mCalculator ;
  void ConcatVector( vector < Token > & v1, const vector < Token > & v2 ) {
    for ( int i = 0 ; i < v2.size() ; i++ ) {
      v1.push_back( v2[ i ] ) ;
    } // for
  } // ConcatVector()

  bool PopToken( vector < Token > & v1 ) {
    while ( mTokenQueue.size() == 0 ) {
      vector < Token > * tokenList = mTokenGetter.GetTokenList() ;
      ConcatVector( mTokenQueue, * tokenList ) ;
      delete tokenList ;
    } // while

    bool stop = false ;
    while ( mTokenQueue.size() > 0 && !stop ) {
      if ( mTokenQueue[ 0 ].mString == ";" ) {
        stop = true ;
      } // if
      
      v1.push_back( mTokenQueue[ 0 ] ) ;
      mTokenQueue.erase( mTokenQueue.begin() ) ;
    } // while

    return stop ;
  } // PopToken()


  public :
  Token GetResult( vector < Token > & statement ) {
    statement.clear() ;
    bool completeStatement = false ;
    Token result ;
    while ( !completeStatement ) {
      try {
        completeStatement = PopToken( statement ) ;
        if ( statement[ 0 ].mString == "quit" ) {
          throw QUIT() ;
        } // if

        for ( int i = 0 ; i < statement.size() ; i++ ) {
          if ( statement[ i ].mType == ERROR ) {
            stringstream ss ;
            ss << "Unrecognized token with first char : '" << statement[ i ].mString[0] << "'" ;
            throw logic_error( ss.str() ) ;
          } // if
        } // for

        mGrammerChecker.Check( statement ) ;
        result = mCalculator.Calculate( statement ) ;
      } // try

      catch( const logic_error & err ) {
        cout << err.what() << endl ;
        cout << "> ";
        statement.clear() ;
        mTokenQueue.clear() ;
        completeStatement = false ;
      } // catch
    } // while

    return result ;
  } // GetResult()
}; // class StatementProcessor


class Interpreter {

  public :
  Interpreter() {
    StatementProcessor statementProcessor ;
    vector < Token > statement ;
    cout << "Program starts..." << endl ;
    try {
      while ( true ) {
        cout << "> " ;
        Token result = statementProcessor.GetResult( statement ) ;
        if ( result.mType == BOOLEAN ) {
          if ( result.mValue == 0 )
            cout << "false" << endl ;
          else
            cout << "true" << endl ;
        } // if
        else if ( result.mType == INT )
          cout << fixed << setprecision( 0 ) << result.mValue << endl ;
        else
          cout << fixed << setprecision( 3 ) << result.mValue << endl ;
      } // while
    } // try
    catch( const END_OF_FILE & err ) {
      cout << "reached end of file" << endl ;
    } // catch
    catch( const QUIT & quit ) {
      cout << "Program exits..." << endl ;
    } // catch
  } // Interpreter()
}; // class Interpreter

int main() {
  string str ;
  TokenGetter::GetLine( str ) ;
  Interpreter interpreter ;
} // main()