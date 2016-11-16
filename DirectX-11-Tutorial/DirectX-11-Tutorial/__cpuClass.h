// The CpuClass is used to determine the percentage of total cpu use that is occurring each second.

#ifndef _CPUCLASS_H_
#define _CPUCLASS_H_

// We use the pdh library to query the cpu usage.
#pragma comment(lib, "pdh.lib")
#include <pdh.h>

class CpuClass {
 public:
	CpuClass();
	CpuClass(const CpuClass&);
   ~CpuClass();

	void Initialize();
	void Shutdown();
	void Frame();
	int  GetCpuPercentage();

 private:
	bool		  m_canReadCpu;
	HQUERY		  m_queryHandle;
	HCOUNTER	  m_counterHandle;
	long		  m_cpuUsage;
	unsigned long m_lastSampleTime;
};

#endif
