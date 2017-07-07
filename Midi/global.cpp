#include "stdafx.h"
#include "global.h"

int fcmp(double x, double y)
{
	double fDiff = x - y;
	if (fDiff > 0.00001)
		return 1;
	else if (fDiff < -0.00001)
		return -1;
	else
		return 0;
}

