rm -rf build/ dist/
pyinstaller --hidden-import=logstash_formatter --hidden-import=dateutil.tz.tz  --path=.. --path=/usr/lib/python2.7/site-packages --onefile app.py
#pyinstaller --path=..  app2.spec
src_base='/home/feng.chen/gitrepo/product/cip/ruleEngD'
dest="dist"
mkdir -p ${dest}/web/logs
mv ${dest}/app ${dest}/web/ruleng
dest_path=$(dirname $(readlink -f ${dest}/web/ruleng))

for f in config.ini request_schema.json logging.conf  
do
    cp ${src_base}/config/${f} ${dest}/web
done

# actually relesuites need not to be copied, it's managed by rulemanager
for folder in cust rulesuites 
do
    cp -r ${src_base}/${folder} ${dest}
done

# TODO:rewrite configfile
sed -i "7s|:.*|:${dest_path}/../rulesuites|; 16s|:.*|:${dest_path}/logging.conf|; 24s|:.*|:${dest_path}/request_schema.json|" ${dest}/web/config.ini


