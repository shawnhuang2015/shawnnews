from collections import defaultdict
import json

def create_vertex(l):
  v_template = 'create vertex {vtype} (primary_id {vtype}_id string not null, {attrs}) with stats="outdegree_by_edgetype"'
  attr_template = '{name} {dtype}'

  vs = []
  for i in l:
    attrs = []
    for _ in i['attr']:
      a = attr_template.format(name=_['name'], dtype=_['dtype'])
      if 'default' in _:
        a += ' default {}'.format(_['default'])
      attrs.append(a)
    attrs = ', '.join(attrs)
    vs.append(v_template.format(vtype=i['type'], attrs=attrs))
  return vs


if __name__ == '__main__':
  with open('ui_schema.json', 'r') as fp:
    f = json.load(fp)
    vs = create_vertex(f['vertex'])
    print '\n'.join(vs)

