// ColorTransformCalculator.h
#ifndef COLORTRANSFORMCALCULATOR_H
#define COLORTRANSFORMCALCULATOR_H

#include <QVector>
#include <QDebug>
#include <cmath>
#include <QString>
#include <QFile>
class ColorTransformCalculator
{
public:
    struct TransformResult {
        QVector<QVector<double>> traMat;
        QVector<QVector<double>> traMat2;
        QVector<QVector<double>> traMat_transpose;
        QVector<QVector<int>> traMat_transpose_int;
        double error1;
        double error2;
        int ct;
        bool state;
        TransformResult() : error1(0.0), error2(0.0), ct(0), state(false) {}
    };

    static TransformResult calTraMat_RGBIR(const QVector<double>& BMs_coIR,
                                          const QVector<double>& GMs_coIR,
                                          const QVector<double>& RMs_coIR,
                                          const QVector<double>& NMs_coIR,
                                          const QVector<double>& BMs_noIR,
                                          const QVector<double>& GMs_noIR,
                                          const QVector<double>& RMs_noIR);

private:
    // 矩阵运算辅助函数
    static QVector<QVector<double>> transpose(const QVector<QVector<double>>& matrix);
    static QVector<QVector<double>> multiply(const QVector<QVector<double>>& A, const QVector<QVector<double>>& B);
    static QVector<QVector<double>> inverse(const QVector<QVector<double>>& matrix);
    static QVector<QVector<double>> leastSquaresMinNorm(const QVector<QVector<double>>& A, const QVector<QVector<double>>& B);
    static double sumAbsolute(const QVector<QVector<double>>& matrix);
    static QVector<QVector<double>> roundMatrix(const QVector<QVector<double>>& matrix, double factor);
    
    // 线性代数辅助函数
    static QVector<QVector<double>> choleskyDecomposition(const QVector<QVector<double>>& matrix);
    static QVector<QVector<double>> matrixSubtract(const QVector<QVector<double>>& A, const QVector<QVector<double>>& B);
};

class MatrixProcessor
{
public:
    struct ProcessResult {
        QVector<int> meanMatrix;
        bool success;
        QString errorMessage;
    };

    static ProcessResult processMatrices(const QVector<QVector<int>> & input_mat);

private:
    static QString intToHexString(int value, int width = 4);
    static QString getAddressSequence();
    static QStringList getRegisterNames();
};

#endif // COLORTRANSFORMCALCULATOR_H