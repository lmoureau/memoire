#include "castor.h"

namespace // anonymous
{
  const unsigned castor_sectors = 16, castor_modules = 14;
  const bool castor_channel_quality[castor_sectors][castor_modules] = {
    {true,  true,  true, false,  true,  true, false,  true,  true,  true,  true,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false, false, false,  true, false,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false, false, false,  true, false,  true, false,  true},
    {true, false,  true,  true,  true,  true, false, false, false,  true,  true,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false, false, false,  true,  true,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false,  true,  true, false, false, false, false, false},
    {true,  true,  true,  true,  true,  true, false, false,  true, false, false, false, false, false},
    {true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true, false,  true},
    {true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false, false,  true,  true, false,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false, false,  true,  true, false,  true,  true,  true},
    {true,  true,  true,  true,  true, false, false, false, false,  true, false,  true, false,  true},
    {true,  true,  true,  true,  true,  true, false, false,  true,  true, false,  true,  true,  true},
    {true,  true,  true,  true,  true,  true, false, false,  true, false,  true,  true,  true,  true},
    {true,  true,  true,  true,  true, false, false, false,  true,  true,  true,  true,  true,  true}
  };
  const double castor_channel_gain[castor_sectors][castor_modules] = {
    {0.7510, 0.8700, 2.7370, 0.0000, 0.3630, 0.6430, 0.0000, 0.3100, 0.2120, 0.2740, 0.3030, 0.1690, 0.2650, 0.1550},
    {0.6190, 0.6160, 1.8130, 0.8690, 0.1820, 0.6280, 0.0000, 0.5070, 0.1680, 0.2910, 0.3380, 0.1460, 0.2490, 0.1250},
    {1.0700, 0.6510, 1.4250, 0.7660, 0.3040, 0.1930, 8.2170, 13.290, 0.4650, 0.2350, 0.0000, 0.2950, 0.3430, 0.3510},
    {0.5310, 0.3300, 0.8910, 0.8260, 0.1170, 0.3300, 0.0000, 0.0000, 0.0000, 0.6390, 0.0000, 0.3170, 0.0000, 0.4580},
    {0.6120, 0.0000, 1.3410, 0.7020, 0.1560, 0.5690, 0.8360, 0.0000, 0.0000, 0.5230, 0.2360, 0.3290, 0.3990, 0.3420},
    {1.3130, 0.4870, 1.4000, 0.6320, 0.1990, 0.7950, 1.2090, 0.0000, 0.5100, 0.7060, 0.2330, 0.2800, 0.4830, 0.4410},
    {0.4160, 0.2820, 1.0430, 0.3130, 0.1140, 0.086, 250.6690, 0.195, 0.4200, 6.9160, 3.4790, 1.5110, 4.8590, 3.5340},
    {0.3420, 0.2950, 1.1980, 1.4030, 0.2130, 1.0730, 0.0000, 0.2060, 0.6350, 27.269, 9.4210, 3.3400, 3.4880, 1.0100},
    {0.3030, 0.8460, 1.4120, 1.0000, 0.2180, 0.8830, 0.0000, 0.1320, 0.1950, 0.2490, 0.2250, 0.2270, 0.2990, 0.2780},
    {0.9040, 1.4030, 2.6580, 1.1900, 0.2350, 1.5570, 0.0000, 0.3160, 0.1990, 0.3100, 0.1790, 0.2510, 0.2510, 0.2520},
    {1.0160, 0.9930, 1.6950, 0.8870, 0.2850, 0.6230, 0.0000, 10.079, 0.3730, 0.2440, 9.6350, 0.5240, 0.6990, 0.3790},
    {1.1690, 1.1300, 2.1400, 1.3920, 0.2630, 1.2470, 0.0000, 0.0000, 0.5670, 0.3030, 99.351, 0.3510, 0.1980, 0.3560},
    {0.9160, 1.2700, 1.6430, 0.8070, 0.2310, 2.3020, 0.0000, 0.0000, 0.3230, 0.2910, 0.0000, 0.3430, 0.1280, 0.3080},
    {0.6010, 0.9840, 2.1400, 0.8210, 0.1770, 1.0970, 0.0000, 0.0000, 0.2030, 0.2920, 16.635, 0.3020, 0.3510, 0.3680},
    {0.7590, 1.3650, 2.9620, 1.1740, 0.3800, 2.3370, 0.000, 517.254, 0.2690, 0.0000, 0.1940, 0.2740, 0.2800, 0.4100},
    {0.7420, 0.9720, 2.4600, 0.9240, 0.2200, 0.1630, 3.9070, 0.1970, 0.2700, 0.2580, 0.1510, 0.1340, 0.2790, 0.2620}
  };
  const double castor_overall_gain = 0.02;
} // anonymous namespace

void castor::add_hit(int module, int sector, double data)
{
  _hits.push_back(hit{ module, sector, data });
  if (castor_channel_quality[sector - 1][module - 1]) {
    _energy += data * castor_channel_gain[sector - 1][module - 1]
                    * castor_overall_gain;
  }
}
