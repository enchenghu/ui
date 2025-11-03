#include "stdafx.h"

#include "FpgaMmcmFreqGenerator.h"
#include <algorithm>

StFpgaMmcmFrepParameter CFpgaMmcmFreqGenerator::stGetMmcmFreqParameter(double fFreqMhz)
{
    double fMinError = 1000;
    StFpgaMmcmFrepParameter stResult = {0};
    for (auto fVco : GetInstance().m_vecVco)
    {
        if (fVco / 128 >= fFreqMhz)
        {
            continue;
        }
        if (fVco <= fFreqMhz)
        {
            break;
        }
        for (double o = 128; o >= 1; o -= 0.125)
        {
            double fOut = fVco / o;
            double fError = abs(fFreqMhz - fOut);
            if (fError < fMinError)
            {
                fMinError = fError;
                stResult.fMultiplier = GetInstance().m_mapVcoToMultiplierDivider[fVco].first;
                stResult.fDivider = GetInstance().m_mapVcoToMultiplierDivider[fVco].second;
                stResult.fVcoDivider = o;
            }
            if (fMinError == 0)
            {
                return stResult;
            }
            if (fOut > fFreqMhz)
            {
                break;
            }
        }
    }
    return stResult;
}

CFpgaMmcmFreqGenerator &CFpgaMmcmFreqGenerator::GetInstance()
{
    static CFpgaMmcmFreqGenerator instance;
    return instance;
}

CFpgaMmcmFreqGenerator::CFpgaMmcmFreqGenerator()
{
    for (double m = 2; m <= 64; m += 0.125)
    {
        for (double d = 1; d <= 106; d++)
        {
            double fVco = 100.0 * m / d;
            if(fVco<600 || fVco>1440)
            {
                continue;
            }
            if (m_mapVcoToMultiplierDivider.find(fVco) == m_mapVcoToMultiplierDivider.end())
            {
                m_mapVcoToMultiplierDivider[fVco] = std::make_pair(m, d);
                m_vecVco.push_back(fVco);
            }
        }
    }
    // sort reverse
    std::sort(m_vecVco.begin(), m_vecVco.end(), std::greater<double>());
}