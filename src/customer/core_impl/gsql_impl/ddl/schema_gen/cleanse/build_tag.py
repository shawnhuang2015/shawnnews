import sys, time
from bisect import bisect_left

AGE_STEP = 5
AGE_BUCKET = range(1, 101, 5)
GENDER_BUCKET = ('male', 'female', 'n/a')
YEAR = time.localtime().tm_year

# "age" ontology is 5yr-segment, e.g. "0~5,5~10"
def build_age_tree(fp=sys.stdout):
  n = len(AGE_BUCKET)
  for i in range(1, n):
    fp.write('age,{0}~{1}\n'.format(AGE_BUCKET[i-1], AGE_BUCKET[i]))

# "gender" ontology
def build_gender_tree(fp=sys.stdout):
  n = len(GENDER_BUCKET)
  for i in range(0, n):
    fp.write('gender,{0}\n'.format(GENDER_BUCKET[i]))

# given "user,birthday", create "user,age_tag"
def build_user_age(ifp, isep='\t', ofp=sys.stdout, osep=';'):
  for l in ifp:
    l = l.strip().split(isep)
    uid = l[0]
    day = l[1]
    year = int(time.strptime(day, '%Y/%m/%d').tm_year)
    age = YEAR - year + 1
    i = bisect_left(AGE_BUCKET, age)
    if AGE_BUCKET[i] > age:
      i -= 1
    ofp.write('{0}{1}{2}~{3}\n'.format(uid, osep, AGE_BUCKET[i], AGE_BUCKET[i+1]))


if __name__ == '__main__':
  if sys.argv[1] == 'age_tree':
    build_age_tree()
  elif sys.argv[1] == 'gender_tree':
    build_gender_tree()
  elif sys.argv[1] == 'user_age':
    with open(sys.argv[2], 'r') as fp:
      build_user_age(fp)
