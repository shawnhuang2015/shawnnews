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
        # This is tricky part, to move ')' in the head of 2nd line upward which will be occurred in 
        # using function as function parameter. eg. fun(fun(),fun())
        elif line == ")":
            il[-1] = il[-1] + line
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

serv_ctx_macro = ("__WS", "__LW_WS", "__W_WS", "__W_RULE_RET","__RULE_RET", "__W_RET")
def p_function(p):
    '''function : VARIABLE LPAREN arguments RPAREN''' 
    fun = p[1]
    if fun in serv_ctx_macro:
        p[0] = "%s(context,%s)\n" % (fun, p[3])
    else:
        p[0] = "%s(%s)\n" %(p[1],p[3])

def p_function_class(p):
    '''function_class : VARIABLE DOT VARIABLE LPAREN arguments RPAREN'''
    p[0] = "%s.%s(%s)\n" %(p[1],p[3],p[5])

def p_field_class(p):
    '''field_class : VARIABLE DOT VARIABLE'''
    p[0] = "%s.%s" %(p[1],p[3])

def p_assignment(p):
    '''assignment : VARIABLE ASSIGN expr
                | VARIABLE ASSIGN function
                | VARIABLE ASSIGN function_class'''
    p[0] = "%s = %s\n" %(p[1],p[3])

def p_arguments(p):
    '''arguments : expr COMMA arguments
                | function COMMA arguments
                | function_class COMMA arguments 
                | field_class COMMA arguments
                | expr 
                | function
                | function_class
                | field_class
                |'''
    p[0] = ''.join(p[1:])

# EXPR ##########################

def p_expr(p):
    '''expr : LPAREN expr RPAREN
            | literal math_operator expr 
            | literal AND expr
            | literal OR expr
            | VARIABLE ASSIGN expr
            | literal math_logic expr
            | function_class math_logic expr
            | field_class math_logic expr
            | NOT expr
            | TRUE
            | FALSE
            | literal '''
    p[0] = ' '.join(p[1:])

def p_math_logic(p):
    '''math_logic : EQUALS
                | LESS
                | LESSEQUAL
                | MORE
                | MOREEQUAL'''
    p[0] = p[1]
def p_math_operator(p):
    '''math_operator : PLUS
                    | MINUS
                    | TIMES
                    | DIVIDE'''
    p[0] = p[1]
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
header ='''
import json
import time, sys
sys.path.append("../")
from algorithm import RuleConfltSol 
###### common functions applied to context ###########
from Context import RuleContext
from ruletask.Context import __REQ_ACTOR
from ruletask.Context import __REQ_CP 
from ruletask.Context import __REQ_EVT 
from ruletask.Context import __WS 
from ruletask.Context import __W_WS
from ruletask.Context import __LW_WS
from ruletask.Context import __W_RET
from ruletask.Context import __RET
from ruletask.Context import __W_RULE_RET
from ruletask.Context import __RULE_RET
########  common biz objects #########################
from rulebase.BizObjBase import BizObjBase
from rulebase.JsonBizObj import JsonBizObj 
from rulebase.RestBizObj import RestBizObj 

def resolve_conflict(context):
    RuleConfltSol.default_sol_conflict(context)

def start(context):
    pass

def end(context):
    resolve_conflict(context)
'''

data4 = '''
#########   customization biz objects ################
#from cust.sample.LakalaRecommendProxy import LakalaRecommendProxy
rule "conflictResolve"
begin
    prod1 = __RULE_RET("rule2")
    prod2 = __RULE_RET("rule3")
    _set = set()
    #  for item in prod1:
        #  for k,v in item.iteritems():
            #  _set.add(v)
    #  for item in prod2:
        #  for k,v in item.iteritems():
            #  _set.add(v)

    k = list(_set)
    __W_RET(5*7)
    __W_RET(list(_set))
    __W_RET(list._set)
    __W_RET(list._set())
    __W_RET(k)
end
'''
rule4 = '''
rule "rule4"
begin
    evt = __REQ_EVT
    when evt.age == "86"
    then{
        __W_RULE_RET("Approve")
    }else{
        __W_RULE_RET("Deny")
    }
end

'''



t = yacc.parse(data4)
# print t
print  normalize(t)

