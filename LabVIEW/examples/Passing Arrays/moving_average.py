import numpy as np

class MovingAverager:
    depth = 5
    insertAt = 0
    sampleSize = 0

    def __init__(self, sampleSize : int):
        self.memory = np.zeros(self.depth * sampleSize)
        self.sampleSize = sampleSize
    
    def add_sample(self, sample : np.ndarray):
        end = self.insertAt+self.sampleSize
        self.memory[self.insertAt:end] = sample
        self.insertAt = end % (self.depth * self.sampleSize)
        return self.memory.mean()