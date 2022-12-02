import multiprocessing as mp
from GraphController import GraphController
from matplotlib.animation import FuncAnimation
from Display import start_display_process, GRAPH_TITLES_MAP

reward = 1
started = False
animation = None

def gen():
    global reward
    while reward > 0 :
        reward = min(10, reward + 1)
        yield reward


def update(graph_control: GraphController):
    # global reward, started
    reward = 1
    # if not started:
    #     return []
    data_list = plotter_pipe.recv()

    if reward > 2 and data_list == None:
        reward = 0
        return []
    # data_list = graph_control.dequeue()
    for title, x, y in data_list:
        subplot = graph_control.get_subplot(title)
        data = subplot.add_data(x, y)

    return data


# def start_animation(graph_control: GraphController):
#     animator = FuncAnimation(graph_control.fig, lambda _: update(graph_control), frames=gen, repeat=False, interval=10)


# def start_process(graph_control):
#     plot_process = mp.Process(target=start_animation, args=[graph_control], daemon=True)
#     # plt.show()
#     plot_process.start()


if __name__ == "__main__":
    titles = ["Distance", "Speed"]
    # graph_control = GraphController(titles)
    graph_control = GraphController(GRAPH_TITLES_MAP.keys())
    processor_pipe, plotter_pipe = mp.Pipe()
    display_process = start_display_process(processor_pipe)
    display_process.start()
    # i = 0

    # while i < 100:
    #     new_val = []
    #     for t in titles:
    #         new_val.append((t, i, random.randint(0, 200)))
    #     graph_control.enqueue(new_val)
    #     started = True
    #     i += 1

    animation = FuncAnimation(graph_control.fig, lambda _: update(graph_control), frames=gen, repeat=False, interval=10, blit=True)
    graph_control.retain_window()
    display_process.join()
