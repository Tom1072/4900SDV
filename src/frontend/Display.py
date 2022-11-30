from enum import Enum
import PySimpleGUI as sg
from ViewDispatcher import ViewDispatcher
from ViewListener import ViewListener
from dotenv import load_dotenv
import os

class Button(Enum):
    BRAKE = "Brake" 
    THROTTLE = "Throttle" 
    ACC = "ACC speed" 
    SPAWN = "Spawn car"
    DESPAWN = "Despawn car"


class Detail(Enum):
    BRAKE_LEVEL = "Brake lv" 
    THROTTLE_LEVEL = "Gas lv" 
    SPEED = "Speed" 
    DESIRED_SPEED = "ACC Speed"
    OBJ_SPEED = "Obj Speed"
    DISTANCE = "Distance"
    SKID = "Skidding"


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
CONTROLLER_WIDTH = 650
LEVEL_SLIDER_WIDTH = 45

class Display:
    def __init__(self, listener_ip, dispatcher_ip):
        # Initialize UDP listener server and dispatcher
        self.udp_listener = ViewListener(listener_ip)
        self.udp_dispatcher = ViewDispatcher(dispatcher_ip)
        self.server_connected = False

        # Initialize environment
        self.env = {
            Detail.BRAKE_LEVEL.value: 0,
            Detail.THROTTLE_LEVEL.value: 0,
            Detail.SPEED.value: 0,
            Detail.DESIRED_SPEED.value: 0.0,
            Detail.OBJ_SPEED.value: 0.0,
            Detail.DISTANCE.value: 0,
            Detail.SKID.value: False,
            "object": False
        }

        sg.theme('Light Brown 3')

        # Brake input elements
        brake_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Level")], [sg.Slider(key=InputKey.BRAKE_LEVEL, range=(0, 100), orientation="h", size=(LEVEL_SLIDER_WIDTH, 14))]]),
                sg.Column([[sg.Text("Skid")], [sg.Radio("on", group_id="skid", key=InputKey.SKID), sg.Radio("off", group_id="skid", default=True)]]),
                sg.Button("Brake", size=(10, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(CONTROLLER_WIDTH, 70)
        )]

        # Throttle input elements
        throttle_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Level")], [sg.Slider(key=InputKey.THROTTLE_LEVEL, range=(0, 100), orientation="h", size=(LEVEL_SLIDER_WIDTH, 15))]]),
                sg.Button("Gas", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(CONTROLLER_WIDTH, 70)
        )]

        # ACC input elements
        acc_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Desired speed")], [sg.Slider(key=InputKey.DESIRED_SPEED, range=(0, 100), orientation="h", size=(LEVEL_SLIDER_WIDTH, 15))]]),
                sg.Button("Set ACC", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(CONTROLLER_WIDTH, 70)
        )]

        # Spawning car input elements
        spawn_input = [sg.Frame(
            "",
            [[
                sg.Column([[sg.Text("Distance")], [sg.Slider(key=InputKey.DISTANCE, range=(0, 100), orientation="h", size=(22, 15))]]),
                sg.Column([[sg.Text("Other car speed")], [sg.Slider(key=InputKey.OTHER_SPEED, range=(0, 100), orientation="h", size=(21, 15))]]),
                sg.Button("Spawn", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(CONTROLLER_WIDTH, 70)
        )]

        # Despawning car input elements
        despawn_input = [sg.Frame(
            "",
            [[
                sg.Button("No car in front", key=CAR_FRONT_FLAG_KEY, size=(36, 4), font=('Arial', 15, 'bold'), disabled=True, disabled_button_color="white"),
                sg.Button("Despawn", size=(26, BUTTON_HEIGHT), font=BUTTON_FONT)
            ]],
            border_width=1,
            size=(CONTROLLER_WIDTH, 39)
        )]

        # Despawning car input elements
        self.input_map = {
            Button.BRAKE.value: brake_input,
            Button.THROTTLE.value: throttle_input,
            Button.ACC.value: acc_input,
            Button.SPAWN.value: spawn_input,
            Button.DESPAWN.value: despawn_input,
        }

        # Main layout for output display
        DETAIL_DISPLAY_SIZE = 10
        layout = [
            [sg.Column([[sg.Button("Server NOT connected", key="server_connected", size=(30, 1), font=('Arial', 8, 'bold'), disabled=True, disabled_button_color="white")]], justification="c")],
            [sg.Text("Your car's information", font=BUTTON_FONT)],
            [
                sg.Column([
                    [sg.Text(d.value, key="t"+d.value, size=DETAIL_DISPLAY_SIZE, justification="c")],
                    [sg.Input(key=d.value, justification="c", size=DETAIL_DISPLAY_SIZE, disabled=True)]
                ], justification="c") for d in Detail
            ],
            [sg.Text("Car controller", font=BUTTON_FONT)],
            [sg.Column([i for i in self.input_map.values()], key="inputs", justification="c")],
            [sg.Button("Exit")],
        ]

        # Tkinter main window
        self.window = sg.Window("Car Speed Safety Control System", layout, finalize=True)


    def update_details(self):
        """
        Update/refresh the GUI view 
        """
        btn_color = "green"
        text = "No car in front"

        if self.env["object"]:
            btn_color = "red"
            text = "A car in front"

        self.window[CAR_FRONT_FLAG_KEY].update(text=text, button_color=btn_color)

        btn_color = "green"
        text = "Server connected"

        if not self.server_connected:
            btn_color = "red"
            text = "Server NOT connected"

        self.window["server_connected"].update(text=text, button_color=btn_color)

        for d in Detail:
            self.window[d.value].update(value=str(self.env[d.value]))
        

    def run(self):
        """
        Starts the main event loop of the GUI
        """
        prev_event = None

        while True:
            self.update_details()
            event, values = self.window.read(timeout=0)
            data = self.udp_listener.listen()
            if data != None:
                if data == "stop":
                    break
                self.env[Detail.BRAKE_LEVEL.value] = data["brake"]
                self.env[Detail.THROTTLE_LEVEL.value] = data["throttle"]
                self.env[Detail.SPEED.value] = data["speed"]
                self.env[Detail.DESIRED_SPEED.value] = data["desired-speed"]
                self.env[Detail.OBJ_SPEED.value] = data["obj-speed"]
                self.env[Detail.DISTANCE.value] = data["distance"] if data["distance"] < 9999 else "INF"
                self.env[Detail.SKID.value] = data["skid"]
                self.env["object"] = data["obj"]
                self.server_connected = True 
            else:
                if prev_event == "Exit":
                    break

            if event in (sg.WIN_CLOSED, "Exit"):
                prev_event = "Exit"
                self.udp_dispatcher.send_message("stop")
                continue
            
            if event and not self.server_connected:
                continue
            
            if event == "Brake":
                command = f"brake {int(values[InputKey.BRAKE_LEVEL])} {'on' if values[InputKey.SKID] else 'off'}"
                self.udp_dispatcher.send_message(command)
                self.udp_dispatcher.receive_message()
                print(command)
            elif event == "Gas":
                command = f"gas {int(values[InputKey.THROTTLE_LEVEL])}"
                self.udp_dispatcher.send_message(command)
                self.udp_dispatcher.receive_message()
                print(command)
            elif event == "Set ACC":
                command = f"acc-speed {int(values[InputKey.DESIRED_SPEED])}"
                self.udp_dispatcher.send_message(command)
                self.udp_dispatcher.receive_message()
                print(command)
            elif event == "Despawn":
                command = "despawn"
                self.udp_dispatcher.send_message(command)
                self.udp_dispatcher.receive_message()
                print(command)
            elif event == "Spawn":
                command = f"spawn {int(values[InputKey.DISTANCE])} {int(values[InputKey.OTHER_SPEED])}"
                self.udp_dispatcher.send_message(command)
                self.udp_dispatcher.receive_message()
                print(command)

        # if user exits, then close the window
        self.window.close()

if __name__ == '__main__':
    try:
        load_dotenv()
        LISTENER_IP = os.getenv("LISTENER_IP")
        DISPATCHER_IP = os.getenv("DISPATCHER_IP")

        if not LISTENER_IP or not DISPATCHER_IP:
            raise ValueError

        Display(LISTENER_IP, DISPATCHER_IP).run()
    except ValueError:
        print("ERROR: listener or dispatcher IP address missing, or .env file missing")
    except:
        print("Window exited")