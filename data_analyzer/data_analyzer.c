#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 8         // 4096.00\n -> 8 bytes

int data_count = 0;

void sigint_handler(int signum) {
  printf("SIGINT detected! data_count: %d\n", data_count);
  exit(0);
}

void init_tty_port(int port_fd) {
  int modem_bits;
  if (ioctl(port_fd, TIOCMGET, &modem_bits) == 0) {
    modem_bits &= ~TIOCM_DTR;  // Clear DTR bit
    modem_bits &= ~TIOCM_RTS;  // Clear RTS bit for good measure
    ioctl(port_fd, TIOCMSET, &modem_bits);
  }

  struct termios tty;
  if (tcgetattr(port_fd, &tty) == -1) {
    fprintf(stderr, "unable to get terminal information!");
    close(port_fd);
    exit(1);
  }

  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);

  // Configure other settings
  tty.c_cflag |= (CLOCAL | CREAD);    // Enable receiver and set local mode
  tty.c_cflag &= ~PARENB;             // No parity
  tty.c_cflag &= ~CSTOPB;             // 1 stop bit
  tty.c_cflag &= ~CSIZE;              // Clear size bits
  tty.c_cflag |= CS8;                // 8 data bits
  tty.c_cflag &= ~CRTSCTS;

  // Configure input flags
  tty.c_lflag &= ~ICANON;        // Disable canonical mode
  tty.c_lflag &= ~ECHO;          // Disable echo
  tty.c_lflag &= ~ECHOE;         // Disable erasure
  tty.c_lflag &= ~ECHONL;        // Disable new-line echo
  tty.c_lflag &= ~ISIG;          // Disable interpretation of INTR, QUIT and SUSP

  // Configure output flags
  tty.c_oflag &= ~OPOST;         // Prevent special interpretation of output bytes
  tty.c_oflag &= ~ONLCR;         // Prevent conversion of newline to carriage return/line feed

  // Configure input flags
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  // Set timeout values
  tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received
  tty.c_cc[VMIN] = 0;      // Minimum number of characters to read

  // Set the new attributes
  if (tcsetattr(port_fd, TCSANOW, &tty) != 0) {
    perror("Error setting serial port attributes");
    close(port_fd);
    exit(1);
  }
}

int main() {
  signal(SIGINT, sigint_handler);
  int port_fd = open("/dev/ttyUSB0", O_RDONLY | O_NOCTTY);

  if (port_fd == -1) {
    fprintf(stderr, "serial port could not be opened!");
    return 1;
  }

  init_tty_port(port_fd);

  char buffer[BUFFER_SIZE];
  int bytes_read = 0;

  while (1) {
    bytes_read = read(port_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read > 0) {
      buffer[bytes_read] = '\0';  // Null terminate the string

      printf("%s", buffer);
      data_count++;
      fflush(stdout);  // Ensure immediate output
    } else if (bytes_read == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // No data available, continue
        usleep(10000);  // Sleep for 10ms
        continue;
      } else {
        printf("Error reading from serial port: %s\n", strerror(errno));
        break;
      }
    }

    // Small delay to prevent excessive CPU usage
    usleep(1000);  // 1ms delay
  }

  close(port_fd);
  return 0;
}
