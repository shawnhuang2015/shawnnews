from BizObjBase import BizObjBase 
import requests

class RestBizObj(BizObjBase):
    def __init__(self, restUri, payload = None):
        self.obj = None
        try:
            response = requests.get(restUri,data = payload, timeout = 0.1)
            if response.status_code == 200:
                super(RestBizObj,self).__init__ (response.json())
            else:
                self.isValid = False
        except requests.exceptions.RequestException as e:
            # TODO: logging
            print e
            self.isValid = False

class RestPostBizObj(BizObjBase):
    def __init__(self, restUri, payload ):
        self.obj = None
        try:
            response = requests.post(restUri,data = payload, timeout = 0.1)
            if response.status_code == 200:
                super(RestPostBizObj,self).__init__ (response.json())
            else:
                self.isValid = False
        except requests.exceptions.RequestException as e:
            # TODO: logging
            print e
            self.isValid = False
if __name__ == "__main__":
    rest = RestBizObj("http://www.google.com")
    print rest.isValid
    rest = RestBizObj("http://192.168.33.70:5000/testrest")
    print rest.isValid
    if rest.isValid:
        print rest.products
    rest = RestBizObj("http://192.168.33.70:5000/cip/api/2.0/recommend", '{"alg":"advanced"}')
    print rest.isValid
    if rest.isValid:
        print rest.products


