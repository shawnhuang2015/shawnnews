import yaml

doc1 = """
a:  1
b:
    c:  3
    d:  4
"""
doc2 = """
-   A
-   B
-   C
"""
a = yaml.dump(yaml.load(doc1))
b = yaml.dump(yaml.load(doc2))
print a
print b
print type(a)

#import RuleEngConfig 

#print RuleEngConfig.RuleEngConfig().getCfg()
