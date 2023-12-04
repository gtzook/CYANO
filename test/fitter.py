import numpy as np
import matplotlib.pyplot as plt
# Given data
data = [(100, 511), (70, 652),(41, 854), (27, 1090), (14, 1462), (2, 3740), (1, 3742), (0.1, 3757)]
# Extract x and y values from the data
y = np.array([np.log10(100/item[0]) for item in data])
x = np.array([item[1] for item in data])
# Fit a polynomial of degree 2 (you can change this degree as needed)
degree = 3
coefficients = np.polyfit(x, y, degree)
print(coefficients)
# Create a polynomial function using the coefficients
poly_function = np.poly1d(coefficients)
# Generate x values for the fitted curve
x_fit = np.linspace(min(x), max(x), 100)
# Calculate y values for the fitted curve using the polynomial function
y_fit = poly_function(x_fit)
# Plot the original data points and the fitted curve
plt.figure(figsize=(8, 6))
plt.scatter(x, y, label='Data Points')
plt.plot(x_fit, y_fit, label=f'Polynomial Fit (Degree {degree})', color='red')
plt.xlabel('X Values')
plt.ylabel('Y Values')
plt.legend()
plt.grid(True)
plt.title('Polynomial Fit')
plt.show()