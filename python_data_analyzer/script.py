import serial.tools.list_ports
from datetime import datetime

ports = serial.tools.list_ports.comports()
serial_instance = serial.Serial()

port_list = []

for one_port in ports:
    port_list.append(str(one_port))
    print(str(one_port))

val = input("Select the port to read from: /dev/tty")

for i in range(0, len(port_list)):
    if port_list[i].startswith("/dev/tty" + str(val)):
        port = "/dev/tty" + str(val)

serial_instance.baudrate = 115200
serial_instance.port = port
serial_instance.setDTR(False)
serial_instance.setRTS(False)
serial_instance.open()

data_incoming = False
data_array = []
time_array = []

start_time = datetime.now()

while True:
    if serial_instance.in_waiting:
        packet = serial_instance.readline()
        data = packet.decode('utf').strip()

        if data == "%":
            data_incoming = True
            start_time = datetime.now()
            print("Collecting data now...")
            continue
        elif data == "%e":
            data_incoming = False
            break

        if data_incoming:
            end_time = datetime.now()
            elasped_time_ms = (end_time - start_time).total_seconds() * 1000;

            data_array.append(data)
            time_array.append(elasped_time_ms)

print(data_array)
print(time_array)
