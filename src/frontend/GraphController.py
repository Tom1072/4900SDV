import random
import matplotlib.pyplot as plt
from collections import deque

class Graph2D:
    def __init__(self, subplot: plt.Axes, show_range: int, title="Graph", grid=True):
        self.title = title
        self.ax = subplot
        self.x_axis, self.y_axis = [], []
        self.grid = grid
        self.show_range = show_range

    def add_data(self, x, y):
        self.ax.clear()

        if self.grid:
            self.ax.grid()

        self.x_axis.append(x)
        self.y_axis.append(y)
        self.ax.set_title(self.title)
        self.ax.axis([max(0, x-self.show_range), x+5, 0, 200])
        return self.ax.plot(self.x_axis, self.y_axis, color="C0")


class GraphController:
    def __init__(self, graph_titles: set[str], show_range=30, grid=True):
        self.graphs = {}
        self.queue = deque()
        self.fig, subplots = plt.subplots(len(graph_titles))
        self.fig.tight_layout(pad=2)
        plt.subplots_adjust(hspace=0.4)

        for i, t in enumerate(graph_titles):
            subplots[i].set_title(t)
            graph = Graph2D(subplots[i], show_range, t, grid)
            self.graphs[t] = graph

    def is_queue_empty(self):
        return len(self.queue) == 0
    
    def enqueue(self, data_list):
        self.queue.append(data_list)

    def dequeue(self):
        return self.queue.popleft()

    def get_subplot(self, title) -> Graph2D:
        return self.graphs[title]

    def retain_window(self):
        plt.show()

