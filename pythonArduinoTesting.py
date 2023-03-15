import serial
import time


if __name__ == '__main__':
    
    
    vin = 2.25 # float(input('Enter expected MCP4725 voltege:').strip())
    arduino = serial.Serial(port = 'COM3',baudrate=115200, timeout=0)
    serial.write(str(vin))
    print("V in to arduino: %f", vin)
    time.sleep(2) #stopes the nrunning progrram 2 sec
    vout = float(serial.readline())
    if vout == 3.25:
        print("success")
    else:
        print("failure")
        print("V out from arduino: %f", vout)
   
