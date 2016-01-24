import ply.lex as lex
reserved = {
        'rule':'RULE',
        'begin':'BEGIN',
        'end':'END',
        'when':'WHEN',
        'then':'THEN',
        'else':'ELSE',
        'true':'TRUE',
        'false':'FALSE',
        'and':'AND',
        'or':'OR',
        'not':'NOT'
        }
tokens = [
        'DIVIDE','TIMES','PLUS','MINUS','DOT','LPAREN','RPAREN','COMMA',
        'EQUALS','ASSIGN',
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

t_TRUE = r'true'
t_FALSE = r'false'
t_AND = r'and'
t_OR = r'or'
t_NOT = r'not'

t_RULE = r'rule'
t_PLUS = r'\+'
t_MINUS = r'-'
t_TIMES = r'\*'
t_DIVIDE = r'/'
t_ASSIGN = r'='
t_EQUALS = r'=='
t_DOT = r'\.'
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_COMMA = r','

t_BEGIN = r'begin'
t_END = r'end'
t_QUOTE = r'"'
t_WHEN = r'when'
t_THEN = r'then'
t_ELSE = r'else'
t_NUMBER = r'\d+'


def t_VARIABLE(t): 
    r'[a-zA-Z][a-zA-Z_]*'
    if t.value in reserved:
        t.type = reserved[t.value]
    return t

lex.lex()
data = '''rule "myrule"
begin 
when x=2 * 2 
then x = 2*2 
end'''
lex.input(data)
while True:
    tok = lex.token()
    if not tok:
        break

    if __name__ == "__main__":
        print tok
