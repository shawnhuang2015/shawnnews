# parse semantic schema and do dynamic schema change
import json, sys, os.path

GSQL_PATH = '~/graphsql/dev/gsql/gsql'

BASIC_SCHEMA_PATH = '/tmp/basic.gsql'
SEMANTIC_SCHEMA_PATH = '/tmp/semantic.json'
SCHEMA_CHANGE_JOB_PATH = '/tmp/sc.gsql'

ONTO = 'ontology'
OBJ_ONTO = 'object_ontology'

# vattrs = [
#   {"name": "attr1", "dtype": "float"}, 
#   {"name": "attr2", "dtype": "string", "default": "abc"}
# ]
def add_vertex(vtype, vattr={'name': 'string'}):
  pat = 'add vertex {} (primary_id id string not null {});'
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
  pat = 'add {} edge {} (from {}, to {} {});'
  attrs = []
  for i in eattr:
    if not all(('name' in i, 'dtype' in i)):
      continue
    j = [i['name'], i['dtype']]
    if 'default' in i:
      j.append('default')
      j.append(str(i['default']))
    attrs.append(' '.join(j))
  attrs = ', '.join(attrs)
  if len(attrs) > 0:
    attrs = ', ' + attrs
  return pat.format('directed' if directed else 'undirected',
      etype, srcv, tgtv, attrs)

def create_schema_change(bs_path, ss_path, sc_path):
  with open(ss_path, 'r') as fp:
    js = json.load(fp)
    if not all((ONTO in js, OBJ_ONTO in js)):
      print >> sys.stderr, '{}, {} not found'.format(ONTO, OBJ_ONTO)
      sys.exit(1)

  vertices = []
  edges = []

  # ontology name -> vtype, etype mapping
  mapping = {}

  # create vertices/edges for ONTOLOGY
  for onto in js[ONTO]:
    vtype = onto['vtype']
    vertices.append(add_vertex(vtype))

    # NOTE: all etype assumed to be DIRECTED
    etype = onto['etype']['up']
    edges.append(add_edge(True, etype, vtype, vtype, {}))
    etype = onto['etype']['down']
    edges.append(add_edge(True, etype, vtype, vtype, {}))

    mapping[onto['name']] = {
        'vtype': onto['vtype'],
        'etype': onto['etype']
      }

  # create edges for OBJ_ONTOLOGY
  for obj_onto in js[OBJ_ONTO]:
    srcv = obj_onto['object']
    for onto in obj_onto['ontology']:
      if onto['name'] not in mapping:
        # error, missing ontology
        sys.exit(1)
      tgtv = mapping[onto['name']]['vtype']
      etype = onto['etype']
      edges.append(add_edge(False, etype, srcv, tgtv, [{'name': 'weight', 'dtype': 'float', 'default': 1.0}]))

  # create schema change job script
  with open(sc_path + '.vertex', 'w') as fp:
    lines = [
        'create schema_change job sc_vertex for graph cip {',
        '\n'.join(vertices),
        '}',
        'run job sc_vertex'
      ]
    fp.write('\n'.join(lines))

  with open(sc_path + '.edge', 'w') as fp:
    lines = [
        'create schema_change job sc_edge for graph cip {',
        '\n'.join(edges),
        '}',
        'run job sc_edge'
      ]
    fp.write('\n'.join(lines))

def run_schema_change(sc_path):
  os.system(GSQL_PATH + ' ' + sc_path + '.vertex')
  os.system(GSQL_PATH + ' ' + sc_path + '.edge')


if __name__ == '__main__':
  create_schema_change(BASIC_SCHEMA_PATH, SEMANTIC_SCHEMA_PATH, SCHEMA_CHANGE_JOB_PATH)
  run_schema_change(SCHEMA_CHANGE_JOB_PATH)
