import sys, math

def run(value):
    top = int(math.sqrt(value)) + 1
    for i in range(2, top):
        if value % i == 0:
            return False
    return True

if __name__ == "__main__":
    value = int(sys.argv[1])
    print("%d is prime" % value) if run(value) else print("%d is not prime" % value)
