import matplotlib.pyplot as plt
import numpy as np

sequential_time = 130053512
multi_thread_times = [131503867, 68501631, 34285338, 17478346, 9013803, 4972394, 4562597, 4153504, 3353392]
ff_times = [133594345, 71319961, 37079476, 19891571, 11343719, 7012673, 5248750, 5079444, 6302879]
ideal_times = [sequential_time / 2 ** i for i in range(0, 9)]
numbers = [1, 2, 4, 8, 16, 32, 64, 128, 256]


def plot_times():
    plt.figure(num=None, figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')
    plt.plot(numbers, ideal_times, label='ideal', linestyle='-', color='k')
    plt.plot(numbers, multi_thread_times, label='multi-thread', linestyle=':', color='k')
    plt.plot(numbers, ff_times, label='fast-flow', linestyle='-.', color='k')
    plt.xscale('log', basex=2)
    plt.xticks([1, 2, 4, 8, 16, 32, 64, 128, 256], [1, 2, 4, 8, 16, 32, 64, 128, 256])
    # plt.yticks(np.arange(0, 160000000, 20000000), ['0', '0.2e8', '0.4e8', '0.6e8', '0.8e8', '1.0e8', '1.2e8', '1.4e8'])
    plt.yticks(np.arange(0, 160000000, 20000000))
    plt.xlabel('N. of workers')
    plt.ylabel('Completion time (ms)')
    plt.legend('True', labels=('Ideal', 'Multi-thread', 'fast-flow'))
    plt.show()


def plot_speedup():
    multi_thread_speedup = [sequential_time / multi_thread_times[i] for i in range(0, 9)]
    ff_speedup = [sequential_time / ff_times[i] for i in range(0, 9)]
    ideal_speedup = [sequential_time / ideal_times[i] for i in range(0, 9)]
    plt.figure(num=None, figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')
    plt.plot(numbers, ideal_speedup, label='ideal', linestyle='-', color='k')
    plt.plot(numbers, multi_thread_speedup, label='multi-thread', linestyle=':', color='k')
    plt.plot(numbers, ff_speedup, label='fast-flow', linestyle='-.', color='k')
    plt.xscale('log', basex=2)
    plt.xticks([1, 2, 4, 8, 16, 32, 64, 128, 256], [1, 2, 4, 8, 16, 32, 64, 128, 256])
    plt.xlabel('N. of workers')
    plt.ylabel('Speedup')
    plt.legend('True', labels=('Ideal', 'Multi-thread', 'fast-flow'))
    plt.show()


def plot_scalability():
    multi_thread_scalability = [multi_thread_times[0] / multi_thread_times[i] for i in range(0, 9)]
    ideal_scalability = [ideal_times[0] / ideal_times[i] for i in range(0, 9)]
    ff_scalability = [ff_times[0] / ff_times[i] for i in range(0, 9)]
    plt.figure(num=None, figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')
    plt.plot(numbers, ideal_scalability, label='ideal', linestyle='-', color='k')
    plt.plot(numbers, multi_thread_scalability, label='multi-thread', linestyle=':', color='k')
    plt.plot(numbers, ff_scalability, label='fast-flow', linestyle='-.', color='k')
    plt.xscale('log', basex=2)
    plt.xticks([1, 2, 4, 8, 16, 32, 64, 128, 256], [1, 2, 4, 8, 16, 32, 64, 128, 256])
    plt.xlabel('N. of workers')
    plt.ylabel('Scalability')
    plt.legend('True', labels=('Ideal', 'Multi-thread', 'fast-flow'))
    plt.show()


def plot_efficiency():
    multi_thread_efficiency = [sequential_time / multi_thread_times[i] / 2 ** i for i in range(0, 9)]
    ff_efficiency = [sequential_time / ff_times[i] / 2 ** i for i in range(0, 9)]
    ideal_efficiency = [sequential_time / ideal_times[i] / 2 ** i for i in range(0, 9)]
    plt.figure(num=None, figsize=(8, 6), dpi=80, facecolor='w', edgecolor='k')
    plt.plot(numbers, ideal_efficiency, label='ideal', linestyle='-', color='k')
    plt.plot(numbers, multi_thread_efficiency, label='multi-thread', linestyle=':', color='k')
    plt.plot(numbers, ff_efficiency, label='fast-flow', linestyle='-.', color='k')
    plt.xscale('log', basex=2)
    plt.xticks([1, 2, 4, 8, 16, 32, 64, 128, 256], [1, 2, 4, 8, 16, 32, 64, 128, 256])
    plt.xlabel('N. of workers')
    plt.ylabel('Efficiency')
    plt.legend('True', labels=('Ideal', 'Multi-thread', 'fast-flow'))
    plt.show()

plot_scalability()