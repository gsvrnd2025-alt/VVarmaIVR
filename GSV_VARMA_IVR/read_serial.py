import serial
import time

def main():
    try:
        # Open serial port COM3 at 115200
        ser = serial.Serial('COM3', 115200, timeout=1)
        print("Port COM3 opened successfully. Resetting ESP32 via DTR/RTS...")
        
        # Toggle DTR/RTS to reset the board (standard ESP32 serial reset sequence)
        ser.setDTR(False)
        ser.setRTS(True)
        time.sleep(0.1)
        ser.setRTS(False)
        ser.setDTR(False)
        
        print("Waiting 10 seconds for boot logs...")
        start_time = time.time()
        while time.time() - start_time < 240:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore')
                print(line, end='')
        ser.close()
    except Exception as e:
        print(f"Error: {e}")

if __name__ == '__main__':
    main()
