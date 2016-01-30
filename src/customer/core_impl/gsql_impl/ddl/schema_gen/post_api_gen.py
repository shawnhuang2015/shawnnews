
import re

# sadly, re doesn't support repeated groups capture
# type, id, attrs
V_PATTERN = 'create\s*vertex\s*(?P<type>\w+)\s*\(primary_id\s*(?P<id>\w+)[^,]+((?:,)(?P<attrs>.+?)){0,1}\)'
# type, source_type, target_type, attrs
E_PATTERN = 'create\s*(?:\w+)\s*edge\s*(?P<type>\w+)\s*\(\s*from\s*(?P<source_type>[\*\w]+)\s*,\s*to\s*(?P<target_type>[\*\w]+)\s*((?:,)(?P<attrs>.+)){0,1}\)'
A_PATTERN = '\s*(\w+)[^,]*,*'

def post_vertex(m, csv=True):
  csv_job = '''
create online_post job post_{type} for graph cip {{
  load to vertex {type} values ({id_and_attrs});
}}
export job post_{type}
  '''

  json_job = '''
create online_post job post_{type} for graph cip {{
  load to vertex {type} values ({id_and_attrs})
  using json_file="true";
}}
export job post_{type}
'''

  attrs = []
  if m.group('attrs') is not None:
    attrs = re.findall(A_PATTERN, m.group('attrs'))

  if csv:
    id_and_attrs = ['$' + str(_) for _ in range(1+len(attrs))]
    id_and_attrs = ', '.join(id_and_attrs)
    job = csv_job.format(type=m.group('type'), id_and_attrs=id_and_attrs)

  else:
    id = '$"{}"'.format(m.group('type'))
    attrs = ['$"{}"'.format(_) for _ in attrs]
    id_and_attrs = [id] + attrs
    id_and_attrs = ', '.join(id_and_attrs)
    job = json_job.format(type=m.group('type'), id_and_attrs=id_and_attrs)

  return job

def post_edge(m, csv=True):
  csv_job = '''
create online_post job post_{type} for graph cip {{
  load to edge {type} values ({id_and_attrs});
}}
export job post_{type}
'''

  json_job = '''
create online_post job post_{type} for graph cip {{
  load to edge {type} values ({id_and_attrs})
  using json_file="true";
}}
export job post_{type}
'''

  attrs = []
  if m.group('attrs') is not None:
    attrs = re.findall(A_PATTERN, m.group('attrs'))

  if csv:
    id_and_attrs = ['$' + str(_) for _ in range(2+len(attrs))]
    id_and_attrs = ', '.join(id_and_attrs)
    job = csv_job.format(type=m.group('type'), id_and_attrs=id_and_attrs)

  else:
    source_type = '$"{}"'.format(m.group('source_type'))
    target_type = '$"{}"'.format(m.group('target_type'))
    attrs = ['$"{}"'.format(_) for _ in attrs]
    id_and_attrs = [source_type, target_type] + attrs
    id_and_attrs = ', '.join(id_and_attrs)
    job = json_job.format(type=m.group('type'), id_and_attrs=id_and_attrs)

  return job


if __name__ == '__main__':
  schema = 'schema.gsql'
  with open(schema, 'r') as fp:
    for l in fp:
      m = re.search(V_PATTERN, l)
      if m is not None:
        print post_vertex(m, False)
        continue

      m = re.search(E_PATTERN, l)
      if m is not None:
        print post_edge(m, False)
        continue
