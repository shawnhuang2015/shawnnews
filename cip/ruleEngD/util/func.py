# functions applied to list where each elemen is a dict
def __R_SUM_DICT_ARRAY(_list):
    sum = 0
    for item in _list:
        if type(item) == dict:
            for k,v in item.iteritems():
                sum += v
    return sum

def __R_MERGE_DICT_ARRAY(_list1, _list2):
    _set = set()

    if not _list1 is None:
        for item in _list1:
            if type(item) == dict:
                for k,v in item.iteritems():
                    _set.add(v)
    if not _list2 is None:
        for item in _list2:
            if type(item) == dict:
                for k,v in item.iteritems():
                    _set.add(v)

    return list(_set)
