#!/usr/bin/python

import sys
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation

def visualize(nx, ny):
    # Setup
    global index
    index = 0
    ground_truth = np.random.rand(nx, ny)

    names = ['Ground truth', 'Compressed (r = 0.75)', 'Compressed (r = 0.5)', 'Compressed (r = 0.25)']
    im = []

    rows = 2
    fig, axs = plt.subplots(rows, int(len(names) / rows))
    for ax, name in zip(axs.flatten(), names):
        ax.set_title(name)
        im.append(ax.imshow(ground_truth, cmap = 'hot', origin = 'lower', interpolation = 'bicubic', animated = True))

    text = fig.text(0.0, 0.99, 'frame = %d' % index)

    # Animate
    def animate(i):
        global index

        im[0].set_array(np.reshape(np.genfromtxt("../data/base_rho_%s.csv" % index, delimiter=","), (nx, -1)))
        im[1].set_array(np.reshape(np.genfromtxt("../data/base_ux_%s.csv" % index, delimiter=","), (nx, -1)))
        im[2].set_array(np.reshape(np.genfromtxt("../data/base_uy_%s.csv" % index, delimiter=","), (nx, -1)))
        im[3].set_array(np.reshape(np.genfromtxt("../data/base_lnrho_%s.csv" % index, delimiter=","), (nx, -1)))

        text.set_text('frame = %d' % index)
        index = (index + 1)

        from pathlib import Path
        next = Path("../data/base_rho_%s.csv" % index)
        if not next.exists():
            index = 0

        return im[0], im[1], im[2], im[3]

    # Draw
    fig.colorbar(im[0], ax = axs.ravel().tolist())
    anim = animation.FuncAnimation(fig, animate, blit = False, save_count=50)

    anim.save('turbulence.mp4', fps = 24, extra_args=['-vcodec', 'libx264'])
    plt.show()

if len(sys.argv) != 3:
    print("Usage: ./visualize.py <nx> <ny>")
else:
    visualize(int(sys.argv[1]), int(sys.argv[2]))
