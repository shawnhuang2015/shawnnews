import json
import requests
import urllib
from time import clock
from optparse import OptionParser
import random

def get_kneighborhood(host, port, vid, depth,edges):
  url = "http://%s:%s/kneighborhood/%s" % (host, port,vid)
  #url+="%3Fdepth=1"
  payload = {"depth":depth, "edges":edges}
  head = {'content-type':'application/json'}
  req  = requests.get(url,params=payload)
  #req  = requests.get(url)
  print req.url
  return req

def get_random_id(n):
  return str(random.randint(12,n))
#  return "12"

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

  start = clock()
  for i in xrange(batch):
    vid = get_random_id(1000)
    result = get_kneighborhood(host, port, vid, depth,edges)
    print result.text
    print result.elapsed        
  end = clock()

  print "Elapsed time for batch size of " + str(batch) + ": " + str(end - start)
          
# Enter main when executed.
if __name__ == "__main__":
    test_main()
