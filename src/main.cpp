// 程序入口（Qt 6 最小 QML 启动模板）
// 知识点：
//   - QGuiApplication（而非 QApplication）：纯 QML/Quick 界面没有
//     QtWidgets，用更轻量的 QGuiApplication 即可。
//   - qmlRegisterSingletonInstance：把 C++ 单例以类型名暴露给 QML，
//     QML 里 `import Backend 1.0` 后即可用 `Backend.supplyTemp` 等绑定。
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "backend/pumpbackend.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 1. 创建后端单例。注意：必须比 QQmlApplicationEngine 活得久，
    //    所以放在 engine 之前声明（栈对象后析构）。
    PumpBackend backend;

    // 2. 注册到 QML 命名空间 "Backend"（URI Backend，主版本 1，次版本 0）。
    //    注册的类型名也是 "Backend"，QML 侧 import Backend 1.0 后直接用。
    qmlRegisterSingletonInstance("Backend", 1, 0, "Backend", &backend);

    // 3. 启动遥测数据模拟器（替换成 PLC 轮询时，接口保持不变）
    backend.start();

    // 4. QML 引擎：按模块 + 类型名加载入口（Qt 6.5+ 推荐写法）
    QQmlApplicationEngine engine;

    // 知识点：loadFromModule 会先按 URI 定位模块（依赖 /qt/qml 前缀下的 qmldir），
    // 再加载该模块里的 Main 类型，同时触发模块的隐式导入——这样 Main.qml 里
    // 引用的同模块类型（ProcessFlowPage / EquipmentBox 等）都能自动解析到。
    // 相比 load(url) 更稳，不必握死资源路径。
    engine.loadFromModule("PumpStationHMI", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}