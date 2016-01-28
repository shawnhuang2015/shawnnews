import json
from BizObjBase import BizObjBase

class JsonBizObj(BizObjBase):
    def __init__(self, body):
        content = None
        try:
            content = json.loads(body)
            super(JsonBizObj,self).__init__(content)
        except ValueError, e:
            self.isValid = False

if __name__ == '__main__':
    obj = JsonBizObj('"name alice"}')
    print obj
    print obj.isValid
    obj = JsonBizObj('{"name": "alice"}')
    print obj
    print obj.name
    print obj.isValid

    obj = JsonBizObj('{"products":[{"item":"ipad"},{"item":"bottle"}]}')
    print obj.products
