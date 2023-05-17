import serial
import time


def serial_connect(port_name):
    try:
        ser = serial.Serial(port_name, baudrate=9600, timeout=.1)
        print("opened port " + ser.name + '\n')
        # give arduino time to reset
        time.sleep(2)
        # flush input buffer, discarding all contents
        ser.reset_input_buffer()
        return ser
    except serial.SerialException:
        raise IOError("problem connecting to " + port_name)


def set_voltage(ser, v_in):
    v_in_str = v_in + '\n'
    ser.write(v_in_str.encode())
    return


def get_voltage(ser):
   v_adc = int(ser.readline().rstrip().decode())
   v_out = (v_adc * 5) / 1023
   return v_out

if __name__ == '__main__':

    port = 'COM3'
    arduino = serial_connect(port)

    set_voltage(arduino, '0012')  # We need to send a string with length of 4, because the Arduino expect receiving 4
    # bytes.
        data = list()
    run = 1
    while run:
        #time.sleep(0.1)
    
        data.append(get_voltage(arduino))
        print(data)
