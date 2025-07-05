import serial.tools.list_ports
from datetime import datetime
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator


def init_serial_port():
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

    return serial_instance


def plot_raw_data(time_array, data_array):
    plt.figure()
    plt.plot(time_array, data_array, color='blue', linewidth=1)
    plt.xlabel("Time (ms)")
    plt.ylabel("Voltage (V)")

    plt.gca().yaxis.set_major_locator(MultipleLocator(0.1))

    plt.tight_layout()
    plt.show()


def main_loop(serial_instance):
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
                print("Data collection finished...")
                data_incoming = False
                plot_data = input("Do you want to plot the data? (y/n)")
                plot_data = plot_data.lower()

                if plot_data == "y":
                    plot_raw_data(time_array, data_array)
                    data_array.clear()
                    time_array.clear()
                    data = None
                else:
                    data_array.clear()
                    time_array.clear()
                    data = None
                    break

            if data_incoming:
                end_time = datetime.now()
                elasped_time_ms = (end_time - start_time).total_seconds() * 1000;

                data_array.append(float(data))
                time_array.append(elasped_time_ms)


if __name__ == "__main__":
    serial_instance = init_serial_port()

    main_loop(serial_instance)
