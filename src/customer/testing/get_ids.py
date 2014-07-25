import sys
import random

f = open(sys.argv[1])

ids = set()

for line in f: 
    x = line.split(',')
    ids.add(x[0])
    ids.add(x[1])

g = open("id_sample.txt","w")

for vid in random.sample(ids,1000):
    g.write(vid + "\n")
