import matplotlib.pyplot as plt
import random

class GraphController:
    def __init__(self, graph_titles: set[str], show_range=30, grid=True):
        self.graphs = {}
        self.fig, subplots = plt.subplots(len(graph_titles))
        self.fig.tight_layout(pad=2)
        plt.subplots_adjust(hspace=0.4)
        for i, t in enumerate(graph_titles):
            graph = Graph2D(subplots[i], show_range, t, grid)
            self.graphs[t] = graph


    def add_data(self, title, x, y):
        self.graphs[title].add_data(x, y)
        plt.pause(0.00001)


    def retain_window(self):
        plt.show()


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
        self.ax.plot(self.x_axis, self.y_axis, color="C0")


if __name__ == "__main__":
    titles = ["Distance", "Speed"]
    graph_control = GraphController(titles)
    i = 0

    while i < 100:
        for t in titles:
            graph_control.add_data(t, i, random.randint(0, 200))
            i += 1


    graph_control.retain_window()