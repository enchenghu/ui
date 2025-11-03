#ifndef QAECTABUICONFIG_H
#define QAECTABUICONFIG_H
#include <QMap>
#include <QString>
typedef QVector<QVector<QPair<QString, QString>>> ConfigType;

const QString TAB_NAME = "AEC";

const QString LABEL_EDIT_GROUP = "QLabelEditGroup";
const QString LABEL_CHECK_BOX  = "QLabelCheckBox";
const QString LABEL_COMBO_BOX  = "QLabelComboBox";

const QMap<QString, ConfigType> EXP_GAIN_CONFIG_MAP = {
    {
        "Exp Type",
        {
            {
                { LABEL_COMBO_BOX, "AECManEnable" },
            },
        },
    },
    {
        "Manual Expo",
        {
            {
                { LABEL_EDIT_GROUP, "Expo 0" },
                { LABEL_EDIT_GROUP, "Expo 1" },
                { LABEL_EDIT_GROUP, "Expo 2" },
                { LABEL_EDIT_GROUP, "Expo 3" },
            },
        },
    },
    {
        "Manual Gain",
        {
            {
                { LABEL_EDIT_GROUP, "Gain 0" },
                { LABEL_EDIT_GROUP, "Gain 1" },
                { LABEL_EDIT_GROUP, "Gain 2" },
                { LABEL_EDIT_GROUP, "Gain 3" },
            },
        },
    },
    {
        "Auto Expo",
        {
            {
                { LABEL_EDIT_GROUP, "MinExpo 0" },
                { LABEL_EDIT_GROUP, "MinExpo 1" },
                { LABEL_EDIT_GROUP, "MinExpo 2" },
                { LABEL_EDIT_GROUP, "MinExpo 3" },
            },
            {
                { LABEL_EDIT_GROUP, "MaxExpo 0" },
                { LABEL_EDIT_GROUP, "MaxExpo 1" },
                { LABEL_EDIT_GROUP, "MaxExpo 2" },
                { LABEL_EDIT_GROUP, "MaxExpo 3" },
            },
        },
    },
    {
        "Auto Gain",
        {
            {
                { LABEL_EDIT_GROUP, "MinGain 0" },
                { LABEL_EDIT_GROUP, "MinGain 1" },
                { LABEL_EDIT_GROUP, "MinGain 2" },
                { LABEL_EDIT_GROUP, "MinGain 3" },
            },
            {
                { LABEL_EDIT_GROUP, "MaxGain 0" },
                { LABEL_EDIT_GROUP, "MaxGain 1" },
                { LABEL_EDIT_GROUP, "MaxGain 2" },
                { LABEL_EDIT_GROUP, "MaxGain 3" },
            },
        },
    },
    {
        "Graded Expo",
        {
            {
                { LABEL_CHECK_BOX, "Enable" },
                { LABEL_EDIT_GROUP, "ExpoLimNode" },
                { LABEL_EDIT_GROUP, "GainLimNode" },
            },
        },
    },
    {
        "Band",
        {
            {
                { LABEL_CHECK_BOX, "BandEnable L" },
                { LABEL_CHECK_BOX, "BandEnable M" },
                { LABEL_CHECK_BOX, "OneBandLim L" },
                { LABEL_CHECK_BOX, "OneBandLim M" },
            },
        },
    },
    {
        "LFS",
        {
            {
                { LABEL_EDIT_GROUP, "PresetExp" },
            },
            {
                { LABEL_EDIT_GROUP, "PresetGain" },
            },
        },
    },
    {
        "Current Gain",
        {
            {
                { LABEL_EDIT_GROUP, "Gain L" },
                { LABEL_EDIT_GROUP, "Gain M" },
                { LABEL_EDIT_GROUP, "Gain S" },
                { LABEL_EDIT_GROUP, "Gain LFS" },
            },
        },
    },
    {
        "Current Expo",
        {
            {
                { LABEL_EDIT_GROUP, "Expo L" },
                { LABEL_EDIT_GROUP, "Expo M" },
                { LABEL_EDIT_GROUP, "Expo S" },
                { LABEL_EDIT_GROUP, "Expo LFS" },
            },
        },
    },
};
const QMap<QString, ConfigType> TARGET_SPEED_CONFIG_MAP = {
    {
        "Target",
        {
            {
                { LABEL_EDIT_GROUP, "TargetL 0" },
                { LABEL_EDIT_GROUP, "TargetL 1" },
                { LABEL_EDIT_GROUP, "TargetL 2" },
                { LABEL_EDIT_GROUP, "TargetL 3" },
                { LABEL_EDIT_GROUP, "TargetL 4" },
                { LABEL_EDIT_GROUP, "TargetL 5" },
                { LABEL_EDIT_GROUP, "TargetL 6" },
                { LABEL_EDIT_GROUP, "TargetL 7" },
                // { LABEL_EDIT_GROUP, "" },
                // { LABEL_EDIT_GROUP, "TargetL M" },
            },
            {
                { LABEL_EDIT_GROUP, "EVThreL 0" },
                { LABEL_EDIT_GROUP, "EVThreL 1" },
                { LABEL_EDIT_GROUP, "EVThreL 2" },
                { LABEL_EDIT_GROUP, "EVThreL 3" },
                { LABEL_EDIT_GROUP, "EVThreL 4" },
                { LABEL_EDIT_GROUP, "EVThreL 5" },
                { LABEL_EDIT_GROUP, "EVThreL 6" },
                { LABEL_EDIT_GROUP, "EVThreL 7" },
                { "", "" },
            },
        },
    },
    {
        "Speed",
        {
            {
                { LABEL_EDIT_GROUP, "FastStep 0" },
                { LABEL_EDIT_GROUP, "FastStep 1" },
                { LABEL_EDIT_GROUP, "SlowStep 0" },
                { LABEL_EDIT_GROUP, "SlowStep 1" },
                { LABEL_EDIT_GROUP, "PresetUp 0" },
                { LABEL_EDIT_GROUP, "PresetDown 0" },
                { LABEL_EDIT_GROUP, "PresetUp 1" },
                { LABEL_EDIT_GROUP, "PresetDown 1" },
                { LABEL_EDIT_GROUP, "StableRange 1-0" },
            },
            {
                { LABEL_EDIT_GROUP, "MaxFastRatio 0" },
                { LABEL_EDIT_GROUP, "MaxFastRatio 1" },
                { LABEL_EDIT_GROUP, "MaxSlowRatio 0" },
                { LABEL_EDIT_GROUP, "MaxSlowRatio 1" },
                { LABEL_EDIT_GROUP, "SlowRange 0" },
                { LABEL_EDIT_GROUP, "SlowRange 1" },
                { LABEL_EDIT_GROUP, "StableRange 0-0" },
                { LABEL_EDIT_GROUP, "StableRange 0-1" },
                { LABEL_EDIT_GROUP, "StableRange 1-1" },
            },
        },
    },
};
const QMap<QString, ConfigType> ROI_CONFIG_MAP = {
    {
        "AEC ROI",
        {
            {
                { LABEL_EDIT_GROUP, "Left" },
                { LABEL_EDIT_GROUP, "Right" },
                { LABEL_EDIT_GROUP, "Top" },
                { LABEL_EDIT_GROUP, "Bottom" },
            },
        },
    },
    {
        "ROI Block",
        {
            {
                { LABEL_EDIT_GROUP, "roi_width" },
                { LABEL_EDIT_GROUP, "roi_height" },
                { LABEL_EDIT_GROUP, "sclh" },
                { LABEL_EDIT_GROUP, "sclv" },
            },
        },
    },
    {
        "roi_weight",
        {
            {
                { LABEL_CHECK_BOX, "roi_weight_en" },
            },
        },
    },
};

const QMap<QPair<QString, QString>, bool> RGBIR_ENABLE_MAP = {
    { {TAB_NAME, "TargetL 0"}, true },
    { {TAB_NAME, "TargetL 1"}, true },
    { {TAB_NAME, "TargetL 2"}, true },
    { {TAB_NAME, "TargetL 3"}, true },
    { {TAB_NAME, "TargetL 4"}, true },
    { {TAB_NAME, "TargetL 5"}, true },
    { {TAB_NAME, "TargetL 6"}, true },
    { {TAB_NAME, "TargetL 7"}, true },
    { {TAB_NAME, "EVThreL 0"}, true },
    { {TAB_NAME, "EVThreL 1"}, true },
    { {TAB_NAME, "EVThreL 2"}, true },
    { {TAB_NAME, "EVThreL 3"}, true },
    { {TAB_NAME, "EVThreL 4"}, true },
    { {TAB_NAME, "EVThreL 5"}, true },
    { {TAB_NAME, "EVThreL 6"}, true },
    { {TAB_NAME, "EVThreL 7"}, true },
    { {TAB_NAME, "MinExpo 0"}, true },
    { {TAB_NAME, "MaxExpo 0"}, true },
    { {TAB_NAME, "MinGain 0"}, true },
    { {TAB_NAME, "MaxGain 0"}, true }
};

#endif // QAECTABUICONFIG_H
