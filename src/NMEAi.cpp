/**
 * @author OttoLi (ottoli.pro@gmail.com)
 * @brief 从串口接收NMEA数据，并输出
 * @version 0.1
 * @date 2023-04-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main()
{
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        std::cerr << "Error opening serial port\n";
        return 1;
    }

    // 配置串口
    struct termios options;
    tcgetattr(fd, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    // 读取NMEA数据并打印到控制台窗口中
    char szBuff[1024] = { 0 };
    while (true) {
        ssize_t nRead = read(fd, szBuff, sizeof(szBuff));
        if (nRead > 0) {
            std::cout << szBuff;
        }
    }

    // 关闭串口
    close(fd);

    return 0;
}
