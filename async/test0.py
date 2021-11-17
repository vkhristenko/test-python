import asyncio as aio

def test_yield():
    yield 1
    yield 2
    yield 3
    yield 4
    yield 5

async def myco1():
    print("from 1")

async def myco2():
    print("from 2")

async def printme(value):
    print("value = {}".format(value))

async def async_main():
    await myco1()
    await myco2()

    t1 = aio.create_task(myco1())
    t2 = aio.create_task(myco2())

    await t1; await t2

    l = []
    for i in range(100):
        l.append(aio.create_task(printme(i)))

    for x in l: await x

def main():
    #aio.run(async_main())
    gen = test_yield()
    print(next(gen))
    print(next(gen))

if __name__ == "__main__":
    main()
