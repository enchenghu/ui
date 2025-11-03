#include "qcurvefitting.h"
#include <omp.h>

double QCurveFitting::calculateCombination(int a, int b)
{
    int    i;
    double sum1, sum2;
    sum1 = sum2 = 1;
    for (i = a; i >= (a - b + 1); i--)
        sum1 = sum1 * i;
    for (i = 1; i <= b; i++)
        sum2 = sum2 * i;
    return sum1 / sum2;
}

double QCurveFitting::calculateDistance(double ax, double ay, double bx, double by)
{
    return sqrt(pow((ax - bx), 2) + pow((ay - by), 2));
}

double QCurveFitting::calculateBezier(double t, int n, const std::vector<std::pair<double, double>> &pos, int indexx,
    int x_y)
{
    double sum = 0;
    n          = n - 1;
    if (x_y == 0) {
        for (int i = 0; i <= n; i++)
            sum = sum + calculateCombination(n, i) * pos[indexx + i].first * pow(1 - t, n - i) * pow(t, i);
    } else {
        for (int i = 0; i <= n; i++)
            sum = sum + calculateCombination(n, i) * pos[indexx + i].second * pow(1 - t, n - i) * pow(t, i);
    }
    return sum;
}

double QCurveFitting::findTForX(double xTarget, int n, const std::vector<std::pair<double, double>> &controlPoints,
    double interval, double tStart, double tEnd)
{
    // 使用二分法找到对应于目标x值的t值
    double tMid = (tStart + tEnd) / 2;
    double xMid = calculateBezier(tMid, n, controlPoints, 0, 0);

    if (abs(xMid - xTarget) < interval / 2) {
        return tMid;
    } else if (xMid < xTarget) {
        return findTForX(xTarget, n, controlPoints, interval, tMid, tEnd);
    } else {
        return findTForX(xTarget, n, controlPoints, interval, tStart, tMid);
    }
}

std::vector<std::pair<double, double>> QCurveFitting::generateBezierCurve(
    const std::vector<std::pair<double, double>> &controlPoints, std::vector<std::pair<int, int>> &pointsToCalc,
    double interval)
{
    std::vector<std::pair<double, double>> curvePoints;
    int                                    n = static_cast<int>(controlPoints.size());
    double                                 x;
    auto                                   it = pointsToCalc.begin() + 1;
#pragma omp parallel for thread_num(32)
    for (x = controlPoints[0].first; x <= controlPoints[n - 1].first; x += interval) {
        double t = findTForX(x, n, controlPoints, interval, 0, 1);
        double y = calculateBezier(t, n, controlPoints, 0, 1);
        curvePoints.push_back({ x, y });
        // calculate y for points in pointsToCalc
        while (it != pointsToCalc.end() - 1 && it->first <= x) {
            double tPoint = findTForX(it->first, n, controlPoints, interval, 0, 1);
            it->second    = static_cast<int>(calculateBezier(tPoint, n, controlPoints, 0, 1));
            ++it;
        }
    }
    return curvePoints;
}

std::vector<double> QCurveFitting::calculateDerivatives(const std::vector<std::pair<double, double>> &controlPoints)
{
    int                 n = static_cast<int>(controlPoints.size());
    std::vector<double> v(n, 0);
    std::vector<double> gamma(n - 2, 0);
    std::vector<double> c(n - 2, 0);
    std::vector<double> beta(n - 2, 0);
    double              b = 2;
    double              a, r, term1, term2, term3, term4, term5;
#pragma omp parallel for
    for (int i = 1; i < n - 1; i++) {
        term1    = controlPoints[i].first - controlPoints[i - 1].first;
        term2    = controlPoints[i + 1].first - controlPoints[i - 1].first;
        term3    = controlPoints[i + 1].second - controlPoints[i].second;
        term4    = controlPoints[i].second - controlPoints[i - 1].second;
        term5    = controlPoints[i + 1].first - controlPoints[i].first;
        a        = term1 / term2;
        r        = (6.0 / term2) * (term3 / term5 - term4 / term1);
        c[i - 1] = 1 - a;
        if (i == 1) {
            beta[i - 1]  = b;
            gamma[i - 1] = r / beta[i - 1];
        } else {
            beta[i - 1]  = b - a * c[i - 2] / beta[i - 2];
            gamma[i - 1] = (r - a * gamma[i - 2]) / beta[i - 1];
        }
    }
    for (int i = n - 2; i >= 1; i--) {
        v[i] = gamma[i - 1] - c[i - 1] * v[i + 1] / beta[i - 1];
    }
    return v;
}

std::vector<std::pair<double, double>> QCurveFitting::generateSplineCurve(
    const std::vector<std::pair<double, double>> &controlPoints, std::vector<std::pair<int, int>> &pointsToCalc,
    double interval)
{
    std::vector<std::pair<double, double>> curvePoints;
    int                                    n = static_cast<int>(controlPoints.size());
    std::vector<double>                    v = calculateDerivatives(controlPoints);
    double                                 x;
    auto                                   it = pointsToCalc.begin() + 1;
#pragma omp parallel for
    for (x = controlPoints[0].first; x <= controlPoints[n - 1].first; x += interval) {
        double ys;
        int    low_limit = 0, high_limit = n - 1;
        while ((high_limit - low_limit) > 1) {
            int i = (high_limit + low_limit) / 2;
            if (controlPoints[i].first > x) {
                high_limit = i;
            } else {
                low_limit = i;
            }
        }
        double term  = controlPoints[high_limit].first - controlPoints[low_limit].first;
        double term1 = (controlPoints[high_limit].first - x) / term;
        double term2 = (x - controlPoints[low_limit].first) / term;
        double term3 = term1 * controlPoints[low_limit].second;
        double term4 = term2 * controlPoints[high_limit].second;
        double term5 = (pow(term1, 3) - term1) * v[low_limit];
        double term6 = (pow(term2, 3) - term2) * v[high_limit];
        ys           = term3 + term4 + ((term5 + term6) * pow(term, 2)) / 6;
        curvePoints.push_back({ x, ys });
        // calculate y for points in pointsToCalc
        while (it != pointsToCalc.end() - 1 && it->first <= x) {
            int low_limit_point = 0, high_limit_point = n - 1;
            while ((high_limit_point - low_limit_point) > 1) {
                int i = (high_limit_point + low_limit_point) / 2;
                if (controlPoints[i].first > it->first) {
                    high_limit_point = i;
                } else {
                    low_limit_point = i;
                }
            }
            double term_point  = controlPoints[high_limit_point].first - controlPoints[low_limit_point].first;
            double term1_point = (controlPoints[high_limit_point].first - it->first) / term_point;
            double term2_point = (it->first - controlPoints[low_limit_point].first) / term_point;
            double term3_point = term1_point * controlPoints[low_limit_point].second;
            double term4_point = term2_point * controlPoints[high_limit_point].second;
            double term5_point = (pow(term1_point, 3) - term1_point) * v[low_limit_point];
            double term6_point = (pow(term2_point, 3) - term2_point) * v[high_limit_point];
            it->second         = static_cast<int>(term3_point + term4_point + ((term5_point + term6_point) * pow(term_point, 2)) / 6);
            ++it;
        }
    }
    return curvePoints;
}

std::vector<std::pair<double, double>> QCurveFitting::generateNormalCurve(
    const std::vector<std::pair<double, double>> &controlPoints, std::vector<std::pair<int, int>> &pointsToCalc,
    double interval)
{
    return controlPoints;
}
