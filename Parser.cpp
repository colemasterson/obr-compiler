/****************************************************************************
 ** NAME: Cole Masterson                                                   **
 ** CLASS: CSC 446                                                         **
 ** ASSIGNMENT: 3                                                          **
 ** DUE DATE: 2/21/23                                                      **
 ** INSTRUCTOR: Dr. Hamer                                                  **
 ***************************************************************************/

#include "Parser.h"
#include "Lexer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cctype>



using namespace std;

list<Token> tempList;
Token tempToken;
Token tempTokenIdt;

ListNode<VariableType> * paramTypes;
ListNode<ParameterPassingMode> * paramPassingModes;

//SymbolTable symTable;
stack<Token> tokenStack;
int maxDepth = 0;
int depth = 0;



/****************************************************************************
 ** FUNCTION: match(TokenType desired)                                     **
 ****************************************************************************
 ** DESCRIPTION: if the current token matches the desired token type then  **
 **              get the next token otherwise throw an error               **
 ** INPUT ARGS: TokenType                                                  **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/

void match(TokenType desired)
{
    if(token.type == desired)
    {
        /*cout << setw(20) << "SUCCESS" 
        << setw(20) << "LEXEME:" 
        << setw(20) << token.lexeme 
        << setw(20) << "TOKEN:" 
        << setw(20) << revTokenMap[token.type] << endl;*/
        token = GetNextToken();
    }
    else
    {
        cout <<setw(20) <<"PARSE ERROR:"
        << setw(20) <<" Lexeme: "<< token.lexeme <<", Token: " <<revTokenMap[token.type]
        << setw(20) <<" Desired Type: "<<revTokenMap[desired]<<endl;

        cin.ignore();
		token = GetNextToken();

    }

    return;
}

/****************************************************************************
 ** FUNCTION: PROG()                                                       **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for PROG                     **
 **          PROG  ->	modulet idt; DeclarativePart StatementPart endt idt. **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void PROG()
{
    match(TokenType::MODULE);
    match(TokenType::IDENTIFIER);
    match(TokenType::SEMICOLON);
    DECLPART();
    STATEPART();
    match(TokenType::END);
    match(TokenType::IDENTIFIER);
    match(TokenType::PERIOD);
    return;
}

/****************************************************************************
 ** FUNCTION: DECLPART()                                                   **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for DECLPART                 **
 **              DECLPART	->	CONSTPART VARPART PROCPART                 **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void DECLPART()
{
	CONSTPART();
	VARPART();
	PROCPART();
	return;
}


/****************************************************************************
 ** FUNCTION: CONSTPART()                                                  **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for CONSTPART                **
 **					ConstPart -> constt ConstTail | lambda				   **e
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void CONSTPART()
{
	if(token.type == TokenType::CONST)
	{
		match(TokenType::CONST);
		CONSTTAIL();
	}
	return;
}

/****************************************************************************
 ** FUNCTION: CONSTTAIL()	                                               **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for STAT_LIST                **
 **              ConstTail -> idt = Value ; ConstTail | lambda			   **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void CONSTTAIL()
{
	string tLexeme;
	if(token.type == TokenType::IDENTIFIER)
	{
		tLexeme = token.lexeme;
		match(TokenType::IDENTIFIER);
		match(TokenType::RELOP);

		if(token.value != 0)
			symTable.insertConst(tLexeme, token.type, depth, false, token.value, 0.0);
		else if(token.valueR != 0)
			symTable.insertConst(tLexeme, token.type, depth, false, 0, token.valueR);

		VALUE();
		match(TokenType::SEMICOLON);
		CONSTTAIL();
	}
	return;
}


/****************************************************************************
 ** FUNCTION: VARPART()                                                    **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for VARS    		           **
 **              VarPart -> vart VarTail | lambda		                   **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void VARPART() 
{
	if(token.type == TokenType::VAR) 
	{
		match(TokenType::VAR);
		VARTAIL();
	}
	return;  
}

/****************************************************************************
 ** FUNCTION: VARTAIL()                                                    **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for SUB_DECL                 **
 ** 			 VarTail -> IdentifierList : TypeMark ; VarTail | lambda   **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void VARTAIL() 
{
	if(token.type == TokenType::IDENTIFIER)
	{
		ID_LIST();
		match(TokenType::COLON);
		TYPEMARK();
		match(TokenType::SEMICOLON);
		tempList.clear();
		VARTAIL();
	}
	return;
}

/****************************************************************************
 ** FUNCTION: ID_LIST()                                                  **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for ID_LIST                **
 **              ID_LIST		-> 	idt | ID_LIST , idt                   **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void ID_LIST()
{
	tempList.push_back(token);
	match(TokenType::IDENTIFIER);
	if(token.type == TokenType::COMMA)
	{
		match(TokenType::COMMA);
		ID_LIST();
	}
	return;
}

/****************************************************************************
 ** FUNCTION: TYPEMARK()                                                       **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for TYPE                     **
 **              TypeMark -> integert | realt | chart                      **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void TYPEMARK()
{
	if(token.type == TokenType::INTEGER)
	{
		if(tempList.size() >= 1)
		{
			for(auto tempToken : tempList)
			{
				tempToken.type = TokenType::INTEGER;
				symTable.insertVar(tempToken.lexeme, tempToken.type, depth, VariableType::INTEGERT);
			}
		}
		match(TokenType::INTEGER);
	}
	else if(token.type == TokenType::REAL) 
	{
		for(auto tempToken:tempList)
		{
			tempToken.type = TokenType::REAL;
			symTable.insertVar(tempToken.lexeme, tempToken.type, depth, VariableType::REALT); 
		}
		match(TokenType::REAL);
	}
	else
	{
		for(auto tempToken:tempList)
		{
			tempToken.type = TokenType::CHAR;
			symTable.insertVar(tempToken.lexeme, tempToken.type, depth, VariableType::CHART); 
		}
		match(TokenType::CHAR);
	}

	return;
}

/****************************************************************************
 ** FUNCTION: VALUE()                                                      **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for VALUE                    **
 **              Value -> NumericalLiteral			                       **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void VALUE()
{
	match(TokenType::NUMBER);
	return;
}


/****************************************************************************
 ** FUNCTION: PART()                                                   **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for PROCPART                 **
 **              ProcPart -> ProcedureDecl ProcPart | lambda               **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void PROCPART()
{
	if(token.type == TokenType::PROCEDURE)
	{
		depth++;
		maxDepth++;
		PROCDECL();
		PROCPART();
		depth--;
	}
 	return;
}

/****************************************************************************
 ** FUNCTION: PROCDECL()                                                   **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for PROCDECL                 **
 **             ProcDecl -> ProcHeading ; ProcBody idt ;              	   **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void PROCDECL()
{
	PROCHEAD();
	match(TokenType::SEMICOLON);
	PROCBODY();
	match(TokenType::IDENTIFIER);
	match(TokenType::SEMICOLON);
	return;
}


/****************************************************************************
 ** FUNCTION: PROCHEAD()                                                   **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for PROCHEAD                 **
 **              ProcHeading -> proct idt Args			                   **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void PROCHEAD()
{
	string tLexeme;
	match(TokenType::PROCEDURE);
	tLexeme = token.lexeme;
	match(TokenType::IDENTIFIER);
	ARGS();
	symTable.insertProc(tLexeme, TokenType::PROCEDURE, depth-1, 0, symTable.getParamListSize(paramPassingModes), paramTypes, paramPassingModes);

	return;
}

/****************************************************************************
 ** FUNCTION: PROCBODY()                                                   **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for PROCBODY                 **
 **              ProcBody -> DeclarativePart StatementPart endt            **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void PROCBODY()
{
	DECLPART();
	STATEPART();
	match(TokenType::END);
	return;
}

/****************************************************************************
 ** FUNCTION: ARGS()                                         	           **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for ARGS       	           **
 **              Args -> ( ArgList ) | lambda			                   **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void ARGS() 
{
	if(token.type == TokenType::LPAREN)
	{
		match(TokenType::LPAREN);
		ARG_LIST();
		match(TokenType::RPAREN);
	}
	return;
}

/****************************************************************************
 ** FUNCTION: ARG_LIST()                                                  **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for ARG_LIST                 **
 **              ArgList -> Mode IdentifierList : TypeMark MoreArgs        **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void ARG_LIST()
{
	ParameterPassingMode passType = ParameterPassingMode::BY_VALUE;

	if(token.type ==TokenType::VAR)
		passType = ParameterPassingMode::BY_REFERENCE;

	MODE();
	ID_LIST();

    for (auto tempToken : tempList) 
	{
        ListNode<ParameterPassingMode>* newNode = new ListNode<ParameterPassingMode>(passType);

        if (paramPassingModes == nullptr)
            paramPassingModes = newNode;
        else 
		{
            ListNode<ParameterPassingMode>* current = paramPassingModes;

            while (current->next != nullptr)
                current = current->next;

            current->next = newNode;
        }
    }

	match(TokenType::COLON);
	TYPEMARK();
	MORE_ARGS();
	return;
}

/****************************************************************************
 ** FUNCTION: MORE_ARGS()                                                  **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for MORE_ARGS                **
 **              ArgList -> Mode IdentifierList : TypeMark MoreArgs        **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void MORE_ARGS()
{
	if(token.type == TokenType::SEMICOLON) 
	{
		match(TokenType::SEMICOLON);
		ARG_LIST();
	}
	return;
}

/****************************************************************************
 ** FUNCTION: MODE()                                                       **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for MODE		               **
 **              Mode -> vart | lambda							           **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void MODE() 
{
	if(token.type == TokenType::VAR)
		match(TokenType::VAR);

	return;
}

/****************************************************************************
 ** FUNCTION: STATEPART()                                                  **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for STATEPART		           **
 **              StatmentPart -> begint SeqOfStatements | lambda           **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void STATEPART() 
{
	if(token.type == TokenType::BEGIN)
	{
		match(TokenType::BEGIN);
		SEQ_STATE();
	}
	return;
}

/****************************************************************************
 ** FUNCTION: SEQ_STATE()                                                  **
 ****************************************************************************
 ** DESCRIPTION: follows the production rules for SEQ_STATE		           **
 **              SeqOfStatements -> lambda						           **
 ** INPUT ARGS: None                                                       **
 ** OUTPUT ARGS: None                                                      **
 ** IN/OUT ARGS: None                                                      ** 
 ****************************************************************************/
void SEQ_STATE() 
{
	return;
}


