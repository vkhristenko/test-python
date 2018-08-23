import ctypes

libtest = ctypes.CDLL("./libtest.dylib")

#
# test native types + functions
#
print "test add_int"
print libtest.add_int(1,1)

print "test add_double"
libtest.add_double.argtypes = [ctypes.c_double, ctypes.c_double]
libtest.add_double.restype = ctypes.c_double
print libtest.add_double(10.5, 15.8)

#
# declare a class
#
class Point(ctypes.Structure):
    _fields_ = [("x", ctypes.c_int), ("y", ctypes.c_int)]

    def __repr__(self):
        return "({0}, {1})".format(self.x, self.y)

#
# test a function with a passed by value c struct
#
print "test a function with a passed by value c struct"
show_point = libtest.show_point
p = Point(1,2)
show_point(p)
print(p)

#
# test a function with a passed by ref c struct
#
print "test a function with a passed by ref c function"
show_point_byref = libtest.show_point_byref
show_point_byref.argtypes = [ctypes.POINTER(Point)]
show_point_byref(ctypes.pointer(p))
print(p)
