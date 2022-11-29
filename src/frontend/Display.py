from enum import Enum
from time import sleep
import PySimpleGUI as sg


class Button(Enum):
    BRAKE = "Brake" 
    THROTTLE = "Throttle" 
    ACC = "ACC speed" 
    SPAWN = "Spawn car"
    DESPAWN = "Despawn car"


class Detail(Enum):
    BRAKE_LEVEL = "BL" 
    THROTTLE_LEVEL = "TL" 
    SPEED = "S" 
    DESIRED_SPEED = "DS"
    OBJ_SPEED = "OS"
    DISTANCE = "D"
    SKID = "Sk"


class Color(Enum):
    UNSELECTED = ("white", "royal blue")
    SELECTED = ("black", "gold")


class InputKey(Enum):
    BRAKE_LEVEL = "brake_level_slider"
    SKID = "skidding_true_radio"
    THROTTLE_LEVEL = "throttle_level_slider"
    DESIRED_SPEED = "desired_speed_slider"
    DISTANCE = "distance_slider"
    OTHER_SPEED = "other_speed_slider"


tooltips_map = {
    Detail.BRAKE_LEVEL.value: "Brake level",
    Detail.THROTTLE_LEVEL.value: "Throttle level",
    Detail.SPEED.value: "Speed",
    Detail.DESIRED_SPEED.value: "Desired speed",
    Detail.OBJ_SPEED.value: "Other car speed",
    Detail.DISTANCE.value: "Distance",
    Detail.SKID.value: "Skidding",
}


BUTTON_HEIGHT = 1
BUTTON_FONT = ('Arial', 10, 'bold')
CAR_FRONT_FLAG_KEY = "car_front_tbn"


class Display:
    def __init__(self):
        sg.theme('Light Brown 3')
        self.env = {
            Detail.BRAKE_LEVEL.value: 0,
            Detail.THROTTLE_LEVEL.value: 0,
            Detail.SPEED.value: 0,
            Detail.DESIRED_SPEED.value: 0,
            Detail.OBJ_SPEED.value: 0,
            Detail.DISTANCE.value: 0,
            Detail.SKID.value: False,
            "object": False
        }

        brake_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Level")], [sg.Slider(key=InputKey.BRAKE_LEVEL, range=(0, 100), orientation="h", size=(22, 14))]]),
                sg.Column([[sg.Text("Skid")], [sg.Radio("on", group_id="skid", key=InputKey.SKID), sg.Radio("off", group_id="skid", default=True)]]),
                sg.Button("Brake", size=(10, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(450, 70)
        )]
        throttle_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Level")], [sg.Slider(key=InputKey.THROTTLE_LEVEL, range=(0, 100), orientation="h", size=(22, 15))]]),
                sg.Button("Speed up", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(450, 70)
        )]
        acc_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Desired speed")], [sg.Slider(key=InputKey.DESIRED_SPEED, range=(0, 100), orientation="h", size=(22, 15))]]),
                sg.Button("Set ACC", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(450, 70)
        )]
        spawn_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Distance")], [sg.Slider(key=InputKey.DISTANCE, range=(0, 100), orientation="h", size=(18, 15))]]),
                sg.Column([[sg.Text("Other car speed")], [sg.Slider(key=InputKey.OTHER_SPEED, range=(0, 100), orientation="h", size=(18, 15))]]),
                sg.Button("Spawn", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(450, 70)
        )]
        despawn_input = [sg.Frame(
            "",
            [[
                sg.Button("No car in front", key=CAR_FRONT_FLAG_KEY, size=(20, 4), font=('Arial', 15, 'bold'), disabled=True, disabled_button_color="white"),
                sg.Button("Despawn", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(450, 39)
        )]

        self.input_map = {
            Button.BRAKE.value: brake_input,
            Button.THROTTLE.value: throttle_input,
            Button.ACC.value: acc_input,
            Button.SPAWN.value: spawn_input,
            Button.DESPAWN.value: despawn_input,
        }

        DETAIL_DISPLAY_SIZE = 6
        layout = [
            [sg.Text('Long task to perform example')],
            [
                sg.Column([
                    [sg.Text(d.value, key="t"+d.value, size=DETAIL_DISPLAY_SIZE, justification="c")],
                    [sg.Input(key=d.value, justification="c", size=DETAIL_DISPLAY_SIZE, disabled=True)]
                ], justification="c") for d in Detail
            ],
            [sg.Column([i for i in self.input_map.values()], key="inputs", justification="c")],
            [sg.Button("Exit")],
        ]

        self.window = sg.Window('Multithreaded Window', layout, finalize=True)


    def set_detail_tooltips(self):
        for d in Detail:
            self.window["t" + d.value].set_tooltip(tooltips_map[d.value])


    def update_details(self):
        btn_color = "green"
        text = "No car in front"

        if self.env["object"]:
            btn_color = "red"
            text = "A car in front"

        self.window[CAR_FRONT_FLAG_KEY].update(text=text, button_color=btn_color)

        for d in Detail:
            self.window[d.value].update(value=str(self.env[d.value]))
        

    def run(self):
        """
        Starts and executes the GUI
        Reads data from a Queue and displays the data to the window
        Returns when the user exits / closes the window
        """

        # --------------------- EVENT LOOP ---------------------
        self.set_detail_tooltips()
        while True:
            self.update_details()
            # TODO: Add UDP receiver
            event, values = self.window.read(timeout=0)
            if event in (sg.WIN_CLOSED, 'Exit'):
                break
            elif event == "Brake":
                command = f"brake {int(values[InputKey.BRAKE_LEVEL])} {'on' if values[InputKey.SKID] else 'off'}"
                print(command)
            elif event == "Speed up":
                command = f"gas {int(values[InputKey.THROTTLE_LEVEL])}"
                print(command)
            elif event == "Set ACC":
                command = f"acc-speed {int(values[InputKey.DESIRED_SPEED])}"
                print(command)
            elif event == "Despawn":
                command = "despawn"
                print(command)
            elif event == "Spawn":
                command = f"spawn {int(values[InputKey.DISTANCE])} {int(values[InputKey.OTHER_SPEED])}"
                print(command)

            # print("Skid", values[InputKey.SKID])
            # print("Brake lvl", values[InputKey.BRAKE_LEVEL])
            # print("Throttle lvl", values[InputKey.THROTTLE_LEVEL])
            # print("Desired speed", values[InputKey.DESIRED_SPEED])
            # print("Distance", values[InputKey.DISTANCE])
            # print("Other speed", values[InputKey.OTHER_SPEED])

        # if user exits the window, then close the window and exit the GUI func
        self.window.close()

if __name__ == '__main__':
    Display().run()