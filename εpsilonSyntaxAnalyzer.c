// εpsilon SYNTAX ANALYZER (study copy with comments)
//
// What it does, in order (Syntax doc 3.3 "Reading Process"):
//   1. Reads the Symbol Table that the lexer wrote, ONE row (one token) at a time.
//   2. Throws away comments, noise words and ERROR rows.
//   3. Puts the other tokens in a statement buffer until the statement's
//      terminator: the end of the line, or the closing } of an if / while.
//   4. Parses that one statement with recursive descent (one function per
//      grammar rule) and builds its parse tree.
//   5. Prints the tree. On a syntax error it uses panic mode: report the line,
//      skip to a safe token, keep going.
//   6. Clears the buffer and repeats until EOF, then lists all errors.
// It checks SYNTAX only. Whether a variable is declared or a type fits is the
// semantic analyzer's job, so this program never looks at that.
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32

// Windows only: switch the console to UTF-8 so it can show the tree symbols
// ├── └── │ →. This is the only line that talks to the operating system.
__declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int codePage);
#endif

enum {
    // Every token type. Same list and same order as in the lexer, so both programs
    // agree on what each token is.
    TOKEN_ERROR,
    TOKEN_END_OF_FILE,
    TOKEN_NEWLINE,
    TOKEN_LINE_COMMENT,
    TOKEN_BLOCK_COMMENT,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER_LITERAL,
    TOKEN_DECIMAL_LITERAL,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_KEYWORD_INTEGER,
    TOKEN_KEYWORD_DECIMAL,
    TOKEN_KEYWORD_CHARACTER,
    TOKEN_KEYWORD_STRING,
    TOKEN_KEYWORD_BOOLEAN,
    TOKEN_KEYWORD_VECTOR,
    TOKEN_KEYWORD_INPUT,
    TOKEN_KEYWORD_PRINT,
    TOKEN_KEYWORD_IF,
    TOKEN_KEYWORD_ELIF,
    TOKEN_KEYWORD_ELSE,
    TOKEN_KEYWORD_WHILE,
    TOKEN_KEYWORD_AND,
    TOKEN_KEYWORD_OR,
    TOKEN_KEYWORD_NOT,
    TOKEN_KEYWORD_TRUE,
    TOKEN_KEYWORD_FALSE,
    TOKEN_FUNCTION_MEAN,
    TOKEN_FUNCTION_MEDIAN,
    TOKEN_FUNCTION_MODE,
    TOKEN_FUNCTION_VARIANCE,
    TOKEN_NOISE_WORD_LET,
    TOKEN_NOISE_WORD_THE,
    TOKEN_NOISE_WORD_OF,
    TOKEN_NOISE_WORD_FROM,
    TOKEN_NOISE_WORD_USING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_FLOOR_DIVIDE,
    TOKEN_MODULO,
    TOKEN_EXPONENT,
    TOKEN_ASSIGN,
    TOKEN_PLUS_ASSIGN,
    TOKEN_MINUS_ASSIGN,
    TOKEN_MULTIPLY_ASSIGN,
    TOKEN_DIVIDE_ASSIGN,
    TOKEN_MODULO_ASSIGN,
    TOKEN_EXPONENT_ASSIGN,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN_OR_EQUAL,
    TOKEN_GREATER_THAN_OR_EQUAL,
    TOKEN_LEFT_PARENTHESIS,
    TOKEN_RIGHT_PARENTHESIS,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_TYPE_COUNT
};

// The name of each token type, exactly as the lexer writes it in the TOKEN
// column of Symbol Table.txt. Used to turn that text back into a token type,
// and to print token names in the tree.
static const char *tokenName[TOKEN_TYPE_COUNT] = {
    [TOKEN_ERROR] = "ERROR",
    [TOKEN_END_OF_FILE] = "EOF",
    [TOKEN_NEWLINE] = "NEWLINE",
    [TOKEN_LINE_COMMENT] = "LINE_COMMENT",
    [TOKEN_BLOCK_COMMENT] = "BLOCK_COMMENT",
    [TOKEN_IDENTIFIER] = "IDENT",
    [TOKEN_INTEGER_LITERAL] = "INT_LIT",
    [TOKEN_DECIMAL_LITERAL] = "DEC_LIT",
    [TOKEN_CHARACTER_LITERAL] = "CHAR_LIT",
    [TOKEN_STRING_LITERAL] = "STR_LIT",
    [TOKEN_KEYWORD_INTEGER] = "KW_INTEGER",
    [TOKEN_KEYWORD_DECIMAL] = "KW_DECIMAL",
    [TOKEN_KEYWORD_CHARACTER] = "KW_CHARACTER",
    [TOKEN_KEYWORD_STRING] = "KW_STRING",
    [TOKEN_KEYWORD_BOOLEAN] = "KW_BOOLEAN",
    [TOKEN_KEYWORD_VECTOR] = "KW_VECTOR",
    [TOKEN_KEYWORD_INPUT] = "KW_INPUT",
    [TOKEN_KEYWORD_PRINT] = "KW_PRINT",
    [TOKEN_KEYWORD_IF] = "KW_IF",
    [TOKEN_KEYWORD_ELIF] = "KW_ELIF",
    [TOKEN_KEYWORD_ELSE] = "KW_ELSE",
    [TOKEN_KEYWORD_WHILE] = "KW_WHILE",
    [TOKEN_KEYWORD_AND] = "KW_AND",
    [TOKEN_KEYWORD_OR] = "KW_OR",
    [TOKEN_KEYWORD_NOT] = "KW_NOT",
    [TOKEN_KEYWORD_TRUE] = "KW_TRUE",
    [TOKEN_KEYWORD_FALSE] = "KW_FALSE",
    [TOKEN_FUNCTION_MEAN] = "FN_MEAN",
    [TOKEN_FUNCTION_MEDIAN] = "FN_MEDIAN",
    [TOKEN_FUNCTION_MODE] = "FN_MODE",
    [TOKEN_FUNCTION_VARIANCE] = "FN_VARIANCE",
    [TOKEN_NOISE_WORD_LET] = "NW_LET",
    [TOKEN_NOISE_WORD_THE] = "NW_THE",
    [TOKEN_NOISE_WORD_OF] = "NW_OF",
    [TOKEN_NOISE_WORD_FROM] = "NW_FROM",
    [TOKEN_NOISE_WORD_USING] = "NW_USING",
    [TOKEN_PLUS] = "PLUS",
    [TOKEN_MINUS] = "MINUS",
    [TOKEN_MULTIPLY] = "STAR",
    [TOKEN_DIVIDE] = "SLASH",
    [TOKEN_FLOOR_DIVIDE] = "FLOORDIV",
    [TOKEN_MODULO] = "PERCENT",
    [TOKEN_EXPONENT] = "CARET",
    [TOKEN_ASSIGN] = "ASSIGN",
    [TOKEN_PLUS_ASSIGN] = "PLUS_EQ",
    [TOKEN_MINUS_ASSIGN] = "MINUS_EQ",
    [TOKEN_MULTIPLY_ASSIGN] = "STAR_EQ",
    [TOKEN_DIVIDE_ASSIGN] = "SLASH_EQ",
    [TOKEN_MODULO_ASSIGN] = "PERCENT_EQ",
    [TOKEN_EXPONENT_ASSIGN] = "CARET_EQ",
    [TOKEN_EQUAL] = "EQ",
    [TOKEN_NOT_EQUAL] = "NOT_EQ",
    [TOKEN_LESS_THAN] = "LT",
    [TOKEN_GREATER_THAN] = "GT",
    [TOKEN_LESS_THAN_OR_EQUAL] = "LE",
    [TOKEN_GREATER_THAN_OR_EQUAL] = "GE",
    [TOKEN_LEFT_PARENTHESIS] = "LPAREN",
    [TOKEN_RIGHT_PARENTHESIS] = "RPAREN",
    [TOKEN_LEFT_BRACKET] = "LBRACKET",
    [TOKEN_RIGHT_BRACKET] = "RBRACKET",
    [TOKEN_LEFT_BRACE] = "LBRACE",
    [TOKEN_RIGHT_BRACE] = "RBRACE",
    [TOKEN_COMMA] = "COMMA",
    [TOKEN_COLON] = "COLON"
};

// The tree drawing symbols (written as UTF-8 bytes):
//   TREE_BRANCH "├── "   a child that has more children after it
//   TREE_LAST   "└── "   the last child
//   TREE_PIPE   "│   "   the line that continues past a child
//   TREE_ARROW  " → "    "A → B" means B is the ONLY child of A (doc 3.4.5)
#define TREE_BRANCH "\xE2\x94\x9C\xE2\x94\x80\xE2\x94\x80 "
#define TREE_LAST "\xE2\x94\x94\xE2\x94\x80\xE2\x94\x80 "
#define TREE_PIPE "\xE2\x94\x82   "
#define TREE_SPACE "    "
#define TREE_ARROW " \xE2\x86\x92 "

// Fixed sizes. Every table below has a fixed size written in the source;
// nothing is created or grown while the program runs (no malloc).
#define ROW_MAX 4096
#define LEXEME_MAX 128
#define LABEL_MAX 200
#define TREE_LINE_MAX 4096
#define PREFIX_MAX 1024
#define MAX_STATEMENT_TOKENS 10000
#define MAX_NODES 20000
#define NAME_MAX 64
#define MAX_ERRORS 500
#define MESSAGE_MAX 256
#define PATH_MAX_LENGTH 1024

// symbolTable = the input file (Symbol Table.txt), treeFile = the output
// file (Parse Tree.txt).
static FILE *symbolTable;
static FILE *treeFile;

// The token that was just read from the FILE (one row of the Symbol Table).
// holdingToken = 1 means "this token was read too early, use it again for the
// next statement" (see holdToken).
static int readType;
static int readLine;
static char readLexeme[LEXEME_MAX];
static int lastReadType = TOKEN_NEWLINE;
static int holdingToken;

// THE STATEMENT BUFFER: the tokens of ONE statement, collected until its
// terminator. statementPosition is how far the parser has read in it.
static int statementType[MAX_STATEMENT_TOKENS];
static int statementLine[MAX_STATEMENT_TOKENS];
static char statementLexeme[MAX_STATEMENT_TOKENS][LEXEME_MAX];
static int statementLength;
static int statementPosition;

// The token the PARSER is looking at right now (the lookahead), taken from
// the statement buffer. groupDepth counts open ( and [ so a NEWLINE inside
// them is ignored (doc 3.3 "Open groups").
static int tokenType;
static int tokenLine;
static char lexeme[LEXEME_MAX];
static char previousLexeme[LEXEME_MAX];
static int groupDepth;

// THE PARSE TREE of the current statement. Each node has a label (for example
// "<declaration>" or "IDENT 'x'"), its first child, its last child and its
// next sibling. nodeToken is -1 for a non-terminal (<...>) and the token type
// for a leaf. The tree is cleared before every statement.
static char nodeLabel[MAX_NODES][LABEL_MAX];
static int nodeToken[MAX_NODES];
static int nodeFirstChild[MAX_NODES];
static int nodeLastChild[MAX_NODES];
static int nodeNextSibling[MAX_NODES];
static int nodeCount;
static int treeFull;

// The syntax errors found so far, printed at the end in line order.
// panicMode       = an error was found and the parser is recovering
// afterRecovery   = do not report a second error caused by the same mistake
// offendingIsName = the error was "reserved word used as a name"
// missingBraceReported = a missing } at EOF was already reported (only once)
static int errorLine[MAX_ERRORS];
static char errorText[MAX_ERRORS][MESSAGE_MAX];
static int errorCount;
static int panicMode;
static int afterRecovery;
static int offendingIsName;
static int missingBraceReported;

// These two call each other (a block holds statements, a statement holds
// expressions), so they are declared before they are defined.
static int parseStatement(int parent);
static void parseExpression(int parent);

// Turns the token name read from the file (for example "KW_IF") back into its
// token type. This only READS the lexer's result; tokens are never decided by
// comparing lexemes.
static int tokenFromName(const char *name) {
    int type;
    for (type = 0; type < TOKEN_TYPE_COUNT; type++) {
        if (strcmp(name, tokenName[type]) == 0) {
            return type;
        }
    }
    return -1;
}

// A row looks like "5     | 12    | IDENT           | name".
// The lexeme starts after the third '|'. Returns where it starts.
static const char *findLexeme(const char *row) {
    int bars = 0;
    while (*row != '\0' && bars < 3) {
        if (*row == '|') {
            bars++;
        }
        row++;
    }
    if (*row == ' ') {
        row++;
    }
    return row;
}

// Copies the lexeme up to the end of the row.
static void copyLexeme(char *destination, const char *source) {
    int length = 0;
    while (source[length] != '\0' && source[length] != '\n' && source[length] != '\r' && length < LEXEME_MAX - 1) {
        destination[length] = source[length];
        length++;
    }
    destination[length] = '\0';
}

// READS ONE ROW of the Symbol Table: line, column, token name, lexeme.
// Rows that are not tokens (the header and the dashes) are skipped.
// At the end of the file the token becomes EOF.
static void readRow(void) {
    char row[ROW_MAX];
    char name[32];
    int line;
    int column;
    int type;
    int c;
    while (fgets(row, ROW_MAX, symbolTable) != NULL) {
        if (strchr(row, '\n') == NULL) {
            while ((c = fgetc(symbolTable)) != EOF && c != '\n') {
            }
        }
        if (sscanf(row, "%d | %d | %31s", &line, &column, name) != 3) {
            continue;
        }
        type = tokenFromName(name);
        if (type < 0) {
            continue;
        }
        readType = type;
        readLine = line;
        copyLexeme(readLexeme, findLexeme(row));
        return;
    }
    readType = TOKEN_END_OF_FILE;
    readLexeme[0] = '\0';
}

// Tokens the parser ignores (doc 3.3): ERROR (the lexer already reported it),
// comments, and noise words (let, the, of, from, using).
static int isDiscarded(int type) {
    return type == TOKEN_ERROR
        || type == TOKEN_LINE_COMMENT
        || type == TOKEN_BLOCK_COMMENT
        || (type >= TOKEN_NOISE_WORD_LET && type <= TOKEN_NOISE_WORD_USING);
}

// Gets the next USEFUL token from the file: reads rows until one is not
// discarded. A NEWLINE right after another NEWLINE is also skipped, so blank
// lines and comment-only lines count as one terminator.
// If a token is being held, it is used instead of reading a new one.
static void readToken(void) {
    if (holdingToken) {
        holdingToken = 0;
        return;
    }
    do {
        readRow();
    } while (isDiscarded(readType) || (readType == TOKEN_NEWLINE && lastReadType == TOKEN_NEWLINE));
    lastReadType = readType;
}

// Keeps the token that was just read for the NEXT statement. Needed because
// the only way to know a statement is over is to look at the token after it.
static void holdToken(void) {
    holdingToken = 1;
}

// Adds the token that was just read to the statement buffer.
static void addToStatement(void) {
    if (statementLength >= MAX_STATEMENT_TOKENS) {
        return;
    }
    statementType[statementLength] = readType;
    statementLine[statementLength] = readLine;
    strcpy(statementLexeme[statementLength], readLexeme);
    statementLength++;
}

// Tokens that can only START a statement: the six data types, input, print,
// if, while. (elif and else are not included: they continue an if.)
static int isStatementKeyword(int type) {
    return (type >= TOKEN_KEYWORD_INTEGER && type <= TOKEN_KEYWORD_IF) || type == TOKEN_KEYWORD_WHILE;
}

// COLLECTS ONE STATEMENT into the buffer, one token at a time, and stops at
// its terminator (doc 3.3 "Statement Terminators"):
//   - declaration, assignment, input, print: the NEWLINE at the end of the
//     line, unless a ( or [ is still open;
//   - if / while: the } that closes the block. For an if it also looks at the
//     next token: elif or else means the statement goes on.
// groups = open ( and [, braces = open { of this if/while,
// waitingForBlock = the if/elif/else/while still needs its {.
static void collectStatement(void) {
    int groups = 0;
    int braces = 0;
    int isIf;
    int isBlockStatement;
    int waitingForBlock;
    statementLength = 0;
    statementPosition = 0;
    do {
        readToken();
    } while (readType == TOKEN_NEWLINE);
    addToStatement();
    if (readType == TOKEN_END_OF_FILE) {
        return;
    }
    isIf = readType == TOKEN_KEYWORD_IF;
    isBlockStatement = isIf || readType == TOKEN_KEYWORD_WHILE;
    waitingForBlock = isBlockStatement;
    for (;;) {
        readToken();
        if (readType == TOKEN_END_OF_FILE) {
            addToStatement();
            return;
        }
        // A NEWLINE inside an open ( or [ does not end the statement (a vector can
        // continue on the next line). Exception: if the next line starts with a
        // statement keyword, the ( or [ was never closed, so the group is dropped.
        if (readType == TOKEN_NEWLINE && groups > 0) {
            addToStatement();
            readToken();
            holdToken();
            if (!isStatementKeyword(readType)) {
                continue;
            }
            groups = 0;
            if (!isBlockStatement || (braces == 0 && !waitingForBlock)) {
                return;
            }
            continue;
        }
        // A normal NEWLINE. It ends a simple statement. Inside a block it is kept.
        // Before the first { of an if/while it is allowed only when { comes next.
        if (readType == TOKEN_NEWLINE) {
            addToStatement();
            if (!isBlockStatement) {
                return;
            }
            if (braces > 0) {
                continue;
            }
            readToken();
            holdToken();
            if (waitingForBlock && readType == TOKEN_LEFT_BRACE) {
                continue;
            }
            return;
        }
        addToStatement();
        if (readType == TOKEN_LEFT_PARENTHESIS || readType == TOKEN_LEFT_BRACKET) {
            groups++;
        } else if ((readType == TOKEN_RIGHT_PARENTHESIS || readType == TOKEN_RIGHT_BRACKET) && groups > 0) {
            groups--;
        } else if (isBlockStatement && readType == TOKEN_LEFT_BRACE) {
            braces++;
            waitingForBlock = 0;
        // A } that closes the block of an if/while. When the outermost block closes,
        // look past one NEWLINE: elif / else continue the if, anything else is held
        // for the next statement and this statement is complete.
        } else if (isBlockStatement && readType == TOKEN_RIGHT_BRACE && braces > 0) {
            braces--;
            if (braces > 0) {
                continue;
            }
            readToken();
            if (readType == TOKEN_NEWLINE) {
                addToStatement();
                readToken();
            }
            if (isIf && (readType == TOKEN_KEYWORD_ELIF || readType == TOKEN_KEYWORD_ELSE)) {
                addToStatement();
                waitingForBlock = 1;
                continue;
            }
            holdToken();
            return;
        }
    }
}

// The PARSER's "next token": takes it from the statement buffer (never from
// the file). When the buffer is used up it returns EOF, so a parse can never
// run past its own statement. NEWLINEs inside an open ( or [ are skipped.
static void getToken(void) {
    strcpy(previousLexeme, lexeme);
    do {
        if (statementPosition < statementLength) {
            tokenType = statementType[statementPosition];
            tokenLine = statementLine[statementPosition];
            strcpy(lexeme, statementLexeme[statementPosition]);
            statementPosition++;
        } else {
            tokenType = TOKEN_END_OF_FILE;
            lexeme[0] = '\0';
        }
    } while (tokenType == TOKEN_NEWLINE && groupDepth > 0);
}

// Adds a node to the parse tree under parent (-1 = it is the root).
static int addNode(int parent, const char *label, int token) {
    int node;
    if (nodeCount >= MAX_NODES) {
        treeFull = 1;
        return -1;
    }
    node = nodeCount++;
    snprintf(nodeLabel[node], LABEL_MAX, "%s", label);
    nodeToken[node] = token;
    nodeFirstChild[node] = -1;
    nodeLastChild[node] = -1;
    nodeNextSibling[node] = -1;
    if (parent >= 0) {
        if (nodeFirstChild[parent] < 0) {
            nodeFirstChild[parent] = node;
        } else {
            nodeNextSibling[nodeLastChild[parent]] = node;
        }
        nodeLastChild[parent] = node;
    }
    return node;
}

// MATCH: the current token is the one the grammar expects, so it becomes a
// leaf of the tree (for example "KW_IF 'if'") and the parser moves to the next
// token. Callers check the token type first.
static int matchToken(int parent) {
    char label[LABEL_MAX];
    int node;
    if (tokenType == TOKEN_NEWLINE) {
        snprintf(label, LABEL_MAX, "\\n");
    } else {
        snprintf(label, LABEL_MAX, "%s '%.60s'", tokenName[tokenType], lexeme);
    }
    node = addNode(parent, label, tokenType);
    if (tokenType == TOKEN_LEFT_PARENTHESIS || tokenType == TOKEN_LEFT_BRACKET) {
        groupDepth++;
    }
    if ((tokenType == TOKEN_RIGHT_PARENTHESIS || tokenType == TOKEN_RIGHT_BRACKET) && groupDepth > 0) {
        groupDepth--;
    }
    afterRecovery = 0;
    getToken();
    return node;
}

// Stores one error message with its line number.
static void addError(int line, const char *message) {
    if (errorCount >= MAX_ERRORS) {
        return;
    }
    errorLine[errorCount] = line;
    snprintf(errorText[errorCount], MESSAGE_MAX, "%s", message);
    errorCount++;
}

// REPORTS A SYNTAX ERROR and starts panic mode. Only the first error of a
// statement is reported; while recovering, more errors are ignored.
// The line is the line of the token where the error was found.
static void syntaxError(const char *format, ...) {
    char message[MESSAGE_MAX];
    va_list arguments;
    if (panicMode) {
        return;
    }
    panicMode = 1;
    if (afterRecovery) {
        return;
    }
    va_start(arguments, format);
    vsnprintf(message, MESSAGE_MAX, format, arguments);
    va_end(arguments);
    addError(tokenLine, message);
}

// "'while' is a reserved word and cannot be a variable name." The keyword is
// being used as a name, so recovery skips it too.
static void reservedWordError(void) {
    if (!panicMode) {
        offendingIsName = 1;
    }
    syntaxError("'%s' is a reserved word and cannot be a variable name.", lexeme);
}

// Keywords, true/false and the four statistical functions (doc 3.4 rule).
static int isReservedWord(int type) {
    return type >= TOKEN_KEYWORD_INTEGER && type <= TOKEN_FUNCTION_VARIANCE;
}

// = += -= *= /= %= ^=
static int isAssignOperator(int type) {
    return type >= TOKEN_ASSIGN && type <= TOKEN_EXPONENT_ASSIGN;
}

// Tokens that can start an expression: names, literals, not, -, ( , [ and
// the statistical functions.
static int startsExpression(int type) {
    return (type >= TOKEN_IDENTIFIER && type <= TOKEN_STRING_LITERAL)
        || (type >= TOKEN_KEYWORD_NOT && type <= TOKEN_FUNCTION_VARIANCE)
        || type == TOKEN_MINUS
        || type == TOKEN_LEFT_PARENTHESIS
        || type == TOKEN_LEFT_BRACKET;
}

// Words for error messages, for example "found integer literal".
static const char *literalName(int type) {
    switch (type) {
    case TOKEN_INTEGER_LITERAL: return "integer literal";
    case TOKEN_DECIMAL_LITERAL: return "decimal literal";
    case TOKEN_CHARACTER_LITERAL: return "character literal";
    case TOKEN_STRING_LITERAL: return "string literal";
    case TOKEN_KEYWORD_TRUE: return "boolean literal";
    case TOKEN_KEYWORD_FALSE: return "boolean literal";
    default: return NULL;
    }
}

// PANIC MODE RECOVERY (doc 3.5):
//   1. skip the token that caused the error (but never a NEWLINE, }, EOF or a
//      statement keyword, because those show where the next statement starts);
//   2. skip tokens until a sync token: NEWLINE, }, EOF, a statement keyword,
//      or a name on a later line;
//   3. if it stopped at a NEWLINE, use it as the broken statement's end;
//   4. turn panic mode off and go on with the next statement.
static void recoverFromError(void) {
    int line = tokenLine;
    groupDepth = 0;
    if (offendingIsName || (tokenType != TOKEN_NEWLINE && tokenType != TOKEN_RIGHT_BRACE
                            && tokenType != TOKEN_END_OF_FILE && !isStatementKeyword(tokenType))) {
        getToken();
    }
    while (tokenType != TOKEN_NEWLINE && tokenType != TOKEN_RIGHT_BRACE && tokenType != TOKEN_END_OF_FILE
           && !isStatementKeyword(tokenType) && !(tokenType == TOKEN_IDENTIFIER && tokenLine != line)) {
        getToken();
    }
    if (tokenType == TOKEN_NEWLINE) {
        getToken();
    }
    panicMode = 0;
    offendingIsName = 0;
    afterRecovery = tokenLine == line;
}

// <terminator> ::= \n | EOF
static void parseTerminator(int parent, int afterParenthesis) {
    int node;
    if (tokenType != TOKEN_NEWLINE && tokenType != TOKEN_END_OF_FILE) {
        if (afterParenthesis) {
            syntaxError("end of line expected after ')'.");
        } else {
            syntaxError("end of line expected, found '%s'.", lexeme);
        }
        return;
    }
    node = addNode(parent, "<terminator>", -1);
    if (tokenType == TOKEN_NEWLINE) {
        matchToken(node);
    } else {
        addNode(node, "EOF", TOKEN_END_OF_FILE);
    }
}

// The optional [ \n ] of the if and while rules (for example "{" on the
// next line).
static void skipOptionalNewline(int parent) {
    if (tokenType == TOKEN_NEWLINE) {
        matchToken(parent);
    }
}

// IDENT [ LBRACKET [ <index> ] RBRACKET ]   with   <index> ::= <expression>
// [ COLON <expression> ]   - covers scores[0], scores[0:2] and scores[].
static void parseIndex(int parent) {
    int index;
    matchToken(parent);
    if (tokenType != TOKEN_RIGHT_BRACKET) {
        index = addNode(parent, "<index>", -1);
        parseExpression(index);
        if (!panicMode && tokenType == TOKEN_COLON) {
            matchToken(index);
            parseExpression(index);
        }
        if (panicMode) {
            return;
        }
    }
    if (tokenType != TOKEN_RIGHT_BRACKET) {
        syntaxError("']' expected to close the index.");
        return;
    }
    matchToken(parent);
}

// <vector_lit> ::= LBRACKET [ <expression> { COMMA <expression> } ] RBRACKET
static void parseVectorLiteral(int parent) {
    int node = addNode(parent, "<vector_lit>", -1);
    matchToken(node);
    if (tokenType == TOKEN_RIGHT_BRACKET) {
        matchToken(node);
        return;
    }
    parseExpression(node);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(node);
        parseExpression(node);
    }
    if (panicMode) {
        return;
    }
    if (tokenType != TOKEN_RIGHT_BRACKET) {
        syntaxError("']' expected to close the vector.");
        return;
    }
    matchToken(node);
}

// <func_call> ::= ( FN_MEAN | FN_MEDIAN | FN_MODE | FN_VARIANCE )
//                 LPAREN <expression> RPAREN
static void parseFunctionCall(int parent) {
    int node = addNode(parent, "<func_call>", -1);
    char name[NAME_MAX];
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    matchToken(node);
    if (tokenType != TOKEN_LEFT_PARENTHESIS) {
        syntaxError("'(' expected after '%s'.", name);
        return;
    }
    matchToken(node);
    parseExpression(node);
    if (panicMode) {
        return;
    }
    if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
        syntaxError("')' expected to close '%s'.", name);
        return;
    }
    matchToken(node);
}

// <primary> ::= INT_LIT | DEC_LIT | CHAR_LIT | STR_LIT | KW_TRUE | KW_FALSE
//             | IDENT [ ... index ... ] | LPAREN <expression> RPAREN
//             | <vector_lit> | <func_call>
// The current token alone decides which one (one token of lookahead).
static void parsePrimary(int parent) {
    int node = addNode(parent, "<primary>", -1);
    switch (tokenType) {
    case TOKEN_INTEGER_LITERAL:
    case TOKEN_DECIMAL_LITERAL:
    case TOKEN_CHARACTER_LITERAL:
    case TOKEN_STRING_LITERAL:
    case TOKEN_KEYWORD_TRUE:
    case TOKEN_KEYWORD_FALSE:
        matchToken(node);
        return;
    case TOKEN_IDENTIFIER:
        matchToken(node);
        if (tokenType == TOKEN_LEFT_BRACKET) {
            parseIndex(node);
        }
        return;
    case TOKEN_LEFT_PARENTHESIS:
        matchToken(node);
        parseExpression(node);
        if (panicMode) {
            return;
        }
        if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
            syntaxError("')' expected to close the expression.");
            return;
        }
        matchToken(node);
        return;
    case TOKEN_LEFT_BRACKET:
        parseVectorLiteral(node);
        return;
    case TOKEN_FUNCTION_MEAN:
    case TOKEN_FUNCTION_MEDIAN:
    case TOKEN_FUNCTION_MODE:
    case TOKEN_FUNCTION_VARIANCE:
        parseFunctionCall(node);
        return;
    default:
        if (isReservedWord(tokenType)) {
            reservedWordError();
        } else {
            syntaxError("expression expected after '%s'.", previousLexeme);
        }
        return;
    }
}

// <power> ::= <primary> [ CARET <power> ]
// Calling itself on the right makes ^ right-associative: 2 ^ 3 ^ 2 = 2 ^ 9.
static void parsePower(int parent) {
    int node = addNode(parent, "<power>", -1);
    parsePrimary(node);
    if (!panicMode && tokenType == TOKEN_EXPONENT) {
        matchToken(node);
        parsePower(node);
    }
}

// <unary> ::= ( KW_NOT | MINUS ) <unary> | <power>
static void parseUnary(int parent) {
    int node = addNode(parent, "<unary>", -1);
    if (tokenType == TOKEN_KEYWORD_NOT || tokenType == TOKEN_MINUS) {
        matchToken(node);
        parseUnary(node);
        return;
    }
    parsePower(node);
}

// <mul_expr> ::= <unary> { ( STAR | SLASH | FLOORDIV | PERCENT ) <unary> }
// The loop makes these left-associative.
// The functions from here up to parseOr follow the precedence table:
// each one handles one level and calls the next higher level.
static void parseMultiplicative(int parent) {
    int node = addNode(parent, "<mul_expr>", -1);
    parseUnary(node);
    while (!panicMode && tokenType >= TOKEN_MULTIPLY && tokenType <= TOKEN_MODULO) {
        matchToken(node);
        parseUnary(node);
    }
}

// <add_expr> ::= <mul_expr> { ( PLUS | MINUS ) <mul_expr> }
static void parseAdditive(int parent) {
    int node = addNode(parent, "<add_expr>", -1);
    parseMultiplicative(node);
    while (!panicMode && (tokenType == TOKEN_PLUS || tokenType == TOKEN_MINUS)) {
        matchToken(node);
        parseMultiplicative(node);
    }
}

// <rel_expr> ::= <add_expr> { RELOP <add_expr> }   (== not= < > <= >=)
static void parseRelational(int parent) {
    int node = addNode(parent, "<rel_expr>", -1);
    parseAdditive(node);
    while (!panicMode && tokenType >= TOKEN_EQUAL && tokenType <= TOKEN_GREATER_THAN_OR_EQUAL) {
        matchToken(node);
        parseAdditive(node);
    }
}

// <and_expr> ::= <rel_expr> { KW_AND <rel_expr> }
static void parseAnd(int parent) {
    int node = addNode(parent, "<and_expr>", -1);
    parseRelational(node);
    while (!panicMode && tokenType == TOKEN_KEYWORD_AND) {
        matchToken(node);
        parseRelational(node);
    }
}

// <or_expr> ::= <and_expr> { KW_OR <and_expr> }
static void parseOr(int parent) {
    int node = addNode(parent, "<or_expr>", -1);
    parseAnd(node);
    while (!panicMode && tokenType == TOKEN_KEYWORD_OR) {
        matchToken(node);
        parseAnd(node);
    }
}

// <expression> ::= <or_expr>   (the lowest precedence level)
static void parseExpression(int parent) {
    int node = addNode(parent, "<expression>", -1);
    parseOr(node);
}

// Recovery for a bad condition (doc 3.5 "Recovery in Loops"): skip to the {
// or the end of the line, so the block itself can still be parsed.
static void skipToBlock(void) {
    groupDepth = 0;
    while (tokenType != TOKEN_LEFT_BRACE && tokenType != TOKEN_NEWLINE
           && tokenType != TOKEN_RIGHT_BRACE && tokenType != TOKEN_END_OF_FILE) {
        getToken();
    }
    panicMode = 0;
    afterRecovery = 1;
}

// <condition> ::= <expression>   for if, elif and while.
static void parseCondition(int parent, const char *keyword) {
    if (!startsExpression(tokenType)) {
        syntaxError("condition expected after '%s'.", keyword);
    } else {
        parseExpression(addNode(parent, "<condition>", -1));
    }
    if (panicMode) {
        skipToBlock();
    }
}

// <block> ::= LBRACE { \n } { <statement> { \n } } RBRACE
// Parses statements until the }. Reaching EOF means the } is missing.
static void parseBlock(int parent, const char *name) {
    int node;
    if (tokenType != TOKEN_LEFT_BRACE) {
        syntaxError("'{' expected to start the %s.", name);
        return;
    }
    node = addNode(parent, "<block>", -1);
    matchToken(node);
    while (tokenType != TOKEN_RIGHT_BRACE && tokenType != TOKEN_END_OF_FILE) {
        if (tokenType == TOKEN_NEWLINE) {
            getToken();
        } else {
            parseStatement(node);
        }
    }
    if (tokenType == TOKEN_END_OF_FILE) {
        if (!missingBraceReported) {
            missingBraceReported = 1;
            afterRecovery = 0;
            syntaxError("'}' expected to close the %s.", name);
        }
        return;
    }
    matchToken(node);
}

// <if_stmt> ::= KW_IF <condition> [ \n ] <block>
//               { [ \n ] KW_ELIF <condition> [ \n ] <block> }
//               [ [ \n ] KW_ELSE [ \n ] <block> ]
// After each block it looks at the next token (past one NEWLINE):
// elif / else = another branch, anything else = the if is finished.
static void parseIf(int parent) {
    int node = addNode(parent, "<if_stmt>", -1);
    int sawNewline;
    int sawElse = 0;
    matchToken(node);
    parseCondition(node, "if");
    skipOptionalNewline(node);
    parseBlock(node, "if body");
    while (!panicMode) {
        sawNewline = 0;
        if (tokenType == TOKEN_NEWLINE) {
            sawNewline = 1;
            getToken();
        }
        if (tokenType != TOKEN_KEYWORD_ELIF && tokenType != TOKEN_KEYWORD_ELSE) {
            return;
        }
        if (sawNewline) {
            addNode(node, "\\n", TOKEN_NEWLINE);
        }
        if (sawElse) {
            syntaxError("unexpected token '%s' after 'else'.", lexeme);
            panicMode = 0;
        }
        if (tokenType == TOKEN_KEYWORD_ELSE) {
            sawElse = 1;
            matchToken(node);
            skipOptionalNewline(node);
            parseBlock(node, "else body");
        } else {
            matchToken(node);
            parseCondition(node, "elif");
            skipOptionalNewline(node);
            parseBlock(node, "elif body");
        }
    }
}

// <while_stmt> ::= KW_WHILE <condition> [ \n ] <block> [ \n ]
static void parseWhile(int parent) {
    int node = addNode(parent, "<while_stmt>", -1);
    matchToken(node);
    parseCondition(node, "while");
    skipOptionalNewline(node);
    parseBlock(node, "loop body");
    if (panicMode) {
        return;
    }
    skipOptionalNewline(node);
}

// <input_stmt> ::= KW_INPUT LPAREN IDENT RPAREN <terminator>
static void parseInput(int parent) {
    int node = addNode(parent, "<input_stmt>", -1);
    matchToken(node);
    if (tokenType != TOKEN_LEFT_PARENTHESIS) {
        syntaxError("'(' expected after 'input'.");
        return;
    }
    matchToken(node);
    if (tokenType != TOKEN_IDENTIFIER) {
        if (literalName(tokenType) != NULL) {
            syntaxError("input target must be a variable, found %s.", literalName(tokenType));
        } else if (isReservedWord(tokenType)) {
            reservedWordError();
        } else {
            syntaxError("identifier expected inside 'input( )'.");
        }
        return;
    }
    matchToken(node);
    if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
        syntaxError("')' expected to close 'input'.");
        return;
    }
    matchToken(node);
    parseTerminator(node, 1);
}

// If a node is just a chain of only-children ending in one token, returns that
// token's node (for example the STR_LIT inside <expression> → ... → <primary>).
static int singleLeaf(int node) {
    int child;
    while (node >= 0 && nodeToken[node] < 0) {
        child = nodeFirstChild[node];
        if (child < 0 || nodeNextSibling[child] >= 0) {
            return -1;
        }
        node = child;
    }
    return node;
}

// Names the output form in the tree, as the doc does (3.4.2):
// one string = <string_output>, one name = <variable_output>,
// string , name = <mixed_output>, anything else stays <output_list>.
// It only looks at the TOKENS, so it is still syntax.
static void nameOutputForm(int list) {
    int first;
    int second;
    int third;
    int text;
    int variable;
    if (list < 0) {
        return;
    }
    first = nodeFirstChild[list];
    second = first >= 0 ? nodeNextSibling[first] : -1;
    third = second >= 0 ? nodeNextSibling[second] : -1;
    text = singleLeaf(first);
    if (second < 0 && text >= 0 && nodeToken[text] == TOKEN_STRING_LITERAL) {
        snprintf(nodeLabel[list], LABEL_MAX, "<string_output>");
        nodeFirstChild[list] = text;
        nodeLastChild[list] = text;
        return;
    }
    if (second < 0 && text >= 0 && nodeToken[text] == TOKEN_IDENTIFIER) {
        snprintf(nodeLabel[list], LABEL_MAX, "<variable_output>");
        nodeFirstChild[list] = text;
        nodeLastChild[list] = text;
        return;
    }
    if (third < 0 || nodeNextSibling[third] >= 0 || text < 0 || nodeToken[text] != TOKEN_STRING_LITERAL) {
        return;
    }
    variable = singleLeaf(third);
    if (variable < 0 || nodeToken[variable] != TOKEN_IDENTIFIER) {
        return;
    }
    snprintf(nodeLabel[list], LABEL_MAX, "<mixed_output>");
    nodeFirstChild[list] = text;
    nodeNextSibling[text] = second;
    nodeNextSibling[second] = variable;
    nodeLastChild[list] = variable;
}

// <output_stmt> ::= KW_PRINT LPAREN <output_content> RPAREN <terminator>
// <output_list> ::= <expression> { COMMA <expression> }
static void parsePrint(int parent) {
    int node = addNode(parent, "<output_stmt>", -1);
    int list;
    matchToken(node);
    if (tokenType != TOKEN_LEFT_PARENTHESIS) {
        syntaxError("'(' expected after 'print'.");
        return;
    }
    matchToken(node);
    if (tokenType == TOKEN_RIGHT_PARENTHESIS) {
        syntaxError("output content expected inside 'print( )'.");
        return;
    }
    list = addNode(addNode(node, "<output_content>", -1), "<output_list>", -1);
    parseExpression(list);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(list);
        parseExpression(list);
    }
    if (panicMode) {
        return;
    }
    if (startsExpression(tokenType)) {
        syntaxError("',' expected between the items of 'print', found '%s'.", lexeme);
        return;
    }
    if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
        syntaxError("')' expected to close 'print'.");
        return;
    }
    matchToken(node);
    parseTerminator(node, 1);
    if (!panicMode) {
        nameOutputForm(list);
    }
}

// <assignment> ::= IDENT <assign_op> <expression> <terminator>
static void parseAssignment(int parent) {
    int node = addNode(parent, "<assignment>", -1);
    char name[NAME_MAX];
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    matchToken(node);
    if (!isAssignOperator(tokenType)) {
        syntaxError("assignment operator expected after '%s'.", name);
        return;
    }
    matchToken(addNode(node, "<assign_op>", -1));
    parseExpression(node);
    if (panicMode) {
        return;
    }
    parseTerminator(node, 0);
}

// <declarator> ::= IDENT [ ASSIGN <expression> ]
static void parseDeclarator(int parent) {
    int node = addNode(parent, "<declarator>", -1);
    char name[NAME_MAX];
    if (tokenType != TOKEN_IDENTIFIER) {
        if (isReservedWord(tokenType)) {
            reservedWordError();
        } else {
            syntaxError("identifier expected after '%s'.", previousLexeme);
        }
        return;
    }
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    matchToken(node);
    if (tokenType == TOKEN_ASSIGN) {
        matchToken(node);
        parseExpression(node);
    } else if (isAssignOperator(tokenType)) {
        syntaxError("'%s' cannot be used in a declaration, use '='.", lexeme);
    } else if (tokenType != TOKEN_COMMA && tokenType != TOKEN_NEWLINE && tokenType != TOKEN_END_OF_FILE) {
        syntaxError("',', '=' or end of line expected after '%s'.", name);
    }
}

// <declaration> ::= <data_type> <declarator> { COMMA <declarator> } <terminator>
static void parseDeclaration(int parent) {
    int node = addNode(parent, "<declaration>", -1);
    matchToken(addNode(node, "<data_type>", -1));
    parseDeclarator(node);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(node);
        parseDeclarator(node);
    }
    if (panicMode) {
        return;
    }
    parseTerminator(node, 0);
}

// The first token cannot start any statement: picks the clearest message.
static void reportBadStatementStart(void) {
    if (tokenType == TOKEN_KEYWORD_ELIF || tokenType == TOKEN_KEYWORD_ELSE) {
        syntaxError("unexpected token '%s'.", lexeme);
    } else if (literalName(tokenType) != NULL) {
        syntaxError("assignment target must be a variable, found %s.", literalName(tokenType));
    } else if (tokenType >= TOKEN_FUNCTION_MEAN && tokenType <= TOKEN_FUNCTION_VARIANCE) {
        reservedWordError();
    } else {
        syntaxError("unexpected token '%s'.", lexeme);
    }
}

// <statement> ::= <declaration> | <assignment> | <input_stmt>
//               | <output_stmt> | <if_stmt> | <while_stmt>
// The FIRST token decides which rule to use (doc 3.2): a data type,
// an identifier, input, print, if or while. If the statement had an error,
// panic mode recovery runs here.
static int parseStatement(int parent) {
    int node = addNode(parent, "<statement>", -1);
    switch (tokenType) {
    case TOKEN_KEYWORD_INTEGER:
    case TOKEN_KEYWORD_DECIMAL:
    case TOKEN_KEYWORD_CHARACTER:
    case TOKEN_KEYWORD_STRING:
    case TOKEN_KEYWORD_BOOLEAN:
    case TOKEN_KEYWORD_VECTOR:
        parseDeclaration(node);
        break;
    case TOKEN_IDENTIFIER:
        parseAssignment(node);
        break;
    case TOKEN_KEYWORD_INPUT:
        parseInput(node);
        break;
    case TOKEN_KEYWORD_PRINT:
        parsePrint(node);
        break;
    case TOKEN_KEYWORD_IF:
        parseIf(node);
        break;
    case TOKEN_KEYWORD_WHILE:
        parseWhile(node);
        break;
    default:
        reportBadStatementStart();
        break;
    }
    if (panicMode) {
        addNode(node, "<syntax error: rest of the statement is skipped>", -1);
        recoverFromError();
    }
    return node;
}

// Prints to the screen AND to Parse Tree.txt.
static void output(const char *text) {
    fputs(text, stdout);
    fputs(text, treeFile);
}

// Prints a node and, under it, its children. A node with only one child is
// printed on the same line with →, like the doc's trees.
static void printTree(int node, const char *prefix, int isLast) {
    char line[TREE_LINE_MAX];
    char childPrefix[PREFIX_MAX];
    size_t used;
    int child;
    snprintf(line, TREE_LINE_MAX, "%s%s%s", prefix, isLast ? TREE_LAST : TREE_BRANCH, nodeLabel[node]);
    while (nodeToken[node] < 0 && nodeFirstChild[node] >= 0 && nodeNextSibling[nodeFirstChild[node]] < 0) {
        node = nodeFirstChild[node];
        used = strlen(line);
        snprintf(line + used, TREE_LINE_MAX - used, "%s%s", TREE_ARROW, nodeLabel[node]);
    }
    output(line);
    output("\n");
    snprintf(childPrefix, PREFIX_MAX, "%.1000s%s", prefix, isLast ? TREE_SPACE : TREE_PIPE);
    for (child = nodeFirstChild[node]; child >= 0; child = nodeNextSibling[child]) {
        printTree(child, childPrefix, nodeNextSibling[child] < 0);
    }
}

// PARSES THE STATEMENT IN THE BUFFER and prints its tree. After a recovery,
// the buffer may still hold another statement, so it loops until the buffer
// is used up. A } with no { is reported and skipped.
static void parseCollectedStatement(void) {
    int root;
    afterRecovery = 0;
    getToken();
    while (tokenType != TOKEN_END_OF_FILE) {
        if (tokenType == TOKEN_NEWLINE) {
            getToken();
            continue;
        }
        if (tokenType == TOKEN_RIGHT_BRACE) {
            syntaxError("unexpected '}' with no matching '{'.");
            panicMode = 0;
            getToken();
            continue;
        }
        nodeCount = 0;
        treeFull = 0;
        root = parseStatement(-1);
        printTree(root, "", 0);
        if (treeFull) {
            output(TREE_PIPE "(this statement is too large, some nodes are not shown)\n");
        }
    }
}

// Puts the errors in line order (doc 3.5 step 6).
static void sortErrorsByLine(void) {
    int i;
    int j;
    int line;
    char text[MESSAGE_MAX];
    for (i = 1; i < errorCount; i++) {
        line = errorLine[i];
        strcpy(text, errorText[i]);
        for (j = i - 1; j >= 0 && errorLine[j] > line; j--) {
            errorLine[j + 1] = errorLine[j];
            strcpy(errorText[j + 1], errorText[j]);
        }
        errorLine[j + 1] = line;
        strcpy(errorText[j + 1], text);
    }
}

// Parse Tree.txt is written in the same folder as the Symbol Table.
static void makeParseTreePath(const char *tablePath, char *treePath, size_t size) {
    int folderLength = 0;
    int i;
    for (i = 0; tablePath[i] != '\0'; i++) {
        if (tablePath[i] == '/' || tablePath[i] == '\\') {
            folderLength = i + 1;
        }
    }
    snprintf(treePath, size, "%.*sParse Tree.txt", folderLength, tablePath);
}

// MAIN: open the files, then repeat "collect one statement, parse it, print
// its tree" until EOF, then print the errors and the verdict.
int main(int argc, char *argv[]) {
    char treePath[PATH_MAX_LENGTH];
    char message[MESSAGE_MAX + 64];
    int i;

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    if (argc < 2) {
        printf("Usage: %s \"Symbol Table.txt\"\n", argv[0]);
        return 1;
    }
    symbolTable = fopen(argv[1], "r");
    if (symbolTable == NULL) {
        printf("Error: cannot open %s.\n", argv[1]);
        return 1;
    }
    makeParseTreePath(argv[1], treePath, sizeof treePath);
    treeFile = fopen(treePath, "w");
    if (treeFile == NULL) {
        printf("Error: cannot create %s.\n", treePath);
        fclose(symbolTable);
        return 1;
    }

    // The root of the whole parse tree. Each statement's tree is printed under it.
    output("<program>\n");
    for (;;) {
        collectStatement();
        if (statementType[0] == TOKEN_END_OF_FILE) {
            break;
        }
        parseCollectedStatement();
    }
    output(TREE_LAST "EOF\n\n");

    // All statements are done: print every syntax error in line order, then the
    // verdict. Only SYNTAX errors count (lexical errors are the lexer's job).
    sortErrorsByLine();
    for (i = 0; i < errorCount; i++) {
        snprintf(message, sizeof message, "Syntax error on line %d: %s\n", errorLine[i], errorText[i]);
        output(message);
    }
    if (errorCount > 0) {
        snprintf(message, sizeof message, "%d syntax error(s) found. The program is rejected.\n", errorCount);
    } else {
        snprintf(message, sizeof message, "No syntax error found. The program is accepted.\n");
    }
    output(message);
    printf("Parse tree written to %s.\n", treePath);

    fclose(symbolTable);
    fclose(treeFile);
    return errorCount > 0;
}
