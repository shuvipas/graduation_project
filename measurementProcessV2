import serial
import time
#import matplotlib.pyplot as plt

DAC_RESOLUTION = 4096
ADC_RESOLUTION = 1023
VCC = 5.0
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
    v_dac = (dac_num * VCC) / DAC_RESOLUTION
    return v_dac


def get_resistor(ser):
    res = ser.read_until().rstrip().decode()
    if (str(res) == "Done"):
        return str(res)
    res = 10 ^ int(res)
    return res


def graph_ploter(data):
    print(data)
    #plt.scatter(*zip(*data))
    #plt.xlabel('current [A]')
    #plt.ylabel('voltage [V]')
    #plt.title("A VI graph")
    #plt.show()


if __name__ == '__main__':

    port = 'COM3'
    arduino = serial_connect(port)
    print("start program")

    handshake_arduino(arduino)

    # Chill out while everything gets set
    time.sleep(1)
    # Set a long timeout to complete handshake
    timeout = arduino.timeout
    arduino.timeout = 1
    # Read and discard everything that may be in the input buffer
    _ = arduino.read_all()  #

    arduino.write(bytes([START]))
    print("(voltage, current)")
    data = list()  # (voltage, current)
    while True:
        res = get_resistor(arduino)
        if res == "Done":
            break
        v_dac = get_dac_voltage(arduino)
        current = v_dac / res
        v_adc = get_adc_voltage(arduino)

        data.append((v_adc, current))

    arduino.close()
    print(data)
    # graph_ploter(data)
