#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "SAP.h"

const char fn[] = "SAP_test";

// Ball diameter
const double diam = 0.06;

//  torus major radius, and torus minor radius
const double majrad = 1.1,
minrad = 0.27,
mrsq = minrad * minrad;

int main(int argc, char** argv) {
	int	n = (argc > 1) ? atoi(argv[1]) : 1000,
	nsteps = (argc > 2) ? atoi(argv[2]) : 100;
	double time_dur = (argc > 3) ? atof(argv[3]) : 10.;
	// Create output directory
	mkdir(fn, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	// Initialize the balls simulation and import the particle positions
	SAP sap_test(n, 5e3, 0.025, diam, fn);
	sap_test.atoli = 1e-7;
	//double *pp = sap_test.w,
	//*vp = sap_test.w + 3 * sap_test.n;
	//for (int i = 0; i < sap_test.n; i++) printf("i: %d p:(%g, %g, %g) v:(%g, %g, %g)\n", i, pp[3 * i],pp[3 * i + 1], pp[3 * i + 2], vp[3 * i], vp[3 * i + 1], vp[3 * i + 2]);
	sap_test.solve(0, time_dur, nsteps);
	printf("Average time for broadphase:%g\n", sap_test.time_sum / sap_test.num_bf);
}
