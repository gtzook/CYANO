import numpy as np

def interpolate_color(start_color, end_color, num_steps):
    # Create linearly spaced values between start and end color components
    r = np.linspace(start_color[0], end_color[0], num_steps)
    g = np.linspace(start_color[1], end_color[1], num_steps)
    b = np.linspace(start_color[2], end_color[2], num_steps)

    # Stack the RGB values into a NumPy array
    interpolated_colors = np.column_stack((r, g, b)).astype(int)

    return interpolated_colors

# RGB values for the rainbow colors
rainbow_colors = [
    (255, 0, 0),     # Red
    (255, 165, 0),   # Orange
    (255, 255, 0),   # Yellow
    (0, 128, 0),     # Green
    (0, 0, 255),     # Blue
    (75, 0, 130),    # Indigo
    (148, 0, 211)    # Violet
]

# Number of steps for interpolation between each pair of colors
num_steps = 10

# Interpolate between each pair of rainbow colors
interpolated_rainbow = np.concatenate([
    interpolate_color(rainbow_colors[i], rainbow_colors[i + 1], num_steps)
    for i in range(len(rainbow_colors) - 1)
])

# Add final interp
interpolated_rainbow = np.concatenate([interpolated_rainbow,
    interpolate_color(rainbow_colors[len(rainbow_colors)-1], rainbow_colors[0], num_steps)
])

# Convert the result to a NumPy array
interpolated_rainbow = np.array(interpolated_rainbow)

# Display the result
print(interpolated_rainbow)
np.save('interpolated_rainbow.npy', interpolated_rainbow)