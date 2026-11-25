#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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

enum {
    TYPE_UNKNOWN,
    TYPE_INTEGER,
    TYPE_DECIMAL,
    TYPE_CHARACTER,
    TYPE_STRING,
    TYPE_BOOLEAN,
    TYPE_VECTOR,
    TYPE_NUMBER,
    TYPE_COUNT
};

static const char *typeName[TYPE_COUNT] = {
    [TYPE_UNKNOWN] = "unknown",
    [TYPE_INTEGER] = "integer",
    [TYPE_DECIMAL] = "decimal",
    [TYPE_CHARACTER] = "character",
    [TYPE_STRING] = "string",
    [TYPE_BOOLEAN] = "boolean",
    [TYPE_VECTOR] = "vector",
    [TYPE_NUMBER] = "number"
};

#define ROW_MAX 4096
#define LEXEME_MAX 256
#define LABEL_MAX 200
#define LINE_MAX 4096
#define PREFIX_MAX 1024
#define MAX_NODES 20000
#define NAME_MAX 64
#define MAX_VARIABLES 500
#define MAX_ERRORS 500
#define MESSAGE_MAX 256
#define PATH_MAX_LENGTH 1024

static FILE *symbolTable;
static FILE *treeFile;

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

static char variableName[MAX_VARIABLES][NAME_MAX];
static int variableType[MAX_VARIABLES];
static int variableCount;

static int errorLine[MAX_ERRORS];
static char errorText[MAX_ERRORS][MESSAGE_MAX];
static int errorCount;
static int lexicalErrorCount;
static int pendingLine[MAX_ERRORS];
static char pendingText[MAX_ERRORS][MESSAGE_MAX];
static int pendingCount;
static int panicMode;
static int afterRecovery;
static int offendingIsName;
static int missingBraceReported;

static int parseStatement(int parent);
static int parseExpression(int parent);

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

static void copyLexeme(const char *start) {
    int length = 0;
    while (start[length] != '\0' && start[length] != '\n' && start[length] != '\r' && length < LEXEME_MAX - 1) {
        lexeme[length] = start[length];
        length++;
    }
    lexeme[length] = '\0';
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
        if (type == TOKEN_ERROR) {
            lexicalErrorCount++;
        }
        tokenType = type;
        tokenLine = line;
        copyLexeme(findLexeme(row));
        return;
    }
    tokenType = TOKEN_END_OF_FILE;
    lexeme[0] = '\0';
}

static int isDiscarded(int type) {
    return type == TOKEN_ERROR
        || type == TOKEN_LINE_COMMENT
        || type == TOKEN_BLOCK_COMMENT
        || (type >= TOKEN_NOISE_WORD_LET && type <= TOKEN_NOISE_WORD_USING);
}

static void getToken(void) {
    int previousType = tokenType;
    strcpy(previousLexeme, lexeme);
    do {
        readRow();
    } while (isDiscarded(tokenType)
             || (tokenType == TOKEN_NEWLINE && (groupDepth > 0 || previousType == TOKEN_NEWLINE)));
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

static void appendLabel(int node, const char *text) {
    size_t used;
    if (node < 0) {
        return;
    }
    used = strlen(nodeLabel[node]);
    snprintf(nodeLabel[node] + used, LABEL_MAX - used, "%s", text);
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

static void semanticError(int line, const char *format, ...) {
    va_list arguments;
    if (panicMode || pendingCount >= MAX_ERRORS) {
        return;
    }
    va_start(arguments, format);
    vsnprintf(pendingText[pendingCount], MESSAGE_MAX, format, arguments);
    va_end(arguments);
    pendingLine[pendingCount] = line;
    pendingCount++;
}

static void keepPendingErrors(int mark) {
    int i;
    if (!panicMode) {
        for (i = mark; i < pendingCount; i++) {
            addError(pendingLine[i], pendingText[i]);
        }
    }
    pendingCount = mark;
}

static int findVariable(const char *name) {
    int i;
    for (i = 0; i < variableCount; i++) {
        if (strcmp(variableName[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

static void declareVariable(const char *name, int type) {
    int index = findVariable(name);
    if (index < 0) {
        if (variableCount >= MAX_VARIABLES) {
            return;
        }
        index = variableCount++;
        snprintf(variableName[index], NAME_MAX, "%s", name);
    }
    variableType[index] = type;
}

static int lookupVariable(int leaf, const char *name, int line) {
    char note[NAME_MAX];
    int index = findVariable(name);
    if (index < 0) {
        semanticError(line, "variable '%s' is not declared.", name);
        appendLabel(leaf, "  (not declared)");
        return TYPE_UNKNOWN;
    }
    snprintf(note, NAME_MAX, "  (%s, declared)", typeName[variableType[index]]);
    appendLabel(leaf, note);
    return variableType[index];
}

static int isNumber(int type) {
    return type == TYPE_INTEGER || type == TYPE_DECIMAL || type == TYPE_NUMBER;
}

static int canAssign(int target, int value) {
    if (target == TYPE_UNKNOWN || value == TYPE_UNKNOWN || target == value) {
        return 1;
    }
    if (target == TYPE_DECIMAL && (value == TYPE_INTEGER || value == TYPE_NUMBER)) {
        return 1;
    }
    if (target == TYPE_INTEGER && value == TYPE_NUMBER) {
        return 1;
    }
    return 0;
}

static void checkBoolean(int type, const char *word, int line) {
    if (type != TYPE_UNKNOWN && type != TYPE_BOOLEAN) {
        semanticError(line, "'%s' needs boolean values, found %s.", word, typeName[type]);
    }
}

static int checkNumeric(int type, const char *operatorText, int line) {
    if (type == TYPE_UNKNOWN) {
        return 0;
    }
    if (!isNumber(type) && type != TYPE_VECTOR) {
        semanticError(line, "'%s' needs numbers or vectors, found %s.", operatorText, typeName[type]);
        return 0;
    }
    return 1;
}

static int arithmeticType(int left, int right, int operatorToken, const char *operatorText, int line) {
    int leftIsGood = checkNumeric(left, operatorText, line);
    int rightIsGood = checkNumeric(right, operatorText, line);
    if (!leftIsGood || !rightIsGood) {
        return TYPE_UNKNOWN;
    }
    if (left == TYPE_VECTOR || right == TYPE_VECTOR) {
        return TYPE_VECTOR;
    }
    if (operatorToken == TOKEN_DIVIDE) {
        return TYPE_DECIMAL;
    }
    if (operatorToken == TOKEN_FLOOR_DIVIDE) {
        return TYPE_INTEGER;
    }
    if (left == TYPE_INTEGER && right == TYPE_INTEGER) {
        return TYPE_INTEGER;
    }
    if (left == TYPE_DECIMAL || right == TYPE_DECIMAL) {
        return TYPE_DECIMAL;
    }
    return TYPE_NUMBER;
}

static int isReservedWord(int type) {
    return type >= TOKEN_KEYWORD_INTEGER && type <= TOKEN_FUNCTION_VARIANCE;
}

static int isStatementKeyword(int type) {
    return (type >= TOKEN_KEYWORD_INTEGER && type <= TOKEN_KEYWORD_IF) || type == TOKEN_KEYWORD_WHILE;
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

static int declaredType(int keyword) {
    switch (keyword) {
    case TOKEN_KEYWORD_INTEGER: return TYPE_INTEGER;
    case TOKEN_KEYWORD_DECIMAL: return TYPE_DECIMAL;
    case TOKEN_KEYWORD_CHARACTER: return TYPE_CHARACTER;
    case TOKEN_KEYWORD_STRING: return TYPE_STRING;
    case TOKEN_KEYWORD_BOOLEAN: return TYPE_BOOLEAN;
    default: return TYPE_VECTOR;
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

static void checkIndex(int type, int line) {
    if (type != TYPE_UNKNOWN && type != TYPE_INTEGER && type != TYPE_NUMBER) {
        semanticError(line, "an index must be an integer, found %s.", typeName[type]);
    }
}

static int parseIndexing(int parent, const char *name, int type, int line) {
    int index;
    int wholeOrSlice = 1;
    matchToken(parent);
    if (tokenType != TOKEN_RIGHT_BRACKET) {
        index = addNode(parent, "<index>", -1);
        checkIndex(parseExpression(index), line);
        wholeOrSlice = 0;
        if (!panicMode && tokenType == TOKEN_COLON) {
            matchToken(index);
            checkIndex(parseExpression(index), line);
            wholeOrSlice = 1;
        }
        if (panicMode) {
            return TYPE_UNKNOWN;
        }
    }
    if (tokenType != TOKEN_RIGHT_BRACKET) {
        syntaxError("']' expected to close the index.");
        return TYPE_UNKNOWN;
    }
    matchToken(parent);
    if (type != TYPE_VECTOR && type != TYPE_UNKNOWN) {
        semanticError(line, "'%s' is a %s, not a vector, so it cannot be indexed.", name, typeName[type]);
        return TYPE_UNKNOWN;
    }
    return wholeOrSlice ? TYPE_VECTOR : TYPE_NUMBER;
}

static int parseVariableUse(int parent) {
    char name[NAME_MAX];
    int line = tokenLine;
    int leaf;
    int type;
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    leaf = matchToken(parent);
    type = lookupVariable(leaf, name, line);
    if (tokenType != TOKEN_LEFT_BRACKET) {
        return type;
    }
    return parseIndexing(parent, name, type, line);
}

static void parseVectorValue(int parent) {
    int line = tokenLine;
    int type = parseExpression(parent);
    if (type != TYPE_UNKNOWN && !isNumber(type)) {
        semanticError(line, "a vector can only hold numbers, found %s.", typeName[type]);
    }
}

static int parseVectorLiteral(int parent) {
    int node = addNode(parent, "<vector_lit>", -1);
    matchToken(node);
    if (tokenType == TOKEN_RIGHT_BRACKET) {
        matchToken(node);
        return TYPE_VECTOR;
    }
    parseVectorValue(node);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(node);
        parseVectorValue(node);
    }
    if (panicMode) {
        return TYPE_UNKNOWN;
    }
    if (tokenType != TOKEN_RIGHT_BRACKET) {
        syntaxError("']' expected to close the vector.");
        return TYPE_UNKNOWN;
    }
    matchToken(node);
    return TYPE_VECTOR;
}

static int parseFunctionCall(int parent) {
    int node = addNode(parent, "<func_call>", -1);
    int function = tokenType;
    int line = tokenLine;
    int argument;
    char name[NAME_MAX];
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    matchToken(node);
    if (tokenType != TOKEN_LEFT_PARENTHESIS) {
        syntaxError("'(' expected after '%s'.", name);
        return TYPE_UNKNOWN;
    }
    matchToken(node);
    argument = parseExpression(node);
    if (panicMode) {
        return TYPE_UNKNOWN;
    }
    if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
        syntaxError("')' expected to close '%s'.", name);
        return TYPE_UNKNOWN;
    }
    matchToken(node);
    if (argument != TYPE_VECTOR && argument != TYPE_UNKNOWN) {
        semanticError(line, "'%s' needs a vector, found %s.", name, typeName[argument]);
    }
    return function == TOKEN_FUNCTION_MODE ? TYPE_NUMBER : TYPE_DECIMAL;
}

static int parsePrimary(int parent) {
    int node = addNode(parent, "<primary>", -1);
    int type;
    switch (tokenType) {
    case TOKEN_INTEGER_LITERAL:
        matchToken(node);
        return TYPE_INTEGER;
    case TOKEN_DECIMAL_LITERAL:
        matchToken(node);
        return TYPE_DECIMAL;
    case TOKEN_CHARACTER_LITERAL:
        matchToken(node);
        return TYPE_CHARACTER;
    case TOKEN_STRING_LITERAL:
        matchToken(node);
        return TYPE_STRING;
    case TOKEN_KEYWORD_TRUE:
    case TOKEN_KEYWORD_FALSE:
        matchToken(node);
        return TYPE_BOOLEAN;
    case TOKEN_IDENTIFIER:
        return parseVariableUse(node);
    case TOKEN_LEFT_PARENTHESIS:
        matchToken(node);
        type = parseExpression(node);
        if (panicMode) {
            return TYPE_UNKNOWN;
        }
        if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
            syntaxError("')' expected to close the expression.");
            return TYPE_UNKNOWN;
        }
        matchToken(node);
        return type;
    case TOKEN_LEFT_BRACKET:
        return parseVectorLiteral(node);
    case TOKEN_FUNCTION_MEAN:
    case TOKEN_FUNCTION_MEDIAN:
    case TOKEN_FUNCTION_MODE:
    case TOKEN_FUNCTION_VARIANCE:
        return parseFunctionCall(node);
    default:
        if (isReservedWord(tokenType)) {
            reservedWordError();
        } else {
            syntaxError("expression expected after '%s'.", previousLexeme);
        }
        return TYPE_UNKNOWN;
    }
}

static int parsePower(int parent) {
    int node = addNode(parent, "<power>", -1);
    int type = parsePrimary(node);
    int line;
    if (!panicMode && tokenType == TOKEN_EXPONENT) {
        line = tokenLine;
        matchToken(node);
        type = arithmeticType(type, parsePower(node), TOKEN_EXPONENT, "^", line);
    }
    return type;
}

static int parseUnary(int parent) {
    int node = addNode(parent, "<unary>", -1);
    int operatorToken = tokenType;
    int line = tokenLine;
    int type;
    if (operatorToken != TOKEN_KEYWORD_NOT && operatorToken != TOKEN_MINUS) {
        return parsePower(node);
    }
    matchToken(node);
    type = parseUnary(node);
    if (operatorToken == TOKEN_KEYWORD_NOT) {
        checkBoolean(type, "not", line);
        return TYPE_BOOLEAN;
    }
    return checkNumeric(type, "-", line) ? type : TYPE_UNKNOWN;
}

static int isMultiplyOperator(int type) {
    return type >= TOKEN_MULTIPLY && type <= TOKEN_MODULO;
}

static int parseMultiplicative(int parent) {
    int node = addNode(parent, "<mul_expr>", -1);
    int type = parseUnary(node);
    int operatorToken;
    int line;
    char operatorText[8];
    while (!panicMode && isMultiplyOperator(tokenType)) {
        operatorToken = tokenType;
        line = tokenLine;
        snprintf(operatorText, sizeof operatorText, "%.7s", lexeme);
        matchToken(node);
        type = arithmeticType(type, parseUnary(node), operatorToken, operatorText, line);
    }
    return type;
}

static int parseAdditive(int parent) {
    int node = addNode(parent, "<add_expr>", -1);
    int type = parseMultiplicative(node);
    int operatorToken;
    int line;
    char operatorText[8];
    while (!panicMode && (tokenType == TOKEN_PLUS || tokenType == TOKEN_MINUS)) {
        operatorToken = tokenType;
        line = tokenLine;
        snprintf(operatorText, sizeof operatorText, "%.7s", lexeme);
        matchToken(node);
        type = arithmeticType(type, parseMultiplicative(node), operatorToken, operatorText, line);
    }
    return type;
}

static int isRelationalOperator(int type) {
    return type >= TOKEN_EQUAL && type <= TOKEN_GREATER_THAN_OR_EQUAL;
}

static int parseRelational(int parent) {
    int node = addNode(parent, "<rel_expr>", -1);
    int type = parseAdditive(node);
    int right;
    int operatorToken;
    int line;
    char operatorText[8];
    while (!panicMode && isRelationalOperator(tokenType)) {
        operatorToken = tokenType;
        line = tokenLine;
        snprintf(operatorText, sizeof operatorText, "%.7s", lexeme);
        matchToken(node);
        right = parseAdditive(node);
        if (operatorToken != TOKEN_EQUAL && operatorToken != TOKEN_NOT_EQUAL) {
            if (type != TYPE_UNKNOWN && !isNumber(type)) {
                semanticError(line, "'%s' needs numbers, found %s.", operatorText, typeName[type]);
            }
            if (right != TYPE_UNKNOWN && !isNumber(right)) {
                semanticError(line, "'%s' needs numbers, found %s.", operatorText, typeName[right]);
            }
        }
        type = TYPE_BOOLEAN;
    }
    return type;
}

static int parseAnd(int parent) {
    int node = addNode(parent, "<and_expr>", -1);
    int type = parseRelational(node);
    int line;
    while (!panicMode && tokenType == TOKEN_KEYWORD_AND) {
        line = tokenLine;
        matchToken(node);
        checkBoolean(type, "and", line);
        checkBoolean(parseRelational(node), "and", line);
        type = TYPE_BOOLEAN;
    }
    return type;
}

static int parseOr(int parent) {
    int node = addNode(parent, "<or_expr>", -1);
    int type = parseAnd(node);
    int line;
    while (!panicMode && tokenType == TOKEN_KEYWORD_OR) {
        line = tokenLine;
        matchToken(node);
        checkBoolean(type, "or", line);
        checkBoolean(parseAnd(node), "or", line);
        type = TYPE_BOOLEAN;
    }
    return type;
}

static int parseExpression(int parent) {
    int node = addNode(parent, "<expression>", -1);
    return parseOr(node);
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

static void parseCondition(int parent, const char *keyword, int isLoop) {
    int mark = pendingCount;
    int line = tokenLine;
    int node;
    int type;
    if (!startsExpression(tokenType)) {
        syntaxError("condition expected after '%s'.", keyword);
    } else {
        node = addNode(parent, "<condition>", -1);
        type = parseExpression(node);
        if (!panicMode && type != TYPE_BOOLEAN && type != TYPE_UNKNOWN) {
            if (isLoop) {
                semanticError(line, "loop condition must be boolean, found %s.", typeName[type]);
            } else {
                semanticError(line, "condition of '%s' must be boolean, found %s.", keyword, typeName[type]);
            }
        }
    }
    if (panicMode) {
        pendingCount = mark;
        skipToBlock();
        return;
    }
    keepPendingErrors(mark);
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
    parseCondition(node, "if", 0);
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
            parseCondition(node, "elif", 0);
            skipOptionalNewline(node);
            parseBlock(node, "elif body");
        }
    }
}

static void parseWhile(int parent) {
    int node = addNode(parent, "<while_stmt>", -1);
    matchToken(node);
    parseCondition(node, "while", 1);
    skipOptionalNewline(node);
    parseBlock(node, "loop body");
    if (panicMode) {
        return;
    }
    skipOptionalNewline(node);
}

static void parseInput(int parent) {
    int node = addNode(parent, "<input_stmt>", -1);
    int mark = pendingCount;
    int line;
    int leaf;
    int type;
    char name[NAME_MAX];
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
    line = tokenLine;
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    leaf = matchToken(node);
    if (tokenType != TOKEN_RIGHT_PARENTHESIS) {
        syntaxError("')' expected to close 'input'.");
        return;
    }
    matchToken(node);
    parseTerminator(node, 1);
    if (panicMode) {
        return;
    }
    type = lookupVariable(leaf, name, line);
    if (type == TYPE_VECTOR) {
        semanticError(line, "cannot read input into vector '%s'.", name);
    }
    keepPendingErrors(mark);
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
    int mark = pendingCount;
    int content;
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
    content = addNode(node, "<output_content>", -1);
    list = addNode(content, "<output_list>", -1);
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
    if (panicMode) {
        return;
    }
    nameOutputForm(list);
    keepPendingErrors(mark);
}

static void parseAssignment(int parent) {
    int node = addNode(parent, "<assignment>", -1);
    int mark = pendingCount;
    int line = tokenLine;
    int target;
    int value;
    int operatorToken;
    int leaf;
    char name[NAME_MAX];
    char operatorText[8];
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    leaf = matchToken(node);
    target = lookupVariable(leaf, name, line);
    if (!isAssignOperator(tokenType)) {
        syntaxError("assignment operator expected after '%s'.", name);
        return;
    }
    operatorToken = tokenType;
    snprintf(operatorText, sizeof operatorText, "%.7s", lexeme);
    matchToken(addNode(node, "<assign_op>", -1));
    value = parseExpression(node);
    if (panicMode) {
        return;
    }
    parseTerminator(node, 0);
    if (panicMode) {
        return;
    }
    if (operatorToken != TOKEN_ASSIGN) {
        int result = operatorToken == TOKEN_DIVIDE_ASSIGN ? TYPE_DECIMAL : value;
        if (target != TYPE_UNKNOWN && target != TYPE_INTEGER && target != TYPE_DECIMAL && target != TYPE_VECTOR) {
            semanticError(line, "'%s' needs an integer, decimal, or vector variable, found %s '%s'.",
                          operatorText, typeName[target], name);
        } else if (checkNumeric(value, operatorText, line) && target != TYPE_VECTOR && !canAssign(target, result)) {
            semanticError(line, "cannot assign %s to %s variable '%s'.", typeName[result], typeName[target], name);
        }
    } else if (!canAssign(target, value)) {
        semanticError(line, "cannot assign %s to %s variable '%s'.", typeName[value], typeName[target], name);
    }
    keepPendingErrors(mark);
}

static void parseDeclarator(int parent, int type) {
    int node = addNode(parent, "<declarator>", -1);
    int line;
    int leaf;
    int value;
    char name[NAME_MAX];
    char note[NAME_MAX];
    if (tokenType != TOKEN_IDENTIFIER) {
        if (isReservedWord(tokenType)) {
            reservedWordError();
        } else {
            syntaxError("identifier expected after '%s'.", previousLexeme);
        }
        return;
    }
    line = tokenLine;
    snprintf(name, NAME_MAX, "%.63s", lexeme);
    leaf = matchToken(node);
    if (tokenType == TOKEN_ASSIGN) {
        matchToken(node);
        value = parseExpression(node);
        if (!panicMode && !canAssign(type, value)) {
            semanticError(line, "cannot assign %s to %s variable '%s'.", typeName[value], typeName[type], name);
        }
    } else if (isAssignOperator(tokenType)) {
        syntaxError("'%s' cannot be used in a declaration, use '='.", lexeme);
    } else if (tokenType != TOKEN_COMMA && tokenType != TOKEN_NEWLINE && tokenType != TOKEN_END_OF_FILE) {
        syntaxError("',', '=' or end of line expected after '%s'.", name);
    }
    declareVariable(name, type);
    snprintf(note, NAME_MAX, "  (declared as %s)", typeName[type]);
    appendLabel(leaf, note);
}

static void parseDeclaration(int parent) {
    int node = addNode(parent, "<declaration>", -1);
    int mark = pendingCount;
    int type = declaredType(tokenType);
    matchToken(addNode(node, "<data_type>", -1));
    parseDeclarator(node, type);
    while (!panicMode && tokenType == TOKEN_COMMA) {
        matchToken(node);
        parseDeclarator(node, type);
    }
    if (panicMode) {
        return;
    }
    parseTerminator(node, 0);
    if (panicMode) {
        return;
    }
    keepPendingErrors(mark);
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
    int mark = pendingCount;
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
        pendingCount = mark;
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
    char line[LINE_MAX];
    char childPrefix[PREFIX_MAX];
    size_t used;
    int child;
    snprintf(line, LINE_MAX, "%s%s%s", prefix, isLast ? "`-- " : "|-- ", nodeLabel[node]);
    while (nodeToken[node] < 0 && nodeFirstChild[node] >= 0 && nodeNextSibling[nodeFirstChild[node]] < 0) {
        node = nodeFirstChild[node];
        used = strlen(line);
        snprintf(line + used, LINE_MAX - used, " -> %s", nodeLabel[node]);
    }
    output(line);
    output("\n");
    snprintf(childPrefix, PREFIX_MAX, "%.1000s%s", prefix, isLast ? "    " : "|   ");
    for (child = nodeFirstChild[node]; child >= 0; child = nodeNextSibling[child]) {
        printTree(child, childPrefix, nodeNextSibling[child] < 0);
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
    int root;
    int i;

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

    getToken();
    output("<program>\n");
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
            output("|   (this statement is too large, some nodes are not shown)\n");
        }
    }
    output("`-- EOF\n\n");

    sortErrorsByLine();
    for (i = 0; i < errorCount; i++) {
        snprintf(message, sizeof message, "Syntax error on line %d: %s\n", errorLine[i], errorText[i]);
        output(message);
    }
    if (errorCount > 0 && lexicalErrorCount > 0) {
        snprintf(message, sizeof message, "%d syntax error(s) found, and the lexer reported %d lexical error(s). The program is rejected.\n", errorCount, lexicalErrorCount);
    } else if (errorCount > 0) {
        snprintf(message, sizeof message, "%d syntax error(s) found. The program is rejected.\n", errorCount);
    } else if (lexicalErrorCount > 0) {
        snprintf(message, sizeof message, "No syntax error found, but the lexer reported %d lexical error(s). The program is rejected.\n", lexicalErrorCount);
    } else {
        snprintf(message, sizeof message, "No syntax error found. The program is accepted.\n");
    }
    output(message);
    printf("Parse tree written to %s.\n", treePath);

    fclose(symbolTable);
    fclose(treeFile);
    return errorCount > 0 || lexicalErrorCount > 0;
}
