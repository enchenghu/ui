// ColorTransformCalculator.cpp
#include "ColorTransformCalculator.h"
#include <algorithm>

ColorTransformCalculator::TransformResult ColorTransformCalculator::calTraMat_RGBIR(
    const QVector<double>& BMs_coIR,
    const QVector<double>& GMs_coIR,
    const QVector<double>& RMs_coIR,
    const QVector<double>& NMs_coIR,
    const QVector<double>& BMs_noIR,
    const QVector<double>& GMs_noIR,
    const QVector<double>& RMs_noIR)
{
    TransformResult result;
    
    // 构建输入输出数据矩阵
    int dataSize = BMs_coIR.size();
    QVector<QVector<double>> indata(dataSize, QVector<double>(4));
    QVector<QVector<double>> outdata(dataSize, QVector<double>(3));
    
    for (int i = 0; i < dataSize; ++i) {
        indata[i][0] = RMs_coIR[i];
        indata[i][1] = GMs_coIR[i];
        indata[i][2] = BMs_coIR[i];
        indata[i][3] = NMs_coIR[i];
        
        outdata[i][0] = RMs_noIR[i];
        outdata[i][1] = GMs_noIR[i];
        outdata[i][2] = BMs_noIR[i];
    }
    
    int inRows = indata.size();
    int inCols = (inRows > 0) ? indata[0].size() : 0;
    int outRows = outdata.size();
    int outCols = (outRows > 0) ? outdata[0].size() : 0;
    
    // 检查数据尺寸
    if (inRows != outRows || inCols != 4 || outCols != 3) {
        qDebug() << "The size of indata/outdata is wrong";
        qDebug() << "indata:" << inRows << "x" << inCols << "outdata:" << outRows << "x" << outCols;
        return result;
    }
    
    // 方法1: traMat = inv(indata'*indata)*indata'*outdata
    auto indataT = transpose(indata);
    auto A = multiply(indataT, indata);
    auto A_inv = inverse(A);
    auto B = multiply(indataT, outdata);
    result.traMat = multiply(A_inv, B);
    
    // 方法2: 最小范数最小二乘解
    result.traMat2 = leastSquaresMinNorm(indata, outdata);
    
    // 计算误差
    auto a = multiply(indata, result.traMat);
    auto c = matrixSubtract(a, outdata);
    result.error1 = sumAbsolute(c);
    
    // 构建无红外数据的比较矩阵
    QVector<QVector<double>> rgbOnly(dataSize, QVector<double>(3));
    for (int i = 0; i < dataSize; ++i) {
        rgbOnly[i][0] = RMs_coIR[i];
        rgbOnly[i][1] = GMs_coIR[i];
        rgbOnly[i][2] = BMs_coIR[i];
    }
    auto d = matrixSubtract(rgbOnly, outdata);
    result.error2 = sumAbsolute(d);
    
    qDebug() << "Error with transform:" << result.error1;
    qDebug() << "Error without transform:" << result.error2;
    
    // 缩放并取整
    result.traMat = roundMatrix(result.traMat, 512.0);
    result.traMat2 = roundMatrix(result.traMat2, 512.0);
    
    qDebug() << "traMat:";
    for (const auto& row : result.traMat) {
        QString rowStr;
        for (double val : row) {
            rowStr += QString::number(val, 'f', 0) + " ";
        }
        qDebug() << rowStr;
    }
    
    // qDebug() << "traMat2:";
    // for (const auto& row : result.traMat2) {
    //     QString rowStr;
    //     for (double val : row) {
    //         rowStr += QString::number(val, 'f', 0) + " ";
    //     }
    //     qDebug() << rowStr;
    // }
    
    return result;
}

QVector<QVector<double>> ColorTransformCalculator::transpose(const QVector<QVector<double>>& matrix)
{
    if (matrix.isEmpty()) return QVector<QVector<double>>();
    
    int rows = matrix.size();
    int cols = matrix[0].size();
    QVector<QVector<double>> result(cols, QVector<double>(rows));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[j][i] = matrix[i][j];
        }
    }
    
    return result;
}

QVector<QVector<double>> ColorTransformCalculator::multiply(const QVector<QVector<double>>& A, const QVector<QVector<double>>& B)
{
    if (A.isEmpty() || B.isEmpty()) return QVector<QVector<double>>();
    
    int aRows = A.size();
    int aCols = A[0].size();
    int bRows = B.size();
    int bCols = B[0].size();
    
    if (aCols != bRows) {
        qDebug() << "Matrix multiplication dimension mismatch";
        return QVector<QVector<double>>();
    }
    
    QVector<QVector<double>> result(aRows, QVector<double>(bCols, 0.0));
    
    for (int i = 0; i < aRows; ++i) {
        for (int j = 0; j < bCols; ++j) {
            for (int k = 0; k < aCols; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return result;
}

QVector<QVector<double>> ColorTransformCalculator::inverse(const QVector<QVector<double>>& matrix)
{
    if (matrix.isEmpty() || matrix.size() != matrix[0].size()) {
        qDebug() << "Matrix must be square for inversion";
        return QVector<QVector<double>>();
    }
    
    int n = matrix.size();
    
    // 使用Cholesky分解求逆（适用于对称正定矩阵）
    auto L = choleskyDecomposition(matrix);
    if (L.isEmpty()) {
        qDebug() << "Cholesky decomposition failed, matrix may not be positive definite";
        return QVector<QVector<double>>();
    }
    
    QVector<QVector<double>> invL(n, QVector<double>(n, 0.0));
    
    // 求L的逆
    for (int i = 0; i < n; ++i) {
        invL[i][i] = 1.0 / L[i][i];
        for (int j = i + 1; j < n; ++j) {
            double sum = 0.0;
            for (int k = i; k < j; ++k) {
                sum -= L[j][k] * invL[k][i];
            }
            invL[j][i] = sum / L[j][j];
        }
    }
    
    // A^{-1} = (L^{-1})^T * L^{-1}
    auto invLT = transpose(invL);
    return multiply(invLT, invL);
}

QVector<QVector<double>> ColorTransformCalculator::leastSquaresMinNorm(const QVector<QVector<double>>& A, const QVector<QVector<double>>& B)
{
    if (A.isEmpty() || B.isEmpty()) return QVector<QVector<double>>();
    
    int m = A.size();
    int n = A[0].size();
    int p = B[0].size();
    
    // 使用正规方程法: X = (A^T * A)^{-1} * A^T * B
    auto AT = transpose(A);
    auto ATA = multiply(AT, A);
    auto ATA_inv = inverse(ATA);
    
    if (ATA_inv.isEmpty()) {
        qDebug() << "Matrix inversion failed in least squares";
        return QVector<QVector<double>>();
    }
    
    auto ATB = multiply(AT, B);
    return multiply(ATA_inv, ATB);
}

double ColorTransformCalculator::sumAbsolute(const QVector<QVector<double>>& matrix)
{
    double sum = 0.0;
    for (const auto& row : matrix) {
        for (double val : row) {
            sum += std::abs(val);
        }
    }
    return sum;
}

QVector<QVector<double>> ColorTransformCalculator::roundMatrix(const QVector<QVector<double>>& matrix, double factor)
{
    QVector<QVector<double>> result;
    
    for (const auto& row : matrix) {
        QVector<double> newRow;
        for (double val : row) {
            newRow.append(std::round(val * factor));
        }
        result.append(newRow);
    }
    
    return result;
}

QVector<QVector<double>> ColorTransformCalculator::choleskyDecomposition(const QVector<QVector<double>>& matrix)
{
    int n = matrix.size();
    QVector<QVector<double>> L(n, QVector<double>(n, 0.0));
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= i; ++j) {
            double sum = 0.0;
            
            if (j == i) {
                for (int k = 0; k < j; ++k) {
                    sum += L[j][k] * L[j][k];
                }
                double diag = matrix[i][i] - sum;
                if (diag <= 0) {
                    qDebug() << "Matrix is not positive definite";
                    return QVector<QVector<double>>();
                }
                L[i][i] = std::sqrt(diag);
            } else {
                for (int k = 0; k < j; ++k) {
                    sum += L[i][k] * L[j][k];
                }
                L[i][j] = (matrix[i][j] - sum) / L[j][j];
            }
        }
    }
    
    return L;
}

QVector<QVector<double>> ColorTransformCalculator::matrixSubtract(const QVector<QVector<double>>& A, const QVector<QVector<double>>& B)
{
    if (A.size() != B.size() || A[0].size() != B[0].size()) {
        return QVector<QVector<double>>();
    }
    
    int rows = A.size();
    int cols = A[0].size();
    QVector<QVector<double>> result(rows, QVector<double>(cols));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i][j] = A[i][j] - B[i][j];
        }
    }
    
    return result;
}

MatrixProcessor::ProcessResult MatrixProcessor::processMatrices(const QVector<QVector<int>> & input_mat)
{
    ProcessResult result;
    result.success = false;

    // 检查输入矩阵尺寸
    for (const auto &it : input_mat){
        if(it.size() != 39){
            qDebug() << QString("Input matrices must have 39 elements. size is : %1").arg(it.size());
            return result;
        }
    }

    // 计算均值矩阵 - 与MATLAB的fix()一致（向零取整）
    QVector<int> M1 = input_mat[0];
    int input_size = input_mat.size();
    result.meanMatrix.resize(M1.size(), 0);
    for(int i = 0; i < input_size; ++i){
        for (int j = 0; j < M1.size(); ++j) {
            result.meanMatrix[j] += input_mat[i][j];
        }
    }
    for (int j = 0; j < M1.size(); ++j) {
        result.meanMatrix[j] = static_cast<int>(result.meanMatrix[j] / input_size);
    }

    // 获取地址序列和寄存器名称
    QString addr1 = getAddressSequence();
    QStringList regnames = getRegisterNames();

    // 检查地址序列长度
    if (addr1.length() != 39 * 4) {
        qDebug() << QString("Address sequence length mismatch. Expected: %1, Actual: %2")
                                .arg(39 * 4).arg(addr1.length());
        return result;
    }

    // 写入文件
    QFile file1("output_regparams.txt");
    QFile file2("output_cmodelparams.txt");

    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open output_regparams.txt for writing";
        return result;
    }

    if (!file2.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Cannot open output_cmodelparams.txt for writing";
        file1.close();
        return result;
    }

    QTextStream out1(&file1);
    QTextStream out2(&file2);

    // 处理每个元素
    for (int i = 0; i < result.meanMatrix.size(); ++i) {
        // 转换为无符号32位整数
        uint32_t unsigned_num = static_cast<uint32_t>(result.meanMatrix[i]);
        
        // 取低16位
        uint16_t result_val = unsigned_num & 0xFFFF;
        
        // 转换为16进制字符串
        QString hexStr = intToHexString(result_val, 4);
        
        // 计算地址索引 - MATLAB: addr1(i*4-3:i*4)
        int addrStart = i * 4;
        QString addrPart1 = addr1.mid(addrStart, 2);      // i*4-3:i*4-2
        QString addrPart2 = addr1.mid(addrStart + 2, 2);  // i*4-1:i*4
        
        // 写入寄存器参数文件
        out1 << QString("22 300813%1 %2 4 1 1 1\n").arg(addrPart1).arg(hexStr.mid(2, 2));
        out1 << QString("22 300813%1 %2 4 1 1 1\n").arg(addrPart2).arg(hexStr.mid(0, 2));
        
        // 写入CModel参数文件
        int regIndex = i % 13;
        int regGroup = i / 13;
        out2 << QString("%1[%2]=%3\n").arg(regnames[regIndex]).arg(regGroup).arg(result.meanMatrix[i]);
    }

    file1.close();
    file2.close();
    
    result.success = true;
    return result;
}

QString MatrixProcessor::intToHexString(int value, int width)
{
    QString hexStr = QString("%1").arg(value & 0xFFFF, width, 16, QChar('0'));
    return hexStr.toLower();
}

QString MatrixProcessor::getAddressSequence()
{
    // MATLAB: addr1 = ['18' '19' '1e' '1f' '24' '25' '2a' '2b' '30' '31' '36' '37' '3c' '3d' '42' '43' '48' '49' '4e' '4f' '54' '55' '5a' '5b' '60' '61', ...
    //          '1a' '1b' '20' '21' '26' '27' '2c' '2d' '32' '33' '38' '39' '3e' '3f' '44' '45' '4a' '4b' '50' '51' '56' '57' '5c' '5d' '62' '63', ...
    //          '1c' '1d' '22' '23' '28' '29' '2e' '2f' '34' '35' '3a' '3b' '40' '41' '46' '47' '4c' '4d' '52' '53' '58' '59' '5e' '5f' '64' '65'];
    
    QStringList addressParts = {
        "18", "19", "1e", "1f", "24", "25", "2a", "2b", "30", "31", "36", "37", "3c", "3d", "42", "43", "48", "49", "4e", "4f", "54", "55", "5a", "5b", "60", "61",
        "1a", "1b", "20", "21", "26", "27", "2c", "2d", "32", "33", "38", "39", "3e", "3f", "44", "45", "4a", "4b", "50", "51", "56", "57", "5c", "5d", "62", "63", 
        "1c", "1d", "22", "23", "28", "29", "2e", "2f", "34", "35", "3a", "3b", "40", "41", "46", "47", "4c", "4d", "52", "53", "58", "59", "5e", "5f", "64", "65"
    };
    
    return addressParts.join("");
}

QStringList MatrixProcessor::getRegisterNames()
{
    // MATLAB: regnames = ["r_nDeIRCT", ...
    //         "r_nDeIrMrx00" "r_nDeIrMrx01" "r_nDeIrMrx02" "r_nDeIrMrx03", ...
    //         "r_nDeIrMrx10" "r_nDeIrMrx11" "r_nDeIrMrx12" "r_nDeIrMrx13", ...
    //         "r_nDeIrMrx20" "r_nDeIrMrx21" "r_nDeIrMrx22" "r_nDeIrMrx23"];
    
    return QStringList{
        "r_nDeIRCT",
        "r_nDeIrMrx00", "r_nDeIrMrx01", "r_nDeIrMrx02", "r_nDeIrMrx03",
        "r_nDeIrMrx10", "r_nDeIrMrx11", "r_nDeIrMrx12", "r_nDeIrMrx13", 
        "r_nDeIrMrx20", "r_nDeIrMrx21", "r_nDeIrMrx22", "r_nDeIrMrx23"
    };
}