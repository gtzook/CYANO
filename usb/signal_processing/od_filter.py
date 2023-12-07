from collections import deque
from numpy import mean
class od_filter:
    """
    Filter for od data
    """
    def __init__(self, filter_len: int = 10):
        self.data =  deque([0]*filter_len,maxlen=filter_len)
    
    def add(self, val: float):
        self.data.append(val)
        
    def filtered(self) -> float:
        return mean(self.data)