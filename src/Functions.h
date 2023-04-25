/**
 * @author OttoLi (ottoli.pro@gmail.com)
 * @brief Functions
 * @version 0.1
 * @date 2023-04-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

//返回yymmdd_hhmmss格式的当前日期时间
std::string getCurrentDateTime();

// 筛选NMEA数据包类型，仅接收$GGA、$RMC和$WTRTK
enum NmeaType {
    NMEA_INVALID,
    GGA,
    RMC,
    WTRTK
};
NmeaType GetNmeaType(const std::string& nmea);

#endif
