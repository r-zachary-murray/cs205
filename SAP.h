#ifndef SAP_HH
#define SAP_HH

#include <vector>
#include "common.hh"
#include "dop853.hh"
struct colpair {
	int a;
	int b;
	colpair() :a(0), b(0) {}
	colpair(int a_, int b_) :a(a_), b(b_) {}
};
class SAP : public dop853 {
public:
	SAP(int n_, double K_, double damp_, double diam_, const char* filename);
	~SAP() { delete[] phase; }
	/** The number of balls. */
	const int n;
	/** The repulsive spring constant between balls in contact. */
	const double K;
	/** The damping coefficient. */
	const double damp;
	/** The ball radius. */
	const double radius;
	/** The ball diameter. */
	const double diam;
	/** The ball diameter squared. */
	const double diamsq;
	/** The color phases. */
	double* const phase;
	/** The output directory filename. */
	const char *filename;
	void init_test();
	virtual void ff(double tt, double *in, double *out);
	virtual void output(int fnum, double *out);
private:
	/** A temporary buffer for assembling output filenames. */
	char buf[256];
	/** A counter for the number of integration steps, used for printing
	 * diagnostic information. */
	int o_nstep;
	/** A wall clock counter, used for printing diagnostic information. */
	double o_wtime;
	double tbuild;
	double tfind;
	/** Candidate set*/
	std::vector<colpair> Candidateset;
	/** Interval list*/
	std::vector<int> IntList_x;// , IntList_y, IntList_z; Version1
	std::vector<int> ActiveList;
	void Narrowphase(double tt, double *in, double *out);
	void Broadphase(double *in);
	void InsertionSort(double *in);
};
#endif
