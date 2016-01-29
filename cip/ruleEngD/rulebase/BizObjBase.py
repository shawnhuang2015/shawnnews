import json

class BizObjBase(object):
    def __init__(self, body):
        self.isValid = False
        if type(body) == dict:
            self.__dict__ = body
            self.isValid = True
        
    def __str__(self):
        return ",".join('{} = {}'.format(k,v) for k,v in self.__dict__.iteritems())


if __name__ == '__main__':
    a = {'username':'Alce','age':18}
    obj = BizObjBase(a)
    print obj

    print obj.username
    print obj.age
    obj.username = "Kate"
    print obj.username

