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
#define TREE_LINE_MAX 16384
#define PREFIX_MAX 8192
#define MAX_STATEMENT_TOKENS 50000
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
static int statementTooLong;

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
static char treeLine[TREE_LINE_MAX];
static char treePrefix[PREFIX_MAX];

static int errorLine[MAX_ERRORS];
static char errorText[MAX_ERRORS][MESSAGE_MAX];
static int errorCount;
static int unlistedErrorCount;
static int panicMode;
static int afterRecovery;
static int offendingIsName;
static int missingBraceReported;

static int parse_statement(int parent);
static void parse_expression(int parent);

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
        statementTooLong = 1;
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
    statementTooLong = 0;
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
        unlistedErrorCount++;
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

static void parse_terminator(int parent, int afterParenthesis) {
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

static void parse_index(int parent) {
    int index;
    matchToken(parent);
    if (tokenType != TOKEN_RIGHT_BRACKET) {
        index = addNode(parent, "<index>", -1);
        parse_expression(index);
        if (!panicMode && tokenType == TOKEN_COLON) {
            matchToken(index);
            parse_expression(index);
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

static void parse_vector_lit(int parent) {
    int node = addNode(parent, "<vector_lit>", -1);
    matchToken(node);
    if (tokenType == TOKEN_RIGHT_BRACKET) {
        matchToken(node);
        return;
    }
    parse_expression(node);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(node);
        parse_expression(node);
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

static void parse_func_call(int parent) {
    int node = addNode(parent, "<func_call>", -1);
    char name[NAME_MAX];
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    matchToken(node);
    if (tokenType != TOKEN_LEFT_PARENTHESIS) {
        syntaxError("'(' expected after '%s'.", name);
        return;
    }
    matchToken(node);
    parse_expression(node);
    if (panicMode) {
        return;
    }
    if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
        syntaxError("')' expected to close '%s'.", name);
        return;
    }
    matchToken(node);
}

static void parse_primary(int parent) {
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
            parse_index(node);
        }
        return;
    case TOKEN_LEFT_PARENTHESIS:
        matchToken(node);
        parse_expression(node);
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
        parse_vector_lit(node);
        return;
    case TOKEN_FUNCTION_MEAN:
    case TOKEN_FUNCTION_MEDIAN:
    case TOKEN_FUNCTION_MODE:
    case TOKEN_FUNCTION_VARIANCE:
        parse_func_call(node);
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

static void parse_power(int parent) {
    int node = addNode(parent, "<power>", -1);
    parse_primary(node);
    if (!panicMode && tokenType == TOKEN_EXPONENT) {
        matchToken(node);
        parse_power(node);
    }
}

static void parse_unary(int parent) {
    int node = addNode(parent, "<unary>", -1);
    if (tokenType == TOKEN_KEYWORD_NOT || tokenType == TOKEN_MINUS) {
        matchToken(node);
        parse_unary(node);
        return;
    }
    parse_power(node);
}

static void parse_mul_expr(int parent) {
    int node = addNode(parent, "<mul_expr>", -1);
    parse_unary(node);
    while (!panicMode && tokenType >= TOKEN_MULTIPLY && tokenType <= TOKEN_MODULO) {
        matchToken(node);
        parse_unary(node);
    }
}

static void parse_add_expr(int parent) {
    int node = addNode(parent, "<add_expr>", -1);
    parse_mul_expr(node);
    while (!panicMode && (tokenType == TOKEN_PLUS || tokenType == TOKEN_MINUS)) {
        matchToken(node);
        parse_mul_expr(node);
    }
}

static void parse_rel_expr(int parent) {
    int node = addNode(parent, "<rel_expr>", -1);
    parse_add_expr(node);
    while (!panicMode && tokenType >= TOKEN_EQUAL && tokenType <= TOKEN_GREATER_THAN_OR_EQUAL) {
        matchToken(node);
        parse_add_expr(node);
    }
}

static void parse_and_expr(int parent) {
    int node = addNode(parent, "<and_expr>", -1);
    parse_rel_expr(node);
    while (!panicMode && tokenType == TOKEN_KEYWORD_AND) {
        matchToken(node);
        parse_rel_expr(node);
    }
}

static void parse_or_expr(int parent) {
    int node = addNode(parent, "<or_expr>", -1);
    parse_and_expr(node);
    while (!panicMode && tokenType == TOKEN_KEYWORD_OR) {
        matchToken(node);
        parse_and_expr(node);
    }
}

static void parse_expression(int parent) {
    int node = addNode(parent, "<expression>", -1);
    parse_or_expr(node);
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

static void parse_condition(int parent, const char *keyword) {
    if (!startsExpression(tokenType)) {
        syntaxError("condition expected after '%s'.", keyword);
    } else {
        parse_expression(addNode(parent, "<condition>", -1));
    }
    if (panicMode) {
        skipToBlock();
    }
}

static void parse_block(int parent, const char *name) {
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
            parse_statement(node);
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

static void parse_if(int parent) {
    int node = addNode(parent, "<if_stmt>", -1);
    int sawNewline;
    int sawElse = 0;
    matchToken(node);
    parse_condition(node, "if");
    skipOptionalNewline(node);
    parse_block(node, "if body");
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
            parse_block(node, "else body");
        } else {
            matchToken(node);
            parse_condition(node, "elif");
            skipOptionalNewline(node);
            parse_block(node, "elif body");
        }
    }
}

static void parse_while(int parent) {
    int node = addNode(parent, "<while_stmt>", -1);
    matchToken(node);
    parse_condition(node, "while");
    skipOptionalNewline(node);
    parse_block(node, "loop body");
    if (panicMode) {
        return;
    }
    skipOptionalNewline(node);
}

static void parse_input(int parent) {
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
    parse_terminator(node, 1);
}

static int peekType(int distance) {
    int position = statementPosition;
    if (distance == 0) {
        return tokenType;
    }
    while (position < statementLength) {
        if (statementType[position] != TOKEN_NEWLINE) {
            distance--;
            if (distance == 0) {
                return statementType[position];
            }
        }
        position++;
    }
    return TOKEN_END_OF_FILE;
}

static void parse_string_output(int parent) {
    matchToken(addNode(parent, "<string_output>", -1));
}

static void parse_variable_output(int parent) {
    matchToken(addNode(parent, "<variable_output>", -1));
}

static void parse_mixed_output(int parent) {
    int node = addNode(parent, "<mixed_output>", -1);
    matchToken(node);
    matchToken(node);
    matchToken(node);
}

static void parse_output_list(int parent) {
    int node = addNode(parent, "<output_list>", -1);
    parse_expression(node);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(node);
        parse_expression(node);
    }
}

static void parse_output_content(int parent) {
    int node = addNode(parent, "<output_content>", -1);
    if (tokenType == TOKEN_STRING_LITERAL && peekType(1) == TOKEN_RIGHT_PARENTHESIS) {
        parse_string_output(node);
    } else if (tokenType == TOKEN_IDENTIFIER && peekType(1) == TOKEN_RIGHT_PARENTHESIS) {
        parse_variable_output(node);
    } else if (tokenType == TOKEN_STRING_LITERAL && peekType(1) == TOKEN_COMMA
               && peekType(2) == TOKEN_IDENTIFIER && peekType(3) == TOKEN_RIGHT_PARENTHESIS) {
        parse_mixed_output(node);
    } else {
        parse_output_list(node);
    }
}

static void parse_output_stmt(int parent) {
    int node = addNode(parent, "<output_stmt>", -1);
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
    parse_output_content(node);
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
    parse_terminator(node, 1);
}

static void parse_assign_op(int parent) {
    if (!isAssignOperator(tokenType)) {
        syntaxError("assignment operator expected after '%s'.", previousLexeme);
        return;
    }
    matchToken(addNode(parent, "<assign_op>", -1));
}

static void parse_assignment(int parent) {
    int node = addNode(parent, "<assignment>", -1);
    matchToken(node);
    parse_assign_op(node);
    if (panicMode) {
        return;
    }
    parse_expression(node);
    if (panicMode) {
        return;
    }
    parse_terminator(node, 0);
}

static void parse_data_type(int parent) {
    matchToken(addNode(parent, "<data_type>", -1));
}

static void parse_declarator(int parent) {
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
        parse_expression(node);
    } else if (isAssignOperator(tokenType)) {
        syntaxError("'%s' cannot be used in a declaration, use '='.", lexeme);
    } else if (tokenType != TOKEN_COMMA && tokenType != TOKEN_NEWLINE && tokenType != TOKEN_END_OF_FILE) {
        syntaxError("',', '=' or end of line expected after '%s'.", name);
    }
}

static void parse_declaration(int parent) {
    int node = addNode(parent, "<declaration>", -1);
    parse_data_type(node);
    parse_declarator(node);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(node);
        parse_declarator(node);
    }
    if (panicMode) {
        return;
    }
    parse_terminator(node, 0);
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

static int parse_statement(int parent) {
    int node = addNode(parent, "<statement>", -1);
    switch (tokenType) {
    case TOKEN_KEYWORD_INTEGER:
    case TOKEN_KEYWORD_DECIMAL:
    case TOKEN_KEYWORD_CHARACTER:
    case TOKEN_KEYWORD_STRING:
    case TOKEN_KEYWORD_BOOLEAN:
    case TOKEN_KEYWORD_VECTOR:
        parse_declaration(node);
        break;
    case TOKEN_IDENTIFIER:
        parse_assignment(node);
        break;
    case TOKEN_KEYWORD_INPUT:
        parse_input(node);
        break;
    case TOKEN_KEYWORD_PRINT:
        parse_output_stmt(node);
        break;
    case TOKEN_KEYWORD_IF:
        parse_if(node);
        break;
    case TOKEN_KEYWORD_WHILE:
        parse_while(node);
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

static void printTree(int node, size_t prefixLength, int isLast) {
    size_t used;
    size_t childLength = prefixLength;
    int child;
    treePrefix[prefixLength] = '\0';
    snprintf(treeLine, TREE_LINE_MAX, "%s%s%s", treePrefix, isLast ? TREE_LAST : TREE_BRANCH, nodeLabel[node]);
    while (nodeToken[node] < 0 && nodeFirstChild[node] >= 0 && nodeNextSibling[nodeFirstChild[node]] < 0) {
        node = nodeFirstChild[node];
        used = strlen(treeLine);
        snprintf(treeLine + used, TREE_LINE_MAX - used, "%s%s", TREE_ARROW, nodeLabel[node]);
    }
    output(treeLine);
    output("\n");
    if (prefixLength + 8 < PREFIX_MAX) {
        strcpy(treePrefix + prefixLength, isLast ? TREE_SPACE : TREE_PIPE);
        childLength = strlen(treePrefix);
    }
    for (child = nodeFirstChild[node]; child >= 0; child = nodeNextSibling[child]) {
        printTree(child, childLength, nodeNextSibling[child] < 0);
    }
}

static void parseCollectedStatement(void) {
    char message[MESSAGE_MAX];
    int root;
    if (statementTooLong) {
        snprintf(message, MESSAGE_MAX, "statement is too long (more than %d tokens), it was not parsed.", MAX_STATEMENT_TOKENS);
        addError(statementLine[0], message);
        return;
    }
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
        root = parse_statement(-1);
        printTree(root, 0, 0);
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
    if (unlistedErrorCount > 0) {
        snprintf(message, sizeof message, "(only the first %d errors are listed)\n", MAX_ERRORS);
        output(message);
    }
    if (errorCount > 0) {
        snprintf(message, sizeof message, "%d syntax error(s) found. The program is rejected.\n", errorCount + unlistedErrorCount);
    } else {
        snprintf(message, sizeof message, "No syntax error found. The program is accepted.\n");
    }
    output(message);
    printf("Parse tree written to %s.\n", treePath);

    fclose(symbolTable);
    fclose(treeFile);
    return errorCount > 0;
}
