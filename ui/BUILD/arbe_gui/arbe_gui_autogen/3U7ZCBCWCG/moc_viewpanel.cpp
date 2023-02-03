/****************************************************************************
** Meta object code from reading C++ file 'viewpanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/arbe_gui/src/arbe_gui_main/viewpanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewpanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_viewpanel_t {
    QByteArrayData data[73];
    char stringdata0[1022];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_viewpanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_viewpanel_t qt_meta_stringdata_viewpanel = {
    {
QT_MOC_LITERAL(0, 0, 9), // "viewpanel"
QT_MOC_LITERAL(1, 10, 12), // "startControl"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 14), // "connectControl"
QT_MOC_LITERAL(4, 39, 11), // "configPower"
QT_MOC_LITERAL(5, 51, 10), // "configCFAR"
QT_MOC_LITERAL(6, 62, 10), // "config3DFT"
QT_MOC_LITERAL(7, 73, 10), // "configDiff"
QT_MOC_LITERAL(8, 84, 9), // "configReg"
QT_MOC_LITERAL(9, 94, 9), // "readPower"
QT_MOC_LITERAL(10, 104, 8), // "readCFAR"
QT_MOC_LITERAL(11, 113, 8), // "read3DFT"
QT_MOC_LITERAL(12, 122, 8), // "readDiff"
QT_MOC_LITERAL(13, 131, 7), // "readReg"
QT_MOC_LITERAL(14, 139, 15), // "ctrlShowWindows"
QT_MOC_LITERAL(15, 155, 11), // "ctrlPcPanel"
QT_MOC_LITERAL(16, 167, 15), // "setLoadFileType"
QT_MOC_LITERAL(17, 183, 13), // "screen_record"
QT_MOC_LITERAL(18, 197, 13), // "setMinDoppler"
QT_MOC_LITERAL(19, 211, 24), // "min_doppler_slider_value"
QT_MOC_LITERAL(20, 236, 13), // "setMaxDoppler"
QT_MOC_LITERAL(21, 250, 24), // "max_doppler_slider_value"
QT_MOC_LITERAL(22, 275, 17), // "setMinColorCoding"
QT_MOC_LITERAL(23, 293, 16), // "min_slider_value"
QT_MOC_LITERAL(24, 310, 17), // "setMaxColorCoding"
QT_MOC_LITERAL(25, 328, 16), // "max_slider_value"
QT_MOC_LITERAL(26, 345, 12), // "setMinHeight"
QT_MOC_LITERAL(27, 358, 23), // "min_height_slider_value"
QT_MOC_LITERAL(28, 382, 12), // "setMaxHeight"
QT_MOC_LITERAL(29, 395, 23), // "max_height_slider_value"
QT_MOC_LITERAL(30, 419, 17), // "recording_control"
QT_MOC_LITERAL(31, 437, 10), // "closeEvent"
QT_MOC_LITERAL(32, 448, 12), // "QCloseEvent*"
QT_MOC_LITERAL(33, 461, 5), // "event"
QT_MOC_LITERAL(34, 467, 15), // "start_save_task"
QT_MOC_LITERAL(35, 483, 13), // "saveDataThead"
QT_MOC_LITERAL(36, 497, 15), // "showSpeedometer"
QT_MOC_LITERAL(37, 513, 12), // "QGridLayout*"
QT_MOC_LITERAL(38, 526, 6), // "layout"
QT_MOC_LITERAL(39, 533, 12), // "showTurnRate"
QT_MOC_LITERAL(40, 546, 13), // "loadLidarFile"
QT_MOC_LITERAL(41, 560, 13), // "setSaveFolder"
QT_MOC_LITERAL(42, 574, 10), // "udpConnect"
QT_MOC_LITERAL(43, 585, 12), // "udpPcConnect"
QT_MOC_LITERAL(44, 598, 14), // "startPcUdpOnce"
QT_MOC_LITERAL(45, 613, 20), // "startPcUdpContinuous"
QT_MOC_LITERAL(46, 634, 8), // "udpClose"
QT_MOC_LITERAL(47, 643, 10), // "udpPcClose"
QT_MOC_LITERAL(48, 654, 14), // "configAxesSize"
QT_MOC_LITERAL(49, 669, 14), // "configCellSize"
QT_MOC_LITERAL(50, 684, 15), // "configPointSize"
QT_MOC_LITERAL(51, 700, 8), // "pcShowBW"
QT_MOC_LITERAL(52, 709, 8), // "pcRecord"
QT_MOC_LITERAL(53, 718, 13), // "updateFFTdata"
QT_MOC_LITERAL(54, 732, 13), // "updateADCdata"
QT_MOC_LITERAL(55, 746, 9), // "showdBFFT"
QT_MOC_LITERAL(56, 756, 9), // "singleFFT"
QT_MOC_LITERAL(57, 766, 9), // "singleADC"
QT_MOC_LITERAL(58, 776, 8), // "resetFFT"
QT_MOC_LITERAL(59, 785, 8), // "resetADC"
QT_MOC_LITERAL(60, 794, 11), // "updateState"
QT_MOC_LITERAL(61, 806, 13), // "printErrorLog"
QT_MOC_LITERAL(62, 820, 18), // "slotTextTcpChanged"
QT_MOC_LITERAL(63, 839, 14), // "recvSerialInfo"
QT_MOC_LITERAL(64, 854, 18), // "recvSerialInfoTest"
QT_MOC_LITERAL(65, 873, 19), // "sendMotorConnectCmd"
QT_MOC_LITERAL(66, 893, 20), // "sendMotorWorkModeCmd"
QT_MOC_LITERAL(67, 914, 24), // "sendMotorDisplayCycleCmd"
QT_MOC_LITERAL(68, 939, 16), // "sendMotorOpenCmd"
QT_MOC_LITERAL(69, 956, 15), // "sendMotorPidCmd"
QT_MOC_LITERAL(70, 972, 12), // "readMotorPid"
QT_MOC_LITERAL(71, 985, 17), // "readMotorWorkMode"
QT_MOC_LITERAL(72, 1003, 18) // "readMotorShowItems"

    },
    "viewpanel\0startControl\0\0connectControl\0"
    "configPower\0configCFAR\0config3DFT\0"
    "configDiff\0configReg\0readPower\0readCFAR\0"
    "read3DFT\0readDiff\0readReg\0ctrlShowWindows\0"
    "ctrlPcPanel\0setLoadFileType\0screen_record\0"
    "setMinDoppler\0min_doppler_slider_value\0"
    "setMaxDoppler\0max_doppler_slider_value\0"
    "setMinColorCoding\0min_slider_value\0"
    "setMaxColorCoding\0max_slider_value\0"
    "setMinHeight\0min_height_slider_value\0"
    "setMaxHeight\0max_height_slider_value\0"
    "recording_control\0closeEvent\0QCloseEvent*\0"
    "event\0start_save_task\0saveDataThead\0"
    "showSpeedometer\0QGridLayout*\0layout\0"
    "showTurnRate\0loadLidarFile\0setSaveFolder\0"
    "udpConnect\0udpPcConnect\0startPcUdpOnce\0"
    "startPcUdpContinuous\0udpClose\0udpPcClose\0"
    "configAxesSize\0configCellSize\0"
    "configPointSize\0pcShowBW\0pcRecord\0"
    "updateFFTdata\0updateADCdata\0showdBFFT\0"
    "singleFFT\0singleADC\0resetFFT\0resetADC\0"
    "updateState\0printErrorLog\0slotTextTcpChanged\0"
    "recvSerialInfo\0recvSerialInfoTest\0"
    "sendMotorConnectCmd\0sendMotorWorkModeCmd\0"
    "sendMotorDisplayCycleCmd\0sendMotorOpenCmd\0"
    "sendMotorPidCmd\0readMotorPid\0"
    "readMotorWorkMode\0readMotorShowItems"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_viewpanel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      61,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  319,    2, 0x0a /* Public */,
       3,    0,  320,    2, 0x0a /* Public */,
       4,    0,  321,    2, 0x0a /* Public */,
       5,    0,  322,    2, 0x0a /* Public */,
       6,    0,  323,    2, 0x0a /* Public */,
       7,    0,  324,    2, 0x0a /* Public */,
       8,    1,  325,    2, 0x0a /* Public */,
       9,    0,  328,    2, 0x0a /* Public */,
      10,    0,  329,    2, 0x0a /* Public */,
      11,    0,  330,    2, 0x0a /* Public */,
      12,    0,  331,    2, 0x0a /* Public */,
      13,    1,  332,    2, 0x0a /* Public */,
      14,    1,  335,    2, 0x0a /* Public */,
      15,    1,  338,    2, 0x0a /* Public */,
      16,    0,  341,    2, 0x08 /* Private */,
      17,    0,  342,    2, 0x08 /* Private */,
      18,    1,  343,    2, 0x08 /* Private */,
      20,    1,  346,    2, 0x08 /* Private */,
      22,    1,  349,    2, 0x08 /* Private */,
      24,    1,  352,    2, 0x08 /* Private */,
      26,    1,  355,    2, 0x08 /* Private */,
      28,    1,  358,    2, 0x08 /* Private */,
      30,    0,  361,    2, 0x08 /* Private */,
      31,    1,  362,    2, 0x08 /* Private */,
      34,    0,  365,    2, 0x08 /* Private */,
      35,    0,  366,    2, 0x08 /* Private */,
      36,    1,  367,    2, 0x08 /* Private */,
      39,    1,  370,    2, 0x08 /* Private */,
      40,    0,  373,    2, 0x08 /* Private */,
      41,    0,  374,    2, 0x08 /* Private */,
      42,    0,  375,    2, 0x08 /* Private */,
      43,    0,  376,    2, 0x08 /* Private */,
      44,    0,  377,    2, 0x08 /* Private */,
      45,    0,  378,    2, 0x08 /* Private */,
      46,    0,  379,    2, 0x08 /* Private */,
      47,    0,  380,    2, 0x08 /* Private */,
      48,    0,  381,    2, 0x08 /* Private */,
      49,    0,  382,    2, 0x08 /* Private */,
      50,    0,  383,    2, 0x08 /* Private */,
      51,    0,  384,    2, 0x08 /* Private */,
      52,    0,  385,    2, 0x08 /* Private */,
      53,    0,  386,    2, 0x08 /* Private */,
      54,    0,  387,    2, 0x08 /* Private */,
      55,    0,  388,    2, 0x08 /* Private */,
      56,    0,  389,    2, 0x08 /* Private */,
      57,    0,  390,    2, 0x08 /* Private */,
      58,    0,  391,    2, 0x08 /* Private */,
      59,    0,  392,    2, 0x08 /* Private */,
      60,    0,  393,    2, 0x08 /* Private */,
      61,    0,  394,    2, 0x08 /* Private */,
      62,    0,  395,    2, 0x08 /* Private */,
      63,    0,  396,    2, 0x08 /* Private */,
      64,    0,  397,    2, 0x08 /* Private */,
      65,    0,  398,    2, 0x08 /* Private */,
      66,    0,  399,    2, 0x08 /* Private */,
      67,    0,  400,    2, 0x08 /* Private */,
      68,    0,  401,    2, 0x08 /* Private */,
      69,    0,  402,    2, 0x08 /* Private */,
      70,    0,  403,    2, 0x08 /* Private */,
      71,    0,  404,    2, 0x08 /* Private */,
      72,    0,  405,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Int,   25,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void, QMetaType::Int,   29,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 32,   33,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 37,   38,
    QMetaType::Void, 0x80000000 | 37,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void viewpanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<viewpanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->startControl(); break;
        case 1: _t->connectControl(); break;
        case 2: _t->configPower(); break;
        case 3: _t->configCFAR(); break;
        case 4: _t->config3DFT(); break;
        case 5: _t->configDiff(); break;
        case 6: _t->configReg((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->readPower(); break;
        case 8: _t->readCFAR(); break;
        case 9: _t->read3DFT(); break;
        case 10: _t->readDiff(); break;
        case 11: _t->readReg((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->ctrlShowWindows((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->ctrlPcPanel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->setLoadFileType(); break;
        case 15: _t->screen_record(); break;
        case 16: _t->setMinDoppler((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->setMaxDoppler((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->setMinColorCoding((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->setMaxColorCoding((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->setMinHeight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->setMaxHeight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->recording_control(); break;
        case 23: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 24: _t->start_save_task(); break;
        case 25: _t->saveDataThead(); break;
        case 26: _t->showSpeedometer((*reinterpret_cast< QGridLayout*(*)>(_a[1]))); break;
        case 27: _t->showTurnRate((*reinterpret_cast< QGridLayout*(*)>(_a[1]))); break;
        case 28: _t->loadLidarFile(); break;
        case 29: _t->setSaveFolder(); break;
        case 30: _t->udpConnect(); break;
        case 31: _t->udpPcConnect(); break;
        case 32: _t->startPcUdpOnce(); break;
        case 33: _t->startPcUdpContinuous(); break;
        case 34: _t->udpClose(); break;
        case 35: _t->udpPcClose(); break;
        case 36: _t->configAxesSize(); break;
        case 37: _t->configCellSize(); break;
        case 38: _t->configPointSize(); break;
        case 39: _t->pcShowBW(); break;
        case 40: _t->pcRecord(); break;
        case 41: _t->updateFFTdata(); break;
        case 42: _t->updateADCdata(); break;
        case 43: _t->showdBFFT(); break;
        case 44: _t->singleFFT(); break;
        case 45: _t->singleADC(); break;
        case 46: _t->resetFFT(); break;
        case 47: _t->resetADC(); break;
        case 48: _t->updateState(); break;
        case 49: _t->printErrorLog(); break;
        case 50: _t->slotTextTcpChanged(); break;
        case 51: _t->recvSerialInfo(); break;
        case 52: _t->recvSerialInfoTest(); break;
        case 53: _t->sendMotorConnectCmd(); break;
        case 54: _t->sendMotorWorkModeCmd(); break;
        case 55: _t->sendMotorDisplayCycleCmd(); break;
        case 56: _t->sendMotorOpenCmd(); break;
        case 57: _t->sendMotorPidCmd(); break;
        case 58: _t->readMotorPid(); break;
        case 59: _t->readMotorWorkMode(); break;
        case 60: _t->readMotorShowItems(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 26:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGridLayout* >(); break;
            }
            break;
        case 27:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QGridLayout* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject viewpanel::staticMetaObject = { {
    &QTabWidget::staticMetaObject,
    qt_meta_stringdata_viewpanel.data,
    qt_meta_data_viewpanel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *viewpanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *viewpanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_viewpanel.stringdata0))
        return static_cast<void*>(this);
    return QTabWidget::qt_metacast(_clname);
}

int viewpanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
