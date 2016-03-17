import sys, os

SET_USER_AGE = "curl -X POST 'http://localhost:9000/set_user_tag?object=inuser&name=tag&more=0&tag_all=1&sep=;' --data-binary @{0}"
SET_USER_GENDER = "curl -X POST 'http://localhost:9000/set_user_tag?object=inuser&name=tag&more=0&tag_all=1&sep=,' --data-binary @{0}"
SET_PICTURE_TYPE = "curl -X POST 'http://localhost:9000/set_item_tag?object=picture&name=pictype&more=0&tag_all=1&sep=;' --data-binary @{0}"
SET_TAG = SET_USER_AGE

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
      else: break
      if i == size:
        # write, post
        with open(ofile, 'w') as ofp:
          ofp.write(''.join(out))
        out = []
        os.system(SET_TAG.format(ofile))
        i = 0

    if len(out) > 0:
      with open(ofile, 'w') as ofp:
        ofp.write(''.join(out))
      os.system(SET_TAG.format(ofile))
