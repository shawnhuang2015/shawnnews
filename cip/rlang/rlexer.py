import ply.lex as lex
reserved = {
        'rule':'RULE',
        'begin':'BEGIN',
        'end':'END',
        'when':'WHEN',
        'then':'THEN',
        'else':'ELSE',
        'true':'TRUE',
        'while':'WHILE',
        'do':'DO',
        'false':'FALSE',
        'and':'AND',
        'or':'OR',
        'not':'NOT'
        }
reserved_cip_ctx_macro = ("__REQ_ACTOR","__REQ_CP", "__REQ_EVT","__RULE_META","__RET" )

tokens = [
        'DIVIDE','TIMES','PLUS','MINUS','DOT','LPAREN','RPAREN','COMMA','LBRACE','RBRACE',
        'EQUALS','LESS','MORE','LESSEQUAL','MOREEQUAL',
        'ASSIGN',
        'QUOTE',
        'NUMBER',
        'VARIABLE'
        ]+ reserved.values()

t_ignore = ' \t'
def t_error(t):
    print "Illegal characters  line:'%s' val:'%s'" % (t.lexer.lineno,t.value[0])
    t.lexer.skip(1)

def t_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

def t_COMMENT(t):
    r'\#.*'
    pass

t_PLUS = r'\+'
t_MINUS = r'-'
t_TIMES = r'\*'
t_DIVIDE = r'/'
t_ASSIGN = r'='
t_EQUALS = r'=='
t_LESS = r'<'
t_MORE = r'>'
t_LESSEQUAL = r'<='
t_MOREEQUAL = r'>='
t_DOT = r'\.'
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_COMMA = r','
t_LBRACE = r'{'
t_RBRACE = r'}'
t_QUOTE = r'"'
t_NUMBER = r'\d+'


def t_VARIABLE(t): 
    r'[a-zA-Z_][a-zA-Z_0-9]*'
    if t.value in reserved:
        t.type = reserved[t.value]
    elif t.value in reserved_cip_ctx_macro:
        t.value = "%s(context)" % t.value
    return t

lex.lex()
data = '''rule "rule1"
begin 
a=__REQ_CTX
when x=2 * 2 
then {x = 2*2 
__bb = 3
}
end'''
lex.input(data)
while True:
    tok = lex.token()
    if not tok:
        break

    if __name__ == "__main__":
        print tok
