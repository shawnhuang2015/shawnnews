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
    fun = p[1]
    if fun == "__REQ_CTX":
        p[0] = 'context.get("requestContext")\n'
    elif fun == "__RET":
        p[0] = "set_rule_result(context, __rule__, %s)\n" % (p[3])
    elif fun == "__WS":
        p[0] = "set_workspace(context, %s)\n" % (p[3])
    else:
        p[0] = "%s(%s)\n" %(p[1],p[3])

def p_function_class(p):
    '''function_class : VARIABLE DOT VARIABLE LPAREN arguments RPAREN'''
    p[0] = "%s.%s(%s)\n" %(p[1],p[3],p[5])

def p_assignment(p):
    '''assignment : VARIABLE ASSIGN expr
                | VARIABLE ASSIGN function
                | VARIABLE ASSIGN function_class'''
    p[0] = "%s = %s\n" %(p[1],p[3])

def p_arguments(p):
    '''arguments : literal COMMA arguments
                | literal 
                |'''
    p[0] = ''.join(p[1:])

# EXPR ##########################

def p_expr(p):
    '''expr : LPAREN expr RPAREN
            | literal PLUS expr 
            | literal MINUS expr 
            | literal TIMES expr 
            | literal DIVIDE expr
            | literal AND expr
            | literal OR expr
            | VARIABLE ASSIGN expr
            | literal EQUALS expr
            | literal LESS expr
            | literal LESSEQUAL expr
            | literal MORE expr
            | literal MOREEQUAL expr
            | NOT expr
            | TRUE
            | FALSE
            | literal '''
    p[0] = ' '.join(p[1:])

def p_literal(p):
    '''literal : VARIABLE
                | NUMBER
                | QUOTE literal QUOTE'''
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
    when 456 and 678< cc
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

    when (k >= 9)
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
data2 = '''rule "rule4"
begin
    reqCtx = __REQ_CTX()
    f.fun()
    d = f.fun(a,b,4)
    user.setresult()
    when reqCtx == "86"
    then
    {
        __WS("key","val")
        __WS("key2",123)
        __RET("REVIEW")
    }else{
        __RET("APPROVE")
    }

end
'''
t = yacc.parse(data2)
# print t
print  normalize(t)

