# How the εpsilon Syntax Analyzer Works

A guide to `εpsilonSyntaxAnalyzer.c` (the commented copy in this folder).
Read it next to the code: every function named here has a comment block above it.

---

## 1. The big picture

The lexer turns a `.epl` file into tokens and writes them in `Symbol Table.txt`.
The syntax analyzer (the **parser**) reads that file and checks that the tokens
are in a valid **order**, using the grammar rules of the Syntax Analyzer document.
For every statement it draws a **parse tree**.

```
 program.epl ──► LEXER ──► Symbol Table.txt ──► PARSER ──► parse trees
                                 │                 │       + syntax errors
                           one row = one token     │       (screen and
                                                   │        Parse Tree.txt)
   readRow() reads ONE row ──► statement buffer ──► parse ──► print tree
          ▲                    (one statement)                   │
          └──────────────── repeat until EOF ◄───────────────────┘
```

It checks **syntax only**. "Is this variable declared?" and "does this type fit?"
are questions for the semantic analyzer (the next program), so this one never
asks them. Example: `x = "hello" + 5` has a correct shape, so the parser accepts it.

---

## 2. What happens when you run it

`main()` does everything in this order:

1. **Opens the files.** The Symbol Table to read, and `Parse Tree.txt` (in the
   same folder) to write. Prints `<program>`, the root of the tree.
2. **Repeats until the end of the file:**
   1. `collectStatement()` collects **one statement** into the statement buffer.
   2. `parseCollectedStatement()` parses it and prints its tree.
3. **Prints all syntax errors in line order**, then the verdict:
   `No syntax error found. The program is accepted.` or
   `N syntax error(s) found. The program is rejected.`
   Errors never stop the program.

---

## 3. Reading the Symbol Table one token at a time

| Function | What it does |
|---|---|
| `readRow()` | Reads **one line** of `Symbol Table.txt` and splits it into line, token, lexeme. Nothing else is read ahead. |
| `readToken()` | Calls `readRow()` until it gets a **useful** token. It drops comments, noise words (`let the of from using`) and ERROR rows (the lexer already reported them). It also drops a NEWLINE that comes right after another NEWLINE, so blank lines and comment-only lines count as **one** end of line. |

This is exactly the doc's **3.3 Reading Process**, steps 1 and 2.

---

## 4. Collecting one statement: `collectStatement()`

Your professor's rule: *take tokens one at a time, and when the terminator is
reached, parse that statement.* So the parser first **collects** the tokens of
one statement into the **statement buffer** (three fixed arrays:
`statementType`, `statementLine`, `statementLexeme`), and stops at the terminator:

| Statement | Its terminator |
|---|---|
| declaration, assignment, `input`, `print` | the NEWLINE at the end of the line |
| `if` | the `}` of its last block (it looks at the next token: `elif` or `else` means the `if` goes on) |
| `while` | the `}` of its block |

Two special cases from the doc (3.3):

- **Open groups.** A NEWLINE inside an open `(` or `[` does not end the statement,
  so a vector can go over several lines. But if the next line starts with a
  statement keyword (`integer`, `print`, `if`, ...), the `(` was clearly never
  closed, so the statement ends there instead of swallowing the rest of the file.
- **Brace on the next line.** `while i < 10` then `{` on the next line is fine.
  If the next line is not `{`, the statement ends (and the parser reports the
  missing `{`).

**Why `holdToken()`?** To know that an `if` is finished, the parser must read the
token *after* it, which already belongs to the next statement. `holdToken()` keeps
that one token so the next `collectStatement()` starts with it. Nothing is lost or
read twice.

---

## 5. Parsing one statement: recursive descent

The doc (3.2) chose **top-down, recursive descent**: one function per grammar rule
(non-terminal). A function looks at the current token, **matches** the tokens its
rule expects, and **calls** the function of any other rule inside it.

- `getToken()` gives the parser the next token **from the statement buffer**
  (never from the file). When the buffer is used up it returns EOF, so a parse
  can never run past its own statement.
- `matchToken()` accepts the current token: it becomes a **leaf** of the tree
  (for example `KW_IF 'if'`) and the parser moves on.
- The **first token** of a statement decides the rule (`parse_statement()`):

| First token | Rule | Function |
|---|---|---|
| `integer decimal character string boolean vector` | `<declaration>` | `parse_declaration()` |
| an identifier | `<assignment>` | `parse_assignment()` |
| `input` | `<input_stmt>` | `parse_input()` |
| `print` | `<output_stmt>` | `parse_output_stmt()` |
| `if` | `<if_stmt>` | `parse_if()` |
| `while` | `<while_stmt>` | `parse_while()` |

**Expressions** follow the precedence table, lowest to highest. Each function
handles one level and calls the next one:

| Function | Operators | Level |
|---|---|---|
| `parse_expression()` → `parse_or_expr()` | `or` | lowest |
| `parse_and_expr()` | `and` | |
| `parse_rel_expr()` | `== not= < > <= >=` | |
| `parse_add_expr()` | `+ -` | |
| `parse_mul_expr()` | `* / // %` | |
| `parse_unary()` | `not`, `-` (negative) | |
| `parse_power()` | `^` | |
| `parse_primary()` | numbers, names, `( )`, `[ ]`, `mean(...)` | highest |

That is why `x + y * z` multiplies first: `*` is handled deeper (higher) than `+`.
`parse_power()` calls **itself** on the right, which makes `^` right-associative.

**Blocks** (`parse_block()`) parse statements until `}`. Because a block calls
`parse_statement()`, an `if` inside a `while` (or a loop in a loop) just works.

**The function names are the doc's names.** Every function the Syntax doc
mentions in its traces (`parse_input()`, `parse_output_content()`,
`parse_assign_op()`, `parse_data_type()`, ...) exists in the code with exactly
that name, so you can follow any trace in the doc line by line in the code.

**Print forms.** `parse_output_content()` decides between the four forms of
3.4.2 by looking ahead inside the statement buffer with `peekType()`:

| Between the parentheses | Function |
|---|---|
| one string: `print("Hi")` | `parse_string_output()` |
| one name: `print(count)` | `parse_variable_output()` |
| string, name: `print("Score: ", score)` | `parse_mixed_output()` |
| anything else: `print(total / count, name)` | `parse_output_list()` |

---

## 6. The parse tree

Each statement's tree is built in fixed arrays (`nodeLabel`, `nodeFirstChild`,
`nodeNextSibling`, ...) and printed by `printTree()` with the doc's own symbols:

- `├──` a child with more after it, `└──` the last child, `│` the line going down
- `<...>` a non-terminal (a grammar rule), `TOKEN 'lexeme'` a leaf (a token)
- `A → B` means **B is the only child of A** (the doc's convention in 3.4.5)

**Fixed limits** (everything is a fixed-size array, nothing grows):

| Limit | What happens when it is passed |
|---|---|
| 50000 tokens in one statement | `statement is too long (more than 50000 tokens), it was not parsed.` |
| 20000 tree nodes in one statement | the tree is printed with `(this statement is too large, some nodes are not shown)`; the check still runs |
| 500 listed errors | all errors are counted, the list ends with `(only the first 500 errors are listed)` |

`printTree()` keeps its work space (`treeLine`, `treePrefix`) outside the
function, so even a very deeply nested program (hundreds of nested `if`s or
parentheses) cannot crash it.

---

## 7. Panic mode: errors without stopping

When a token does not fit the rule, `syntaxError()`:

1. **reports** one message: `Syntax error on line <n>: <reason>.`
2. turns on **panic mode**, so the parse functions stop.

Then `recoverFromError()` (doc 3.5):

1. skips the token that caused the error (never a NEWLINE, `}`, EOF or a
   statement keyword, because those show where the next statement starts);
2. skips until a **sync token**: NEWLINE, `}`, EOF, a statement keyword, or a
   name on a later line;
3. uses the NEWLINE as the end of the broken statement;
4. turns panic mode off: **parsing continues with the next statement**.

Only the **first** error of a broken statement is reported, so one mistake does
not cause a chain of messages.

---

## 8. Worked example

`Sample input:`
```
integer x = 5
input count)
while x < 3 {
    x += 1
}
```

**Line 1.** Rows read: `KW_INTEGER IDENT ASSIGN INT_LIT NEWLINE`. The NEWLINE ends
the statement. `KW_INTEGER` → `parse_declaration()` → match the type, call
`parse_declarator()` (match `x`, `=`, parse the expression `5`), match the terminator.

**Line 2.** Buffer: `KW_INPUT IDENT RPAREN NEWLINE`. `parse_input()` matches
`input`, then expects `(` but finds `count` → **error**. Recovery skips `count`
and `)`, uses the NEWLINE as the end, and moves on.

**Lines 3-5.** The `while` is collected until its `}`, then parsed:
condition `x < 3`, then the block with the statement `x += 1`.

The real output:
```
<program>
├── <statement> → <declaration>
│   ├── <data_type> → KW_INTEGER 'integer'
│   ├── <declarator>
│   │   ├── IDENT 'x'
│   │   ├── ASSIGN '='
│   │   └── <expression> → <or_expr> → ... → <primary> → INT_LIT '5'
│   └── <terminator> → \n
├── <statement>
│   ├── <input_stmt> → KW_INPUT 'input'
│   └── <syntax error: rest of the statement is skipped>
├── <statement> → <while_stmt>
│   ├── KW_WHILE 'while'
│   ├── <condition> → <expression> → <or_expr> → <and_expr> → <rel_expr>
│   │   ├── <add_expr> → ... → <primary> → IDENT 'x'
│   │   ├── LT '<'
│   │   └── <add_expr> → ... → <primary> → INT_LIT '3'
│   ├── <block>
│   │   ├── LBRACE '{'
│   │   ├── <statement> → <assignment>
│   │   │   ├── IDENT 'x'
│   │   │   ├── <assign_op> → PLUS_EQ '+='
│   │   │   ├── <expression> → ... → <primary> → INT_LIT '1'
│   │   │   └── <terminator> → \n
│   │   └── RBRACE '}'
│   └── \n
└── EOF

Syntax error on line 2: '(' expected after 'input'.
1 syntax error(s) found. The program is rejected.
```
(`...` shortens the long `<or_expr> → <and_expr> → ...` chains here; the program
prints them in full.)

The error on line 2 did **not** stop the program: the `while` on line 3 was still
parsed.

---

## 9. Does it do what the professor asked?

| Requirement | Met? | Where to show it |
|---|---|---|
| Reads the Symbol Table one token at a time | Yes | `readRow()` reads one row with `fgets`; `readToken()` calls it once per token. |
| Collects a statement until its terminator, then parses it | Yes | `collectStatement()` fills the buffer and stops at the terminator; then `parseCollectedStatement()` parses it. |
| Outputs a parse tree | Yes | `printTree()`, on screen and in `Parse Tree.txt`. |
| Panic mode recovery | Yes | `syntaxError()` + `recoverFromError()`; `main()` keeps going until EOF. |
| Shows the line of each error | Yes | every message is `Syntax error on line <n>: ...`, and all are listed in line order at the end. |
| Short, clear messages | Yes | for example `'(' expected after 'input'.`, `condition expected after 'while'.` |
| Only a syntax analyzer | Yes | there is no variable table and no type check anywhere in the code. |
| Nothing dynamic | Yes | the buffer and the tree are fixed-size arrays declared in the source (no `malloc`), reused for every statement. |

### Questions the professor might ask, and the answers

- **"Why is there a buffer if you read one token at a time?"** You read one token at
  a time *into* the buffer. To parse a statement *when its terminator is reached*,
  its tokens must be kept until then. The buffer holds only **one** statement and
  is cleared after it.
- **"`tokenFromName()` uses `strcmp` - isn't that string comparison?"** It only
  turns the token name the lexer already wrote (`KW_IF`) back into a number. The
  lexer decided the tokens; the parser never looks inside a lexeme to choose a rule.
- **"Why is `sampleLexicalErrors.epl` accepted?"** Its errors are lexical. The lexer
  reports them; the parser skips ERROR tokens (doc 3.3) and the rest is valid
  syntax. Each analyzer reports its own kind of error.
- **"Why is `x = "hello" + 5` accepted?"** Its shape is valid. Mixing a string and a
  number is a type error, which the semantic analyzer checks.
- **"Why does a missing `)` sometimes report the next line?"** The doc says a
  NEWLINE does not end a statement while a `(` is open, so the next line is read
  as part of it, and the error is found there.
- **"How does it know which rule to use without trying them all?"** One token of
  lookahead: the first token of a statement (or the next token inside a rule)
  always points to exactly one rule, so there is no backtracking.

---

## 10. Function map

| Function | Job | Doc section |
|---|---|---|
| `readRow()` | Read one row of the Symbol Table | 3.3 |
| `readToken()` | Next useful token (drop comments, noise words, ERROR) | 3.3 Reading Process 1-2 |
| `collectStatement()` | Collect one statement until its terminator | 3.3 Statement Terminators |
| `holdToken()` | Keep a token read too early for the next statement | 3.3 Blocks |
| `getToken()` | Parser's next token, from the buffer | 3.2 (lookahead) |
| `matchToken()` | Accept a token, add it as a leaf | 3.2 |
| `parse_statement()` | Choose the rule from the first token | 3.2, 3.4 `<statement>` |
| `parse_declaration()`, `parse_data_type()`, `parse_declarator()` | `integer x = 1, y` | 3.4.6 |
| `parse_vector_lit()` | `[1, 2, 3]` | 3.4.6 |
| `parse_assignment()`, `parse_assign_op()` | `x += 1` | 3.4.3 |
| `parse_input()` | `input(x)` | 3.4.1 |
| `parse_output_stmt()`, `parse_output_content()` | `print(...)` | 3.4.2 |
| `parse_string_output()`, `parse_variable_output()`, `parse_mixed_output()`, `parse_output_list()` | the four print forms | 3.4.2 |
| `peekType()` | Look ahead in the buffer (for the print form) | 3.4.2 |
| `parse_if()`, `parse_while()`, `parse_block()`, `parse_condition()` | if / elif / else, while, `{ }` | 3.4.4, 3.4.5 |
| `parse_terminator()` | `\n` or EOF at the end of a statement | 3.4 `<terminator>` |
| `parse_expression()` ... `parse_primary()` | Expressions by precedence | 3.4 shared rules |
| `parse_index()`, `parse_func_call()` | `scores[0:2]`, `mean(scores)` | 3.4 `<index>`, `<func_call>` |
| `syntaxError()`, `recoverFromError()`, `skipToBlock()` | Report and recover | 3.5 |
| `printTree()` | Draw the tree | 3.4 parse trees |
| `main()` | Run everything, print errors and the verdict | 3.3, 3.5 |

---

## 11. Running it

First run the lexer, then the parser on the Symbol Table it wrote:

```
gcc -std=c99 -Wall -Wextra -o lexer  εpsilonLexicalAnalyzer.c
gcc -std=c99 -Wall -Wextra -o parser εpsilonSyntaxAnalyzer.c
./lexer  "Sample Programs/sampleValidProgram.epl"
./parser "Sample Programs/Symbol Table.txt"
```

| Sample | Lexer | Parser |
|---|---|---|
| `sampleValidProgram.epl` | `No lexical error found. 464 tokens.` | `No syntax error found. The program is accepted.` |
| `sampleSyntaxErrors.epl` | `No lexical error found. 270 tokens.` | `33 syntax error(s) found. The program is rejected.` |
| `sampleLexicalErrors.epl` | `16 lexical error(s) found.` | `No syntax error found. The program is accepted.` |

The parser writes `Parse Tree.txt` next to the Symbol Table. On an old Windows
console the tree symbols need UTF-8; the program switches the console to UTF-8
itself.
