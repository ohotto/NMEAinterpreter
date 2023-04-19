# 简介
在 Linux 下实现 NMEA 数据包的接收、解析、保存、输出，作为定位模块的基本“轮子程序”

# 开发环境
- 基于维特智能 WTRTK-4G 模块
- 仅在 Linux 发行版 Ubuntu 进行验证

# 程序功能
- [x] 从串口读出模块发送的 NMEA 数据包
- [x] 能够判断数据包类型，选择性解析、输出指定数据包
    - 目前仅处理 $GPGGA、$GPRMC 和 $WTRTK
- [x] 解析数据包内容，并实现格式化输出
    - [x] 解析 $GPGGA
    - [x] 解析 $GPRMC
    - [x] 解析 $WTRTK
- [ ] 保存解析后的数据到 csv 或 txt 文件