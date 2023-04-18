/**
 * @author OttoLi (ottoli.pro@gmail.com)
 * @brief 从串口接收NMEA数据，并筛选判断数据包类型，只输出$GPGGA、$GPRMC和$WTRTK
 * @version 0.3
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>


enum NmeaType {
    NMEA_INVALID,
    GPGGA,
    GPRMC,
    WTRTK
};

// 筛选NMEA数据包类型，仅接收$GPGGA、$GPRMC和$WTRTK
NmeaType GetNmeaType(const std::string& nmea) {
    if (nmea.empty() || nmea[nmea.size() - 1] != '\n') {
        return NMEA_INVALID;
    }

    if (nmea.compare(0, strlen("$GPGGA"), "$GPGGA") == 0) {
        return GPGGA;
    }

    if (nmea.compare(0, strlen("$GPRMC"), "$GPRMC") == 0) {
        return GPRMC;
    }

    if (nmea.compare(0, strlen("$WTRTK"), "$WTRTK") == 0) {
        return WTRTK;
    }

    return NMEA_INVALID;
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
                NmeaType type = GetNmeaType(sentence);
                if (type != NMEA_INVALID) {
                    std::cout << "Received ";
                    switch (type) {
                        case GPGGA:
                            std::cout << "GPGGA: ";
                            break;
                        case GPRMC:
                            std::cout << "GPRMC: ";
                            break;
                        case WTRTK:
                            std::cout << "WTRTK: ";
                            break;
                        default:
                            break;
                    }
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
