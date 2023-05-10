import serial
import time
import matplotlib.pyplot as plt
DAC_RESALUTION = 4096
ADC_RESALUTION = 1023
VCC = 5.0

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



def get_adc_voltage(ser):
   adc_num = int(ser.read_until().rstrip().decode())
   v_adc = (adc_num * VCC) / ADC_RESALUTION
   return v_adc

def get_dac_voltage(ser):
   dac_num = int(ser.read_until().rstrip().decode())
   v_dac = (dac_num * VCC) / DAC_RESALUTION
   return v_dac


def get_resistor(ser):
    res = ser.read_until().rstrip().decode()
    if(str(res) =="Done"):
        return  str(res)
    res = 10^int(res)
    return res

def graph_ploter(data):
    plt.scatter(*zip(*data))
    plt.xlabel('current [A]')
    plt.ylabel('voltage [V]')
    plt.title("A VI graph")
    plt.show()


if __name__ == '__main__':

    port = 'COM3'
    arduino = serial_connect(port)
    
    arduino.close()##
    arduino.open()##
    _ = arduino.read_all()##
    hand_shake_str = '0\n'
    arduino.write(hand_shake_str.encode()) #ser.write(bytes([0]))
    print(arduino.read_until().decode()) #print(arduino.readline().rstrip().decode())
    start_str = '1\n'
    arduino.write(start_str.encode()) #ser.write(bytes([1]))
    
    data = list()  #(current, voltage)
    while True:
        res = get_resistor(arduino) 
        if(res =="Done"):
            break
        v_dac = get_dac_voltage(arduino)
        current = v_dac/res
        v_adc = get_adc_voltage(arduino)
        
        data.append((current, v_adc))
    
    arduino.close()
    print(data)
    graph_ploter(data)
    