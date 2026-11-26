/****************************************************************************
** Meta object code from reading C++ file 'reportmodel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/backend/reportmodel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'reportmodel.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11ReportModelE_t {};
} // unnamed namespace

template <> constexpr inline auto ReportModel::qt_create_metaobjectdata<qt_meta_tag_ZN11ReportModelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ReportModel",
        "headerChanged",
        "",
        "selectType",
        "type",
        "exportCsv",
        "headers",
        "columnCount",
        "title",
        "count",
        "trendAxis",
        "trendLabels",
        "trendValues",
        "QVariantList"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'headerChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'selectType'
        QtMocHelpers::MethodData<void(int)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Method 'exportCsv'
        QtMocHelpers::MethodData<QString() const>(5, 2, QMC::AccessPublic, QMetaType::QString),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'headers'
        QtMocHelpers::PropertyData<QStringList>(6, QMetaType::QStringList, QMC::DefaultPropertyFlags, 0),
        // property 'columnCount'
        QtMocHelpers::PropertyData<int>(7, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'title'
        QtMocHelpers::PropertyData<QString>(8, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'count'
        QtMocHelpers::PropertyData<int>(9, QMetaType::Int, QMC::DefaultPropertyFlags, 0),
        // property 'trendAxis'
        QtMocHelpers::PropertyData<QString>(10, QMetaType::QString, QMC::DefaultPropertyFlags, 0),
        // property 'trendLabels'
        QtMocHelpers::PropertyData<QStringList>(11, QMetaType::QStringList, QMC::DefaultPropertyFlags, 0),
        // property 'trendValues'
        QtMocHelpers::PropertyData<QVariantList>(12, 0x80000000 | 13, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ReportModel, qt_meta_tag_ZN11ReportModelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ReportModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11ReportModelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11ReportModelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11ReportModelE_t>.metaTypes,
    nullptr
} };

void ReportModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ReportModel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->headerChanged(); break;
        case 1: _t->selectType((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: { QString _r = _t->exportCsv();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ReportModel::*)()>(_a, &ReportModel::headerChanged, 0))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QStringList*>(_v) = _t->headers(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->columnCount(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->title(); break;
        case 3: *reinterpret_cast<int*>(_v) = _t->count(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->trendAxis(); break;
        case 5: *reinterpret_cast<QStringList*>(_v) = _t->trendLabels(); break;
        case 6: *reinterpret_cast<QVariantList*>(_v) = _t->trendValues(); break;
        default: break;
        }
    }
}

const QMetaObject *ReportModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ReportModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11ReportModelE_t>.strings))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int ReportModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ReportModel::headerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
