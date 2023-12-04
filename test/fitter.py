import numpy as np
import matplotlib.pyplot as plt
# Given data
data = [(100, 511), (99, 527), (98, 532), (97, 534), (96, 546), (95, 547), (94, 550), (93, 554), (92, 559), (91, 563), (90, 563), (89, 566), (88, 582), (87, 580), (86, 580), (85, 579), (84, 598), (83, 598), (82, 598), (81, 599), (80, 598), (79, 611), (78, 615), (77, 615), (76, 631), (75, 631), (74, 636), (73, 631), (72, 654), (71, 654), (70, 652), (69, 652), (68, 676), (67, 668), (66, 667), (65, 671), (64, 691), (63, 691), (62, 700), (61, 700), (60, 695), (59, 711), (58, 716), (57, 719), (56, 743), (55, 748), (54, 742), (53, 743), (52, 775), (51, 772), (50, 780), (49, 780), (48, 806), (47, 811), (46, 810), (45, 814), (44, 851), (43, 847), (42, 846), (41, 854), (40, 847), (39, 896), (38, 898), (37, 898), (36, 950), (35, 943), (34, 935), (33, 948), (32, 1006), (31, 1006), (30, 1006), (29, 1002), (28, 1078), (27, 1090), (26, 1088), (25, 1079), (24, 1175), (23, 1174), (22, 1167), (21, 1174), (20, 1163), (19, 1294), (18, 1294), (17, 1299), (16, 1472), (15, 1474), (14, 1462), (13, 1472), (12, 1723), (11, 1719), (10, 1731), (9, 1732), (8, 2200), (7, 2196), (6, 2196), (5, 2187), (4, 3669), (3, 3716), (2, 3740), (1, 3742), (0, 3757)]
# Extract x and y values from the data
y = np.array([item[0] for item in data])
x = np.array([item[1] for item in data])
# Fit a polynomial of degree 2 (you can change this degree as needed)
degree = 2
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