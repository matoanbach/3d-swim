# Import all dependecies
import sys
import time
import serial
import signal
import asyncio
import aiohttp
import requests
import threading
import numpy as np
import matplotlib.pyplot as plt
from collections import deque
from matplotlib.animation import FuncAnimation


class Delta:
    def __init__(self, port, baud_rate, base_url):
        self.ARDUINO_PORT = port
        self.BAUD_RATE = baud_rate
        self.BASE_URL = base_url
        self.send_points = deque()
        self.plot_points = deque()
        self.csv_points = deque()
        self.is_running = True
        # For plotter: after the line is out of max_line bound, it will be reset to be 0
        self.max_line = 30
        # Register signal handler for graceful shutdown
        signal.signal(signal.SIGINT, self.signal_handler)
        self.ser = serial.Serial(self.ARDUINO_PORT, self.BAUD_RATE, timeout=1)

    def signal_handler(self, sig, frame):
        print("Signal received, stopping...")
        self.is_running = False

    def read_data_from_file(self):
        while self.is_running:
            with open('spiral.txt', 'r') as file:
                lines = file.readlines()
                for line in lines:
                    if not self.is_running:
                        print("Stopping data read...")
                        break
                    x, y, z = line.split(",")
                    self.send_points.append((float(x), float(y)))
        print("read_data_from_file: stopping...")

    def read_data_from_serial(self):
        # Main loop to read coordinates and send G-code commands
        with open("LittleDelta.csv", "w") as file:
            while self.is_running:
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8').strip()
                    try:
                        x, y = map(lambda value: self.linear_mapping(
                            float(value), 0, 1023, -9, 9), line.split(','))
                        # self.send_points.append((x, y)) # only used when sending data to delta
                        file.write(f"{x},{y}\n")
                        self.plot_points.append((x, y))
                    except ValueError:
                        print(f"Error parsing line: {line}")
            print("read_data_from_serial: stopping...")
                    
    # to prove this formula, take a look at:
    def linear_mapping(self, value, from_min, from_max, to_min, to_max):
        return round((value - from_min) * (to_max - to_min) / (from_max - from_min) + to_min, 2)

    def plotter(self):
        global fig, ax, r_values, theta_values, line, anim
        fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
        r_values, theta_values = [], []
        line, = ax.plot([], [], 'bo-')

        def plot_real_time(frame):
            global r_values, theta_values
            if not self.is_running:
                anim.event_source.stop()
                print("Stopping animation...")
                return
            if self.plot_points:
                x, y = self.plot_points.popleft()
                r = np.sqrt(x**2 + y**2)
                # reverse the direction of the circle
                theta = (-1) * np.arctan2(y, x)
                r_values.append(r)
                theta_values.append(theta)
                line.set_data(theta_values, r_values)
            if len(r_values) >= self.max_line:
                r_values, theta_values = r_values[-2:], theta_values[-2:]
            return line,

        def init():
            ax.set_rmax(9)  # Adjust the max radius as needed
            ax.grid(True)
            ax.set_title("Real-Time Polar Plot")
            return line,

        anim = FuncAnimation(fig, plot_real_time, interval=0,
                             init_func=init, blit=True, cache_frame_data=False)
        plt.show()
        self.is_running = False
        print("plotter: stopping...")

    def send_gcode(self, command):
        url = f"{self.BASE_URL}/printer/gcode/script?script={command}"
        headers = {
            "Content-Type": "application/json"
        }
        data = {
            "jsonrpc": "2.0",
            "method": "printer.gcode.script",
            "params": {
                "script": command
            },
            "id": 7466
        }
        response = requests.post(url, json=data, headers=headers)
        if response.status_code == 200:
            print(f"Command '{command}' sent successfully")
        else:
            print(f"Failed to send command '{command}': {response.status_code} - {response.text}")
        return response.status_code

    def send_data(self):
        z = 0
        flag = True
        while self.is_running:
            if self.send_points:
                x, y = self.send_points.popleft()
                # after sending data, give the x and y to the plotter
                self.plot_points.append((x, y))
                command = f"G1 X{x:.2f} Y{y:.2f} Z{z:.2f}"
                self.send_gcode(command)
                z = self.check_z(z)
        print("send_data: stopping...")
    
    # make z go up and down forever
    def check_z(self, z):
        global flag
        if z > 30:
            flag = False
        elif z < 1:
            flag = True
        if flag:
            z += 0.03
        else:
            z -= 0.03
        return z
    def home(self):
        self.send_gcode("G28")  # Home all axes
        self.send_gcode("G90")

    def go_to_bottom(self):
        self.send_gcode("G1 X0 Y0 Z1")

    def up_and_down(self):
        z = 0
        flag = True  # True is going up
        # False is going down
        while self.is_running:
            if z > 30:
                flag = False
            elif z < 1:
                flag = True
            if flag:
                z += 0.03
            else:
                z -= 0.03
            self.send_gcode(f"G1 X0 Y0 Z{z:.2f}")
            time.sleep(0.05)


# Change these 3 lines to your own setting
arduino_port = "/dev/cu.usbmodem101"
baud_rate = 115200
base_url = "http://172.20.10.3:7125"

if __name__ == "__main__":
    delta = Delta(arduino_port, baud_rate, base_url)
    
    # delta.home()
    # delta.go_to_bottom()
    # # send_thread = threading.Thread(target=delta.up_and_down)
    # # send_thread.start()
    # # send_thread.join()

    read_thread = threading.Thread(target=delta.read_data_from_serial)
    # send_thread = threading.Thread(target=delta.send_data) # only used when sending coordinates to delta
    read_thread.start()
    
    # send_thread.start()
    delta.plotter()
    
    read_thread.join()
    # send_thread.join()