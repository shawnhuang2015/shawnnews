
# generate json schema
python ui_schema_fake.py | python -m json.tool > ui_schema.json

# json schema -> gsql schema
python schema_gen.py ui_schema.json > schema.gsql

# schema -> post api
python post_api_gen.py schema.gsql > post_api.gsql

# piece together
cp schema.gsql post_api.gsql /tmp/

cat > /tmp/online_ddl.gsql <<EOF
@/tmp/schema.gsql

EOF

cat post_api.gsql >> /tmp/online_ddl.gsql

cat >> /tmp/online_ddl.gsql <<EOF

clear graph store -HARD
init graph store for graph cip

EOF

cp /tmp/online_ddl.gsql .
