/**
 * @author OttoLi (ottoli.pro@gmail.com)
 * @brief 从串口接收NMEA数据，并筛选，只输出$GPGGA、$GPRMC和$WTRTK
 * @version 0.2
 * @date 2023-04-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

const std::string NMEA_GGA = "$GPGGA";
const std::string NMEA_RMC = "$GPRMC";
const std::string NMEA_WTRTK = "$WTRTK";

bool IsNmeaValid(const std::string& nmea)
{
    if (nmea.empty() || nmea[nmea.size() - 1] != '\n') {
        return false;
    }

    if (nmea.compare(0, NMEA_GGA.size(), NMEA_GGA) != 0
        && nmea.compare(0, NMEA_RMC.size(), NMEA_RMC) != 0
        && nmea.compare(0, NMEA_WTRTK.size(), NMEA_WTRTK) != 0) {
        return false;
    }

    return true;
}

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
    std::string nmea;
    while (true) {
        ssize_t nRead = read(fd, szBuff, sizeof(szBuff));
        if (nRead > 0) {
            nmea.append(szBuff, nRead);
            size_t pos = 0;
            while ((pos = nmea.find('\n')) != std::string::npos) {
                std::string sentence = nmea.substr(0, pos + 1);
                if (IsNmeaValid(sentence)) {
                    std::cout << sentence;
                }
                nmea.erase(0, pos + 1);
            }
        }
    }

    // 关闭串口
    close(fd);

    return 0;
}
