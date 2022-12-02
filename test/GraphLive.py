import time
import matplotlib.pyplot as plt
import random
import multiprocessing as mp

class Plotter:
    """
    Handle plotting data on matplotlib for multiprocessing design
    Resource: https://matplotlib.org/stable/gallery/misc/multiprocess_sgskip.html
    """
    def __init__(self):
        self.graphs = {}


    def terminate(self):
        plt.close("all")

    
    def run(self):
        data = self.pipe.get()
        while data != None:
            for title, x, y in data:
                print(title, x, y)
                self.add_data(title, x, y)

            plt.pause(0.00001)


    def add_data(self, title, x, y):
        self.graphs[title].add_data(x, y)


    def __call__(self, pipe, graph_titles: set[str], show_range=30, grid=True):
        self.pipe = pipe
        self.fig, subplots = plt.subplots(len(graph_titles))
        self.fig.tight_layout(pad=2)
        plt.subplots_adjust(hspace=0.5)

        for i, t in enumerate(graph_titles):
            graph = Graph2D(subplots[i], show_range, t, grid)
            self.graphs[t] = graph

        self.run()


class GraphProcessor:
    def __init__(self, graph_titles):
        self.pipe = mp.Queue()
        self.plotter = Plotter()
        self.plot_process = mp.Process(target=self.plotter, args=(self.pipe, graph_titles), daemon=True)
        self.plot_process.start()
    
    
    def plot(self, updates=[], finished=False):
        print(self.pipe.qsize())
        self.pipe.put(updates)
    
    
    def wait(self):
        self.plot_process.join()


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
        print("Finish once")


if __name__ == "__main__":
    titles = ["Distance", "Speed", "Death"]
    graph_processor = GraphProcessor(titles)
    i = 0

    while i < 100:
        graph_processor.plot([(t, i, random.randint(0, 200)) for t in titles])
        i += 1
    
    graph_processor.plot(finished=True)
    graph_processor.wait()
    