import ctypes

def wrap_cfunc(lib, fname, result, *arg_types):
    return ctypes.CFUNCTYPE(result, *arg_types)((fname, lib))

if __name__ == "__main__":
    lib = ctypes.CDLL("./libdefs.dylib")

    context = """
    helpful wrapper to extract a function from the shared obj
    """
    print context
    my_print = wrap_cfunc(lib, "my_print", None, ctypes.c_int)
    for i in range(10):
        my_print(i)

    context = """
    sugar definitions around the declarations from the lib
    """
    print context
    class Structure(ctypes.Structure):
        def __repr__(self):
            """print the fields"""
            res = []
            for field in self._fields_:
                res.append("%s=%s" % (field[0], repr(getattr(self, field[0]))))
            return self.__class__.__name__ + "(" + ",".join(res) + ")"

        # TODO: understand this
        @classmethod
        def from_param(cls, obj):
            """Magically construct from a tuple"""
            if isinstance(obj, cls):
                return obj
            if isinstance(obj, tuple):
                return cls(obj)
            
            raise TypeError

    class Point(Structure):
        _fields_ = [("x", ctypes.c_int), ("y", ctypes.c_int)]

        def __init__(self, lib, x=1, y=2):
            self.x = x
            self.y = y

            self.func_show_point_by_value = wrap_cfunc(lib, "show_point_by_value",
                                                       None, Point)
            self.func_show_point_by_ref = wrap_cfunc(lib, "show_point_by_ref",
                                                     None, ctypes.POINTER(Point))
            self.func_move_point = wrap_cfunc(lib, "move_point", 
                                              Point, Point, ctypes.c_int, ctypes.c_int)
            self.func_move_point_inplace = wrap_cfunc(lib, "move_point_inplace", 
                                              None, ctypes.POINTER(Point), ctypes.c_int, ctypes.c_int)

        def show_point_by_value(self):
            self.func_show_point_by_value(self)

        def show_point_by_ref(self):
            self.func_show_point_by_ref(self)

        def move_point(self, x, y):
            return self.func_move_point(self, x, y)

        def move_point_inplace(self, x, y):
            self.func_move_point_inplace(self, x, y)

    class String(Structure):
        _fields_ = [
            ("size", ctypes.c_int),
            ("str", ctypes.c_char_p)
        ]

    class StringWrapper(Structure):
        _fields_ = [
            ("str", String)
        ]

    p1 = Point(lib, 1,2)
    print p1
    p1.show_point_by_value()
    p1.show_point_by_ref()
    p2 = p1.move_point(1, 2)
    print p1
    print p2
    p1.move_point_inplace(1, 2)
    print p1
    print p2

    context = """
    testing how to send arrays
    """
    n = 10
    cints = ctypes.c_int * n
    arr = cints(*[i for i in range(n)])
    print_ints = wrap_cfunc(lib, "print_ints", None, cints, ctypes.c_int)
    modify_and_print_ints = wrap_cfunc(lib, "modify_and_print_ints", None, cints, ctypes.c_int)
    print_ints(arr, n)
    modify_and_print_ints(arr, n)

    new_string = wrap_cfunc(lib, "new_string", String)
    s = new_string()
    print s

    new_wrapper = wrap_cfunc(lib, "new_string_wrapper", StringWrapper)
    w = new_wrapper()
    print w
