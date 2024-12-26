import matplotlib.pyplot as plt

def print_array(name, array):
    print("static const std::uint8_t %s[] = {"%name, end='')
    for i in range(len(array) - 1):
        print("%d,"%array[i], end='')
    print("%d};"%array[-1])

def generate(colormap):
    rs = []
    gs = []
    bs = []
    for i in range(256):
        r, g, b, _ = colormap(i)
        rs.append(int(r * 255))
        gs.append(int(g * 255))
        bs.append(int(b * 255))
    print_array('r', rs)
    print_array('g', gs)
    print_array('b', bs)

# print(plt.cm.jet(1))
# plt.cm.bwr, plt.cm.bwr, plt.cm.plasma
generate(plt.cm.jet)
