import json
import requests
import urllib
from optparse import OptionParser


def post_graph_update(host, port, payload):
  url = "http://%s:%s/updategraph" % (host, port)
  head = {'content-type':'application/json'}
  req  = requests.post(url, data=json.dumps(payload), headers=head)
  print req.url
  return req

def get_n_edges_payload(f,n):
  c = 0
  payload = {}
  payload["edgelist"] = []
  nodelist = set()

  # print "!" + line
  # if line : 
  #   edge = line.strip().split(',')
  #   nodelist.add(edge[0])
  #   nodelist.add(edge[1])
  #   payload["edgelist"].append({"startNode":edge[0], "endNode":edge[1], "weight":int(edge[2])})

  while c < n:
    line = f.readline()
    if not line: 
      break
    edge = line.strip().split(',')
    nodelist.add(edge[0])
    nodelist.add(edge[1])
    payload["edgelist"].append({"startNode":edge[0], "endNode":edge[1], "weight":int(edge[2])})
    c+=1
    print line
    

  payload["nodelist"] = [{"id":x} for x in nodelist]
  return payload

def make_edges_payload(edges):
  payload = {}
  nodelist= set()
  payload["edgelist"] = []
  for edge in edges: 
    nodelist.add(edge[0])
    payload["edgelist"].append({"startNode":edge[0], "endNode":edge[1], "weight":int(edge[2])})

  payload["nodelist"] = [{"id":x} for x in nodelist]
  return payload
  
    



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
  parser.add_option("-f", 
      dest="file_name", 
      type="string",
      default="",
      help="Edge file to use for graph updates. Required")
  parser.add_option("-b", 
      dest="batch_size", 
      type="int",
      default=1,
      help="The number of edges to include in each batch submission")
  parser.add_option("-s", 
      dest="system", 
      type="string",
      default="GSQL",
      help="System to use, GSQL or N4J")
  (options, args) = parser.parse_args()
  

  host = options.ip_address
  port = options.port
  batch = options.batch_size
  #vid = aputil.random_user_id()
  print options.file_name
  with open(options.file_name) as f:
      payload = get_n_edges_payload(f,batch)
      while payload["edgelist"]:
        result = post_graph_update(host,port,payload)
        print payload
        print result.text
        payload = get_n_edges_payload(f,batch)
        
          
# Enter main when executed.
if __name__ == "__main__":
    test_main()
