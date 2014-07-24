Instructions:
1) Install the neo4j package.
2) Install Jersey.
3) Create a folder "lib/" and then put all the neo4j jar files and Jersey jar files into that folder.

Compile and run
REST API Insert
1) Change the souce to load in RestLoadTraversalExample.java and the graph file are tab seperated. 
2) Rest API Insert:
    i. javac -cp ".:lib/*" RestLoadTraversalExample.java Relation.java TraversalDefinition.java
    ii. java -cp ".:lib/*" RestLoadTraversalExample


Batch Load
1) Change the souce to load in RestLoadTraversalExample.java and the graph file are tab seperated. 
2) Rest API Insert:
    i. javac -cp ".:lib/*" BatchLoadTraversalExample.java
    ii. java -cp ".:lib/*" BatchLoadTraversalExample.java > traverse_test_result 
