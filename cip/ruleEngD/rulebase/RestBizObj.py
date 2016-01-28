from BizObjBase import BizObjBase 
import requests

class RestBizObj(BizObjBase):
    def __init__(self, restUri):
        self.obj = None
        try:
            response = requests.get(restUri,timeout = 0.1)
            if response.status_code == 200:
                super(RestBizObj,self).__init__ (response.json())
            else:
                self.isValid = False
        except requests.exceptions.RequestException as e:
            # TODO: logging
            self.isValid = False

if __name__ == "__main__":
    rest = RestBizObj("http://www.google.com")
    print rest.isValid
    rest = RestBizObj("http://192.168.33.70:5000/testrest")
    print rest.isValid
    if rest.isValid:
        print rest.products

