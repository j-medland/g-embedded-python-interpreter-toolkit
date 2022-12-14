class Counter:
    x: int

    def __init__(self,startingValue : int):
        self.x = startingValue
    
    def increment(self):
        self.x += 1
        return self.x

    def decrement(self):
        self.x -= 1
        return self.x