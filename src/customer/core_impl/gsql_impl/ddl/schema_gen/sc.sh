LOG_FILE=/tmp/sc.log

{

if [[ $# != 3 ]]; then
  echo "usage: bash sc.sh diff.json old.json new.json" >> stderr
  exit 1
fi

diff=$1
old=$2
new=$3

python /tmp/sc.py $diff
if [[ $? -eq 0 ]]; then
  echo "replace old with new schema"
  mv $old{,.$(date)}
  mv $new $old

  # restart gpe
  gsql_admin restart -y gpe
fi

} &> $LOG_FILE
