import json
import requests

class BizObjectBase(object):
    def gload(self, gpath):
        obj = '''
            {
                "user":
                {
                    "user_id":"111",
                    "gender":"M",
                    "age":"26"
                }
            }
        '''
        return json.loads(obj)

    def restCall(self, uri):
        resp = requests.get(uri)
        return json.loads(resp.json())

if __name__ == '__main__':
    objBase = BizObjectBase()
    print objBase.gload("dummyGPath")
    print objBase.restCall("http://services.groupkt.com/country/get/all")

