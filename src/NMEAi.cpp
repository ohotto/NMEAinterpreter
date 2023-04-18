/**
 * @author OttoLi (legionz@foxmail.com)
 * @brief 创建WTRTK类，并能够输出可读性强的信息
 * @version 0.3.1
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
#include <vector>
#include <sstream>

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

class WTRTKData {
    //$WTRTK,x.xx,y.yy,z.zz,r.rr,p.pp,o.oo,w.ww,s,gg,k,q
    private:
        double diffX_;              //差分X距离，以米为单位，表示当前位置与差分基准站在东西方向上的距离差
        double diffY_;              //差分Y距离，以米为单位，表示当前位置与差分基准站在南北方向上的距离差
        double diffZ_;              //差分Z距离，以米为单位，表示当前位置与差分基准站在天线高度方向上的距离差
        double diffR_;              //差分R距离，以米为单位，表示当前位置与差分基准站在水平面上的距离差
        double pitch_;              //俯仰角，以度为单位，表示当前天线相对于水平面的仰角
        double roll_;               //横滚角，以度为单位，表示当前天线相对于水平面的横滚角
        double heading_;            //航向角，以度为单位，表示当前方向与正北方向之间的夹角
        std::string locState_;      //移动站定位状态，表示当前移动站的定位状态
        int fourGState_;            //4G状态flag
        std::string fourGState__;   //4G状态，表示当前4G网络连接状态
        std::string fixBaseState_;  //固定基站状态，表示当前是否选择了固定基站
        //int fourGQuality_;
        
    public:
        WTRTKData(const std::string& sentence) {
            // 分割语句
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream ss(sentence);
            while (std::getline(ss, token, ',')) {
                // 检查是否包含回车符或换行符
                if (token.find("\r\n") != std::string::npos) {
                    // 如果包含，则删除回车符和换行符
                    token.erase(token.find("\r\n"), 2);
                }
                tokens.push_back(token);
            }
            
            // 解析字段
            //if (tokens.size() >= 11) {
            if (tokens.size() >= 10) {
                diffX_ = std::stod(tokens[1]);
                diffY_ = std::stod(tokens[2]);
                diffZ_ = std::stod(tokens[3]);
                diffR_ = std::stod(tokens[4]);
                pitch_ = std::stod(tokens[5]);
                roll_ = std::stod(tokens[6]);
                heading_ = std::stod(tokens[7]);
                switch (std::stoi(tokens[8])) {
                    case 0:
                        locState_ = "初始化";
                        break;
                    case 1:
                        locState_ = "单点定位";
                        break;
                    case 2:
                        locState_ = "码差分";
                        break;
                    case 4:
                        locState_ = "固定解";
                        break;
                    case 5:
                        locState_ = "浮点解";
                        break;
                    default:
                        locState_ = "未知状态";
                        break;
                }
                fourGState_ = std::stoi(tokens[9]);
                if(fourGState_ >= 0 && fourGState_ < 14){
                    fourGState__ = "4G连接中";
                }else if(fourGState_ > 14 && fourGState_ < 18){
                    fourGState__ = "服务器连接中";
                }else if(fourGState_ == 19){
                    fourGState__ = "连接完成";
                }else{
                    fourGState__ = "未知状态";
                }
                switch (std::stoi(tokens[10])) {
                    case -1:
                        fixBaseState_ = "选择固定基站失败";
                        break;
                    case 0:
                        fixBaseState_ = "无固定基站选择";
                        break;
                    case 1:
                        fixBaseState_ = "选择固定基站成功";
                        break;
                    default:
                        fixBaseState_ = "未知状态";
                        break;
                }
                //fourGQuality_ = std::stoi(tokens[11]);
            }
        }

        void PrintData() {
            std::cout << "差分X距离：" << diffX_ << "米" << std::endl;
            std::cout << "差分Y距离：" << diffY_ << "米" << std::endl;
            std::cout << "差分Z距离：" << diffZ_ << "米" << std::endl;
            std::cout << "差分R距离：" << diffR_ << "米" << std::endl;
            std::cout << "俯仰角：" << pitch_ << "度" << std::endl;
            std::cout << "横滚角：" << roll_ << "度" << std::endl;
            std::cout << "航向角：" << heading_ << "度" << std::endl;
            std::cout << "移动站定位状态：" << locState_ << std::endl;
            std::cout << "4G状态：" << fourGState__ << std::endl;
            std::cout << "固定基站状态：" << fixBaseState_ << std::endl;
            //std::cout << "4G信号质量：" << fourGQuality_ << std::endl;
        }
};

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
                        case GPGGA:
                            std::cout << "GPGGA: ";
                            break;
                        case GPRMC:
                            std::cout << "GPRMC: ";
                            break;
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
