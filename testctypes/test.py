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
# test a c string concatenation
#
print "test c string concatenation"
libtest.my_strconcat.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
libtest.my_strconcat.restype = ctypes.c_char_p
res = libtest.my_strconcat(ctypes.c_char_p("hello "), ctypes.c_char_p("world"))
print res

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

#
# test a nested c struct
#
print "test a nested c struct"
class Line(ctypes.Structure):
    _fields_ = [("start", Point), ("end", Point)]

    def __init__(self, start, end):
        self.start = start
        self.end = end

    def __repr__(self):
        return repr(self.start) + "  " + repr(self.end)

line = Line(Point(1,2), Point(3,4))
show_line = libtest.show_line
show_line(line)
print(line)

#
# test out passing around a dummy struct with opened FileContext
#
print "test out passing around a dummy struct with a FILE* as a member"
class File(ctypes.Structure):
    pass
class FileContext(ctypes.Structure):
    _fields_ = [("file", ctypes.POINTER(File))]
open_context_toread = libtest.open_context_toread
open_context_toread.restype = FileContext
close_context = libtest.close_context
ctx = open_context_toread()
close_context(ctx)
ctx = open_context_toread()
close_context(ctx)

#
# test out write/read byte buffers (char*) to/from FILE*
#
print "test out write/read byte buffers (char*) to/from FILE*"
open_context_towrite = libtest.open_context_towrite
open_context_towrite.restype = FileContext
ctx = open_context_towrite()

write_buffer = libtest.write_buffer
buf = ctypes.c_char_p("abcdefg")
print buf, buf.value
write_buffer(ctx, buf, ctypes.c_int(len(buf.value)))
close_context(ctx)

read_buffer = libtest.read_buffer
buf  = ctypes.create_string_buffer(100)
ctx = open_context_toread()
read_buffer(ctx, buf, 100)
close_context(ctx)

print buf
print dir(buf)
print buf.value
print buf.raw
