#ifndef SOIPRXS_GLOBAL_H
#define SOIPRXS_GLOBAL_H

#include "WinSock.h"


#ifdef SOIPRXS_LIB
# define SOIPRXS_EXPORT __declspec(dllexport)
#else
# define SOIPRXS_EXPORT __declspec(dllimport)
#endif

struct QoSInfo
{
  struct timeval measurementStartTime, measurementEndTime;
  double kbits_per_second_min, kbits_per_second_max;
  double kBytesTotal;
  double packet_loss_fraction_min, packet_loss_fraction_max;
  unsigned totNumPacketsReceived, totNumPacketsExpected;
};

typedef QoSInfo QOSINFO;

#endif // SOIPRXS_GLOBAL_H
