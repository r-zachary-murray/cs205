#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

#include "SAP.h"

const char fn[] = "SAP_test";

// Ball diameter
const double diam = 0.06;

//  torus major radius, and torus minor radius
const double majrad = 1.1,
minrad = 0.27,
mrsq = minrad * minrad;


int main() {

	// Create output directory
	mkdir(fn, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	// Initialize the balls simulation and import the particle positions
	SAP sap_test(5e3, 5e3, 0.25, diam, fn);
	sap_test.atoli = 1e-7;
	sap_test.init_test();
	double *pp = sap_test.w, *vp = sap_test.w + 3 * sap_test.n;
	for (int i = 0; i < sap_test.n; i++, pp += 3, vp += 3) {
		double x = pp[3 * i], y = pp[3 * i + 1], z = pp[3 * i + 2], th = atan2(y, x),
			sth = sin(th), cth = cos(th),

			// Add twist to ball positions
			r = sqrt(x*x + y * y), r0 = r - majrad;
		*pp = x / r * (majrad + r0 * cth + z * sth);
		pp[1] = y / r * (majrad + r0 * cth + z * sth);
		pp[2] = z * cth - r0 * sth;

		*vp = -y; vp[1] = x; vp[2] = 0;
		sap_test.phase[i] = th;
	}
	pp = sap_test.w;
	vp = sap_test.w + 3 * sap_test.n;
	for (int i = 0; i < sap_test.n; i++) printf("i: %d p:(%g, %g, %g) v:(%g, %g, %g)\n", i, pp[3 * i],pp[3 * i + 1], pp[3 * i + 2], vp[3 * i], vp[3 * i + 1], vp[3 * i + 2]);
	sap_test.solve(0, 120, 20);
}