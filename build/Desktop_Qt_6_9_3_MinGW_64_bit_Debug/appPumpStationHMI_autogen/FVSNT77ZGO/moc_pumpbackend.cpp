/****************************************************************************
** Meta object code from reading C++ file 'pumpbackend.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../backend/pumpbackend.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pumpbackend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11PumpBackendE_t {};
} // unnamed namespace

template <> constexpr inline auto PumpBackend::qt_create_metaobjectdata<qt_meta_tag_ZN11PumpBackendE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PumpBackend",
        "supplyTempChanged",
        "",
        "returnTempChanged",
        "coolingTowerOutletChanged",
        "exchangerInChanged",
        "exchangerOutChanged",
        "supplyPressureChanged",
        "returnPressureChanged",
        "makeupPressureChanged",
        "supplyFlowChanged",
        "cumulativeFlowChanged",
        "conductivityChanged",
        "tankLevelChanged",
        "expansionLevelChanged",
        "valvePos1Changed",
        "valvePos2Changed",
        "supplyPumpFreqChanged",
        "coolingFanFreqChanged",
        "cyclePump1CurrentChanged",
        "cyclePump2CurrentChanged",
        "makeupPump1CurrentChanged",
        "makeupPump2CurrentChanged",
        "conductivityAlarmChanged",
        "copChanged",
        "wetBulbTempChanged",
        "dailyPowerChanged",
        "pumpRunHoursChanged",
        "cyclePump1RunningChanged",
        "cyclePump2RunningChanged",
        "cyclePump3RunningChanged",
        "coolingFan1RunningChanged",
        "coolingFan2RunningChanged",
        "makeupPump1RunningChanged",
        "makeupPump2RunningChanged",
        "sprayPump1RunningChanged",
        "makeupValveOpenChanged",
        "drainValveOpenChanged",
        "tempSetpointChanged",
        "pressSetpointChanged",
        "coolingSetpointChanged",
        "tempProportionalChanged",
        "tempIntegralChanged",
        "pressProportionalChanged",
        "pressIntegralChanged",
        "tempAlarmL1Changed",
        "tempAlarmL2Changed",
        "pressLowAlarmChanged",
        "conductivityLimitChanged",
        "saveMessageChanged",
        "onTick",
        "syncDeviceStates",
        "start",
        "saveParams",
        "simulatePumpFault",
        "key",
        "supplyTemp",
        "returnTemp",
        "coolingTowerOutlet",
        "exchangerIn",
        "exchangerOut",
        "supplyPressure",
        "returnPressure",
        "makeupPressure",
        "supplyFlow",
        "cumulativeFlow",
        "conductivity",
        "tankLevel",
        "expansionLevel",
        "valvePos1",
        "valvePos2",
        "supplyPumpFreq",
        "coolingFanFreq",
        "cyclePump1Current",
        "cyclePump2Current",
        "makeupPump1Current",
        "makeupPump2Current",
        "conductivityAlarm",
        "cop",
        "wetBulbTemp",
        "dailyPower",
        "pumpRunHours",
        "cyclePump1Running",
        "cyclePump2Running",
        "cyclePump3Running",
        "coolingFan1Running",
        "coolingFan2Running",
        "makeupPump1Running",
        "makeupPump2Running",
        "sprayPump1Running",
        "makeupValveOpen",
        "drainValveOpen",
        "tempSetpoint",
        "pressSetpoint",
        "coolingSetpoint",
        "tempProportional",
        "tempIntegral",
        "pressProportional",
        "pressIntegral",
        "tempAlarmL1",
        "tempAlarmL2",
        "pressLowAlarm",
        "conductivityLimit",
        "devices",
        "QAbstractItemModel*",
        "reports",
        "alarms",
        "saveMessage",
        "dbPath"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'supplyTempChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'returnTempChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'coolingTowerOutletChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'exchangerInChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'exchangerOutChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'supplyPressureChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'returnPressureChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'makeupPressureChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'supplyFlowChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cumulativeFlowChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'conductivityChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tankLevelChanged'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'expansionLevelChanged'
        QtMocHelpers::SignalData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'valvePos1Changed'
        QtMocHelpers::SignalData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'valvePos2Changed'
        QtMocHelpers::SignalData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'supplyPumpFreqChanged'
        QtMocHelpers::SignalData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'coolingFanFreqChanged'
        QtMocHelpers::SignalData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cyclePump1CurrentChanged'
        QtMocHelpers::SignalData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cyclePump2CurrentChanged'
        QtMocHelpers::SignalData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'makeupPump1CurrentChanged'
        QtMocHelpers::SignalData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'makeupPump2CurrentChanged'
        QtMocHelpers::SignalData<void()>(22, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'conductivityAlarmChanged'
        QtMocHelpers::SignalData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'copChanged'
        QtMocHelpers::SignalData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wetBulbTempChanged'
        QtMocHelpers::SignalData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'dailyPowerChanged'
        QtMocHelpers::SignalData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pumpRunHoursChanged'
        QtMocHelpers::SignalData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cyclePump1RunningChanged'
        QtMocHelpers::SignalData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cyclePump2RunningChanged'
        QtMocHelpers::SignalData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'cyclePump3RunningChanged'
        QtMocHelpers::SignalData<void()>(30, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'coolingFan1RunningChanged'
        QtMocHelpers::SignalData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'coolingFan2RunningChanged'
        QtMocHelpers::SignalData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'makeupPump1RunningChanged'
        QtMocHelpers::SignalData<void()>(33, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'makeupPump2RunningChanged'
        QtMocHelpers::SignalData<void()>(34, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sprayPump1RunningChanged'
        QtMocHelpers::SignalData<void()>(35, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'makeupValveOpenChanged'
        QtMocHelpers::SignalData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'drainValveOpenChanged'
        QtMocHelpers::SignalData<void()>(37, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tempSetpointChanged'
        QtMocHelpers::SignalData<void()>(38, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pressSetpointChanged'
        QtMocHelpers::SignalData<void()>(39, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'coolingSetpointChanged'
        QtMocHelpers::SignalData<void()>(40, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tempProportionalChanged'
        QtMocHelpers::SignalData<void()>(41, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tempIntegralChanged'
        QtMocHelpers::SignalData<void()>(42, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pressProportionalChanged'
        QtMocHelpers::SignalData<void()>(43, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pressIntegralChanged'
        QtMocHelpers::SignalData<void()>(44, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tempAlarmL1Changed'
        QtMocHelpers::SignalData<void()>(45, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tempAlarmL2Changed'
        QtMocHelpers::SignalData<void()>(46, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pressLowAlarmChanged'
        QtMocHelpers::SignalData<void()>(47, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'conductivityLimitChanged'
        QtMocHelpers::SignalData<void()>(48, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'saveMessageChanged'
        QtMocHelpers::SignalData<void()>(49, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onTick'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'syncDeviceStates'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPrivate, QMetaType::Void),
        // Method 'start'
        QtMocHelpers::MethodData<void()>(52, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'saveParams'
        QtMocHelpers::MethodData<void()>(53, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'simulatePumpFault'
        QtMocHelpers::MethodData<void(const QString &)>(54, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 55 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'supplyTemp'
        QtMocHelpers::PropertyData<double>(56, QMetaType::Double, QMC::DefaultPropertyFlags, 0),
        // property 'returnTemp'
        QtMocHelpers::PropertyData<double>(57, QMetaType::Double, QMC::DefaultPropertyFlags, 1),
        // property 'coolingTowerOutlet'
        QtMocHelpers::PropertyData<double>(58, QMetaType::Double, QMC::DefaultPropertyFlags, 2),
        // property 'exchangerIn'
        QtMocHelpers::PropertyData<double>(59, QMetaType::Double, QMC::DefaultPropertyFlags, 3),
        // property 'exchangerOut'
        QtMocHelpers::PropertyData<double>(60, QMetaType::Double, QMC::DefaultPropertyFlags, 4),
        // property 'supplyPressure'
        QtMocHelpers::PropertyData<double>(61, QMetaType::Double, QMC::DefaultPropertyFlags, 5),
        // property 'returnPressure'
        QtMocHelpers::PropertyData<double>(62, QMetaType::Double, QMC::DefaultPropertyFlags, 6),
        // property 'makeupPressure'
        QtMocHelpers::PropertyData<double>(63, QMetaType::Double, QMC::DefaultPropertyFlags, 7),
        // property 'supplyFlow'
        QtMocHelpers::PropertyData<double>(64, QMetaType::Double, QMC::DefaultPropertyFlags, 8),
        // property 'cumulativeFlow'
        QtMocHelpers::PropertyData<double>(65, QMetaType::Double, QMC::DefaultPropertyFlags, 9),
        // property 'conductivity'
        QtMocHelpers::PropertyData<double>(66, QMetaType::Double, QMC::DefaultPropertyFlags, 10),
        // property 'tankLevel'
        QtMocHelpers::PropertyData<double>(67, QMetaType::Double, QMC::DefaultPropertyFlags, 11),
        // property 'expansionLevel'
        QtMocHelpers::PropertyData<double>(68, QMetaType::Double, QMC::DefaultPropertyFlags, 12),
        // property 'valvePos1'
        QtMocHelpers::PropertyData<double>(69, QMetaType::Double, QMC::DefaultPropertyFlags, 13),
        // property 'valvePos2'
        QtMocHelpers::PropertyData<double>(70, QMetaType::Double, QMC::DefaultPropertyFlags, 14),
        // property 'supplyPumpFreq'
        QtMocHelpers::PropertyData<double>(71, QMetaType::Double, QMC::DefaultPropertyFlags, 15),
        // property 'coolingFanFreq'
        QtMocHelpers::PropertyData<double>(72, QMetaType::Double, QMC::DefaultPropertyFlags, 16),
        // property 'cyclePump1Current'
        QtMocHelpers::PropertyData<double>(73, QMetaType::Double, QMC::DefaultPropertyFlags, 17),
        // property 'cyclePump2Current'
        QtMocHelpers::PropertyData<double>(74, QMetaType::Double, QMC::DefaultPropertyFlags, 18),
        // property 'makeupPump1Current'
        QtMocHelpers::PropertyData<double>(75, QMetaType::Double, QMC::DefaultPropertyFlags, 19),
        // property 'makeupPump2Current'
        QtMocHelpers::PropertyData<double>(76, QMetaType::Double, QMC::DefaultPropertyFlags, 20),
        // property 'conductivityAlarm'
        QtMocHelpers::PropertyData<bool>(77, QMetaType::Bool, QMC::DefaultPropertyFlags, 21),
        // property 'cop'
        QtMocHelpers::PropertyData<double>(78, QMetaType::Double, QMC::DefaultPropertyFlags, 22),
        // property 'wetBulbTemp'
        QtMocHelpers::PropertyData<double>(79, QMetaType::Double, QMC::DefaultPropertyFlags, 23),
        // property 'dailyPower'
        QtMocHelpers::PropertyData<double>(80, QMetaType::Double, QMC::DefaultPropertyFlags, 24),
        // property 'pumpRunHours'
        QtMocHelpers::PropertyData<double>(81, QMetaType::Double, QMC::DefaultPropertyFlags, 25),
        // property 'cyclePump1Running'
        QtMocHelpers::PropertyData<bool>(82, QMetaType::Bool, QMC::DefaultPropertyFlags, 26),
        // property 'cyclePump2Running'
        QtMocHelpers::PropertyData<bool>(83, QMetaType::Bool, QMC::DefaultPropertyFlags, 27),
        // property 'cyclePump3Running'
        QtMocHelpers::PropertyData<bool>(84, QMetaType::Bool, QMC::DefaultPropertyFlags, 28),
        // property 'coolingFan1Running'
        QtMocHelpers::PropertyData<bool>(85, QMetaType::Bool, QMC::DefaultPropertyFlags, 29),
        // property 'coolingFan2Running'
        QtMocHelpers::PropertyData<bool>(86, QMetaType::Bool, QMC::DefaultPropertyFlags, 30),
        // property 'makeupPump1Running'
        QtMocHelpers::PropertyData<bool>(87, QMetaType::Bool, QMC::DefaultPropertyFlags, 31),
        // property 'makeupPump2Running'
        QtMocHelpers::PropertyData<bool>(88, QMetaType::Bool, QMC::DefaultPropertyFlags, 32),
        // property 'sprayPump1Running'
        QtMocHelpers::PropertyData<bool>(89, QMetaType::Bool, QMC::DefaultPropertyFlags, 33),
        // property 'makeupValveOpen'
        QtMocHelpers::PropertyData<bool>(90, QMetaType::Bool, QMC::DefaultPropertyFlags, 34),
        // property 'drainValveOpen'
        QtMocHelpers::PropertyData<bool>(91, QMetaType::Bool, QMC::DefaultPropertyFlags, 35),
        // property 'tempSetpoint'
        QtMocHelpers::PropertyData<double>(92, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 36),
        // property 'pressSetpoint'
        QtMocHelpers::PropertyData<double>(93, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 37),
        // property 'coolingSetpoint'
        QtMocHelpers::PropertyData<double>(94, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 38),
        // property 'tempProportional'
        QtMocHelpers::PropertyData<double>(95, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 39),
        // property 'tempIntegral'
        QtMocHelpers::PropertyData<double>(96, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 40),
        // property 'pressProportional'
        QtMocHelpers::PropertyData<double>(97, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 41),
        // property 'pressIntegral'
        QtMocHelpers::PropertyData<double>(98, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 42),
        // property 'tempAlarmL1'
        QtMocHelpers::PropertyData<double>(99, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 43),
        // property 'tempAlarmL2'
        QtMocHelpers::PropertyData<double>(100, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 44),
        // property 'pressLowAlarm'
        QtMocHelpers::PropertyData<double>(101, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 45),
        // property 'conductivityLimit'
        QtMocHelpers::PropertyData<double>(102, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 46),
        // property 'devices'
        QtMocHelpers::PropertyData<QAbstractItemModel*>(103, 0x80000000 | 104, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'reports'
        QtMocHelpers::PropertyData<QAbstractItemModel*>(105, 0x80000000 | 104, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'alarms'
        QtMocHelpers::PropertyData<QAbstractItemModel*>(106, 0x80000000 | 104, QMC::DefaultPropertyFlags | QMC::EnumOrFlag | QMC::Constant),
        // property 'saveMessage'
        QtMocHelpers::PropertyData<QString>(107, QMetaType::QString, QMC::DefaultPropertyFlags, 47),
        // property 'dbPath'
        QtMocHelpers::PropertyData<QString>(108, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Constant),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PumpBackend, qt_meta_tag_ZN11PumpBackendE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PumpBackend::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11PumpBackendE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11PumpBackendE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11PumpBackendE_t>.metaTypes,
    nullptr
} };

void PumpBackend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PumpBackend *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->supplyTempChanged(); break;
        case 1: _t->returnTempChanged(); break;
        case 2: _t->coolingTowerOutletChanged(); break;
        case 3: _t->exchangerInChanged(); break;
        case 4: _t->exchangerOutChanged(); break;
        case 5: _t->supplyPressureChanged(); break;
        case 6: _t->returnPressureChanged(); break;
        case 7: _t->makeupPressureChanged(); break;
        case 8: _t->supplyFlowChanged(); break;
        case 9: _t->cumulativeFlowChanged(); break;
        case 10: _t->conductivityChanged(); break;
        case 11: _t->tankLevelChanged(); break;
        case 12: _t->expansionLevelChanged(); break;
        case 13: _t->valvePos1Changed(); break;
        case 14: _t->valvePos2Changed(); break;
        case 15: _t->supplyPumpFreqChanged(); break;
        case 16: _t->coolingFanFreqChanged(); break;
        case 17: _t->cyclePump1CurrentChanged(); break;
        case 18: _t->cyclePump2CurrentChanged(); break;
        case 19: _t->makeupPump1CurrentChanged(); break;
        case 20: _t->makeupPump2CurrentChanged(); break;
        case 21: _t->conductivityAlarmChanged(); break;
        case 22: _t->copChanged(); break;
        case 23: _t->wetBulbTempChanged(); break;
        case 24: _t->dailyPowerChanged(); break;
        case 25: _t->pumpRunHoursChanged(); break;
        case 26: _t->cyclePump1RunningChanged(); break;
        case 27: _t->cyclePump2RunningChanged(); break;
        case 28: _t->cyclePump3RunningChanged(); break;
        case 29: _t->coolingFan1RunningChanged(); break;
        case 30: _t->coolingFan2RunningChanged(); break;
        case 31: _t->makeupPump1RunningChanged(); break;
        case 32: _t->makeupPump2RunningChanged(); break;
        case 33: _t->sprayPump1RunningChanged(); break;
        case 34: _t->makeupValveOpenChanged(); break;
        case 35: _t->drainValveOpenChanged(); break;
        case 36: _t->tempSetpointChanged(); break;
        case 37: _t->pressSetpointChanged(); break;
        case 38: _t->coolingSetpointChanged(); break;
        case 39: _t->tempProportionalChanged(); break;
        case 40: _t->tempIntegralChanged(); break;
        case 41: _t->pressProportionalChanged(); break;
        case 42: _t->pressIntegralChanged(); break;
        case 43: _t->tempAlarmL1Changed(); break;
        case 44: _t->tempAlarmL2Changed(); break;
        case 45: _t->pressLowAlarmChanged(); break;
        case 46: _t->conductivityLimitChanged(); break;
        case 47: _t->saveMessageChanged(); break;
        case 48: _t->onTick(); break;
        case 49: _t->syncDeviceStates(); break;
        case 50: _t->start(); break;
        case 51: _t->saveParams(); break;
        case 52: _t->simulatePumpFault((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::supplyTempChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::returnTempChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::coolingTowerOutletChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::exchangerInChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::exchangerOutChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::supplyPressureChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::returnPressureChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::makeupPressureChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::supplyFlowChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::cumulativeFlowChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::conductivityChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::tankLevelChanged, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::expansionLevelChanged, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::valvePos1Changed, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::valvePos2Changed, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::supplyPumpFreqChanged, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::coolingFanFreqChanged, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::cyclePump1CurrentChanged, 17))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::cyclePump2CurrentChanged, 18))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::makeupPump1CurrentChanged, 19))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::makeupPump2CurrentChanged, 20))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::conductivityAlarmChanged, 21))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::copChanged, 22))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::wetBulbTempChanged, 23))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::dailyPowerChanged, 24))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::pumpRunHoursChanged, 25))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::cyclePump1RunningChanged, 26))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::cyclePump2RunningChanged, 27))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::cyclePump3RunningChanged, 28))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::coolingFan1RunningChanged, 29))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::coolingFan2RunningChanged, 30))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::makeupPump1RunningChanged, 31))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::makeupPump2RunningChanged, 32))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::sprayPump1RunningChanged, 33))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::makeupValveOpenChanged, 34))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::drainValveOpenChanged, 35))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::tempSetpointChanged, 36))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::pressSetpointChanged, 37))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::coolingSetpointChanged, 38))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::tempProportionalChanged, 39))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::tempIntegralChanged, 40))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::pressProportionalChanged, 41))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::pressIntegralChanged, 42))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::tempAlarmL1Changed, 43))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::tempAlarmL2Changed, 44))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::pressLowAlarmChanged, 45))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::conductivityLimitChanged, 46))
            return;
        if (QtMocHelpers::indexOfMethod<void (PumpBackend::*)()>(_a, &PumpBackend::saveMessageChanged, 47))
            return;
    }
    if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 49:
        case 48:
        case 47:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractItemModel* >(); break;
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<double*>(_v) = _t->supplyTemp(); break;
        case 1: *reinterpret_cast<double*>(_v) = _t->returnTemp(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->coolingTowerOutlet(); break;
        case 3: *reinterpret_cast<double*>(_v) = _t->exchangerIn(); break;
        case 4: *reinterpret_cast<double*>(_v) = _t->exchangerOut(); break;
        case 5: *reinterpret_cast<double*>(_v) = _t->supplyPressure(); break;
        case 6: *reinterpret_cast<double*>(_v) = _t->returnPressure(); break;
        case 7: *reinterpret_cast<double*>(_v) = _t->makeupPressure(); break;
        case 8: *reinterpret_cast<double*>(_v) = _t->supplyFlow(); break;
        case 9: *reinterpret_cast<double*>(_v) = _t->cumulativeFlow(); break;
        case 10: *reinterpret_cast<double*>(_v) = _t->conductivity(); break;
        case 11: *reinterpret_cast<double*>(_v) = _t->tankLevel(); break;
        case 12: *reinterpret_cast<double*>(_v) = _t->expansionLevel(); break;
        case 13: *reinterpret_cast<double*>(_v) = _t->valvePos1(); break;
        case 14: *reinterpret_cast<double*>(_v) = _t->valvePos2(); break;
        case 15: *reinterpret_cast<double*>(_v) = _t->supplyPumpFreq(); break;
        case 16: *reinterpret_cast<double*>(_v) = _t->coolingFanFreq(); break;
        case 17: *reinterpret_cast<double*>(_v) = _t->cyclePump1Current(); break;
        case 18: *reinterpret_cast<double*>(_v) = _t->cyclePump2Current(); break;
        case 19: *reinterpret_cast<double*>(_v) = _t->makeupPump1Current(); break;
        case 20: *reinterpret_cast<double*>(_v) = _t->makeupPump2Current(); break;
        case 21: *reinterpret_cast<bool*>(_v) = _t->conductivityAlarm(); break;
        case 22: *reinterpret_cast<double*>(_v) = _t->cop(); break;
        case 23: *reinterpret_cast<double*>(_v) = _t->wetBulbTemp(); break;
        case 24: *reinterpret_cast<double*>(_v) = _t->dailyPower(); break;
        case 25: *reinterpret_cast<double*>(_v) = _t->pumpRunHours(); break;
        case 26: *reinterpret_cast<bool*>(_v) = _t->cyclePump1Running(); break;
        case 27: *reinterpret_cast<bool*>(_v) = _t->cyclePump2Running(); break;
        case 28: *reinterpret_cast<bool*>(_v) = _t->cyclePump3Running(); break;
        case 29: *reinterpret_cast<bool*>(_v) = _t->coolingFan1Running(); break;
        case 30: *reinterpret_cast<bool*>(_v) = _t->coolingFan2Running(); break;
        case 31: *reinterpret_cast<bool*>(_v) = _t->makeupPump1Running(); break;
        case 32: *reinterpret_cast<bool*>(_v) = _t->makeupPump2Running(); break;
        case 33: *reinterpret_cast<bool*>(_v) = _t->sprayPump1Running(); break;
        case 34: *reinterpret_cast<bool*>(_v) = _t->makeupValveOpen(); break;
        case 35: *reinterpret_cast<bool*>(_v) = _t->drainValveOpen(); break;
        case 36: *reinterpret_cast<double*>(_v) = _t->tempSetpoint(); break;
        case 37: *reinterpret_cast<double*>(_v) = _t->pressSetpoint(); break;
        case 38: *reinterpret_cast<double*>(_v) = _t->coolingSetpoint(); break;
        case 39: *reinterpret_cast<double*>(_v) = _t->tempProportional(); break;
        case 40: *reinterpret_cast<double*>(_v) = _t->tempIntegral(); break;
        case 41: *reinterpret_cast<double*>(_v) = _t->pressProportional(); break;
        case 42: *reinterpret_cast<double*>(_v) = _t->pressIntegral(); break;
        case 43: *reinterpret_cast<double*>(_v) = _t->tempAlarmL1(); break;
        case 44: *reinterpret_cast<double*>(_v) = _t->tempAlarmL2(); break;
        case 45: *reinterpret_cast<double*>(_v) = _t->pressLowAlarm(); break;
        case 46: *reinterpret_cast<double*>(_v) = _t->conductivityLimit(); break;
        case 47: *reinterpret_cast<QAbstractItemModel**>(_v) = _t->devices(); break;
        case 48: *reinterpret_cast<QAbstractItemModel**>(_v) = _t->reports(); break;
        case 49: *reinterpret_cast<QAbstractItemModel**>(_v) = _t->alarms(); break;
        case 50: *reinterpret_cast<QString*>(_v) = _t->saveMessage(); break;
        case 51: *reinterpret_cast<QString*>(_v) = _t->dbPath(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 36: _t->setTempSetpoint(*reinterpret_cast<double*>(_v)); break;
        case 37: _t->setPressSetpoint(*reinterpret_cast<double*>(_v)); break;
        case 38: _t->setCoolingSetpoint(*reinterpret_cast<double*>(_v)); break;
        case 39: _t->setTempProportional(*reinterpret_cast<double*>(_v)); break;
        case 40: _t->setTempIntegral(*reinterpret_cast<double*>(_v)); break;
        case 41: _t->setPressProportional(*reinterpret_cast<double*>(_v)); break;
        case 42: _t->setPressIntegral(*reinterpret_cast<double*>(_v)); break;
        case 43: _t->setTempAlarmL1(*reinterpret_cast<double*>(_v)); break;
        case 44: _t->setTempAlarmL2(*reinterpret_cast<double*>(_v)); break;
        case 45: _t->setPressLowAlarm(*reinterpret_cast<double*>(_v)); break;
        case 46: _t->setConductivityLimit(*reinterpret_cast<double*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *PumpBackend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PumpBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11PumpBackendE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PumpBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 53)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 53;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 53)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 53;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 52;
    }
    return _id;
}

// SIGNAL 0
void PumpBackend::supplyTempChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PumpBackend::returnTempChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PumpBackend::coolingTowerOutletChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PumpBackend::exchangerInChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void PumpBackend::exchangerOutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PumpBackend::supplyPressureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void PumpBackend::returnPressureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void PumpBackend::makeupPressureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void PumpBackend::supplyFlowChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void PumpBackend::cumulativeFlowChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void PumpBackend::conductivityChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void PumpBackend::tankLevelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void PumpBackend::expansionLevelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void PumpBackend::valvePos1Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void PumpBackend::valvePos2Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void PumpBackend::supplyPumpFreqChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void PumpBackend::coolingFanFreqChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void PumpBackend::cyclePump1CurrentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void PumpBackend::cyclePump2CurrentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void PumpBackend::makeupPump1CurrentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void PumpBackend::makeupPump2CurrentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 20, nullptr);
}

// SIGNAL 21
void PumpBackend::conductivityAlarmChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 21, nullptr);
}

// SIGNAL 22
void PumpBackend::copChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 22, nullptr);
}

// SIGNAL 23
void PumpBackend::wetBulbTempChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 23, nullptr);
}

// SIGNAL 24
void PumpBackend::dailyPowerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 24, nullptr);
}

// SIGNAL 25
void PumpBackend::pumpRunHoursChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 25, nullptr);
}

// SIGNAL 26
void PumpBackend::cyclePump1RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 26, nullptr);
}

// SIGNAL 27
void PumpBackend::cyclePump2RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 27, nullptr);
}

// SIGNAL 28
void PumpBackend::cyclePump3RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 28, nullptr);
}

// SIGNAL 29
void PumpBackend::coolingFan1RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 29, nullptr);
}

// SIGNAL 30
void PumpBackend::coolingFan2RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 30, nullptr);
}

// SIGNAL 31
void PumpBackend::makeupPump1RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 31, nullptr);
}

// SIGNAL 32
void PumpBackend::makeupPump2RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 32, nullptr);
}

// SIGNAL 33
void PumpBackend::sprayPump1RunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 33, nullptr);
}

// SIGNAL 34
void PumpBackend::makeupValveOpenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 34, nullptr);
}

// SIGNAL 35
void PumpBackend::drainValveOpenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 35, nullptr);
}

// SIGNAL 36
void PumpBackend::tempSetpointChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 36, nullptr);
}

// SIGNAL 37
void PumpBackend::pressSetpointChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 37, nullptr);
}

// SIGNAL 38
void PumpBackend::coolingSetpointChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 38, nullptr);
}

// SIGNAL 39
void PumpBackend::tempProportionalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 39, nullptr);
}

// SIGNAL 40
void PumpBackend::tempIntegralChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 40, nullptr);
}

// SIGNAL 41
void PumpBackend::pressProportionalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 41, nullptr);
}

// SIGNAL 42
void PumpBackend::pressIntegralChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 42, nullptr);
}

// SIGNAL 43
void PumpBackend::tempAlarmL1Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 43, nullptr);
}

// SIGNAL 44
void PumpBackend::tempAlarmL2Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 44, nullptr);
}

// SIGNAL 45
void PumpBackend::pressLowAlarmChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 45, nullptr);
}

// SIGNAL 46
void PumpBackend::conductivityLimitChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 46, nullptr);
}

// SIGNAL 47
void PumpBackend::saveMessageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 47, nullptr);
}
QT_WARNING_POP
