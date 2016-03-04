# parse semantic schema and do dynamic schema change
import json, sys, os.path, subprocess, time

GSQL_PATH = os.path.expanduser('~/graphsql/dev/gsql/gsql')
BASIC_SCHEMA_PATH = '/tmp/basic.gsql'
JOB_DIR = '/tmp/'
GRAPH_NAME = 'cip'

# attrs = [
#   {"name": "attr1", "dtype": "float"}, 
#   {"name": "attr2", "dtype": "string", "default": "abc"}
# ]
def add_vertex(vert):
  pat = 'add vertex {} (primary_id id string not null {});'
  assert vert.has_key('vtype')
  vtype = vert['vtype']
  attr = [] if not vert.has_key('attr') else vert['attr']

  attrs = []
  for i in attr:
    if not all(('name' in i, 'dtype' in i)):
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

def drop_vertex(vert):
  pat = 'drop vertex {};'
  assert vert.has_key('vtype')
  vtype = vert['vtype']
  return pat.format(vtype)

def add_edge(edge):
  pat = 'add {} edge {} (from {}, to {} {});'
  direct = 'undirected' if (not edge.has_key('directed')) or \
          edge['directed'] == False else 'directed'
  assert edge.has_key('etype')
  assert edge.has_key('source_vtype')
  assert edge.has_key('target_vtype')
  etype = edge['etype']
  srcv = edge['source_vtype']
  tgtv = edge['target_vtype']
  attr = [] if not edge.has_key('attr') else edge['attr']

  attrs = []
  for i in attr:
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
  return pat.format(direct, etype, srcv, tgtv, attrs)

def drop_edge(edge):
  pat = 'drop edge {};'
  assert edge.has_key('etype')
  etype = edge['etype']
  return pat.format(etype)

def run_job(job_file):
  job = subprocess.Popen([GSQL_PATH, job_file], stdout=sys.stdout, stderr=sys.stderr)
  job.communicate()
  return job.returncode == 0


if __name__ == '__main__':
  diff_file = sys.argv[1]
  with open(diff_file, 'r') as fp:
    delta = json.load(fp)
    if delta is None:
      sys.exit(0)

  vertices = []
  edges = []

  if delta.has_key('add'):
    add = delta['add']
    if add.has_key('vertex'):
      [vertices.append(add_vertex(v)) for v in add['vertex']]
    if add.has_key('edge'):
      [edges.append(add_edge(e)) for e in add['edge']]

  if delta.has_key('drop'):
    drop = delta['drop']
    if drop.has_key('vertex'):
      [vertices.append(drop_vertex(v)) for v in drop['vertex']]
    if drop.has_key('edge'):
      [edges.append(drop_edge(e)) for e in drop['edge']]

  if len(vertices) > 0:
    with open(JOB_DIR + 'sc_vertex.gsql', 'w') as fp:
      lines = [
          'drop job sc_vertex',
          'create schema_change job sc_vertex for graph {} {{'.format(GRAPH_NAME),
          '\n'.join(vertices),
          '}',
          'run job sc_vertex'
        ]
      fp.write('\n'.join(lines))
    if not run_job(JOB_DIR + 'sc_vertex.gsql'):
      sys.exit(1)

  if len(edges) > 0:
    with open(JOB_DIR + 'sc_edge.gsql', 'w') as fp:
      lines = [
          'drop job sc_edge',
          'create schema_change job sc_edge for graph {} {{'.format(GRAPH_NAME),
          '\n'.join(edges),
          '}',
          'run job sc_edge'
        ]
      fp.write('\n'.join(lines))
    if not run_job(JOB_DIR + 'sc_edge.gsql'):
      sys.exit(1)
