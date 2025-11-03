#include <vector>
enum class CurveFittingType { NONE = -1, SPLINE = 0, BEZIER = 1 };

class QCurveFitting
{
public:
    static std::vector<std::pair<double, double>> generateCurve(
        const std::vector<std::pair<double, double>> &controlPoints, std::vector<std::pair<int, int>> &pointsToCalc,
        double interval, const CurveFittingType &method)
    {
        if (controlPoints.size() < 2) {
            return std::vector<std::pair<double, double>>();
        }
        if (method == CurveFittingType::BEZIER) {
            return generateBezierCurve(controlPoints, pointsToCalc, interval);
        } else if (method == CurveFittingType::SPLINE) {
            return generateSplineCurve(controlPoints, pointsToCalc, interval);
        } else {
            return generateNormalCurve(controlPoints, pointsToCalc, interval);
        }
    }

private:
    static double calculateCombination(int a, int b);
    static double calculateDistance(double ax, double ay, double bx, double by);
    static double calculateBezier(double t, int n, const std::vector<std::pair<double, double>> &pos, int indexx,
        int x_y);
    static double findTForX(double xTarget, int n, const std::vector<std::pair<double, double>> &controlPoints,
        double interval, double tStart, double tEnd);
    static std::vector<double> calculateDerivatives(const std::vector<std::pair<double, double>> &controlPoints);

    static std::vector<std::pair<double, double>> generateBezierCurve(
        const std::vector<std::pair<double, double>> &controlPoints, std::vector<std::pair<int, int>> &pointsToCalc,
        double interval);
    static std::vector<std::pair<double, double>> generateSplineCurve(
        const std::vector<std::pair<double, double>> &controlPoints, std::vector<std::pair<int, int>> &pointsToCalc,
        double interval);
    static std::vector<std::pair<double, double>> generateNormalCurve(
        const std::vector<std::pair<double, double>> &controlPoints, std::vector<std::pair<int, int>> &pointsToCalc,
        double interval);
};
