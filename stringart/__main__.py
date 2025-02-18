import cProfile
import pstats

import matplotlib.pyplot as plt
import time as tm

from stringart import StringArtGenerator
from filehandling import browseFiles

if __name__ == '__main__':
    outfile = open('stringart/output/NailOutput.txt', 'w')

    profiler = cProfile.Profile()
    profiler.enable()

    generator = StringArtGenerator()
    generator.load_image(browseFiles())
    generator.preprocess()
    generator.set_nails(400)
    generator.set_seed(0)
    generator.set_iterations(5000)
    pattern = generator.generate(outfile)

    lines_x = []
    lines_y = []
    for i, j in zip(pattern, pattern[1:]):
        lines_x.append((i[0], j[0]))
        lines_y.append((i[1], j[1]))

    xmin = 0.
    ymin = 0.
    xmax = generator.data.shape[0]
    ymax = generator.data.shape[1]

    plt.ion()
    plt.figure(figsize=(8, 8))
    plt.axis('off')
    axes = plt.gca()
    axes.set_xlim([xmin, xmax])
    axes.set_ylim([ymin, ymax])
    axes.get_xaxis().set_visible(False)
    axes.get_yaxis().set_visible(False)
    axes.set_aspect('equal')
    plt.draw()

    batchsize = 100  # Number of Lines per sketch
    for i in range(0, len(lines_x), batchsize):
        plt.plot(lines_x[i:i+batchsize], lines_y[i:i+batchsize],
                 linewidth=0.05, color='k')
        plt.draw()
        plt.pause(0.0000000001)

    plt.savefig('stringart/output/Output.png', dpi=500,
                bbox_inches='tight', pad_inches=0)

    profiler.disable()
    stats = pstats.Stats(profiler).sort_stats('cumtime')
    # stats.print_stats()

    outfile.close()
