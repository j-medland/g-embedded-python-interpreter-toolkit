def my_custom_addition_fn(a :int, b: int):
    return a + b

class MyCustomClass:
    a : int=0
    b: int =0

    def __init__(self,a: int, b: int):
        self.a = a
        self.b = b
    
    def computeSum(self):
        return self.a + self.b

if __name__ == "__main__":
    x = MyCustomClass(100,200)
    print(x.computeSum())