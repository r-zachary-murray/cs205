#include "SAP.h"
#include "common.hh"
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstdio>
inline double rnd() {
	return (1. / RAND_MAX)*static_cast<double>(rand());
}
/** Initializes the moving balls simulation.
 * \param[in] n_ the total number of balls.
 * \param[in] K_ the repulsive spring constant between balls in contact.
 * \parma[in] damp_ the damping coefficient.
 * \param[in] diam_ the ball diameter.
 * \param[in] filename_ the name of the output directory to write to. */
SAP::SAP(int n_, double K_, double damp_, double diam_, const char *filename_) :
	dop853(6 * n_), n(n_), K(K_), damp(damp_), diam(diam_),radius(diam_/2.), diamsq(diam_*diam_),
	phase(new double[n]), filename(filename_) {
	for (int i = 0; i < n; i++) {
		IntList_x.push_back(i);
		//IntList_y.push_back(i);
		//IntList_z.push_back(i);
	}
}
void SAP::init_test() {
	const double h = 2 * M_PI / n;
	double *pp = w, *vp = w + 3 * n;
	for (int i = 0; i < n; i++, pp += 3, vp += 3) {
		double r = 1 + 0.3*cos(90 * h*i),
			s = 0.3*sin(90 * h*i),
			cth = cos(h*i), sth = sin(h*i);

		// Set position
		*pp = cth * r;
		pp[1] = sth * r;
		pp[2] = s;

		// Set velocity and color phase
		*vp = -sth * r;
		vp[1] = cth * r;
		vp[2] = 0;
		phase[i] = i * h;
	}
}
void SAP::InsertionSort(double *in)
{
	for (unsigned i = 1; i < IntList_x.size(); ++i) {
		for (int j = i; j > 0&& (in[3*IntList_x[j]] < in[3 * IntList_x[j-1]]); --j) {
			std::swap(IntList_x[j], IntList_x[j - 1]);
		}
		/**First version only requires one dimension list*/
		/*for (int j = i; j > 0 && (w[6 * IntList_x[j]+1] < w[6 * IntList_x[j - 1]+1]); --j) {
			std::swap(IntList_y[j], IntList_y[j - 1]);
		}
		for (int j = i; j > 0 && (w[6 * IntList_x[j]+2] < w[6 * IntList_x[j - 1]+2]); --j) {
			std::swap(IntList_z[j], IntList_z[j - 1]);
		}*/
	}
}
void SAP::Broadphase(double *in) {
	//============Sort==============
	InsertionSort(in);
	//============Prune=============
	Candidateset.clear();
	ActiveList.clear();
	//loop over IntList_x
	for (int i = 1; i < n; i++) {
		int cur_id = IntList_x[i];
		//loop over ActiveList
		for (unsigned j = 0; j < ActiveList.size(); j++) {
			int comp_id = ActiveList[j];
			if ((in[3*comp_id]+radius)<(in[3*cur_id]-radius))
				ActiveList.erase(ActiveList.begin() + j);
			//check y axis and z axis
			else if(abs(in[3 * comp_id+1]-in[3 * cur_id+1])<diam&&abs(in[3 * comp_id + 2] - in[3 * cur_id + 2]) < diam)
			Candidateset.push_back(colpair(cur_id, comp_id));
		}
	}
}
void SAP::Narrowphase(double tt, double *in, double *out) {
	
	double *vel = in + 3 * n, *acc = out + 3 * n;
	for (int i = 0; i < 3 * n; i++) out[i] = in[3 * n + i];
	double dx, dy, dz, rsq;
	//damping force
	for (int i = 0; i < 3 * n; i += 3) {
		acc[i] = -in[i] - damp * vel[i];
		acc[i + 1] = -in[i + 1] - damp * vel[i + 1];
		acc[i + 2] = -in[i + 2] - damp * vel[i + 2];
	}
	//loop over candidateset
	for (unsigned i = 0; i < Candidateset.size();i++) {
		colpair pair = Candidateset[i];
		dx = in[3 * pair.a] - in[3 * pair.b];dy= in[3 * pair.a+1] - in[3 * pair.b+1];dz= in[3 * pair.a+2] - in[3 * pair.b+2];
		rsq = dx * dx + dy * dy + dz * dz;
		if (rsq < diamsq) {
			double fac = K * (1 - diam / sqrt(rsq)), *op = acc + 3 * pair.a;
			dx *= fac; dy *= fac; dz *= fac;
			*op += dx;
			op[1] += dy;
			op[2] += dz;
			op = acc + 3 * pair.b;
			*op -= dx;
			op[1] -= dy;
			op[2] -= dz;
		}
	}
}
void SAP::ff(double tt, double *in, double *out) {
	Broadphase(in);
	Narrowphase(tt, in, out);
}
/** Saves a snapshot of point positions at a particular output frame.
 * \param[in] fnum the frame number.
 * \param[in] out a pointer to the array of point positions. */
void SAP::output(int fnum, double *out) {

	// Print diagnostic information
	if (fnum == 0) puts("# Output frame 0");
	else {
		//double d_wtime = wtime() - o_wtime;
		int d_nstep = nstep - o_nstep;
		printf("%d %d\n", fnum, d_nstep);
		//printf("%d %d %g %g %g\n", fnum, d_nstep, d_wtime / d_nstep, tbuild / d_nstep, tfind / d_nstep);
		//        printf("# Output frame %d [com = %g s, evals = %d, com/eval = %g s]\n",
		//               fnum,d_wtime,d_nstep,d_wtime/d_nstep);
	}
	tbuild = tfind = 0;

	// Assemble the output filename and output the particle information
	sprintf(buf, "%s/pts.%d", filename, fnum);
	FILE *outf = safe_fopen(buf, "w");
	double *pp = out;
	for (int i = 0; i < n; i++, pp += 3) fprintf(outf, "%g %g %g %g\n", *pp, pp[1], pp[2], phase[i]);
	fclose(outf);

	// Save the current time
	//o_wtime = wtime();
	o_nstep = nstep;
}
