import serial
import time
import struct
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import openpyxl
import tkinter as tk
from tkinter import ttk
from tkinter.filedialog import asksaveasfilename

# DAC_RESOLUTION = 4096
ADC_RESOLUTION = 1023
DAC_RESOLUTION = 4096
MEASUREMENT_TIMES = 2
VCC = 5.0
V_SUPPLY = 18
SWEEP = 1
USER_INPUT = 2
END_PROGRAM = "Done"
# The V.D. of the InAmp
R1 = 47.1
R2 = 10.015 + 6.796


def serial_connect(port_name):
    try:
        ser = serial.Serial(port_name, baudrate=115200, timeout=None)
        print("opened port " + ser.name + '\n')
        # give arduino time to reset
        time.sleep(2)
        # flush input buffer, discarding all contents
        ser.reset_input_buffer()
        return ser
    except serial.SerialException:
        raise IOError("problem connecting to " + port_name)


def handshake_arduino(ser, sleep_time=1, print_handshake_message=True, handshake_code=0):
    """Make sure connection is established by sending
    and receiving bytes."""
    # Close and reopen
    ser.close()
    ser.open()
    # Chill out while everything gets set
    time.sleep(sleep_time)
    # Set a long timeout to complete handshake
    # timeout = ser.timeout
    # ser.timeout = 2
    # Read and discard everything that may be in the input buffer
    _ = ser.read_all()
    # Send request to Arduino
    ser.write(bytes([handshake_code]))
    # Read in what Arduino sent
    handshake_message = ser.read_until()
    # Send and receive request again
    ser.write(bytes([handshake_code]))
    handshake_message = ser.read_until()

    # Print the handshake message, if desired
    if print_handshake_message:
        print("Handshake message: " + handshake_message.decode())


def get_adc_voltage(ser):
    adc_num = int(ser.read_until().rstrip().decode())
    v_adc = (adc_num * VCC) / ADC_RESOLUTION
    return v_adc


def get_resistor(ser):
    res = ser.read_until().rstrip().decode()
    if res == END_PROGRAM:
        return res
    res = 10 ** int(res)
    return res


def graph_plot(data):
    # split tuples into separate lists
    v_out, v_in, cur, r = zip(*data)
    fig = plt.figure(figsize=(2, 2), dpi=100)
    plt.scatter(v_out, cur)
    # plt.plot(v_out, cur)
    plt.xlabel('Voltage [V]')
    plt.ylabel('current [mA]')
    plt.title('V/I Curve')
    canvas = FigureCanvasTkAgg(fig, master=window)
    canvas.draw()
    canvas.get_tk_widget().grid(row=0, column=1, sticky="nsew")


def save_data(data_list):
    workbook = openpyxl.Workbook()
    sheet = workbook.active
    head_line = ("v_adc", "v_dac", "current", "dut_res")
    sheet.append(head_line)
    for row_data in data_list:
        sheet.append(row_data)
    file_path = asksaveasfilename(defaultextension=".xlsx", filetypes=[("Excel files", "*.xlsx")])
    if file_path:
        try:
            workbook.save(file_path)

            tk.messagebox.showinfo("Success", "File saved successfully.")
        except Exception as e:
            tk.messagebox.showerror("Error", str(e))
    else:
        tk.messagebox.showwarning("Warning", "No file selected.")


def reconnect(ser):
    # Chill out while everything gets set
    time.sleep(1)
    # Set a long timeout to complete handshake
    timeout = ser.timeout
    ser.timeout = 2
    # Read and discard everything that may be in the input buffer
    _ = ser.read_all()


def sweep(ser):
    # reconnect(ser)
    _ = ser.read_all()
    ser.write(bytes([SWEEP]))

    data = list()
    while True:
        res = get_resistor(ser)
        if res == END_PROGRAM:
            break

        v_dac = get_adc_voltage(ser)
        v_rs = get_adc_voltage(ser)
        current = (v_rs / res) * 1e3  # current in mA units
        v_adc = get_adc_voltage(ser) * ((R1+R2)/R2)  # multiply by the V.D of the InAmp
        dut_res = 0
        if current != 0:
            dut_res = v_adc / current
        if v_adc > 0.75 and abs(v_dac - v_rs) < 0.1:
            data.append((v_adc, v_dac, current, dut_res))

    graph_plot(data)
    save_data(data)

'''
def user_input(case):
    max_volt = 0.0
    read_num = 1
    val_diff = 0
    res = int(input("insert resistor (int between 2-6 for res = 10^i): "))
    while 6 < res or res < 2:
        print("the correct value for the resistor is a int between 2-6")
        res = int(input("insert resistor (int between 2-6 for res = 10^i): "))

    min_volt = float(input("insert voltage (float up to 5): "))

    if case == 2:
        # min_volt = float(input("insert voltage (float up to 5): "))
        while 5 < min_volt or min_volt < 0:
            print("the correct value for the voltage is a float between 0-5")
            min_volt = float(input("insert voltage (float up to 5): "))

    elif case == 3:
        # min_volt = float(input("insert min voltage (float up to 5): "))
        while 5 < min_volt or min_volt < 0:
            print("the correct value for the voltage is a float between 0-5")
            min_volt = float(input("insert voltage (float up to 5): "))
        max_volt = float(input("insert max voltage (float up to 5): "))
        while 5 < max_volt or max_volt < min_volt:
            print("the correct value for the voltage is a float between " + str(min_volt) + "-5")
            max_volt = float(input("insert voltage (float up to 5): "))
        read_diff = 100  # difference between volt reads
        val_diff = int(DAC_RESOLUTION * (max_volt - min_volt) / VCC)  # difference between min\max volt in DAC values
        read_num = int(val_diff / read_diff)

    return res, min_volt, read_num
'''


def user_input(case):
    max_volt = 0.0
    read_num = 1
    val_diff = 0
    res = tk.StringVar()
    min_volt = tk.StringVar()

    def get_inputs():
        nonlocal max_volt, read_num, val_diff
        res_value = res.get()
        min_volt_value = min_volt.get()

        try:
            res_value = int(res_value)
            if res_value < 2 or res_value > 6:
                tk.messagebox.showwarning("Invalid Input", "The resistor value should be an integer between 2 and 6.")
                return
        except ValueError:
            tk.messagebox.showwarning("Invalid Input", "Please enter a valid integer for the resistor value.")
            return

        try:
            min_volt_value = float(min_volt_value)
            if min_volt_value < 0 or min_volt_value > 5:
                tk.messagebox.showwarning("Invalid Input", "The voltage value should be a float between 0 and 5.")
                return
        except ValueError:
            tk.messagebox.showwarning("Invalid Input", "Please enter a valid float for the voltage value.")
            return

        if case == 3:
            try:
                max_volt_value = float(max_volt.get())
                if max_volt_value < min_volt_value or max_volt_value > 5:
                    tk.messagebox.showwarning("Invalid Input", "The maximum voltage value should be a float between "
                                           + str(min_volt_value) + " and 5.")
                    return
                read_diff = 100
                val_diff = int(DAC_RESOLUTION * (max_volt_value - min_volt_value) / VCC)
                read_num = int(val_diff / read_diff)
            except ValueError:
                tk.messagebox.showwarning("Invalid Input", "Please enter a valid float for the maximum voltage value.")
                return

        cmd_window.destroy()

    cmd_window = tk.Tk()
    cmd_window.title("User Input")

    resistor_label = tk.Label(cmd_window, text="Resistor")
    resistor_label.grid(row=0, column=0, padx=20, pady=10)
    resistor_entry = tk.Entry(cmd_window, textvariable=res)
    resistor_entry.grid(row=1, column=0)

    min_voltage_label = tk.Label(cmd_window, text="Min Voltage")
    min_voltage_label.grid(row=0, column=1, padx=20, pady=10)
    min_voltage_entry = tk.Entry(cmd_window, textvariable=min_volt)
    min_voltage_entry.grid(row=1, column=1)

    if case == 3:
        max_voltage_label = tk.Label(cmd_window, text="Max Voltage")
        max_voltage_label.grid(row=0, column=2, padx=20, pady=10)
        max_voltage_entry = tk.Entry(cmd_window, textvariable=max_volt)
        max_voltage_entry.grid(row=1, column=2)

    ok_button = tk.Button(cmd_window, text="OK", command=get_inputs)
    ok_button.grid(row=2, column=1)

    cmd_window.mainloop()

    return int(res.get()), float(min_volt.get()), read_num


def user_res_and_volt(ser, case):
    _ = ser.read_all()
    res, min_volt, read_num = user_input(case)

    dac_v_in = int(DAC_RESOLUTION * (min_volt / VCC))
    ser.write(bytes([USER_INPUT]))
    ser.write(bytes([res]))
    ser.write(struct.pack('<i', dac_v_in))
    ser.write(bytes([read_num]))
    data = list()

    headline = ("v_adc", "v_dac", "current", "dut_res")
    # print(headline)
    _ = ser.read_all()
    while True:
        res = get_resistor(ser)
        if res == END_PROGRAM:
            break

        v_dac = get_adc_voltage(ser)
        v_rs = get_adc_voltage(ser)
        current = (v_rs / res) * 1e3
        v_adc = get_adc_voltage(ser) * ((R1+R2)/R2)  # multiply by the V.D of the InAmp
        dut_res = 0
        if current > 0:
            dut_res = v_adc / current
        if v_adc > 0.75 and abs(v_dac - v_rs) < 0.1:
            data.append((v_adc, v_dac, current, dut_res))

    graph_plot(data)
    save_data(data)


port = 'COM3'
arduino = serial_connect(port)


# Function to execute the sweep option
def sweep_option():
    sweep(arduino)


# Function to execute the user resistor and voltage option
def user_option(case):
    user_res_and_volt(arduino, case)


if __name__ == '__main__':

    window = tk.Tk()
    window.title("Chip Tester System")
    window.rowconfigure(0, minsize=600, weight=1)
    window.columnconfigure(1, minsize=600, weight=1)

    frm_buttons = tk.Frame(window, relief=tk.RAISED, bd=2)
    frm_buttons.grid(row=0, column=0, sticky="ns")

    lbl_buttons = tk.Label(frm_buttons, text="Measurements Options")
    lbl_buttons.grid(row=0, column=0, sticky="ew", padx=5, pady=5)

    sweep_button = tk.Button(frm_buttons, text="Sweep Process", command=sweep_option)
    sweep_button.grid(row=1, column=0, sticky="ew", padx=5, pady=5)

    one_sample_button = tk.Button(frm_buttons, text="Select Point",
                                  command=lambda: user_option(2))
    one_sample_button.grid(row=2, column=0, sticky="ew", padx=5, pady=5)

    select_range_button = tk.Button(frm_buttons, text="Select Range",
                                    command=lambda: user_option(3))
    select_range_button.grid(row=3, column=0, sticky="ew", padx=5, pady=5)

    window.mainloop()
    # arduino.close()
