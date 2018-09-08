import ctypes

class ListPOINTER(object):
    '''Just like a POINTER but accept a list of ctype as an argument'''
    def __init__(self, etype):
        self.etype = etype

    def from_param(self, param):
        if isinstance(param, (list,tuple)):
            return (self.etype * len(param))(*param)

if __name__ == "__main__":
    arr = (ctypes.c_int *3)()
    print type(arr)
    arr[0] = 1
    arr[1] = 2
    arr[2] = 3
