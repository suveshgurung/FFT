import serial.tools.list_ports
from datetime import datetime
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator
import numpy as np
import sys
import os


# Recursive FFT algorithm
def FFT(data_samples):
    N = len(data_samples)

    # End of recursion once we have only one even/odd split.
    if N <= 1:
        return data_samples

    M = N // 2

    # Separate the even and odd components of the data sample.
    Xeven = []
    Xodd = []
    for i in range(0, M):
        Xeven.append(data_samples[2 * i])
        Xodd.append(data_samples[2 * i + 1])

    Feven = FFT(Xeven)
    Fodd = FFT(Xodd)
    frequencies_arr = [0] * N

    for k in range(0, M):
        complex_exponential = np.exp(-(2j * np.pi * k) / N) * Fodd[k]
        frequencies_arr[k] = Feven[k] + complex_exponential

        # Repeating property
        frequencies_arr[k + N//2] = Feven[k] - complex_exponential

    return frequencies_arr


def plot_frequency_spectrum(FFT_result, sampling_rate):
    N = len(FFT_result)
    T = 1.0 / sampling_rate             # Time period of sample

    FFT_result = np.array(FFT_result)

    magnitudes = np.abs(FFT_result)

    freq = np.fft.fftfreq(N, d=T)

    half_N = N // 2
    plt.figure(figsize=(10, 5))
    plt.plot(freq[:half_N], magnitudes[:half_N], color='purple')
    plt.title("Frequency Spectrum (Magnitude)")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude")
    plt.grid(True)
    plt.tight_layout()
    plt.show()


# Initialize the serial port for reading data from it.
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

    os.system("clear")

    serial_instance.baudrate = 115200
    serial_instance.port = port
    serial_instance.setDTR(False)
    serial_instance.setRTS(False)
    serial_instance.open()

    return serial_instance


# Plot the raw data (voltage data).
def plot_raw_data(time_array, data_array):
    plt.figure()
    plt.plot(time_array, data_array, color='blue', linewidth=1)
    plt.xlabel("Time (ms)")
    plt.ylabel("Voltage (V)")

    plt.gca().yaxis.set_major_locator(MultipleLocator(0.1))
    plt.tight_layout()
    plt.show()


# Main loop for the program.
def main_loop(serial_instance):
    data_incoming = False
    data_array = []
    time_array = []

    start_time = datetime.now()

    sampling_rate = None

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

                sampling_rate = 4000.0

                if plot_data == "y":
                    plot_raw_data(time_array, data_array)

                    if len(data_array) % 2 != 0:
                        data_array.pop()
                    FFT_result = FFT(data_array)
                    plot_fft = input("Plot the frequency spectrum (y/n)")
                    plot_fft = plot_fft.lower()
                    if plot_fft == "y":
                        plot_frequency_spectrum(FFT_result, sampling_rate)

                    data_array.clear()
                    time_array.clear()
                    data = None
                else:
                    if len(data_array) % 2 != 0:
                        data_array.pop()
                    FFT_result = FFT(data_array)
                    plot_fft = input("Plot the frequency spectrum (y/n)")
                    plot_fft = plot_fft.lower()
                    if plot_fft == "y":
                        plot_frequency_spectrum(FFT_result, sampling_rate)

                    data_array.clear()
                    time_array.clear()
                    data = None
                    continue

            if data_incoming:
                end_time = datetime.now()
                elasped_time_ms = (end_time - start_time).total_seconds() * 1000

                data_array.append(float(data))
                time_array.append(elasped_time_ms)


if __name__ == "__main__":
    sys.setrecursionlimit(10000)
    serial_instance = init_serial_port()

    main_loop(serial_instance)
