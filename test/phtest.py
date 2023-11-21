from USB.adc import ADC
import time
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
adc = ADC()
phs = [0] * 5

figure, ax = plt.subplots()
points, = plt.plot(range(5),phs)

# This function is called periodically from FuncAnimation
def animate(i, xs, ys):
    ph = adc.get_ph()

    ys.append(ph)

    # Limit y list to 5
    ys = ys[-5:]

    # Draw x and y lists
    ax.clear()
    ax.plot(xs, ys)

    # Format plot
    plt.xticks(range(5))
    plt.ylim(5,9)
    plt.title('pH')

# Set up plot to call animate() function periodically
ani = FuncAnimation(figure, animate, fargs=(range(5), phs), interval=1, cache_frame_data=False)
plt.show()