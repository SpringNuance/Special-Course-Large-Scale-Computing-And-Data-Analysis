import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation

"""
# Simple 1D plot
data0 = np.genfromtxt("data/data0.csv", delimiter=",")
data1 = np.genfromtxt("data/data1.csv", delimiter=",")
data2 = np.genfromtxt("data/data2.csv", delimiter=",")
data3 = np.genfromtxt("data/data3.csv", delimiter=",")


names = ['Ground truth', 'Compressed', 'Absolute error', 'ULP error']
fields = [data0, data1, data2, data3]

rows = 4
fig, axs = plt.subplots(rows, int(len(names) / rows))

for ax, name, field in zip(axs.flatten(), names, fields):
    ax.set_title(name)
    ax.grid()
    ax.plot(field)

fig.text(0.0, 0.99, 'frame = %d' % 0)
fig.text(0.0, 0.97, 'time = %.1f' % 0.0)
plt.show()
"""

"""
# Simple 2D plot
a = np.random.random((16, 16))
plt.imshow(a, origin = 'lower', cmap='hot')
plt.show()
"""

"""
# Simple 2D subplots
dim = 64

lnrho = np.reshape(np.genfromtxt("data/lnrho.csv", delimiter=","), (dim, -1))
ux = np.reshape(np.genfromtxt("data/ux.csv", delimiter=","), (dim, -1))
uy = np.reshape(np.genfromtxt("data/uy.csv", delimiter=","), (dim, -1))


names = ['lnrho', 'ux', 'uy', 'uy again']
fields = [lnrho, ux, uy, uy]

rows = 2
fig, axs = plt.subplots(rows, int(len(names) / rows))

for ax, name, field in zip(axs.flatten(), names, fields):
    ax.set_title(name)
    im = ax.imshow(field, cmap = 'hot', origin = 'lower', interpolation = 'bicubic')

fig.colorbar(im, ax = axs.ravel().tolist())
plt.show()

def test():
    print("Hello!")
"""


# Animated
global index
index = 0


ground_truth = np.random.rand(64, 64)

names = ['Ground truth', 'Compressed (r = 0.75)', 'Compressed (r = 0.5)', 'Compressed (r = 0.25)']
im = []

rows = 2
fig, axs = plt.subplots(rows, int(len(names) / rows))
for ax, name in zip(axs.flatten(), names):
    ax.set_title(name)
    im.append(ax.imshow(ground_truth, cmap = 'hot', origin = 'lower', interpolation = 'bicubic', animated = True))

text = fig.text(0.0, 0.99, 'frame = %d' % index)

def animate(i):
    global index
    dim = 64
    #im[0].set_array(np.reshape(np.genfromtxt("data/lnrho_%s.csv" % index, delimiter=","), (dim, -1)))
    #im[1].set_array(np.reshape(np.genfromtxt("data/ux_%s.csv" % index, delimiter=","), (dim, -1)))
    #im[2].set_array(np.reshape(np.genfromtxt("data/uy_%s.csv" % index, delimiter=","), (dim, -1)))

    im[0].set_array(np.reshape(np.genfromtxt("data/base_lnrho_%s.csv" % index, delimiter=","), (dim, -1)))
    im[1].set_array(np.reshape(np.genfromtxt("data/a_lnrho_%s.csv" % index, delimiter=","), (dim, -1)))
    im[2].set_array(np.reshape(np.genfromtxt("data/b_lnrho_%s.csv" % index, delimiter=","), (dim, -1)))
    im[3].set_array(np.reshape(np.genfromtxt("data/c_lnrho_%s.csv" % index, delimiter=","), (dim, -1)))

    text.set_text('frame = %d' % index)
    index = (index + 1)

    from pathlib import Path
    next = Path("data/base_lnrho_%s.csv" % index)
    if not next.exists():
        index = 0

    # im[0].set_array(np.random.rand(32, 32))
    #im[1].set_array(np.random.rand(32, 32))
    #im[2].set_array(np.random.rand(32, 32))
    #im[3].set_array(np.random.rand(32, 32))
    return im[0], im[1], im[2], im[3]

fig.colorbar(im[0], ax = axs.ravel().tolist())
anim = animation.FuncAnimation(fig, animate, blit = False, save_count=300)

anim.save('turbulence.mp4', fps = 24, extra_args=['-vcodec', 'libx264'])
plt.show()
