import matplotlib.pyplot as plt
import matplotlib.animation as animation
import random
from collections import deque
from functools import partial

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
    

    def enqueue(self, title, x, y):
        self.queue.append((title, x, y))


    def dequeue(self):
        return self.queue.popleft()

    
    def get_subplot(self, title) -> Graph2D:
        return self.graphs[title]


    def retain_window(self):
        plt.show()


# def add_data(graph_control: GraphController):
def add_data():
    print("asfa")
    # title, x, y = graph_control.dequeue()
    # subplot = graph_control.get_subplot(title)
    # subplot.add_data(x, y)


anim = None

if __name__ == "__main__":
    titles = ["Distance", "Speed"]
    graph_control = GraphController(titles)
    i = 0

    while i < 100:
        for t in titles:
            # graph_control.add_data(t, i, random.randint(0, 200))
            graph_control.enqueue(t, i, random.randint(0, 200))
            i += 1

    anim = animation.FuncAnimation(
        graph_control.fig, add_data,
        # fargs=(graph_control),
        interval=20
    )

    # anim.save('basic_animation.mp4', fps=30)
    # graph_control.retain_window()
    print(list(graph_control.queue))
