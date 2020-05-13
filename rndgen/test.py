import sys

import random
import string

def randomStringwithDigitsAndSymbols(stringLength=10):
    password_characters = string.ascii_letters + string.digits + string.punctuation
    return ''.join(random.choice(password_characters) for i in range(stringLength))

def main():
    n = int(sys.argv[1])
    randomStr = randomStringwithDigitsAndSymbols(n)
    print(randomStr)

if __name__ == "__main__":
    main()
