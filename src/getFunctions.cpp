/**
 * @author OttoLi (ottoli.pro@gmail.com)
 * @brief 一些小函数
 * @version 0.1
 * @date 2023-04-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "Functions.h"

//返回yymmdd_hhmmss格式的当前日期时间
std::string getCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now);

    // 将日期各部分转换为字符串类型
    std::stringstream ss;
    ss << std::put_time(ptm, "%y%m%d_%H%M%S");
    std::string dateTimeStr = ss.str();

    return dateTimeStr;
}

// 筛选NMEA数据包类型，仅接收$GGA、$RMC和$WTRTK
NmeaType GetNmeaType(const std::string& nmea) {
    if (nmea.empty() || nmea[nmea.size() - 1] != '\n') {
        return NMEA_INVALID;
    }
    if (nmea.compare(0, strlen("$GPGGA"), "$GPGGA") == 0) {
        return GGA;
    }
    if (nmea.compare(0, strlen("$BDGGA"), "$BDGGA") == 0) {
        return GGA;
    }
    if (nmea.compare(0, strlen("$GAGGA"), "$GAGGA") == 0) {
        return GGA;
    }
    if (nmea.compare(0, strlen("$GLGGA"), "$GLGGA") == 0) {
        return GGA;
    }
    if (nmea.compare(0, strlen("$GNGGA"), "$GNGGA") == 0) {
        return GGA;
    }
    if (nmea.compare(0, strlen("$GPRMC"), "$GPRMC") == 0) {
        return RMC;
    }
    if (nmea.compare(0, strlen("$BDGGA"), "$BDGGA") == 0) {
        return RMC;
    }
    if (nmea.compare(0, strlen("$GAGGA"), "$GAGGA") == 0) {
        return RMC;
    }
    if (nmea.compare(0, strlen("$GLGGA"), "$GLGGA") == 0) {
        return RMC;
    }
    if (nmea.compare(0, strlen("$GNGGA"), "$GNGGA") == 0) {
        return RMC;
    }
    if (nmea.compare(0, strlen("$WTRTK"), "$WTRTK") == 0) {
        return WTRTK;
    }
    return NMEA_INVALID;
}
