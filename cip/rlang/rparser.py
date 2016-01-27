import ply.yacc as yacc
import rlexer
tokens = rlexer.tokens

def make_indent(indent_num):
    return "".ljust(indent_num,"\t")

# Do normalization
def normalize(source):
    indent = 0
    il = []
    for line in source.split("\n"):
        if line == "":
            continue
        if line[-1] == '{':
            indent += 1
        elif line[-1] == '}':
            indent -= 1
        else:
            il.append(make_indent(indent)+line)

    return "\n".join(il)

def p_error(p):
    print "syntax error !!!!!!!!!!"
    print p
    print "error line:'%s' val:'%s'" % (p.lexer.lineno, p.value())

# RULE ##############
def p_rule(p):
    '''rule : RULE QUOTE VARIABLE QUOTE BEGIN section END''' 
    p[0] = "def rule_%s(context):\n{\n%s\n}\n" %(p[3],p[6])

# SECTION ############
def p_section_stmt(p):
    '''section : stmt section'''
    p[0] = "%s%s" %(p[1],p[2])

def p_section_empty(p):
    '''section : '''
    p[0] = "pass\n"

def p_section_if(p):
    '''section : WHEN expr THEN LBRACE section RBRACE section''' 
    p[0] = "if %s:\n{\n%s}\n%s" %(p[2], p[5], p[7])

def p_section_if_else(p):
    '''section : WHEN expr THEN LBRACE section RBRACE ELSE LBRACE section RBRACE section''' 
    p[0] = "if %s:\n{\n%s}\nelse:\n{\n%s}\n%s" %(p[2], p[5], p[9], p[11])

def p_section_while(p):
    '''section : WHILE expr DO LBRACE section RBRACE section'''
    p[0] = "while %s:\n{\n%s}\n%s" %(p[2],p[5],p[7])

# STMT ####################
def p_stmt(p):
    '''stmt : function
            | function_class
            | assignment'''
    p[0] = "".join(p[1:])

# Function/ClassFunction/Assignment  ##################

def p_function(p):
    '''function : VARIABLE LPAREN arguments RPAREN''' 
    p[0] = "%s(%s)\n" %(p[1],p[3])

def p_function_class(p):
    '''function_class : VARIABLE DOT VARIABLE LPAREN arguments RPAREN'''
    p[0] = "%s.%s(%s)\n" %(p[1],p[3],p[5])

def p_assignment(p):
    '''assignment : VARIABLE ASSIGN expr'''
    p[0] = "%s = %s\n" %(p[1],p[3])

def p_arguments(p):
    '''arguments : VARIABLE COMMA arguments
                | VARIABLE 
                |'''
    p[0] = ''.join(p[1:])

# EXPR ##########################

def p_expr(p):
    '''expr : LPAREN expr RPAREN
            | VARIABLE PLUS expr 
            | NUMBER PLUS expr 
            | VARIABLE MINUS expr 
            | NUMBER MINUS expr 
            | VARIABLE TIMES expr 
            | NUMBER TIMES expr 
            | VARIABLE DIVIDE expr
            | NUMBER DIVIDE expr
            | VARIABLE AND expr
            | NUMBER AND expr
            | VARIABLE OR expr
            | NUMBER OR expr
            | VARIABLE ASSIGN expr
            | VARIABLE EQUALS expr
            | NUMBER EQUALS expr
            | NOT expr
            | TRUE
            | FALSE
            | NUMBER
            | VARIABLE'''
    p[0] = ''.join(p[1:])

yacc.yacc()
data = '''rule "test"
begin
    user.setresult()
    setGlobalResult()
    abc(a,b)
    a.b(a,b,c)
    b = c
    b = 4
    b = 1*4+5*(4-1)
    when 456
    then
    {
            b = 4*5
    }else{
        when nested
        then
        {
           bbbbb = cc 
        }
        k = 7
    }

    when (True)
    then
    {
        c = d * 5
    }
    while True
    do{
        a = a+5
    }


    while True
    do{
        a = a+5
        when nested
        then
        {
            a = 5
        }
    }
end'''
t = yacc.parse(data)
# print t
print  normalize(t)

