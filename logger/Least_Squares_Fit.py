import numpy as np


input = np.array([3095, 2500, 1987, 952, 910, 792, 720, 625])
output = np.array([1.978, 1.505, 1.134, 0.394, 0.392, 0.197, 0.096, 0]).T

def linear_fit(input, output):
    """
    Takes array of data as input and output
    Returns a tuple with fitted (slope, intercept) output
    """
    A = np.array([[x**2, x, 1] for x in input])
    b = output
    v = np.matmul(np.matmul(np.linalg.inv((np.matmul(A.T, A))),A.T),output)
    return v




quad, slope, intercept = linear_fit(input, output)
print(slope)
print(quad*(2780**2) + slope*2780 + intercept)

print(quad*(677**2) + slope*677 + intercept)
