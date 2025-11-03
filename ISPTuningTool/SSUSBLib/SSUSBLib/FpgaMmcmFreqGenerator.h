#include <map>
#include <vector>

struct StFpgaMmcmFrepParameter
{
    double fMultiplier; // m
    double fDivider;    // d
    double fVcoDivider; // o
};

class CFpgaMmcmFreqGenerator
{
  public:
    static StFpgaMmcmFrepParameter stGetMmcmFreqParameter(double fFreqMhz);

  private:
    static CFpgaMmcmFreqGenerator &GetInstance();
    CFpgaMmcmFreqGenerator();
    std::map<double, std::pair<double, double>> m_mapVcoToMultiplierDivider;
    std::vector<double> m_vecVco;
};