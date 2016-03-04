LOG_FILE=/tmp/sc.log

{

if [[ $# != 3 ]]; then
  echo "usage: bash sc.sh diff.json old.json new.json"
  exit 1
fi

diff=$1
old=$2
new=$3

python /tmp/sc.py $diff

# FIXME: even `gsql sc.gsql` job fails, the return code is still 0!!! WHY?
grep -i 'fail' $LOG_FILE &>/dev/null
if [[ $? -eq 0 ]]; then
  echo "fail to do schema change"
  exit 1
fi
grep -i 'finish' $LOG_FILE &>/dev/null
if [[ $? -ne 0 ]]; then
  echo "fail to do schema change"
  exit 1
fi

echo "replace old with new schema"
mv $old{,.$(date)}
mv $new $old

# restart gpe
gsql_admin restart -y gpe

} &> $LOG_FILE
