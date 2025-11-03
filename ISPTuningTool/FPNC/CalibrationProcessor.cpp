#include "CalibrationProcessor.h"
#include <QDirIterator>
#include <QVector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <QMessageBox>

template<typename T>
void csvWrite(const QString& filename, const QVector<T> &data, int rows, int cols)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open file" << filename;
        return;
    }
    QTextStream out(&file);
    // 按行和列格式写入数据
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int index = i * cols + j;
            if (index < rows) {
                out << data[index];
                if (j < cols - 1) {
                    out << ",";
                }
            }
        }
        out << "\n";
    }
    
    file.close();
    qDebug() << "CSV file saved:" << filename;
}

CalibrationProcessor::CalibrationProcessor(QObject *parent) : QObject(parent)
{
}

int CalibrationProcessor::getCalibFile(CalibrationResult& result, \
    const QString& dataDir, const QString& dataDir_output)
{
    int validCount = 0;
    
    // 1. 读取图像并计算平均
    readImagesAndCalculateMean(result, dataDir, validCount);
    
    if (validCount == 0) {
        //throw std::runtime_error("Error: no valid image for calibration!");
        qDebug() << "Error: no valid image for calibration";
        return -1;
    }  
    // 2. 消除异常值
    eliminateOutliers(result);
    
    // 3. 归一化和位移
    int ret = normalizeAndShift(result);
    if(ret) {
        return ret;
    }

    applyFpnc(result);
    return 0;
}

void CalibrationProcessor::applyFpnc(CalibrationResult &res)
{
    int height = res.fpn.size();
    int width = height > 0 ? res.fpn[0].size() : 0;
    double i_gain = res.sensGain / std::pow(2, res.gain_pre_shift);
    double i_offset = std::floor(res.offset * res.sensGain + 0.5) / std::pow(2, 12 - res.dataBitWidth);

    QVector<QVector<uint16_t>> in_img(height, QVector<uint16_t>(width));
    QVector<QVector<uint16_t>> out_img(height, QVector<uint16_t>(width));

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            in_img[i][j] = static_cast<uint16_t>(std::floor(res.fpn[i][j] + 0.5));
        }
    }
    res.in_img = in_img;
    // height = res.calib_img.size();
    // width = height > 0 ? res.calib_img[0].size() : 0;
    int ori_shift = res.calib_shift + 8 - res.dataBitWidth;

    res.calib_img_new.resize(height);
    for (int i = 0; i < height; ++i) {
        res.calib_img_new[i].resize(width);
        for (int j = 0; j < width; ++j) {
            res.calib_img_new[i][j] = static_cast<uint16_t>(res.calib_img[i][j] * i_gain / std::pow(2, ori_shift));
        }
    } 

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            out_img[i][j] = in_img[i][j] - res.calib_img_new[i][j] + i_offset;
        }
    } 
    res.out_img = out_img;
}

void CalibrationProcessor::readImagesAndCalculateMean(
    CalibrationResult &res, const QString& dataDir, int& validCount)
{
    QVector<QVector<double>> sum(res.size2D.width(), 
                                QVector<double>(res.size2D.height(), 0.0));
    validCount = 0;
    
    QDir dir(dataDir);
    QStringList filters = {"*.raw", "*.bin", "*.RAW", "*.BIN"};
    QStringList fileList = dir.entryList(filters, QDir::Files);
    
    for (const QString& filename : fileList) {
        qDebug() << ("=========Processing: " + filename);
        
        QString filePath = dataDir + "/" + filename;
        QVector<QVector<double>> im = readImFile(filePath, res.size2D, res.dataBitWidth, 1);
        
        if (!im.isEmpty()) {
            for (int i = 0; i < res.size2D.width(); ++i) {
                for (int j = 0; j < res.size2D.height(); ++j) {
                    sum[i][j] += im[i][j];
                }
            }
            validCount++;
        }
        
        //emit progressUpdate((validCount * 100) / fileList.size());
    }
    qDebug() << "=========validCount: " << validCount;
    // 计算平均值
    if (validCount > 1) {
        for (int i = 0; i < res.size2D.width(); ++i) {
            for (int j = 0; j < res.size2D.height(); ++j) {
                sum[i][j] /= validCount;
            }
        }
    }
    res.fpn = sum;
}

QVector<QVector<double>> CalibrationProcessor::readImFile(
    const QString& filename, const QSize& size2D, int bit_width, int chNum)
{
    QVector<QVector<double>> result;
    
    if (filename.endsWith(".raw", Qt::CaseInsensitive) || 
        filename.endsWith(".bin", Qt::CaseInsensitive)) {
        
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Cannot open file:" << filename;
            return result;
        }
        
        // 计算预期的数据大小
        int expectedSize = 0;
        if(bit_width <= 8) {
            expectedSize = size2D.width() * size2D.height() * chNum;
        } else {
            expectedSize = size2D.width() * size2D.height() * chNum * sizeof(uint16_t);
        }

        QByteArray data = file.readAll();
        file.close();
        // 验证文件大小是否匹配
        if (data.size() != expectedSize) {
            qWarning() << "File size mismatch. Expected:" << expectedSize 
                       << "bytes, Got:" << data.size() << "bytes";
            QMessageBox::warning(nullptr, "calibration",
                                "输入图像尺寸和位宽和数据文件不一致！",
                                QMessageBox::Ok);
            return result;
        }
        
        const uint8_t* pRawData_8 = nullptr;
        const uint16_t* pRawData_16 = nullptr;
        int totalElements = 0;
        if(bit_width <= 8) {
            pRawData_8 = reinterpret_cast<const uint8_t*>(data.constData());
            totalElements = data.size();
        } else {
            pRawData_16 = reinterpret_cast<const uint16_t*>(data.constData());
            totalElements = data.size() / sizeof(uint16_t);
        }

        if (chNum == 1) {
            // 正确实现MATLAB的 reshape(data, [width, height])'
            result.resize(size2D.width());
            for (int row = 0; row < size2D.width(); ++row) {
                result[row].resize(size2D.height());
                for (int col = 0; col < size2D.height(); ++col) {
                    int index = col * size2D.width() + row;
                    if (index < totalElements) {
                        if(bit_width <= 8){
                            result[row][col] = static_cast<double>(pRawData_8[index]);
                        } else {
                            result[row][col] = static_cast<double>(pRawData_16[index]);
                        }
                    }
                }
            } 
        } else if (chNum == 3) {
            // 处理RGB三通道数据
            result.resize(size2D.height());
            for (int i = 0; i < size2D.height(); ++i) {
                result[i].resize(size2D.width() * 3); // 每个像素3个通道
            }
            
            // 处理像素交错数据
            for (int row = 0; row < size2D.height(); ++row) {
                for (int col = 0; col < size2D.width(); ++col) {
                    // 计算在交错数据中的位置
                    int pixelIndex = col * size2D.height() + row;
                    int dataIndex = pixelIndex * 3;    
                    if (dataIndex + 2 < totalElements) {
                        if(bit_width <= 8){
                            result[row][col * 3] = static_cast<double>(pRawData_8[dataIndex]);     // R
                            result[row][col * 3 + 1] = static_cast<double>(pRawData_8[dataIndex + 1]); // G
                            result[row][col * 3 + 2] = static_cast<double>(pRawData_8[dataIndex + 2]); // B
                        } else {
                            result[row][col * 3] = static_cast<double>(pRawData_16[dataIndex]);     // R
                            result[row][col * 3 + 1] = static_cast<double>(pRawData_16[dataIndex + 1]); // G
                            result[row][col * 3 + 2] = static_cast<double>(pRawData_16[dataIndex + 2]); // B
                        }

                    }
                }
            }
        }
        else {
            qWarning() << "Unsupported channel number:" << chNum;
        }
    }
    else if (filename.endsWith(".txt", Qt::CaseInsensitive)) {
        // 实现.txt文件读取（十六进制格式）
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Cannot open text file:" << filename;
            return result;
        }
        
        QTextStream in(&file);
        QVector<double> data;
        while (!in.atEnd()) {
            QString line = in.readLine();
            bool ok;
            double value = line.toInt(&ok, 16); // 十六进制转换
            if (ok) {
                data.append(value);
            }
        }
        file.close();
        
        // 重塑数据（与raw文件相同的逻辑）
        if (chNum == 1) {
            result.resize(size2D.width());
            for (int row = 0; row < size2D.width(); ++row) {
                result[row].resize(size2D.height());
                for (int col = 0; col < size2D.height(); ++col) {
                    int index = col * size2D.width() + row;
                    if (index < data.size()) {
                        result[row][col] = data[index];
                    }
                }
            } 
        }
        // ... 处理chNum==3的情况 ...
    }
    else if (filename.endsWith(".bmp", Qt::CaseInsensitive)) {
        // 使用Qt的图像读取功能
        QImage image(filename);
        if (image.isNull()) {
            qWarning() << "Cannot open BMP file:" << filename;
            return result;
        }
        
        result.resize(image.height());
        for (int y = 0; y < image.height(); ++y) {
            result[y].resize(image.width());
            for (int x = 0; x < image.width(); ++x) {
                QRgb pixel = image.pixel(x, y);
                // 转换为灰度值或保持RGB
                if (chNum == 1) {
                    result[y][x] = static_cast<double>(qGray(pixel));
                }
                // ... 处理RGB情况 ...
            }
        }
    }
    else {
        qWarning() << "Unsupported file format:" << filename;
    }
    
    return result;
}

template<typename T>
QVector<T> CalibrationProcessor::flattenMatrix(const QVector<QVector<T>>& matrix)
{
    QVector<T> flattened;
    for (const auto& row : matrix) {
        for (T value : row) {
            flattened.append(value);
        }
    }
    return flattened;
}

QVector<int> CalibrationProcessor::computeHistogram(const QVector<double>& data, 
                                                   double minEdge, double maxEdge, 
                                                   int& binCount)
{
    binCount = static_cast<int>(maxEdge - minEdge);
    QVector<int> histogram(binCount, 0);
    
    for (double value : data) {
        // 计算value属于哪个bin
        int binIndex = static_cast<int>(std::floor(value - minEdge));
        if (binIndex >= 0 && binIndex < binCount) {
            histogram[binIndex]++;
        }
    }
    
    return histogram;
}

QVector<double> CalibrationProcessor::matlabConvSame(const QVector<double>& input, const QVector<double>& kernel)
{
    int inputSize = input.size();
    int kernelSize = kernel.size();
    QVector<double> output(inputSize, 0.0);
    
    if (inputSize == 0 || kernelSize == 0) return output;
    
    int kernelCenter = kernelSize / 2;
    
    for (int n = 0; n < inputSize; ++n) {
        output[n] = 0.0;
        for (int k = 0; k < kernelSize; ++k) {
            int m = n + k - kernelCenter;
            if (m >= 0 && m < inputSize) {
                output[n] += input[m] * kernel[k];
            }
        }
    }
    
    return output;
}

QVector<double> CalibrationProcessor::smoothWithMovingAverage(const QVector<double>& input, int windowSize)
{
    int inputSize = input.size();
    QVector<double> output(inputSize, 0.0);
    
    if (inputSize == 0) return output;
    
    // 处理边界情况
    for (int i = 0; i < inputSize; ++i) {
        double sum = 0.0;
        int count = 0;
        
        for (int j = -windowSize/2; j <= windowSize/2; ++j) {
            int index = i + j;
            if (index >= 0 && index < inputSize) {
                sum += input[index];
                count++;
            }
        }
        
        output[i] = sum;
    }
    
    return output;
}

void CalibrationProcessor::eliminateOutliers(CalibrationResult &res)
{
    int height = res.fpn.size();
    int width = height > 0 ? res.fpn[0].size() : 0;
    int binCount = 0;
    
    // 计算统计信息
    double fpn_min = std::numeric_limits<double>::max();
    double fpn_max = std::numeric_limits<double>::lowest();
    double sum = 0.0;
    int count = height * width;
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            double val = res.fpn[i][j];
            fpn_min = std::min(fpn_min, val);
            fpn_max = std::max(fpn_max, val);
            sum += val;
        }
    }
    double fpn_mean = sum / count;
    res.fpn_min = fpn_min;
    res.fpn_max = fpn_max;
    res.fpn_mean = fpn_mean;
    
    qDebug() << "min:" << fpn_min << "max:" << fpn_max << "mean:" << fpn_mean << "blc:" << res.blc;

    // 应用BLC
    QVector<QVector<double>> fpn_blc(height, QVector<double>(width, 0));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fpn_blc[i][j] = res.fpn[i][j] - res.blc;
        }
    }
    res.fpn_blc  = fpn_blc;
    res.histogram_blc = computeHistogram(flattenMatrix<double>(fpn_blc), -512.5, 511.5, binCount);
    res.histogram = computeHistogram(flattenMatrix<double>(res.fpn), -0.5, 1023.5, binCount);
 
    // 取32个bin near zero point (497:528)
    QVector<double> bins;
    for (int i = 496; i < 528 && i < res.histogram_blc.size(); ++i) {
        bins.append(res.histogram_blc[i]);
        //qDebug() << "bins[" << i << "] :" << res.histogram_blc[i];
    }
    
    // 卷积平滑 (ones(1,5))
    QVector<double> kernel(5, 1.0);
    QVector<double> sum5 = matlabConvSame(bins, kernel); 
    // 找到最大值位置
    auto max_it = std::max_element(sum5.begin(), sum5.end());
    int ind = std::distance(sum5.begin(), max_it);
    
    // 计算黑电平误差
    res.bl_err = -512.5 + (496 + ind) + 0.5;
    qDebug() << "bl_err:" << res.bl_err << ", max index is " << ind;

    QVector<QVector<double>> fpn_ble(height, QVector<double>(width));
    QVector<double> flattened_ble;
    
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fpn_ble[i][j] = fpn_blc[i][j] - res.bl_err;
            flattened_ble.append(fpn_ble[i][j]);
        }
    }

    double sum_sq = 0.0;
    for (double val : flattened_ble) {
        sum_sq += val * val;
    }
    double var_ = sum_sq / (flattened_ble.size() - 1);
    res.std_val = std::sqrt(var_);
    double fpn_range = fpn_max - fpn_min;
    double bit_range = std::pow(2, res.calibBitWidth) - 1;

    if (std::ceil(fpn_range) <= bit_range) {
        qDebug() << "No clipping";
    } else {
        // 计算各种裁剪参数
        double min_ble = *std::min_element(flattened_ble.begin(), flattened_ble.end());
        double neg_range = std::ceil(std::abs(min_ble));
        double std_4 = std::ceil(res.std_val * 4);
        double std_3_5 = std::ceil(res.std_val * 3.5);
        double std_3 = std::ceil(res.std_val * 3);
        
        double clip_value;
        if (std::min(neg_range, std_4) + std_4 <= bit_range * 2) {
            qDebug() << "Clip to 4*std";
            clip_value = std_4;
        } else if (std::min(neg_range, std_3_5) + std_3_5 <= bit_range * 2) {
            qDebug() << "Clip to 3.5*std";
            clip_value = std_3_5;
        } else {
            qDebug() << "Clip to 3*std";
            clip_value = std_3;
        }
        
        // 应用裁剪
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                fpn_ble[i][j] = std::min(clip_value, std::max(fpn_ble[i][j], -clip_value));
            }
        }
        res.clip_value = clip_value;
    }   
    res.fpn_ble = fpn_ble;
}

int CalibrationProcessor::normalizeAndShift(CalibrationResult &res)
{
    int height = res.fpn_ble.size();
    int width = height > 0 ? res.fpn_ble[0].size() : 0;
    
    // 恢复BLC和BLE
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            res.fpn_blc[i][j] = res.fpn_ble[i][j] + res.bl_err;
        }
    }
    
    // 计算偏移量
    double min_val_ = std::numeric_limits<double>::max();
    double max_val_ = std::numeric_limits<double>::lowest();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            min_val_ = std::min(min_val_,  res.fpn_blc[i][j]);
            max_val_ = std::max(max_val_,  res.fpn_blc[i][j]);
        }
    }
    res.offset = -min_val_;
    
    if (res.offset <= 0) {
        qDebug() << "Error: offset is not positive! offset is " << res.offset 
        << ", min val is " << min_val_ << ", max val is " << max_val_ << ": " << height <<", "<< width;
        return -1;
    }
  
    if (res.sensGain == 0.0) {
        qDebug() << "Error: sensGain is 0!";
        return -1;
    }    
    // 应用传感器增益归一化
    QVector<QVector<double>> calib_img_double(height, QVector<double>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            calib_img_double[i][j] = (res.fpn_blc[i][j] + res.offset) / res.sensGain;
        }
    }
    res.offset /= res.sensGain;
    
    // 计算位移
    double max_val = std::numeric_limits<double>::lowest();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            max_val = std::max(max_val, calib_img_double[i][j]);
        }
    }
    
    double bit_range = std::pow(2, res.calibBitWidth ) - 1;
    double step = max_val / bit_range;
    int shift = std::ceil(std::log2(step));
    
    // 应用位移并四舍五入
    res.calib_img.resize(height);
    for (int i = 0; i < height; ++i) {
        res.calib_img[i].resize(width);
        for (int j = 0; j < width; ++j) {
            double shifted_val = calib_img_double[i][j] / std::pow(2, shift);
            res.calib_img[i][j] = std::round(shifted_val);
        }
    }
    
    // 计算校准参数
    int data_l_shift = 12 - res.dataBitWidth;
    int total_r_shift = -shift + 4 - data_l_shift;
    
    if (total_r_shift < 0) {
        qDebug() << "Values exceed the range of" << res.calibBitWidth 
                 << "-bit, enable (l) gain_pre_shift";
        res.gain_pre_shift = total_r_shift;
        res.calib_shift = 0;
    } else if (total_r_shift > 7) {
        qDebug() << "Precision exceeds the range of calib_shift, enable (r) gain_pre_shift";
        res.gain_pre_shift = total_r_shift - 7;
        res.calib_shift = 7;
    } else {
        res.gain_pre_shift = 0;
        res.calib_shift = total_r_shift;
    }
    
    res.offset *= std::pow(2, data_l_shift);
    qDebug() << "offset is " << res.offset << ", calib_shift is " << res.calib_shift;
    return 0;
}

int CalibrationProcessor::saveCalibrationFile(const QString& dataDir, 
                                             const CalibrationResult &res)
{
    int height = res.calib_img.size();
    int width = res.calib_img[0].size();
    QByteArray output_6bit;
    // 填充
    QVector<QVector<int>> calib_pad(height + 16, QVector<int>(width, 0));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            calib_pad[i + 8][j] = res.calib_img[i][j];
        }
    }
    // 使用新的 duplicateArray 实现
    QVector<QVector<uint8_t>> calib_dup = duplicateArray(calib_pad, width , height + 16, 8);
    //qDebug() << "calib_dup height is " << calib_dup.size() << ", calib_dup width is " << calib_dup[0].size();
    
    QVector<uint8_t> img_col;
    img_col = flattenMatrix<uint8_t>(calib_dup);
    if(res.calibBitWidth == 6) {
        output_6bit = pack8bTo6b_opt(img_col);
    }
    // 生成文件名
    QString filename = QString("%1x%2_blc%3err%4_bit%5_gain%6_preShift%7_calibShift%8_offset%9_pad_dup.fpn")
        .arg(width).arg(height)
        .arg(res.blc, 0, 'f', 0)
        .arg(static_cast<int>(res.bl_err))
        .arg(res.calibBitWidth)
        .arg(res.sensGain, 0, 'f', 3)
        .arg(res.gain_pre_shift)
        .arg(res.calib_shift)
        .arg(res.offset, 0, 'f', 3);

#if 1
    QString filePath = dataDir + "/" + filename;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        if(res.calibBitWidth == 6) {
            file.write(output_6bit);
        } else {
            file.write(reinterpret_cast<const char*>(img_col.constData()), img_col.size());
        }
        file.close();
        qDebug() << "Calibration file saved: " << filePath;
    } else {
        qDebug() << "Error: Cannot save file: " << filePath;
        return -1;
    }
#endif
    return 0;
}

QVector<QVector<uint8_t>> CalibrationProcessor::duplicateArray(
    const QVector<QVector<int>>& input, int width, int height, int interval)
{
    int input_len = width * height;
    
    // 将输入矩阵转换为列优先的一维向量 (模拟MATLAB的 input' 和 reshape)
    QVector<uint8_t> tmp(input_len);
    int index = 0;
    for (int j = 0; j < width; ++j) {
        for (int i = 0; i < height; ++i) {
            if (i < input.size() && j < input[i].size()) {
                tmp[index] = static_cast<uint8_t>(input[i][j]);
            } else {
                tmp[index] = 0;
            }
            index++;
        }
    }

    // 创建输出向量，大小为输入的两倍
    QVector<uint8_t> output_vec(input_len * 2, 0);
    
    // 执行复制操作
    for (int i = 0; i <= input_len - interval; i += interval) {
        if (i + 7 < input_len) { // 确保有足够的元素
            // 第一组复制
            output_vec[i * 2 + 0] = tmp[i];
            output_vec[i * 2 + 1] = tmp[i + 1];
            output_vec[i * 2 + 2] = tmp[i + 2];
            output_vec[i * 2 + 3] = tmp[i + 3];
            output_vec[i * 2 + 4] = tmp[i + 4];
            output_vec[i * 2 + 5] = tmp[i + 5];
            output_vec[i * 2 + 6] = tmp[i + 6];
            output_vec[i * 2 + 7] = tmp[i + 7];
            
            // 第二组复制（重复第一组）
            output_vec[i * 2 + 8] = tmp[i];
            output_vec[i * 2 + 9] = tmp[i + 1];
            output_vec[i * 2 + 10] = tmp[i + 2];
            output_vec[i * 2 + 11] = tmp[i + 3];
            output_vec[i * 2 + 12] = tmp[i + 4];
            output_vec[i * 2 + 13] = tmp[i + 5];
            output_vec[i * 2 + 14] = tmp[i + 6];
            output_vec[i * 2 + 15] = tmp[i + 7];
        }
    }
    
    // QString filename = "matlab2qt_output_vec.txt";        
    // QString filePath =  "D:/M1_Calibration_tool/M1_Calibration_tool/FPNC/out/" + filename;
    // csvWrite<uint8_t>(filePath, output_vec, output_vec.size(), 1);
    QVector<QVector<uint8_t>> output_matrix(width, QVector<uint8_t>(height * 2, 0));

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height * 2 ; ++j) {
            int linear_index = i * height * 2 + j;
            if (linear_index < output_vec.size()) {
                output_matrix[i][j] = output_vec[linear_index];
            }
        }
    }
    
    return output_matrix;
}

QByteArray CalibrationProcessor::pack8bTo6b(const QVector<uint8_t>& data_8b)
{
    int num_data_8b = data_8b.size();
    
    // num_data_6b = ceil(num_data_8b * 6 / 8)
    int num_data_6b = (num_data_8b * 6 + 7) / 8;
    
    // 计算需要填充的字节数
    int num_pad = num_data_8b % BYTES_PER_BLOCK;
    QVector<uint8_t> padded_data = data_8b;
    
    // 如果需要填充
    if (num_pad != 0) {
        int num_zeros = BYTES_PER_BLOCK - num_pad;
        padded_data.resize(num_data_8b + num_zeros);
        for (int i = num_data_8b; i < num_data_8b + num_zeros; ++i) {
            padded_data[i] = 0; // 填充0
        }
        num_data_8b += num_zeros;
    }
    
    QByteArray data_6b;
    data_6b.resize(num_data_6b);
    
    // 处理每个8字节块
    for (int i = 0; i < num_data_8b / BYTES_PER_BLOCK; ++i) {
        int start_idx = i * BYTES_PER_BLOCK;
        
        // 将8个6位值打包到48位（6字节）
        uint64_t packed = 0;
        
        for (int j = 0; j < BYTES_PER_BLOCK; ++j) {
            // 取高6位（右移2位）
            uint8_t six_bit_val = padded_data[start_idx + j] >> 2;
            // 将6位值打包到64位整数中
            packed |= (static_cast<uint64_t>(six_bit_val) << (42 - j * 6));
        }
        
        // 从48位数据中提取6个字节
        for (int j = 0; j < 6; ++j) {
            // 提取每个8位段
            uint8_t byte_val = (packed >> (40 - j * 8)) & 0xFF;
            data_6b[i * 6 + j] = static_cast<char>(byte_val);
        }
    }
    
    return data_6b;
}

QByteArray CalibrationProcessor::pack8bTo6b_opt(const QVector<uint8_t>& data_8b) {
    if (data_8b.isEmpty()) {
        return QByteArray();
    }

    int num_data_8b = data_8b.size();
    int num_data_6b = std::ceil(num_data_8b * 6.0 / 8.0);
    
    // Pad input to make it multiple of 8
    int num_pad = num_data_8b % 8;
    QVector<uint8_t> padded_data = data_8b;
    
    if (num_pad != 0) {
        int num_0_pad = 8 - num_pad;
        for (int i = 0; i < num_0_pad; ++i) {
            padded_data.append(0);
        }
    }
    
    int total_blocks = padded_data.size() / 8;
    QByteArray data_6b;
    data_6b.resize(total_blocks * 6);
    
    for (int i = 0; i < total_blocks; ++i) {
        // Get the 8 bytes for this block
        uint8_t byte1 = padded_data[(i * 8) + 0];
        uint8_t byte2 = padded_data[(i * 8) + 1];
        uint8_t byte3 = padded_data[(i * 8) + 2];
        uint8_t byte4 = padded_data[(i * 8) + 3];
        uint8_t byte5 = padded_data[(i * 8) + 4];
        uint8_t byte6 = padded_data[(i * 8) + 5];
        uint8_t byte7 = padded_data[(i * 8) + 6];
        uint8_t byte8 = padded_data[(i * 8) + 7];
        
        // Pack 8 bytes into 48 bits (using uint64_t)
        uint64_t data_8x6b = (static_cast<uint64_t>(byte8) << 42) |
                             (static_cast<uint64_t>(byte7) << 36) |
                             (static_cast<uint64_t>(byte6) << 30) |
                             (static_cast<uint64_t>(byte5) << 24) |
                             (static_cast<uint64_t>(byte4) << 18) |
                             (static_cast<uint64_t>(byte3) << 12) |
                             (static_cast<uint64_t>(byte2) << 6)  |
                             (static_cast<uint64_t>(byte1));
        
        // Extract 6 output bytes
        for (int j = 0; j < 6; ++j) {
            // Calculate bit range: 8 bits starting from (j*8) to (j*8+7)
            int start_bit = j * 8;
            uint8_t extracted_byte = 0;
            
            // Extract 8 bits and form a byte
            for (int k = 0; k < 8; ++k) {
                int bit_pos = start_bit + (7 - k); // Extract from MSB to LSB
                uint64_t bit_mask = static_cast<uint64_t>(1) << bit_pos;
                bool bit_value = (data_8x6b & bit_mask) != 0;
                extracted_byte = (extracted_byte << 1) | (bit_value ? 1 : 0);
            }
            
            data_6b[i * 6 + j] = static_cast<char>(extracted_byte);
        }
    }
    
    // Trim output to correct size
    if (num_data_6b < data_6b.size()) {
        data_6b.resize(num_data_6b);
    }
    
    return data_6b;
}

