class X:
    def __init__(self, *fields):
        for field in fields:
            print field
            self.__dict__[field[0]] = field[1]

if __name__ == "__main__":
    x = X(("a", 1), ("b", 2), ("c", 3))
    x.a
    x.b
    x.c
