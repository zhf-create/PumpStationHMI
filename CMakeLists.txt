# 泵站模具冷却供水控制系统 —— Qt Quick 上位机（页面设计）
# 技术栈：Qt QML / C++ 桌面端

# 最低 CMake 版本 3.16。
# 注：qt_standard_project_setup() / qt_add_qml_module() 需 Qt 6.2+，
#     本工程按 Qt 6.5 编写（2026 年环境通常已为 6.5 或更高）。
cmake_minimum_required(VERSION 3.16)

project(PumpStationHMI VERSION 1.0 LANGUAGES CXX)

# 使用 C++17，Qt 6 标准写法
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 依赖 Qt Quick（界面）+ Qt Sql（SQLite 数据存储）+ Qt SerialBus（Modbus TCP 通信）
find_package(Qt6 REQUIRED COMPONENTS Quick Sql SerialBus)

# Qt 6.2+ 推荐：统一设置 AUTOMOC / AUTORCC 等属性
qt_standard_project_setup()

# 让 CMake 生成 .qmlls.ini（知识点）：
# Qt Creator 的 QML 代码模型（qmlls）靠这个文件里的 buildDir
# 定位 build 目录下的 qmldir，从而识别本模块自己的类型
# （ProcessFlowPage / ParamTile / EquipmentBox / Theme 等）。
# 原因：编辑器里出现的 “Unknown component (M300)” 就是缺了这个文件，
#       qmlls 找不到模块的 qmldir，无法解析同模块类型（并非运行时报错）。
#       这个开关默认关闭，需显式置 ON 才会生成 .qmlls.ini。
set(QT_QML_GENERATE_QMLLS_INI ON)

# 生成可执行程序（.cpp 参与编译，.h 让 IDE/构建可见）
# 知识点：加 WIN32 让 exe 生成GUI 子系统而非控制台子系统。
# 不加时双击运行会先弹一个黑色控制台窗口（Windows 给控制台程序默认开的），
# 加 WIN32 后 exe 文件头标记为窗口程序，双击直接出界面、不再弹终端。
qt_add_executable(appPumpStationHMI WIN32
    src/main.cpp
    src/backend/devicemodel.cpp
    src/backend/devicemodel.h
    src/backend/reportmodel.cpp
    src/backend/reportmodel.h
    src/backend/alarmmodel.cpp
    src/backend/alarmmodel.h
    src/backend/pumpbackend.cpp
    src/backend/pumpbackend.h
    src/backend/datastore.cpp
    src/backend/datastore.h
)

# 把 src/ 加入 include 路径，保证源码内 `#include "backend/pumpbackend.h"` 被正确解析
target_include_directories(appPumpStationHMI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 标记 Theme.qml 为单例（知识点）：
# QML 里的 `pragma Singleton` 只是声明，真正让引擎把它当单例使用，
# 还需要下面这行配合，否则其它页面里的 `Theme.xxx` 会解析失败。
# 关键：这行必须放在 qt_add_qml_module 之前！
# 因为 CMake 宏在 qt_add_qml_module 内部生成 qmldir 时会读取
# QT_QML_SINGLETON_TYPE，若放后面（设置太晚）qmldir 会写成
# `Theme 1.0 Theme.qml` 而不是 `singleton Theme 1.0 Theme.qml`，
# 导致 Theme 无法按单例解析，进而连累引用 Theme 的组件
# （ParamTile/EquipmentBox/StatusPill）报 “is not a type”。
set_source_files_properties(qml/Theme.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

# 开启 QTP0004 策略（知识点 / 原因）：
# 本项目把 QML 拆到了 pages/、components/ 子目录里。默认（OLD）
# 行为下，qt_add_qml_module 不会为这些子目录生成各自的 qmldir，
# 导致子目录里的文件（如 pages/ProcessFlowPage.qml）在运行时做
# 模块隐式导入时找不到模块的 qmldir，进而解析不到同模块组件
# （components/EquipmentBox.qml 报 “EquipmentBox is not a type”）。
# 设为 NEW 后，CMake 会为 pages/、components/ 各生成一个内容为
# “prefer :/qt/qml/PumpStationHMI/” 的子目录 qmldir，把解析重定向
# 回模块根 qmldir，同模块类型就能正常解析了。
qt_policy(SET QTP0004 NEW)

# qt_add_qml_module：把本目录下的所有 .qml 打包成"同一 QML 模块"
# 好处（知识点）：
#   1) 自动生成 qmldir + qrc，无需手写资源列表
#   2) 模块内所有 QML 类型按文件名互相引用，跨子目录无需 import
#   3) 编译期校验引用，减少运行时"找不到类型"错误
qt_add_qml_module(appPumpStationHMI
    URI PumpStationHMI            # 模块唯一标识（外部引用时用）
    VERSION 1.0
    # 知识点：显式指定资源前缀为标准的 /qt/qml。
    # 若不指定，Qt 策略 QTP0001（6.5 引入）在未开启 NEW 时默认把前缀设为 /，
    # 模块会落到 qrc:/PumpStationHMI/ 而不是 qrc:/qt/qml/PumpStationHMI/，
    # 导致运行时 QML 引擎找不到该模块、同模块类型（如 EquipmentBox）无法解析。
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/pages/ProcessFlowPage.qml
        qml/pages/OperationDataPage.qml
        qml/pages/DeviceControlPage.qml
        qml/pages/AutoParamPage.qml
        qml/pages/ReportPage.qml
        qml/pages/AlarmPage.qml
        qml/components/StatusPill.qml
        qml/components/ParamTile.qml
        qml/components/EquipmentBox.qml
        qml/components/EquipmentIcon.qml
)

# 把 qtquickcontrols2.conf 打进资源（知识点 / 原因）：
# Qt Quick Controls 在启动时按固定顺序查找样式配置，优先级最高的
# 是 qrc 里的 ":/qtquickcontrols2.conf"。用 PREFIX "/" 可让其落在
# 该标准路径，引擎启动即读到 Style=Basic，从而允许自定义控件外观。
qt_add_resources(appPumpStationHMI "qtquickcontrols2_conf"
    PREFIX "/"
    FILES
        resources/qtquickcontrols2.conf
)

# 链接 QtQuick（提供 QGuiApplication、QQmlApplicationEngine 等）+ QtSql（SQLite 存储）
# + Qt6::SerialBus（QModbusTcpClient，读取 Modbus Slave 软件的手动输入数据）
target_link_libraries(appPumpStationHMI PRIVATE Qt6::Quick Qt6::Sql Qt6::SerialBus)