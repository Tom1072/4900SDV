import csv
import matplotlib.pyplot as plt
from collections.abc import Iterable

col_names = ["interval", "gas", "brake","speed", "object", "object_speed","distance", "skid"]
col_types = [int, int, int, float, lambda x: bool(int(x)), float, lambda d: float(d), lambda x: bool(int(x))]
INTERVAL = 50

class GraphController:
    def __init__(self):
        self.graphs = []

        # for i in range(len(subplots)):
        #     subplots[i] = my_subplots[i]

    def add_graph(self, x_type, y_type, data):
        graph = Graph2D(x_type, y_type)
        xi, yi = col_names.index(x_type), col_names.index(y_type)

        for row in data:
            x, y = row[xi], row[yi]
            graph.add_data(x, y)

        self.graphs.append(graph)


    def display_graph(self, x_right_limit=None, grid=False):
        fig, subplots = plt.subplots(len(self.graphs), sharex=True)

        if not isinstance(subplots, Iterable):
            subplots = [subplots] 
            
        for i, sp in enumerate(subplots):
            g = self.graphs[i]
            sp.plot(g.x_axis, g.y_axis)
            sp.grid(grid)
            sp.set_xlabel(g.x_label)
            sp.set_ylabel(g.y_label)
            sp.set_xlim(left=0, right=x_right_limit)
            sp.label_outer()
        fig.tight_layout(pad=2)
        fig.show()
        plt.subplots_adjust(hspace=0.1)

class Graph2D:
    def __init__(self, x_label, y_label):
        self.x_label = x_label
        self.y_label = y_label
        self.x_axis, self.y_axis = [], []

    def add_data(self, x, y):
        if y >= 9999:
            return

        self.x_axis.append(x)
        self.y_axis.append(y)
        # self.ax.set_title(self.title)
        # self.ax.plot(self.x_axis, self.y_axis, color="C0")


class CSVParser:
    def __init__(self, filename):
        self.filename = filename
        self.index_map = {n: i for i, n in enumerate(col_names)}
        self.data = []
        self.graphs = []
        self.graph_controller = GraphController()
        self.parse_csv()


    def parse_csv(self):
        try:
            with open(self.filename, 'r') as csv_file:
                reader = csv.reader(csv_file)
                for i, row in enumerate(reader):
                    data_row = self.str_row_to_numbers([i * INTERVAL] + row)
                    self.data.append(data_row)
        except FileNotFoundError:
            print("Error: Input file doesn't exist")
    

    def str_row_to_numbers(self, row):
        if len(row) > len(col_names):
            return []

        return tuple(col_types[i](val) for i, val in enumerate(row))
    

    def print_data(self):
        print(self.data)
    
    
    def plot_data(self):
        self.graph_controller.display_graph(self.data[-1][0])
    

    def add_graph(self, y_type: str, x_type="interval"):
        '''
        Takes a pair of data type to add a graph on x and y axis.
        Default of x axis will be "interval".
        E.g. y_axis = "gas" and x_axis = "interval" 
        '''
        self.graph_controller.add_graph(x_type, y_type, self.data)


if __name__ == "__main__":
    # csv_reader = CSVParser("./csv_files/data_log.csv")
    # csv_reader.add_graph("distance")
    # csv_reader.add_graph("speed")
    # csv_reader.plot_data()

    csv_reader = CSVParser("./csv_files/ABS_test2.csv")
    csv_reader.add_graph("speed")
    csv_reader.add_graph("skid")
    csv_reader.add_graph("brake")
    csv_reader.plot_data()

    plt.show()