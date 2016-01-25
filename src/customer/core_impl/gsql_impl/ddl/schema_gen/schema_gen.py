from collections import defaultdict
import json

def create_vertex(l):
  v_template = 'create vertex {vtype} (primary_id {vtype}_id string not null {attrs}) with stats="outdegree_by_edgetype"'
  attr_template = '{name} {dtype}'

  vtypes = []
  vs = []
  for i in l:
    attrs = []
    for _ in i['attr']:
      a = attr_template.format(name=_['name'], dtype=_['dtype'])
      if 'default' in _:
        a += ' default {}'.format(_['default'])
      attrs.append(a)
    if len(attrs) > 0:
      attrs = ', ' + ', '.join(attrs)
    else:
      attrs = ''
    vs.append(v_template.format(vtype=i['type'], attrs=attrs))
    vtypes.append(i['type'])
  return vs, vtypes

def create_edge(l):
  e_template = 'create {directed} edge {etype} (from {source}, to {target} {attrs})'
  attr_template = '{name} {dtype}'

  etypes = []
  es = []
  for i in l:
    attrs = []
    for _ in i['attr']:
      a = attr_template.format(name=_['name'], dtype=_['dtype'])
      if 'default' in _:
        a += ' default {}'.format(_['default'])
      attrs.append(a)
    if len(attrs) > 0:
      attrs = ', ' + ', '.join(attrs)
    else:
      attrs = ''
    es.append(e_template.format(
      directed='directed' if i['directed'] else 'undirected', 
      etype=i['type'], 
      attrs=attrs,
      source=i['source'],
      target=i['target']))
    etypes.append(i['type'])
  return es, etypes

def create_graph(schema_json):
  builtin = [
      ]
  with open('ui_schema.json', 'r') as fp:
    f = json.load(fp)
    vs, vtypes = create_vertex(f['vertex'])
    es, etypes = create_edge(f['edge'])
    graph = ['drop all']
    graph.extend(vs)
    graph.extend(es)
    graph.append('create graph cip ({})'.format(','.join(vtypes + etypes)))
    graph.append('export schema cip')
  return graph


if __name__ == '__main__':
  print '\n'.join(create_graph('ui_schema.json'))
