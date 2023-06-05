import serial
import time
import struct
#  import matplotlib.pyplot as plt
import openpyxl

# DAC_RESOLUTION = 4096
ADC_RESOLUTION = 1023
DAC_RESOLUTION = 4096
MEASUREMENT_TIMES = 2
VCC = 5.0
V_SUPPLY = 18
START_SWEEP = 1
END_PROGRAM = "Done"
# The V.D. of the InAmp
R1 = 47.1
R2 = 10.015 + 6.796


def serial_connect(port_name):
    try:
        ser = serial.Serial(port_name, baudrate=115200, timeout=None) # 2)
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

    # Reset the timeout
    # ser.timeout = timeout


def get_adc_voltage(ser):
    adc_num = int(ser.read_until().rstrip().decode())
    v_adc = (adc_num * VCC) / ADC_RESOLUTION
    return v_adc


'''
def get_dac_voltage(ser):
    dac_num = int(ser.read_until().rstrip().decode())
    v_dac = (dac_num * VCC) / DAC_RESOLUTION
    return v_dac
'''


def get_resistor(ser):
    res = ser.read_until().rstrip().decode()
    print(res)
    if res == END_PROGRAM:
        return res
    res = 10 ** int(res)
    return res


'''
def remove_data(data_list, measurement_time):
    i = 0
    diff_points = len(data_list) / measurement_time
    while i < diff_points - 1:
        diff = data_list[(i + 1) * measurement_time][0] - data_list[i * measurement_time][0]
        if diff < VCC / ADC_RESOLUTION:
            del data_list[(i + 1) * measurement_time:(i + 1) * measurement_time + measurement_time]
        else:
            i += 1
    return data_list
'''

'''
def graph_plot(data):
    # split tuples into separate lists
    v_out, v_in, cur, r = zip(*data)

    # plot x against y
    plt.scatter(v_out, cur)
    plt.plot(v_out, cur)
    plt.xlabel('Voltage [V]')
    plt.ylabel('cur [A]')
    plt.title('V/I Curve')
    plt.show()
'''

def convert_list_to_excel(data_list):
    head_line = ("v_adc", "v_dac", "current", "dut_res")
    file_name = input("insert file name: ")
    file_path = "C:\\Users\\Meir Sokolik\\OneDrive\\Documents\\Engineering Project\\" + file_name + ".xlsx"
    workbook = openpyxl.Workbook()
    sheet = workbook.active
    sheet.append(head_line)
    for row_data in data_list:
        sheet.append(row_data)

    workbook.save(file_path)


'''
def calibration(ser):
    adc_num = int(ser.read_until().rstrip().decode())
    vcc = adc_num / ADC_RESOLUTION  # * ((r2+ r1) / r1)
'''


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
    ser.write(bytes([START_SWEEP]))

    data = list()
    while True:
        res = get_resistor(ser)
        if res == END_PROGRAM:
            break

        v_dac = get_adc_voltage(ser)
        # print(res)
        current = v_dac / res
        v_adc = get_adc_voltage(ser) * ((R1+R2)/R2) # (47.1 + (10.015 + 6.796)) / (10.015 + 6.796))  # multiply by the V.D of the InAmp
        dut_res = 0
        if current != 0:
            dut_res = v_adc / current
        data.append((v_adc, v_dac, current, dut_res))

        print((v_adc, v_dac, current, dut_res))

    convert_list_to_excel(data)


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


def user_res_and_volt(ser, case):
    _ = ser.read_all()
    res, min_volt, read_num = user_input(case)

    dac_v_in = int(DAC_RESOLUTION * (min_volt / VCC))
    ser.write(bytes([int(case)]))
    # print("case = ")
    # print(ser.read_until().rstrip().decode())
    # time.sleep(1)
    ser.write(bytes([res]))
    # print("res = ")
    # print(ser.read_until().rstrip().decode())
    #volt_start = struct.pack('<i', dac_v_in)

    # time.sleep(1)
    # ser.write(volt_start)
    ser.write(bytearray(str(dac_v_in), 'utf-8'))
    # ser.write(bytes([int(4)]))
    # print("volt_start = ", volt_start)

    # print(ser.read_until().rstrip().decode())
    # time.sleep(1)
    ser.write(bytes([read_num]))
    # print("read_num = ")
    # print(ser.read_until().rstrip().decode())
    # print("i = ")
    # print(ser.read_until().rstrip().decode())
    data = list()

    headline = ("v_adc", "v_dac", "current", "dut_res")
    print(headline)

    while True:
        res = get_resistor(ser)
        if res == END_PROGRAM:
            break

        v_dac = get_adc_voltage(ser)
        current = float(v_dac) / res
        v_adc = get_adc_voltage(ser) * ((R1+R2)/R2) # ((47.1 + (10.015 + 6.796)) / (10.015 + 6.796))  # multiply by the V.D of the InAmp

        dut_res = 0
        if current > 0:
            dut_res = v_adc / current

        data.append((v_adc, v_dac, current, dut_res))

        print(v_adc, v_dac, current, dut_res)


if __name__ == '__main__':
    port = 'COM3'
    arduino = serial_connect(port)
    print("start program")
    # handshake_arduino(arduino)
    # reconnect(arduino)
    # Read and discard everything that may be in the input buffer
    # _ = arduino.read_all()
    command = 0
    while command != "end":
        command = input("insert '1' for the sweep process,\n'2' for user resistor and voltage\n"
                        "'3' for user resistor and voltage min and max  or 'end' to end the program: ")
        if command == '1':
            sweep(arduino)

        elif command == '2' or command == '3':
            user_res_and_volt(arduino, int(command))

    arduino.close()

    # graph_plot(data)
