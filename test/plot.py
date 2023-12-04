import matplotlib.pyplot as plt
import numpy as np
# Data points
data = [
    (100, 503), (90, 538), (80, 563),
    (70, 607), (60, 651), (50, 711),
    (40, 788), (30, 929), (20, 1084),
    (10, 1567), (0, 2891)
]
# Unpacking the data into x and y coordinates
x, y = zip(*data)
xs = np.linspace(0.000000001,100,500)
ys = 500 + 1000/(0.1*xs)
ys_log = np.log10(100/xs)

new_xs = np.linspace(x[0],x[-1],100)
new_ys = (10*1567)/(new_xs[:-1])
print(new_ys)
new_ys = np.append(new_ys,y[-1])
print(len(new_xs), len(new_ys))
# Plotting the graph
#plt.figure(figsize=(10, 6))
#plt.plot(x, y, marker='o')
#plt.plot(new_xs,new_ys)
plt.plot(xs,ys_log)
plt.title('Graph of Given Data Points')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.grid(True)
plt.show()