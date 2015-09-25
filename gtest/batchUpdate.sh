cd /home/xinyuchang/poc4.3/product/gtest;./gtest gsql.sh 5 > /tmp/a

grep "Update\: cp"  /tmp/a > /tmp/b
sed 's/Update\://g' /tmp/b > /tmp/a

while read -r line
do
    eval $line
    echo $line
done < "/tmp/a"

rm /tmp/a
rm /tmp/b

echo "done"
