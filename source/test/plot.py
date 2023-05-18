import matplotlib.pyplot as plt
import numpy as np

# Define the pbox1 function
def pbox1(a, b, c):
    e = (a ^ b) ^ ~c
    s0 = ((a >> 2) ^ (a << 12) ^ (a >> (c % 32))) & 0xFFFFFFFF
    s1 = ((e << 6) ^ (e >> 9) ^ (e << (e % 32))) & 0xFFFFFFFF
    return (s0 * e + s1) & 0xFFFFFFFF

# Generate random samples for inputs a, b, and c
num_samples = 100
c = np.random.randint(0, 2**24, num_samples, dtype=np.uint32)
a = np.random.randint(0, 2**24, num_samples, dtype=np.uint32)
b = np.random.randint(0, 2**24, num_samples, dtype=np.uint32)

# Apply the pbox1 function to generate outputs
output = np.zeros(num_samples, dtype=np.uint32)
for i in range(num_samples):
    output[i] = pbox1(a[i], b[i], c[i]) ^ pbox1(b[i], c[i], a[i])

# Extract the first 8 bits of the output
output_bits = (output >> 24) & 0xFF

# Plot the first 8 bits of the output in a distribution
plt.hist(output_bits, bins=128)
plt.xlabel('Input a')
plt.ylabel('Input b')
plt.title('First 8 bits of pbox1 output')
plt.show()
