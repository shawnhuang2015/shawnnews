import json
import requests
import urllib
from time import clock
from time import time
from optparse import OptionParser
import random

def get_kneighborhood(host, port, vid, depth,edges):
  url = "http://%s:%s/kneighborhood/%s" % (host, port,vid)
  #url+="%3Fdepth=1"
  payload = {"depth":depth}
  head = {'content-type':'application/json'}
  req  = requests.get(url,params=payload)
  #req  = requests.get(url)
  print req.url
  return req

def get_next_id(f):
#  return str(random.randint(12,f))
#  return "12"
  with open(f) as data: 
    for line in data: 
      yield line.strip()


def test_main():
  # Define the command line options.
  parser = OptionParser()
  
  parser.add_option("-i",
      dest="ip_address",
      type="string",
      default="localhost",
      help="[optional] the IP address or hostname of the GraphSQL server. default=localhost")
  parser.add_option("-p",
      dest="port",
      type="int",
      default=9000,
      help="[optional] the port of the GraphSQL server. default=9000")
  #TODO(ntietz): add option for source id filename
  parser.add_option("-b", 
      dest="batch_size", 
      type="int",
      default=1,
      help="The number of edges to include in each batch submission")
  parser.add_option("-d", 
      dest="depth", 
      type="int",
      default=1,
      help="the max depth allowed, randomly selected between 1 and the value given default= 1")
  parser.add_option("-e", 
      dest="edges", 
      type="string",
      default="false",
      help="include edges? choose default false (option: true)")
  parser.add_option("-s", 
      dest="system", 
      type="string",
      default="GSQL",
      help="System to use, GSQL or N4J")
  (options, args) = parser.parse_args()
  

  host = options.ip_address
  port = options.port
  batch = options.batch_size
  depth = options.depth
  edges = options.edges
  #vid = aputil.random_user_id()
  times = []
  lengths = []
  start = clock()
  start_t = time()
  n = 0
  for vid in get_next_id("id_sample.txt"):
    result = get_kneighborhood(host, port, vid, depth,edges)
    times.append(result.elapsed.total_seconds())
    n+=1
    print "text length: " + str(len(result.text))
    lengths.append(len(result.text))
#   print result.text
 #   print result.elapsed        
  end = clock()
  end_t = time()

  
  
  print "clock() Elapsed time for " + str(n) + " queries: " + str(end - start)
  print "time() Elapsed time: " + str(end_t - start_t)
  print "min elapsed query: " + str(min(times))
  print "max elapsed query: " + str(max(times))
  print "total by elapsed time: " + str(sum(times))
  print "average elapsed time per query: "+ str(sum(times)/len(times))
  print "min result length: " + str(min(lengths))
  print "max result length: " + str(max(lengths))
  print "average result length per query: "+ str(sum(lengths)/len(lengths))


# Enter main when executed.
if __name__ == '__main__':
  test_main()
#    import timeit
#    print(timeit.timeit("test_main()", setup="from __main__ import test_main"))
