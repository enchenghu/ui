// ManualWBProcessor.cpp
#include "ManualWBProcessor.h"
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QDebug>

bool saveToCSV(const QVector<QVector<double>>& data, const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << filename;
        return false;
    }
    
    QTextStream out(&file);
    
    for (int row = 0; row < data.size(); ++row) {
        const QVector<double>& currentRow = data[row];
        for (int col = 0; col < currentRow.size(); ++col) {
            out << currentRow[col];
            if (col < currentRow.size() - 1) {
                out << ",";  // 列分隔符
            }
        }
        out << "\n";  // 行分隔符
    }
    
    file.close();
    qDebug() << "CSV文件保存成功:" << filename;
    return true;
}

ManualWBProcessor::ManualWBProcessor()
    : m_bitwidth(8)
    , m_blcTarget(16)
    , m_overexp(225.0)
    , m_roiWidth(48)
    , m_roiHeight(48)
    , m_hasValidROI(false)
    , m_hasValidWB(false)
{
    // 默认CFA模式
    m_CFAPattern = {
        {0, 1, 2, 1},
        {1, 3, 1, 3},
        {2, 1, 0, 1},
        {1, 3, 1, 3}
    };
    roi_x_.clear();
    roi_y_.clear();
    coordinatesMap_.clear();
    // QVector<qreal> data_x = {
    //     937, 984, 1073, 1120, 1213, 1260, 1353, 1400, 1493, 1540, 1633, 1680,
    //     933, 980, 1073, 1120, 1213, 1260, 1357, 1404, 1497, 1544, 1637, 1684,
    //     929, 976, 1069, 1116, 1213, 1260, 1357, 1404, 1501, 1548, 1645, 1692,
    //     925, 972, 1069, 1116, 1213, 1260, 1361, 1408, 1505, 1552, 1653, 1700
    // };
    // roi_x_ = data_x;
    // QVector<qreal> data_y = {
    //     593, 640, 589, 636, 585, 632, 581, 628, 577, 624, 573, 620,
    //     733, 780, 729, 776, 725, 772, 721, 768, 717, 764, 713, 760,
    //     873, 920, 869, 916, 865, 912, 865, 912, 861, 908, 857, 904,
    //     1017, 1064, 1013, 1060, 1009, 1056, 1005, 1052, 1001, 1048, 1001, 1048
    // };
    // roi_y_ = data_y;
}

void ManualWBProcessor::setROIVector(const QVector<qreal>& roi_x, const QVector<qreal>& roi_y)
{
    roi_x_ = roi_x;
    roi_y_ = roi_y;
}

void ManualWBProcessor::setImageParams(int bitwidth, double blcTarget)
{
    m_bitwidth = bitwidth;
    m_blcTarget = blcTarget;
}

void ManualWBProcessor::setCFAPattern(const QVector<QVector<int>>& pattern)
{
    m_CFAPattern = pattern;
}

void ManualWBProcessor::setOverexposureThreshold(double threshold)
{
    m_overexp = threshold;
}

void ManualWBProcessor::setROISize(int width, int height)
{
    m_roiWidth = width;
    m_roiHeight = height;
}

ManualWBProcessor::ROIResult ManualWBProcessor::manualWB_RGBIR_coIR(const QVector<QVector<double>>& imsrc, const QString& file_name)
{
    qDebug() << "imsrc height is " << imsrc.size() << ", width is " << imsrc[0].size();
    // if (saveToCSV(imsrc, "imsrc.csv")) {
    //     qDebug() << "文件保存成功";
    // }
    ROIResult result;
    m_hasValidROI = false;
    if(coordinatesMap_.empty()) return result;
    if(coordinatesMap_.find(file_name) == coordinatesMap_.end() ) {
        auto it  = coordinatesMap_.rbegin();
        qDebug() << "can't find roi in coordinatesMap_ for " << file_name << ", will use the last image roi: " << it->first;
        roi_x_ = it->second.xCoordinates;
        roi_y_ = it->second.yCoordinates;
    } else{
        roi_x_ = coordinatesMap_[file_name].xCoordinates;
        roi_y_ = coordinatesMap_[file_name].yCoordinates;
    }
    if (roi_x_.isEmpty() || roi_y_.isEmpty()) {
        qDebug() << "No valid ROIs generated!";
        return result;
    }
    int num = roi_x_.size() / 2;
    result.roi_x = roi_x_;
    result.roi_y = roi_y_;   
    QVector<int> reMask_xy(roi_x_.size(), 1);
    if (num == 0) {
        qDebug() << "Error in selecting ROI!";
        return result;
    } 
    QVector<int> remove_pos;
    QVector<int> reMask_rgbn(num, 1);
    QSize ptnSZ(m_CFAPattern[0].size(), m_CFAPattern.size());
    double maxval = std::pow(2, m_bitwidth) - 1;      
    // 创建图像副本并减去黑电平
    QVector<QVector<double>> imsrc_adj = imsrc;
    for (int i = 0; i < imsrc_adj.size(); ++i) {
        for (int j = 0; j < imsrc_adj[i].size(); ++j) {
            imsrc_adj[i][j] -= m_blcTarget;
        }
    }
    
    for (int i = 0; i < num; ++i) {
        int x1 = roi_x_[i * 2];
        int x2 = roi_x_[i * 2 + 1];
        int y1 = roi_y_[i * 2];
        int y2 = roi_y_[i * 2 + 1];
        
        // 修正：严格的边界检查
        if (x1 < 0 || x2 >= imsrc_adj[0].size() || y1 < 0 || y2 >= imsrc_adj.size() || x1 > x2 || y1 > y2) {
            remove_pos.append(i * 2);
            remove_pos.append(i * 2 + 1);
            // 填充默认值以避免数组越界
            result.BMs.append(0.0);
            result.GMs.append(0.0);
            result.RMs.append(0.0);
            result.NMs.append(0.0);
            continue;
        }
        
        // 提取ROI区域 - 修正：确保与MATLAB索引一致
        QVector<QVector<double>> roi;
        for (int row = y1 - 1; row < y2; ++row) { // MATLAB索引从1开始，C++从0开始
            QVector<double> roi_row;
            for (int col = x1 - 1; col < x2; ++col) {
                if (row >= 0 && row < imsrc_adj.size() && col >= 0 && col < imsrc_adj[row].size()) {
                    roi_row.append(imsrc_adj[row][col]);
                } else {
                    roi_row.append(0.0);
                }
            }
            roi.append(roi_row);
        }
        
        int h = roi.size();
        int w = roi[0].size();
        
        // 修正：与MATLAB完全一致的掩码创建
        auto maskB = createMaskExact(m_CFAPattern, 0, h, w, ptnSZ);
        auto maskG = createMaskExact(m_CFAPattern, 1, h, w, ptnSZ);
        auto maskR = createMaskExact(m_CFAPattern, 2, h, w, ptnSZ);
        auto maskN = createMaskExact(m_CFAPattern, 3, h, w, ptnSZ);
        
        // 计算各通道均值 - 修正：与MATLAB计算方式一致
        double sumB = 0.0, sumG = 0.0, sumR = 0.0, sumN = 0.0;
        int countB = 0, countG = 0, countR = 0, countN = 0;
        
        for (int row = 0; row < h; ++row) {
            for (int col = 0; col < w; ++col) {
                double pixel = roi[row][col];
                if (maskB[row][col]) {
                    sumB += pixel;
                    countB++;
                }
                if (maskG[row][col]) {
                    sumG += pixel;
                    countG++;
                }
                if (maskR[row][col]) {
                    sumR += pixel;
                    countR++;
                }
                if (maskN[row][col]) {
                    sumN += pixel;
                    countN++;
                }
            }
        }
        double blo_Bmean = (countB > 0) ? sumB / countB : 0.0;
        double blo_Gmean = (countG > 0) ? sumG / countG : 0.0;
        double blo_Rmean = (countR > 0) ? sumR / countR : 0.0;
        double blo_Nmean = (countN > 0) ? sumN / countN : 0.0;

        // 修正：过曝检测逻辑
        if (blo_Bmean > m_overexp || blo_Gmean > m_overexp || 
            blo_Rmean > m_overexp || blo_Nmean > m_overexp) {
            remove_pos.append(i * 2);
            remove_pos.append(i * 2 + 1);
        }

        result.BMs.append(blo_Bmean);
        result.GMs.append(blo_Gmean);
        result.RMs.append(blo_Rmean);
        result.NMs.append(blo_Nmean);
    }
    
    // 修正：数据过滤逻辑 - 与MATLAB一致
    if (!remove_pos.isEmpty()) {
        // 先对remove_pos排序去重
        std::sort(remove_pos.begin(), remove_pos.end());
        auto last = std::unique(remove_pos.begin(), remove_pos.end());
        remove_pos.erase(last, remove_pos.end());
        
        for (int pos : remove_pos) {
            reMask_xy[pos] = 0;
            if (pos % 2 == 0) { // 偶数位置对应ROI起始索引
                reMask_rgbn[pos / 2] = 0;
            }
        }
    }
    // 修正：数据过滤 - 创建新的过滤后数组
    QVector<double> filtered_BMs, filtered_GMs, filtered_RMs, filtered_NMs;
    QVector<qreal> filtered_roi_x, filtered_roi_y;
    
    for (int i = 0; i < reMask_rgbn.size(); ++i) {
        if (reMask_rgbn[i] == 1 && i < result.BMs.size()) {
            filtered_BMs.append(result.BMs[i]);
            filtered_GMs.append(result.GMs[i]);
            filtered_RMs.append(result.RMs[i]);
            filtered_NMs.append(result.NMs[i]);
        }
    }
    
    for (int i = 0; i < reMask_xy.size(); ++i) {
        if (reMask_xy[i] == 1 && i < result.roi_x.size()) {
            filtered_roi_x.append(result.roi_x[i]);
            filtered_roi_y.append(result.roi_y[i]);
        }
    }
    
    result.BMs = filtered_BMs;
    result.GMs = filtered_GMs;
    result.RMs = filtered_RMs;
    result.NMs = filtered_NMs;
    result.roi_x = filtered_roi_x;
    result.roi_y = filtered_roi_y;
    
    m_lastROIResult = result;
    m_hasValidROI = !result.BMs.isEmpty();

    qDebug() << "ROI processing completed. Valid ROIs:" << result.BMs.size();
    // qDebug() << "B means:" << result.BMs;
    // qDebug() << "G means:" << result.GMs;
    // qDebug() << "R means:" << result.RMs;
    // qDebug() << "N means:" << result.NMs;
    // qDebug() << "result.roi_x: " << result.roi_x;
    // qDebug() << "result.roi_y: " << result.roi_y;
    return result;
}
// 新增：与MATLAB repmat完全一致的掩码创建函数
QVector<QVector<bool>> ManualWBProcessor::createMaskExact(const QVector<QVector<int>>& pattern, 
                                                         int targetValue, 
                                                         int h, int w, 
                                                         const QSize& ptnSZ)
{
    QVector<QVector<bool>> mask(h, QVector<bool>(w, false));
    
    // MATLAB: repmat(CFAPattern==0, h/ptnSZ(1), w/ptnSZ(2))
    int repH = h / ptnSZ.height();
    int repW = w / ptnSZ.width();
    
    for (int repI = 0; repI < repH; ++repI) {
        for (int repJ = 0; repJ < repW; ++repJ) {
            for (int i = 0; i < ptnSZ.height(); ++i) {
                for (int j = 0; j < ptnSZ.width(); ++j) {
                    int row = repI * ptnSZ.height() + i;
                    int col = repJ * ptnSZ.width() + j;
                    if (row < h && col < w) {
                        mask[row][col] = (pattern[i][j] == targetValue);
                    }
                }
            }
        }
    }
    
    return mask;
}

// ManualWBProcessor.cpp - manualWB_RGBIR_noIR 修正版本
ManualWBProcessor::WBResult ManualWBProcessor::manualWB_RGBIR_noIR(const QVector<QVector<double>>& imsrc,
                                                                  const QVector<qreal>& roi_x,
                                                                  const QVector<qreal>& roi_y)
{
    qDebug() << "imsrc noir height is " << imsrc.size() << ", width is " << imsrc[0].size();
    // if (saveToCSV(imsrc, "imsrc_noir.csv")) {
    //     qDebug() << "文件保存成功";
    // }
    WBResult result;
    m_hasValidWB = false;
    
    QSize ptnSZ(m_CFAPattern[0].size(), m_CFAPattern.size());
    double maxval = std::pow(2, m_bitwidth) - 1;
    
    // 修正：直接使用输入的roi_x, roi_y，不进行转换
    QVector<qreal> x = roi_x;
    QVector<qreal> y = roi_y;
    int num = x.size() / 2;
    
    if (num == 0) {
        qDebug() << "Error in selecting ROI!";
        return result;
    }
    
    // 修正：与MATLAB完全一致的变量初始化
    double sum_B = 0.0;
    double sum_G = 0.0;
    double sum_R = 0.0;
    int NUM_B = 0;
    int NUM_G = 0;
    int NUM_R = 0;
    
    // 修正：创建图像副本并减去黑电平
    QVector<QVector<double>> imsrc_adj = imsrc;
    for (int i = 0; i < imsrc_adj.size(); ++i) {
        for (int j = 0; j < imsrc_adj[i].size(); ++j) {
            imsrc_adj[i][j] -= m_blcTarget;
        }
    }
    
    // 处理每个ROI - 完全按照MATLAB逻辑
    for (int i = 0; i < num; ++i) {
        int x1 = roi_x[i * 2];
        int x2 = roi_x[i * 2 + 1];
        int y1 = roi_y[i * 2];
        int y2 = roi_y[i * 2 + 1];
        
        // 修正：严格的边界检查
        if (x1 < 0 || x2 >= imsrc_adj[0].size() || y1 < 0 || y2 >= imsrc_adj.size() || x1 > x2 || y1 > y2) {
            result.BMs.append(0.0);
            result.GMs.append(0.0);
            result.RMs.append(0.0);
            result.NMs.append(0.0);
            continue;
        }
        
        //qDebug() << "ROI" << i << "coordinates - x1:" << x1 << "x2:" << x2 << "y1:" << y1 << "y2:" << y2; 
        // 修正：提取ROI区域 - MATLAB索引从1开始，需要减1
        QVector<QVector<double>> roi;
        for (int row = y1 - 1; row < y2; ++row) { // MATLAB索引从1开始，C++从0开始
            QVector<double> roi_row;
            for (int col = x1 - 1; col < x2; ++col) {
                if (row >= 0 && row < imsrc_adj.size() && col >= 0 && col < imsrc_adj[row].size()) {
                    roi_row.append(imsrc_adj[row][col]);
                } else {
                    roi_row.append(0.0);
                }
            }
            roi.append(roi_row);
        }
        
        int h = roi.size();
        int w = roi[0].size();
        
        //qDebug() << "ROI" << i << "size:" << h << "x" << w;
        
        // 修正：创建精确的掩码
        auto maskB = createMaskExact(m_CFAPattern, 0, h, w, ptnSZ);
        auto maskG = createMaskExact(m_CFAPattern, 1, h, w, ptnSZ);
        auto maskR = createMaskExact(m_CFAPattern, 2, h, w, ptnSZ);
        auto maskN = createMaskExact(m_CFAPattern, 3, h, w, ptnSZ);
        
        // 修正：计算各通道的和与像素数量 - 完全按照MATLAB逻辑
        double roi_sum_B = 0.0, roi_sum_G = 0.0, roi_sum_R = 0.0;
        int roi_num_B = 0, roi_num_G = 0, roi_num_R = 0, roi_num_N = 0;
        
        for (int row = 0; row < h; ++row) {
            for (int col = 0; col < w; ++col) {
                double pixel = roi[row][col];
                if (maskB[row][col]) {
                    roi_sum_B += pixel;
                    roi_num_B++;
                }
                if (maskG[row][col]) {
                    roi_sum_G += pixel;
                    roi_num_G++;
                }
                if (maskR[row][col]) {
                    roi_sum_R += pixel;
                    roi_num_R++;
                }
                if (maskN[row][col]) {
                    roi_num_N++; // 只计数，不累加（MATLAB中N通道只用于计算均值）
                }
            }
        }
        
        // 修正：累加到全局统计
        sum_B += roi_sum_B;
        sum_G += roi_sum_G;
        sum_R += roi_sum_R;
        NUM_B += roi_num_B;
        NUM_G += roi_num_G;
        NUM_R += roi_num_R;
        
        // 修正：计算当前ROI的各通道均值
        double blo_Bmean = (roi_num_B > 0) ? roi_sum_B / roi_num_B : 0.0;
        double blo_Gmean = (roi_num_G > 0) ? roi_sum_G / roi_num_G : 0.0;
        double blo_Rmean = (roi_num_R > 0) ? roi_sum_R / roi_num_R : 0.0;
        
        // 修正：计算N通道均值（需要重新计算N通道的和）
        double roi_sum_N = 0.0;
        for (int row = 0; row < h; ++row) {
            for (int col = 0; col < w; ++col) {
                if (maskN[row][col]) {
                    roi_sum_N += roi[row][col];
                }
            }
        }
        double blo_Nmean = (roi_num_N > 0) ? roi_sum_N / roi_num_N : 0.0;
        
        result.BMs.append(blo_Bmean);
        result.GMs.append(blo_Gmean);
        result.RMs.append(blo_Rmean);
        result.NMs.append(blo_Nmean);
        
        // qDebug() << "ROI" << i << "means - B:" << blo_Bmean << "G:" << blo_Gmean << "R:" << blo_Rmean << "N:" << blo_Nmean;
        // qDebug() << "ROI" << i << "counts - B:" << roi_num_B << "G:" << roi_num_G << "R:" << roi_num_R << "N:" << roi_num_N;
        // qDebug() << "ROI" << i << "sums - B:" << roi_sum_B << "G:" << roi_sum_G << "R:" << roi_sum_R;
    }
    
    // 修正：白平衡增益计算 - 完全按照MATLAB逻辑
    if (NUM_G > 0 && NUM_R > 0 && NUM_B > 0) {
        double mean_G = sum_G / NUM_G;
        double mean_R = sum_R / NUM_R;
        double mean_B = sum_B / NUM_B;
        
        // qDebug() << "Global statistics:";
        // qDebug() << "Total sums - B:" << sum_B << "G:" << sum_G << "R:" << sum_R;
        // qDebug() << "Total counts - B:" << NUM_B << "G:" << NUM_G << "R:" << NUM_R;
        // qDebug() << "Global means - B:" << mean_B << "G:" << mean_G << "R:" << mean_R;
        
        // MATLAB: R_gain = (sum_G/NUM_G) / (sum_R/NUM_R);
        double R_gain = mean_G / mean_R;
        double B_gain = mean_G / mean_B;
        
        //qDebug() << "Raw gains - B_gain:" << B_gain << "R_gain:" << R_gain;
        
        // MATLAB: wbGain = [ fix(B_gain * 1024), 1024, fix(R_gain * 1024)];
        result.wbGain = {
            static_cast<int>(std::floor(B_gain * 1024)),  // fix(B_gain * 1024)
            1024,
            static_cast<int>(std::floor(R_gain * 1024))   // fix(R_gain * 1024)
        };
        
        // MATLAB: ct = fix( (B_gain / R_gain)*1024 );
        result.ct = static_cast<int>(std::floor((B_gain / R_gain) * 1024));
        
        qDebug() << "Final WB gains - B:" << result.wbGain[0] << "G:" << result.wbGain[1] << "R:" << result.wbGain[2];
        qDebug() << "Color temperature:" << result.ct;
        
        // 修正：应用白平衡增益
        result.imdst = applyWBGainExact(imsrc_adj, result.wbGain, m_blcTarget, maxval, ptnSZ);
        m_hasValidWB = true;
    } else {
        qDebug() << "Insufficient data for WB calculation";
        qDebug() << "Counts - B:" << NUM_B << "G:" << NUM_G << "R:" << NUM_R;
    }
    // qDebug() << "noIR B means:" << result.BMs;
    // qDebug() << "noIR G means:" << result.GMs;
    // qDebug() << "noIR R means:" << result.RMs;    
    m_lastWBResult = result;
    return result;
}

// 新增：计算N通道均值的辅助函数
double ManualWBProcessor::calculateROINMean(const QVector<QVector<double>>& roi, 
                                           const QVector<QVector<bool>>& maskN)
{
    double sum = 0.0;
    int count = 0;
    
    for (int i = 0; i < roi.size(); ++i) {
        for (int j = 0; j < roi[i].size(); ++j) {
            if (maskN[i][j]) {
                sum += roi[i][j];
                count++;
            }
        }
    }
    
    return (count > 0) ? sum / count : 0.0;
}

// 修正：与MATLAB完全一致的白平衡增益应用
QVector<QVector<double>> ManualWBProcessor::applyWBGainExact(const QVector<QVector<double>>& imsrc,
                                                            const QVector<int>& wbGain,
                                                            double blcTarget,
                                                            double maxval,
                                                            const QSize& ptnSZ)
{
    int h = imsrc.size();
    int w = imsrc[0].size();
    
    // 修正：与MATLAB完全一致的ratio矩阵创建
    // MATLAB: ratio = zeros(size(CFAPattern));
    QVector<QVector<double>> ratio(ptnSZ.height(), QVector<double>(ptnSZ.width(), 0.0));
    
    // MATLAB: ratio(CFAPattern==0) = wbGain(1);
    // MATLAB: ratio(CFAPattern==1) = wbGain(2);
    // MATLAB: ratio(CFAPattern==2) = wbGain(3);
    for (int i = 0; i < ptnSZ.height(); ++i) {
        for (int j = 0; j < ptnSZ.width(); ++j) {
            int patternVal = m_CFAPattern[i][j];
            if (patternVal == 0) {
                ratio[i][j] = wbGain[0] / 1024.0;  // B通道，注意MATLAB是fix值，需要除以1024
            } else if (patternVal == 1) {
                ratio[i][j] = wbGain[1] / 1024.0;  // G通道
            } else if (patternVal == 2) {
                ratio[i][j] = wbGain[2] / 1024.0;  // R通道
            } else {
                ratio[i][j] = 1.0;  // N通道或其他，保持原样
            }
        }
    }
    
    // 修正：应用增益 - 与MATLAB的repmat完全一致
    // MATLAB: imdst = imsrc.*repmat(ratio, size(imsrc,1)/ptnSZ(1), size(imsrc,2)/ptnSZ(2));
    QVector<QVector<double>> imdst(h, QVector<double>(w, 0.0));
    
    int repH = h / ptnSZ.height();
    int repW = w / ptnSZ.width();
    
    for (int repI = 0; repI < repH; ++repI) {
        for (int repJ = 0; repJ < repW; ++repJ) {
            for (int i = 0; i < ptnSZ.height(); ++i) {
                for (int j = 0; j < ptnSZ.width(); ++j) {
                    int row = repI * ptnSZ.height() + i;
                    int col = repJ * ptnSZ.width() + j;
                    if (row < h && col < w) {
                        double gain = ratio[i][j];
                        imdst[row][col] = imsrc[row][col] * gain + blcTarget;
                        
                        // MATLAB: imdst(imdst<0) = 0; imdst(imdst>maxval) = maxval;
                        if (imdst[row][col] < 0) {
                            imdst[row][col] = 0;
                        } else if (imdst[row][col] > maxval) {
                            imdst[row][col] = maxval;
                        }
                    }
                }
            }
        }
    }
    
    return imdst;
}

QVector<QVector<bool>> ManualWBProcessor::createMask(const QVector<QVector<int>>& pattern,
                                                    int targetValue,
                                                    int h, int w) const
{
    int patternH = pattern.size();
    int patternW = pattern[0].size();
    
    QVector<QVector<bool>> mask(h, QVector<bool>(w, false));
    
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int pattern_i = i % patternH;
            int pattern_j = j % patternW;
            mask[i][j] = (pattern[pattern_i][pattern_j] == targetValue);
        }
    }
    
    return mask;
}

double ManualWBProcessor::calculateSum(const QVector<QVector<double>>& roi,
                                     const QVector<QVector<bool>>& mask) const
{
    double sum = 0.0;
    
    for (int i = 0; i < roi.size(); ++i) {
        for (int j = 0; j < roi[i].size(); ++j) {
            if (mask[i][j]) {
                sum += roi[i][j];
            }
        }
    }
    
    return sum;
}

int ManualWBProcessor::calculateCount(const QVector<QVector<bool>>& mask) const
{
    int count = 0;
    
    for (int i = 0; i < mask.size(); ++i) {
        for (int j = 0; j < mask[i].size(); ++j) {
            if (mask[i][j]) {
                count++;
            }
        }
    }
    
    return count;
}

double ManualWBProcessor::calculateMean(const QVector<QVector<double>>& roi,
                                      const QVector<QVector<bool>>& mask) const
{
    double sum = calculateSum(roi, mask);
    int count = calculateCount(mask);
    return (count > 0) ? (sum / count) : 0.0;
}

QVector<QVector<double>> ManualWBProcessor::applyWBGain(const QVector<QVector<double>>& imsrc,
                                                       const QVector<int>& wbGain,
                                                       double blcTarget,
                                                       double maxval) const
{
    int h = imsrc.size();
    int w = imsrc[0].size();
    int patternH = m_CFAPattern.size();
    int patternW = m_CFAPattern[0].size();
    
    // 创建增益比矩阵
    QVector<QVector<double>> ratio(patternH, QVector<double>(patternW, 0.0));
    
    for (int i = 0; i < patternH; ++i) {
        for (int j = 0; j < patternW; ++j) {
            int patternVal = m_CFAPattern[i][j];
            if (patternVal == 0) {
                ratio[i][j] = wbGain[0] / 1024.0;  // B gain
            } else if (patternVal == 1) {
                ratio[i][j] = wbGain[1] / 1024.0;  // G gain
            } else if (patternVal == 2) {
                ratio[i][j] = wbGain[2] / 1024.0;  // R gain
            } else {
                ratio[i][j] = 1.0;  // 其他通道保持原样
            }
        }
    }
    
    // 应用白平衡增益
    QVector<QVector<double>> imdst(h, QVector<double>(w, 0.0));
    
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int pattern_i = i % patternH;
            int pattern_j = j % patternW;
            double gain = ratio[pattern_i][pattern_j];
            imdst[i][j] = imsrc[i][j] * gain + blcTarget;
            
            // 限制数值范围
            imdst[i][j] = std::max(0.0, std::min(imdst[i][j], maxval));
        }
    }
    
    return imdst;
}

QVector<QVector<int>> ManualWBProcessor::generate24Rois(const QVector<QVector<double>>& imsrc, int roiH, int roiW) const
{
    QVector<QVector<int>> boxes;
    
    // 简化的ROI生成逻辑 - 在实际应用中需要根据具体需求实现
    int imgH = imsrc.size();
    int imgW = imsrc[0].size();
    
    // 在图像中均匀分布24个ROI
    int numH = 6;
    int numV = 4;
    
    for (int i = 0; i < numV; ++i) {
        for (int j = 0; j < numH; ++j) {
            int x = j * (imgW - roiW) / (numH - 1);
            int y = i * (imgH - roiH) / (numV - 1);
            boxes.append({x, y, roiW, roiH});
        }
    }
    
    return boxes;
}