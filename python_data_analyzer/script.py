import serial.tools.list_ports

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

while True:
    if serial_instance.in_waiting:
        packet = serial_instance.readline()
        print(packet.decode('utf'))
