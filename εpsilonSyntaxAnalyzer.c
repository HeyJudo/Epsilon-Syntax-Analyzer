#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
__declspec(dllimport) int __stdcall SetConsoleOutputCP(unsigned int codePage);
#endif

enum {
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

#define TREE_BRANCH "\xE2\x94\x9C\xE2\x94\x80\xE2\x94\x80 "
#define TREE_LAST "\xE2\x94\x94\xE2\x94\x80\xE2\x94\x80 "
#define TREE_PIPE "\xE2\x94\x82   "
#define TREE_SPACE "    "
#define TREE_ARROW " \xE2\x86\x92 "

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

static FILE *symbolTable;
static FILE *treeFile;

static int readType;
static int readLine;
static char readLexeme[LEXEME_MAX];
static int lastReadType = TOKEN_NEWLINE;
static int holdingToken;

static int statementType[MAX_STATEMENT_TOKENS];
static int statementLine[MAX_STATEMENT_TOKENS];
static char statementLexeme[MAX_STATEMENT_TOKENS][LEXEME_MAX];
static int statementLength;
static int statementPosition;

static int tokenType;
static int tokenLine;
static char lexeme[LEXEME_MAX];
static char previousLexeme[LEXEME_MAX];
static int groupDepth;

static char nodeLabel[MAX_NODES][LABEL_MAX];
static int nodeToken[MAX_NODES];
static int nodeFirstChild[MAX_NODES];
static int nodeLastChild[MAX_NODES];
static int nodeNextSibling[MAX_NODES];
static int nodeCount;
static int treeFull;

static int errorLine[MAX_ERRORS];
static char errorText[MAX_ERRORS][MESSAGE_MAX];
static int errorCount;
static int panicMode;
static int afterRecovery;
static int offendingIsName;
static int missingBraceReported;

static int parseStatement(int parent);
static void parseExpression(int parent);

static int tokenFromName(const char *name) {
    int type;
    for (type = 0; type < TOKEN_TYPE_COUNT; type++) {
        if (strcmp(name, tokenName[type]) == 0) {
            return type;
        }
    }
    return -1;
}

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

static void copyLexeme(char *destination, const char *source) {
    int length = 0;
    while (source[length] != '\0' && source[length] != '\n' && source[length] != '\r' && length < LEXEME_MAX - 1) {
        destination[length] = source[length];
        length++;
    }
    destination[length] = '\0';
}

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

static int isDiscarded(int type) {
    return type == TOKEN_ERROR
        || type == TOKEN_LINE_COMMENT
        || type == TOKEN_BLOCK_COMMENT
        || (type >= TOKEN_NOISE_WORD_LET && type <= TOKEN_NOISE_WORD_USING);
}

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

static void holdToken(void) {
    holdingToken = 1;
}

static void addToStatement(void) {
    if (statementLength >= MAX_STATEMENT_TOKENS) {
        return;
    }
    statementType[statementLength] = readType;
    statementLine[statementLength] = readLine;
    strcpy(statementLexeme[statementLength], readLexeme);
    statementLength++;
}

static int isStatementKeyword(int type) {
    return (type >= TOKEN_KEYWORD_INTEGER && type <= TOKEN_KEYWORD_IF) || type == TOKEN_KEYWORD_WHILE;
}

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

static void addError(int line, const char *message) {
    if (errorCount >= MAX_ERRORS) {
        return;
    }
    errorLine[errorCount] = line;
    snprintf(errorText[errorCount], MESSAGE_MAX, "%s", message);
    errorCount++;
}

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

static void reservedWordError(void) {
    if (!panicMode) {
        offendingIsName = 1;
    }
    syntaxError("'%s' is a reserved word and cannot be a variable name.", lexeme);
}

static int isReservedWord(int type) {
    return type >= TOKEN_KEYWORD_INTEGER && type <= TOKEN_FUNCTION_VARIANCE;
}

static int isAssignOperator(int type) {
    return type >= TOKEN_ASSIGN && type <= TOKEN_EXPONENT_ASSIGN;
}

static int startsExpression(int type) {
    return (type >= TOKEN_IDENTIFIER && type <= TOKEN_STRING_LITERAL)
        || (type >= TOKEN_KEYWORD_NOT && type <= TOKEN_FUNCTION_VARIANCE)
        || type == TOKEN_MINUS
        || type == TOKEN_LEFT_PARENTHESIS
        || type == TOKEN_LEFT_BRACKET;
}

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

static void skipOptionalNewline(int parent) {
    if (tokenType == TOKEN_NEWLINE) {
        matchToken(parent);
    }
}

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

static void parsePower(int parent) {
    int node = addNode(parent, "<power>", -1);
    parsePrimary(node);
    if (!panicMode && tokenType == TOKEN_EXPONENT) {
        matchToken(node);
        parsePower(node);
    }
}

static void parseUnary(int parent) {
    int node = addNode(parent, "<unary>", -1);
    if (tokenType == TOKEN_KEYWORD_NOT || tokenType == TOKEN_MINUS) {
        matchToken(node);
        parseUnary(node);
        return;
    }
    parsePower(node);
}

static void parseMultiplicative(int parent) {
    int node = addNode(parent, "<mul_expr>", -1);
    parseUnary(node);
    while (!panicMode && tokenType >= TOKEN_MULTIPLY && tokenType <= TOKEN_MODULO) {
        matchToken(node);
        parseUnary(node);
    }
}

static void parseAdditive(int parent) {
    int node = addNode(parent, "<add_expr>", -1);
    parseMultiplicative(node);
    while (!panicMode && (tokenType == TOKEN_PLUS || tokenType == TOKEN_MINUS)) {
        matchToken(node);
        parseMultiplicative(node);
    }
}

static void parseRelational(int parent) {
    int node = addNode(parent, "<rel_expr>", -1);
    parseAdditive(node);
    while (!panicMode && tokenType >= TOKEN_EQUAL && tokenType <= TOKEN_GREATER_THAN_OR_EQUAL) {
        matchToken(node);
        parseAdditive(node);
    }
}

static void parseAnd(int parent) {
    int node = addNode(parent, "<and_expr>", -1);
    parseRelational(node);
    while (!panicMode && tokenType == TOKEN_KEYWORD_AND) {
        matchToken(node);
        parseRelational(node);
    }
}

static void parseOr(int parent) {
    int node = addNode(parent, "<or_expr>", -1);
    parseAnd(node);
    while (!panicMode && tokenType == TOKEN_KEYWORD_OR) {
        matchToken(node);
        parseAnd(node);
    }
}

static void parseExpression(int parent) {
    int node = addNode(parent, "<expression>", -1);
    parseOr(node);
}

static void skipToBlock(void) {
    groupDepth = 0;
    while (tokenType != TOKEN_LEFT_BRACE && tokenType != TOKEN_NEWLINE
           && tokenType != TOKEN_RIGHT_BRACE && tokenType != TOKEN_END_OF_FILE) {
        getToken();
    }
    panicMode = 0;
    afterRecovery = 1;
}

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

static void output(const char *text) {
    fputs(text, stdout);
    fputs(text, treeFile);
}

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

    output("<program>\n");
    for (;;) {
        collectStatement();
        if (statementType[0] == TOKEN_END_OF_FILE) {
            break;
        }
        parseCollectedStatement();
    }
    output(TREE_LAST "EOF\n\n");

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
