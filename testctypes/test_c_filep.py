import sys

from ctypes import POINTER, Structure, py_object, pythonapi


class File(Structure):
    pass

if sys.version_info[0] > 2:
    convert_file = pythonapi.PyObject_AsFileDescriptor
    convert_file.restype = c_int
else:
    convert_file = pythonapi.PyFile_AsFile
    convert_file.restype = POINTER(File)

convert_file.argtypes = [py_object]

f = open('test.bin', "r")
print f
c_file = convert_file(f)
print c_file
print dir(c_file)
