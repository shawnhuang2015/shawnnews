import json
import jsonschema

class SchemaValidator(object):
    def __init__(self, schemafile):
        self.schema = json.loads(open(schemafile).read())
        self.errors = []

    def validate(self, data):
        try:
            v = jsonschema.Draft4Validator(self.schema, format_checker=jsonschema.FormatChecker())
            for error in sorted(v.iter_errors(json.loads(data)), key = str):
                self.errors.append(error.message)
        except jsonschema.ValidationError as e:
            self.errors.append(e.message)


appSchemaValidator = SchemaValidator("/home/feng.chen/gitrepo/product/cip/ruleEngD/config/request_schema.json")

if __name__ == "__main__":
    validator = SchemaValidator("request_schema.json")
    json.loads(open("sample.json").read())
    validator.validate(open("sample.json").read())
    for error in validator.errors:
        print error

#try:
#	jsonschema.validate(json.loads(data), json.loads(schema))
#except jsonschema.ValidationError as e:
#	print e.message
#except jsonschema.SchemaError as e:
#  #	print e

#  try:
	#  v = jsonschema.Draft4Validator(json.loads(schema), format_checker=jsonschema.FormatChecker())
	#  for error in sorted(v.iter_errors(json.loads(data)), key = str):
		#  print (error.message)
#  except jsonschema.ValidationError as e:
	#  print e.message
