# parse semantic schema and do dynamic schema change
import json, sys

PATH = '/tmp/semantic.json'
ONTO = 'ontology'
OBJ_ONTO = 'object_ontology'

# vattrs = [
#   {"name": "attr1", "dtype": "float"}, 
#   {"name": "attr2", "dtype": "string", "default": "abc"}
# ]
def add_vertex(vtype, vattr):
  pat = 'add vertex {} (primary_id id string not null {})'
  attrs = []
  for i in vattr:
    if not all('name' in i, 'dtype' in i):
      continue
    j = [i['name'], i['dtype']]
    if 'default' in i:
      j.append('default')
      j.append(i['default'])
    attrs.append(' '.join(j))
  attrs = ', '.join(attrs)
  if len(attrs) > 0:
    attrs = ', ' + attrs
  return pat.format(vtype, attrs)

def add_edge(directed, etype, srcv, tgtv, eattr):
  pat = 'add {} edge {} (from {}, to {} {})'
  attrs = []
  for i in eattr:
    if not all('name' in i, 'dtype' in i):
      continue
    j = [i['name'], i['dtype']]
    if 'default' in i:
      j.append('default')
      j.append(i['default'])
    attrs.append(' '.join(j))
  attrs = ', '.join(attrs)
  if len(attrs) > 0:
    attrs = ', ' + attrs
  return pat.format('directed' if directed else 'undirected',
      etype, srcv, tgtv, attrs)


if __name__ == '__main__':
  with open(PATH, 'r') as fp:
    js = json.load(fp)
    if not all((ONTO in js, OBJ_ONTO in js)):
      print >> sys.stderr, '{}, {} not found'.format(ONTO, OBJ_ONTO)
      sys.exit(1)

  vertices = []
  edges = []

  # create vertices/edges for ONTOLOGY
  for onto in js[ONTO]:
    vtype = js[ONTO][onto]['vtype']
    vertices.append(add_vertex(vtype, {}))

    # NOTE: all etype assumed to be DIRECTED
    for etype in js[ONTO][onto]['etype']:
      edges.append(add_edge(True, etype, vtype, vtype, {}))

  # create edges for OBJ_ONTOLOGY

  print vertices
  print edges
