import ply.yacc as yacc
import rlexer
tokens = rlexer.tokens

def p_error(p):
    print "error line:'%s' val:'%s'" % (p.lexer.lineno, p.value())

def p_rule(p):
    '''rule : RULE QUOTE VARIABLE QUOTE BEGIN section END''' 
    p[0] = "def rule_%s(context):\n%s" %(p[3],p[6])

def p_section_empty(p):
    '''section : '''

def p_section_if(p):
    '''section : WHEN expr THEN section'''
    p[0] = "if %s:\nthen:\n %s\n" % (p[2],p[4])

#  def p_section_if_else(p):
    #  '''section : WHEN expr THEN section ELSE section'''
    #  p[0] = "if %s:\nthen: %s\nelse:\n%s" % (p[2],p[4],p[6])

def p_section_stmt(p):
    '''section : stmt'''
    p[0] = p[1]

#  def p_section_expr(p):
    #  '''section : expr'''
    #  p[0] = p[1]

def p_stmt(p):
    '''stmt : stmt function
            | stmt function_class'''
    p[0] = "%s\n%s\n" %(p[1],p[2])

def p_stmt_function(p):
    '''stmt : function'''
    p[0] = p[1]

def p_stmt_function_class(p):
    '''stmt : function_class'''
    p[0] = p[1]

def p_function(p):
    '''function : VARIABLE LPAREN RPAREN''' 
    p[0] = "%s()" %(p[1])

def p_function_class(p):
    '''function_class : VARIABLE DOT VARIABLE LPAREN RPAREN'''
    p[0] = "%s.%s()" %(p[1],p[3])

def p_expr(p):
    '''expr : LPAREN expr RPAREN
            | expr PLUS expr 
            | expr MINUS expr 
            | expr TIMES expr 
            | expr DIVIDE expr
            | expr AND expr
            | expr OR expr
            | expr ASSIGN expr
            | expr EQUALS expr
            | NOT expr
            | TRUE
            | FALSE
            | NUMBER
            | VARIABLE'''
    p[0] = ''.join(p[1:])

yacc.yacc()
data = '''
rule "myrule" 
begin 
    when True 
    then
        when 456
        then 
            user.setresult()
            setGlobalResult()
end
'''
data2 = '''
rule "mytest"
begin
    when a==77
    then
end
'''
t = yacc.parse(data)
print  t
