import matplotlib.pyplot as plt
import numpy as np

x = []
y = []
for theta in np.linspace(0,10*np.pi):
    r = ((theta)**2)
    x.append(r*np.cos(theta))
    y.append(r*np.sin(theta))

plt.plot(x,y)
plt.show()
