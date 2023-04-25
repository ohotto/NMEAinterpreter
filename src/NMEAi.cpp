/**
 * @author OttoLi (ottoli.pro@gmail.com)
 * @brief 串口连接WTRTK，接收、解析、输出GGA、RMC、WTRTK数据包
 * @version 0.4.2
 * @date 2023-04-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <termios.h>
#include <unistd.h>
#include "NMEADatas.h"
#include "Functions.h"

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
                    std::cout << "\nReceived\n";
                    switch (type) {
                        case GGA: {
                            GGAData gga(sentence);
                            gga.PrintData();
                            break;
                        }
                        case RMC: {
                            RMCData rmc(sentence);
                            rmc.PrintData();
                            break;
                        }
                        case WTRTK: {
                            WTRTKData wtdata(sentence);
                            wtdata.PrintData();
                            break;
                        }
                        default:
                            //do nothing
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
