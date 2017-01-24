import json

'''
Based on a tuple (vtype name, attr list), generate a json object.
Support dtype and default value on UI.
'''

def tojson(func, *args, **kwargs):
  def _wrapper(*args, **kwargs):
    rez = func(*args, **kwargs)
    return json.dumps(rez)
  return _wrapper

l = [
  ('user', [('name', 'string'), ('age', 'uint'), ('gender', 'string')]),
  ('product', [('title', 'string'), ('on_sale', 'bool', 1), ('price', 'float')]),
  ('pic', []),

  ('profile', [('name', 'string')]),
  ('cate', [('name', 'string')]),
  ('tag', [('name', 'string')]),
]

# FIXME: behavior is not needed now, since we could do all this in semantic layer thru an API
# type, source, target, directed, behavior, attr
l1 = [
  ('click', 'user', 'product', False, True, [('dtime', 'uint')]),
  ('view_', 'user', 'product', False, True, [('dtime', 'uint')]),
  ('order_', 'user', 'product', False, True, [('dtime', 'uint')]),
  ('post', 'user', 'pic', False, True, [('dtime', 'uint')]),

  ('', 'user', 'profile', False, False, []),
  ('', 'pic', 'tag', False, False, []),
  ('', 'pic', 'cate', False, False, []),
  ('', 'product', 'cate', False, False, []),
]

@tojson
def create_vertex(l):
  vertices = []
  for i in l:
    v = {}
    v['type'] = i[0]
    v['attr'] = []
    for j in i[1]:
      v['attr'].append(dict(zip(('name', 'dtype', 'default'), j)))
    vertices.append(v)
  return vertices

@tojson
def create_edge(l):
  edges = []
  for i in l:
    e = {}
    e['type'] = i[0] if len(i[0]) > 0 else '_to_'.join(i[1:3])
    e['source'] = i[1]
    e['target'] = i[2]
    e['directed'] = i[3]
    e['behavior'] = i[4]
    e['attr'] = []
    for j in i[5]:
      e['attr'].append(dict(zip(('name', 'dtype'), j)))
    edges.append(e)
  return edges

@tojson
def create_graph(vl, el):
  graph = {}
  graph['vertex'] = json.loads(create_vertex(vl))
  graph['edge'] = json.loads(create_edge(el))
  return graph

print create_graph(l, l1)
