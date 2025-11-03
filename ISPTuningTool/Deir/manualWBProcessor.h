// ManualWBProcessor.h
#ifndef MANUALWBPROCESSOR_H
#define MANUALWBPROCESSOR_H

#include <QVector>
#include <QSize>
#include <QDebug>
#include <cmath>
#include "deir_common.h"
class ManualWBProcessor
{
public:
    // ROI选择结果结构体
    struct ROIResult {
        QVector<qreal> roi_x;
        QVector<qreal> roi_y;
        QVector<double> BMs;
        QVector<double> GMs;
        QVector<double> RMs;
        QVector<double> NMs;
        
        ROIResult() = default;
        
        // 转换为qreal类型的ROI坐标（用于manualWB_RGBIR_noIR）
        QVector<qreal> toQRealROI() const {
            QVector<qreal> result;
            for (int i = 0; i < roi_x.size(); ++i) {
                result.append(static_cast<qreal>(roi_x[i]));
            }
            return result;
        }
    };

    // 白平衡结果结构体
    struct WBResult {
        QVector<QVector<double>> imdst;
        QVector<int> wbGain;
        int ct;
        QVector<double> BMs;
        QVector<double> GMs;
        QVector<double> RMs;
        QVector<double> NMs;
        
        WBResult() : ct(0) {}
    };

    // 构造函数
    ManualWBProcessor();
    
    // 设置图像参数
    void setImageParams(int bitwidth, double blcTarget);
    
    // 设置CFA模式
    void setCFAPattern(const QVector<QVector<int>>& pattern);
    
    // 设置过曝阈值
    void setOverexposureThreshold(double threshold);
    
    // 设置ROI尺寸
    void setROISize(int width, int height);
    void setROIVector(const QVector<qreal>& roi_x, const QVector<qreal>& roi_y);
    void setROIMap(const std::map<QString, Coordinates> map) {coordinatesMap_ = map;}
    void setBitWidth(int bitwidth){m_bitwidth = bitwidth;};
    void setBlcTarget(int blcTarget){m_blcTarget = blcTarget;};
    void setOverexp(int overexp){m_overexp = overexp;};
    void setRoiWidth(int roiWidth){m_roiWidth = roiWidth;};
    void setRoiHeight(int roiHeight){m_roiHeight = roiHeight;};

    
    // 主要处理函数 - 考虑红外通道和过曝检测
    ROIResult manualWB_RGBIR_coIR(const QVector<QVector<double>>& imsrc, const QString& file_name);
    
    // 主要处理函数 - 不考虑红外通道
    WBResult manualWB_RGBIR_noIR(const QVector<QVector<double>>& imsrc, 
                                const QVector<qreal>& roi_x, 
                                const QVector<qreal>& roi_y);
    
    // 获取处理状态
    bool hasValidROI() const { return m_hasValidROI; }
    bool hasValidWB() const { return m_hasValidWB; }
    
    // 获取最近的处理结果
    ROIResult getLastROIResult() const { return m_lastROIResult; }
    WBResult getLastWBResult() const { return m_lastWBResult; }
private:
    // 私有辅助函数
    QVector<QVector<bool>> createMask(const QVector<QVector<int>>& pattern, int targetValue, int h, int w) const;
    QVector<QVector<bool>> createMaskExact(const QVector<QVector<int>>& pattern, 
                                                            int targetValue, 
                                                            int h, int w, 
                                                            const QSize& ptnSZ);
    double calculateSum(const QVector<QVector<double>>& roi, const QVector<QVector<bool>>& mask) const;
    int calculateCount(const QVector<QVector<bool>>& mask) const;
    double calculateMean(const QVector<QVector<double>>& roi, const QVector<QVector<bool>>& mask) const;
    QVector<QVector<double>> applyWBGain(const QVector<QVector<double>>& imsrc,
                                        const QVector<int>& wbGain,
                                        double blcTarget,
                                        double maxval) const;
    QVector<QVector<double>> applyWBGainExact(const QVector<QVector<double>>& imsrc,
                                                                const QVector<int>& wbGain,
                                                                double blcTarget,
                                                                double maxval,
                                                                const QSize& ptnSZ);

    double calculateROINMean(const QVector<QVector<double>>& roi, 
                                            const QVector<QVector<bool>>& maskN);    
    // 模拟Generate24Rois函数（需要根据实际实现补充）
    QVector<QVector<int>> generate24Rois(const QVector<QVector<double>>& imsrc, int roiH, int roiW) const;
    
    // 成员变量
    int m_bitwidth;
    double m_blcTarget;
    double m_overexp;
    int m_roiWidth;
    int m_roiHeight;
    QVector<QVector<int>> m_CFAPattern;
    
    // 状态标志
    bool m_hasValidROI;
    bool m_hasValidWB;
    
    // 最近处理结果
    ROIResult m_lastROIResult;
    WBResult m_lastWBResult;
    QVector<qreal> roi_x_;  // 存储所有矩形的x1和x2坐标
    QVector<qreal> roi_y_;  // 存储所有矩形的y1和y2坐标
    std::map<QString, Coordinates> coordinatesMap_;
};

#endif // MANUALWBPROCESSOR_H