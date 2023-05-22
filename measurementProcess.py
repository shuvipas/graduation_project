import serial
import time
import matplotlib.pyplot as plt
import openpyxl

# DAC_RESOLUTION = 4096
ADC_RESOLUTION = 1023
MEASUREMENT_TIMES = 2
VCC = 5.0
V_SUPPLY = 18
START = 1


def serial_connect(port_name):
    try:
        ser = serial.Serial(port_name, baudrate=115200, timeout=.1)
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
    timeout = ser.timeout
    ser.timeout = 2
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
    ser.timeout = timeout


def get_adc_voltage(ser):
    adc_num = int(ser.read_until().rstrip().decode())
    v_adc = (adc_num * VCC) / ADC_RESOLUTION
    return v_adc


def get_dac_voltage(ser):
    dac_num = int(ser.read_until().rstrip().decode())
    v_dac = (dac_num * VCC) / ADC_RESOLUTION  # DAC_RESOLUTION
    return v_dac


def get_resistor(ser):
    res = ser.read_until().rstrip().decode()
    if str(res) == "Done":
        return str(res)
    res = 10 ** int(res)
    return res


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


def convert_list_to_excel(data_list, head_line, file_name):
    workbook = openpyxl.Workbook()
    sheet = workbook.active
    sheet.append(head_line)
    for row_data in data_list:
        sheet.append(row_data)

    workbook.save(file_name)


def calibration(arduino):
    adc_num = int(arduino.read_until().rstrip().decode())
    vcc = adc_num / ADC_RESOLUTION #* ((r2+ r1) / r1)

if __name__ == '__main__':

    port = 'COM3'
    arduino = serial_connect(port)
    print("start program")

    handshake_arduino(arduino)

    # Chill out while everything gets set
    time.sleep(1)
    # Set a long timeout to complete handshake
    timeout = arduino.timeout
    arduino.timeout = 2
    # Read and discard everything that may be in the input buffer
    _ = arduino.read_all()

    arduino.write(bytes([START]))
    headline = ("v_adc", "v_dac", "current", "dut_res")
    data = list()  # (voltage, current)

    while True:
        res = get_resistor(arduino)
        if res == "Done":
            break

        v_dac = get_adc_voltage(arduino)
        current = v_dac / res
        v_adc = get_adc_voltage(arduino) * ((47.1 + (10.015 + 6.796)) / (10.015 + 6.796))  #  multiplied with the V.D of the InAmp

        dut_res = 0
        if current > 0:
            dut_res = v_adc / current

        #  if v_adc + v_dac < V_SUPPLY:
        data.append((v_adc, v_dac, current, dut_res))

    arduino.close()
    for i in data:
        print(i)
    #file = "C:\\Users\\Meir Sokolik\\OneDrive\\Documents\\Engineering Project\\new_rs1Meg.xlsx"

    #convert_list_to_excel(data, headline, file)
#   graph_plot(data)
