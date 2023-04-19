/**
 * @author OttoLi (ottoli.pro@gmail.com)
 * @brief 已创建GPGGA、GPRMC、WTRTK类，并实现解析和输出
 * @version 0.4
 * @date 2023-04-19
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
        double diffX_ = 0;                   //差分X距离，以米为单位，表示当前位置与差分基准站在东西方向上的距离差
        double diffY_ = 0;                   //差分Y距离，以米为单位，表示当前位置与差分基准站在南北方向上的距离差
        double diffZ_ = 0;                   //差分Z距离，以米为单位，表示当前位置与差分基准站在天线高度方向上的距离差
        double diffR_ = 0;                   //差分R距离，以米为单位，表示当前位置与差分基准站在水平面上的距离差
        double pitch_ = 0;                   //俯仰角，以度为单位，表示当前天线相对于水平面的仰角
        double roll_ = 0;                    //横滚角，以度为单位，表示当前天线相对于水平面的横滚角
        double heading_ = 0;                 //航向角，以度为单位，表示当前方向与正北方向之间的夹角
        std::string locState_ = "NULL";      //移动站定位状态，表示当前移动站的定位状态
        int fourGState_ = 0;                 //4G状态flag
        std::string fourGState__ = "NULL";   //4G状态，表示当前4G网络连接状态
        std::string fixBaseState_ = "NULL";  //固定基站状态，表示当前是否选择了固定基站
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
            if (tokens.size() >= 11) {
                if(tokens[1] != "")
                diffX_ = std::stod(tokens[1]);
                if(tokens[2] != "")
                diffY_ = std::stod(tokens[2]);
                if(tokens[3] != "")
                diffZ_ = std::stod(tokens[3]);
                if(tokens[4] != "")
                diffR_ = std::stod(tokens[4]);
                if(tokens[5] != "")
                pitch_ = std::stod(tokens[5]);
                if(tokens[6] != "")
                roll_ = std::stod(tokens[6]);
                if(tokens[7] != "")
                heading_ = std::stod(tokens[7]);
                if(tokens[8] != "")
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
                if(tokens[9] != "")
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
                if(tokens[10] != "")
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
                //if(tokens[11] != "")
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

class GPGGAData {
    //$GPGGA,hhmmss.sss,ddmm.mmmm,N/S,dddmm.mmmm,E/W,quality,satellites,HDOP,altitude,altitude_units,geoid_height,geoid_height_units,differential_age,differential_ref_station_id,checksum
    private:
        std::string utc_time = "NULL";                       // UTC时间（hhmmss.sss）
        double latitude = 0;                                 // 纬度（ddmm.mmmm）
        std::string latitude_direction = "NULL";             // 纬度方向（N/S）
        double longitude = 0;                                // 经度（dddmm.mmmm）
        std::string longitude_direction = "NULL";            // 经度方向（E/W）
        std::string quality_ = "NULL";                       // GPS状态|0=未定位|1=单点定位|2=伪距/SBAS|3=无效PPS|4=RTK固定|5=RTK浮动|6=正在估算|7=手动启动基准站|8=RTK宽巷解|9=伪距（诺瓦泰615）
        int satellites = 0;                                  // 正在使用的卫星数量（00-12）
        double hdop = 0;                                     // HDOP水平精度因子（0.5-99.9）
        double altitude = 0;                                 // 海拔高度（-9999.9-99999.9）
        std::string altitude_units = "NULL";                 // 高度单位
        double geoid_height = 0;                             // 大地水准面相对椭球面的高度
        std::string geoid_height_units = "NULL";             // 大地水准面高度单位
        double differential_age = 0;                         // 差分时间
        std::string differential_ref_station_id = "NULL";    // 差分基准站ID
        int checksum = 0;                                    // 校验值

    public:
        GPGGAData(std::string sentence) {
            // 将 GPGGA 数据包按逗号分割为一个字符串向量
            std::vector<std::string> tokens;
            std::string token;
            std::stringstream ss(sentence);
            while (std::getline(ss, token, ',')) {
                // 检查是否包含回车符或换行符
                if (token.find("\r\n") != std::string::npos) {
                    // 如果包含，则删除回车符和换行符
                    token.erase(token.find("\r\n"), 2);
                }
                tokens.push_back(token);
            }

            // 解析并存储各个参数
            //statement_id = tokens[0];
            if (tokens.size() >= 15) {
                if (tokens[1] != "")
                utc_time = tokens[1];
                if (tokens[2] != "")
                latitude = std::stod(tokens[2]);
                if (tokens[3] != "")
                latitude_direction = tokens[3];
                if (tokens[4] != "")
                longitude = std::stod(tokens[4]);
                if (tokens[5] != "")
                longitude_direction = tokens[5];
                if (tokens[6] != "")
                switch (std::stoi(tokens[6])) {
                    case 0:
                        quality_ = "未定位";
                        break;
                    case 1:
                        quality_ = "单点定位";
                        break;
                    case 2:
                        quality_ = "伪距/SBAS";
                        break;
                    case 3:
                        quality_ = "无效PPS";
                        break;
                    case 4:
                        quality_ = "RTK固定";
                        break;
                    case 5:
                        quality_ = "RTK浮动";
                        break;
                    case 6:
                        quality_ = "正在估算";
                        break;
                    case 7:
                        quality_ = "手动启动基准站";
                        break;
                    case 8:
                        quality_ = "RTK宽巷解";
                        break;
                    case 9:
                        quality_ = "伪距（诺瓦泰615）";
                        break;
                    default:
                        quality_ = "未知状态";
                        break;
                }
                if (tokens[7] != "")
                    satellites = std::stoi(tokens[7]);
                if (tokens[8] != "")
                    hdop = std::stod(tokens[8]);
                if (tokens[9] != "")
                    altitude = std::stod(tokens[9]);
                if (tokens[10] != "")
                    altitude_units = tokens[10];
                if (tokens[11] != "")
                    geoid_height = std::stod(tokens[11]);
                if (tokens[12] != "")
                    geoid_height_units = tokens[12];
                if (tokens[13] != "")
                    differential_age = std::stod(tokens[13]);
                //token[14] = 0000*1F
                if (tokens[14] != ""){
                    int pos = tokens[14].find('*');
                    std::string str1 = tokens[14].substr(0, pos);
                    std::string str2 = tokens[14].substr(pos+1, tokens[14].length()-pos-1);
                    differential_ref_station_id = str1;
                    checksum = std::stoi(str2,0,16);
                }
            }
        }

        void PrintData(){
            std::cout << "UTC时间: " << utc_time << std::endl;
            std::cout << "纬度: " << latitude << latitude_direction << std::endl;
            std::cout << "经度: " << longitude << longitude_direction << std::endl;
            std::cout << "GPS状态: " << quality_ << std::endl;
            std::cout << "正在使用的卫星数量: " << satellites << std::endl;
            std::cout << "HDOP水平精度因子: " << hdop << std::endl;
            std::cout << "海拔高度: " << altitude << altitude_units << std::endl;
            std::cout << "大地水准面相对椭球面的高度: " << geoid_height << geoid_height_units << std::endl;
            std::cout << "差分时间: " << differential_age << std::endl;
            std::cout << "差分基准站ID: " << differential_ref_station_id << std::endl;
            std::cout << "校验值: " << checksum << std::endl;
        }
};

class GPRMCData {
    //$GPRMC,144326.00,A,5107.0017737,N,11402.3291611,W,0.080,323.3,210307,0.0,E,A*20
    private:
        std::string time = "NULL";                      // UTC时间，hhmmss（时分秒）格式
        std::string status = "NULL";                    // 定位状态，A：有效定位，V：无效定位
        double latitude = 0;                            // 纬度ddmm.mmmm（度分）格式（前面的0也将被传输）
        std::string ns = "NULL";                        // 纬度半球，N：北半球，S：南半球
        double longitude = 0;                           // 经度dddmm.mmmm（度分）格式（前面的0也将被传输）
        std::string ew = "NULL";                        // 经度半球，E：东经，W：西经
        double speed = 0;                               // 地面速率（000.0~999.9节，前面的0也将被传输）
        double heading = 0;                             // 地面航向（000.0~359.9度，以真北为参考基准，前面的0也将被传输）
        std::string date = "NULL";                      // UTC日期, ddmmyy（日月年）格式
        double declination = 0;                         // 磁偏角（000.0~180.0度，前面的0也将被传输）
        std::string declinationDirection = "NULL";      // 磁偏角方向，E：东，W：西
        std::string mode = "NULL";                      // 模式指示，A：自主定位，D：差分，E：估算，N：数据无效
        int checksum = 0;                  // 校验值

    public:
        GPRMCData(std::string sentence) {
            // 将 GPGGA 数据包按逗号分割为一个字符串向量
            std::vector<std::string> tokens;
            std::string token;
            std::stringstream ss(sentence);
            while (std::getline(ss, token, ',')) {
                // 检查是否包含回车符或换行符
                if (token.find("\r\n") != std::string::npos) {
                    // 如果包含，则删除回车符和换行符
                    token.erase(token.find("\r\n"), 2);
                }
                tokens.push_back(token);
            }

            // 解析并存储各个参数
            //statement_id = tokens[0];
            if (tokens.size() >= 12) {
                if (tokens[1] != "")
                time = tokens[1];
                if (tokens[2] != ""){
                    if (tokens[2] == "A") {
                        status = "有效定位";
                    }
                    else if(tokens[2] == "V"){
                        status = "无效定位";
                    }
                    else
                        status = "未知状态";
                }
                if (tokens[3] != "")
                latitude = std::stod(tokens[3]);
                if (tokens[4] != "")
                ns = tokens[4];
                if (tokens[5] != "")
                longitude = std::stod(tokens[5]);
                if (tokens[6] != "")
                ew = tokens[6];
                if (tokens[7] != "")
                speed = std::stod(tokens[7]);
                if (tokens[8] != "")
                heading = std::stod(tokens[8]);
                if (tokens[9] != "")
                date = tokens[9];
                if (tokens[10] != "")
                declination = std::stod(tokens[10]);
                if (tokens[11] != ""){
                    if (tokens[11] == "E") {
                        declinationDirection = "东";
                    }
                    else if(tokens[11] == "W"){
                        declinationDirection = "西";
                    }
                    else
                        declinationDirection = "未知方向";
                }
                //token[12] = A*20
                if (tokens[12] != ""){
                    int pos = tokens[12].find('*');
                    std::string str1 = tokens[12].substr(0, pos);
                    std::string str2 = tokens[12].substr(pos+1, tokens[12].length()-pos-1);
                    if (str1 == "A") {
                        mode = "自主定位";
                    }
                    else if(str1 == "D"){
                        mode = "差分";
                    }
                    else if(str1 == "E"){
                        mode = "估算";
                    }
                    else if(str1 == "N"){
                        mode = "数据无效";
                    }
                    else
                        mode = "未知模式";
                    checksum = std::stoi(str2,0,16);
                }
            }
        }

        void PrintData(){
            std::cout << "UTC时间: " << time << std::endl;
            std::cout << "定位状态: " << status << std::endl;
            std::cout << "纬度: " << latitude << ns << std::endl;
            std::cout << "经度: " << longitude << ew << std::endl;
            std::cout << "地面速率: " << speed << "节" << std::endl;
            std::cout << "地面航向: " << heading << "度" << std::endl;
            std::cout << "UTC时间: " << date << std::endl;
            std::cout << "磁偏角: " << declination << "度" << declinationDirection << std::endl;
            std::cout << "模式指示: " << mode << std::endl;
            std::cout << "校验值: " << checksum << std::endl;
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
                        case GPGGA: {
                            GPGGAData gga(sentence);
                            gga.PrintData();
                            break;
                        }
                        case GPRMC: {
                            GPRMCData rmc(sentence);
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
