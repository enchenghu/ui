#ifndef CALIBRATIONPROCESSOR_H
#define CALIBRATIONPROCESSOR_H

#include <QObject>
#include <QVector>
#include <QImage>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <cmath>
struct CalibrationResult {
    QVector<QVector<double>> fpn;
    QVector<QVector<double>> fpn_blc;
    QVector<QVector<double>> fpn_ble;
    QVector<int> histogram_blc;
    QVector<int> histogram;
    double bl_err;
    int gain_pre_shift;
    QVector<QVector<int>> calib_img;
    QVector<QVector<int>> calib_img_new;
    QVector<QVector<uint16_t>> in_img;
    QVector<QVector<uint16_t>> out_img;
    int calib_shift;
    double offset;
    double fpn_min;
    double fpn_max;
    double fpn_mean;
    double clip_value; 
    double std_val;
    int dataBitWidth;
    int calibBitWidth;
    QSize size2D;
    double blc;
    double sensGain;
};

class CalibrationProcessor : public QObject
{
    Q_OBJECT
public:
    explicit CalibrationProcessor(QObject *parent = nullptr);
    
    template<typename T>
    QVector<T> flattenMatrix(const QVector<QVector<T>>& matrix);

    QVector<int> computeHistogram(const QVector<double>& data, 
                                                    double minEdge, double maxEdge, 
                                                    int& binCount);    
    int getCalibFile(CalibrationResult &res, const QString& dataDir_input, const QString& dataDir_output);
    int saveCalibrationFile(const QString& dataDir, 
                            const CalibrationResult &res);
    QVector<double> matlabConvSame(const QVector<double>& input, const QVector<double>& kernel); 
    QVector<double> smoothWithMovingAverage(const QVector<double>& input, int windowSize);
signals:
    void progressUpdate(int value);
    void message(const QString& msg);
    
private:
    void readImagesAndCalculateMean(
        CalibrationResult &res, const QString& dataDir, int& validCount);
    
    QVector<QVector<double>> readImFile(const QString& filename, 
                                       const QSize& size2D, int bit_width,
                                       int chNum);
    
    void eliminateOutliers(CalibrationResult &res);
    
    int normalizeAndShift(CalibrationResult &res);
    
    void applyFpnc(CalibrationResult &res);    
    QVector<QVector<uint8_t>> duplicateArray(const QVector<QVector<int>>& input, 
                                       int width, 
                                       int height, 
                                       int padSize);
    QByteArray pack8bTo6b(const QVector<uint8_t>& data_8b);
    QByteArray pack8bTo6b_opt(const QVector<uint8_t>& data_8b);
    static const int BITS_PER_BYTE = 8;
    static const int BITS_PER_6BIT = 6;
    static const int BYTES_PER_BLOCK = 8;

};

#endif // CALIBRATIONPROCESSOR_H