import sys, os

if __name__ == '__main__':
  ifile = sys.argv[1]
size = int(sys.argv[2])
  ofile = '/tmp/.chunk'
  with open(ifile, 'r') as ifp:
  out = []
  i = 0
  while True:
l = ifp.readline()
  if len(l) > 0:
out.append(l)
  i += 1
  else:
  break
  if i == size:
# write, post
  with open(ofile, 'w') as ofp:
  ofp.write(''.join(out))
  out = []
  os.system("curl -X POST 'http://localhost:9000/set_item_tag?object=picture&name=type&more=0' --data-binary @{0}".format(ofile))
#        os.system("curl -X POST 'http://localhost:9000/set_user_tag?object=inuser&name=tag&more=0&sep=,' --data-binary @{0}".format(ofile))
#        os.system('cat {0}'.format(ofile))
  i = 0

  with open(ofile, 'w') as ofp:
  ofp.write(''.join(out))
  out = []
  os.system("curl -X POST 'http://localhost:9000/set_item_tag?object=picture&name=type&more=0' --data-binary @{0}".format(ofile))
#    os.system("curl -X POST 'http://localhost:9000/set_user_tag?object=inuser&name=tag&more=0&sep=,' --data-binary @{0}".format(ofile))
