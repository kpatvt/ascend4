/*
	Python Bindings for FPROPS 2.x with support
	for numerous different equations of state.
*/
%module fprops
%feature("autodoc");

%{
#include "../fprops.h"
#include "../fluids.h"
#include "../sat.h"
#include "../common.h"
#include "../solve_ph.h"
#include "../solve_Tx.h"
#include "../solve_px.h"
#include "../solve_pT.h"
#include "../refstate.h"
#include "../filedata.h"
#include "../derivs.h"

/*----------------- REFERENCE STATES -------------------*/

const ReferenceState REF_IIR = {FPROPS_REF_IIR};
const ReferenceState REF_NBP = {FPROPS_REF_NBP};
const ReferenceState REF_TPF = {FPROPS_REF_TPF};
const ReferenceState REF_TPFU = {FPROPS_REF_TPFU};


ReferenceState *REF_TRHS(double T0, double rho0, double h0, double s0){
	ReferenceState ref = {FPROPS_REF_TRHS,{.trhs={T0,rho0,h0,s0}}};
	ReferenceState *p = FPROPS_NEW(ReferenceState);
	*p = ref;
	return p;
}

ReferenceState *REF_TPUS(double T0, double p0, double u0, double s0){
	ReferenceState ref = {FPROPS_REF_TPUS,{.trhs={T0,p0,u0,s0}}};
	ReferenceState *p = FPROPS_NEW(ReferenceState);
	*p = ref;
	return p;
}

ReferenceState *REF_TPHS(double T0, double p0, double h0, double s0){
	ReferenceState ref = {FPROPS_REF_TPHS,{.trhs={T0,p0,h0,s0}}};
	ReferenceState *p = FPROPS_NEW(ReferenceState);
	*p = ref;
	return p;
}

ReferenceState *REF_PHI0(double c, double m){
	ReferenceState ref = {FPROPS_REF_PHI0,{.phi0={.c=c,.m=m}}};
	ReferenceState *p = FPROPS_NEW(ReferenceState);
	*p = ref;
	return p;
}

%}

// TODO can we make these into a python submodule instead?
const ReferenceState REF_IIR;
const ReferenceState REF_NBP;
const ReferenceState REF_TPF;
const ReferenceState REF_TPFU;
const ReferenceState *REF_TRHS(double T0, double rho0, double h0, double s0);
const ReferenceState *REF_TPUS(double T0, double p0, double u0, double s0);
const ReferenceState *REF_TPHS(double T0, double p0, double h0, double s0);
const ReferenceState *REF_PHI0(double c, double m);

#if 0 // no way to interrogate reference states from python so far
typedef struct ReferenceState_struct ReferenceState;
%extend ReferenceState{
	const char *typename;
}

%{
const char *ReferenceState_typename_get(const ReferenceState *ref){
	return fprops_refstate_type(ref->type);
}
%}
#endif

/* ----------------- ACCESSING FLUID SPECIFICATIONS -----------------*/

// load and initialise a PureFluid
%rename(fluid) fprops_fluid;
%exception {
	$action
	if(!result){
		PyErr_SetString(PyExc_RuntimeError,"Invalid fluid requested");
		return NULL;
	}
}
PureFluid *fprops_fluid(char *name, const char *corrtype = NULL, const char *source = NULL);

// get a fluid by index position (don't assume that these numbers are constant!)
%rename(get_fluid) fprops_get_fluid;
PureFluid *fprops_get_fluid(int i);

%exception;

// how many fluids are available
%rename(num_fluids) fprops_num_fluids;
int fprops_num_fluids();

/*------------------- PURE FLUID OBJECT -------------------*/

%nodefaultctor;

typedef struct{} FluidState2;
typedef struct{} PureFluid;

/* FIXME what should we do with ctors and dtors...? */
//%nodefaultdtor PureFluid;
%nodefaultctor PureFluid;

// use SWIG's generalised exceptions
%include <exception.i>

%extend PureFluid{
	// destructor: doesn't see to work
	~PureFluid();

	// use a local _fprops___err variable to catch and raise errors from FPROPS
	%typemap(in,numinputs=0) FpropsError *err (FpropsError _fprops___err = 0) {
		$1 = &_fprops___err;
	}
	%typemap(argout) FpropsError *err {
		if(*$1 != 0) {
		    SWIG_exception(SWIG_ValueError,fprops_error(*$1));
		}
	}

	// set the reference state for a fluid
	void set_ref(ReferenceState *ref, FpropsError *err){
		int res;
		res = fprops_set_reference_state($self, ref);
		if(res)*err = FPROPS_NUMERIC_ERROR;
	}

	FluidState2 set_Trho(double T, double rho, FpropsError *err){
		FluidState2 state = fprops_set_Trho(T,rho,$self,err);
		return state;
	}

	FluidState2 set_ph(double p, double h, FpropsError *err){
		FluidState2 state = fprops_solve_ph(p, h, $self, err);
		return state;
	}
	FluidState2 set_pT(double p, double T, FpropsError *err){
		FluidState2 state = fprops_solve_pT(p, T, $self, err);
		return state;
	}

	int region_ph(double p, double h, FpropsError *err){
		return fprops_region_ph(p, h, $self,err);
	}

	FluidState2 set_Tx(double T, double x, FpropsError *err){
		FluidState2 state = fprops_solve_Tx(T, x, $self, err);
		//state.T = T;
		//state.fluid = $self;
		return state;
	}

	int region_Tx(double T, double x, FpropsError *err){
		return fprops_region_Tx(T, x, $self,err);
	}

	FluidState2 set_px(double p, double x, FpropsError *err){
		FluidState2 state = fprops_solve_px(p, x, $self, err);
		return state;
	}

	int region_px(double p, double x, FpropsError *err){
		return fprops_region_px(p, x, $self,err);
	}


	double psat_T_acentric(double T){
		return fprops_psat_T_acentric(T, $self->data);
	}
	
	double psat_T_xiang(double T){
		return fprops_psat_T_xiang(T, $self->data);
	}

	double rhof_T_rackett(double T){
		return fprops_rhof_T_rackett(T, $self->data);
	}

	double rhog_T_chouaieb(double T){
		return fprops_rhog_T_chouaieb(T, $self->data);
	}

	%apply double *OUTPUT { double *rho_f };
	%apply double *OUTPUT { double *rho_g };
	double triple_point(double *rho_f, double *rho_g, FpropsError *err){
		double p;
		fprops_triple_point(&p,rho_f,rho_g,$self, err);
		return p;
	}

	int can_sat(){
		// FIXME this should be implemented elsewhere...
		switch($self->type){
		case FPROPS_PENGROB:
		case FPROPS_HELMHOLTZ:
			return 1;
		case FPROPS_IDEAL:
		case FPROPS_INCOMP:
		default:
			return 0;
		}
	}

	double sat_T(double T, double *rho_f, double *rho_g, FpropsError *err){
		double p;
		fprops_sat_T(T, &p, rho_f, rho_g, $self, err);
		return p;
	}

	double sat_p(double p, double *rho_f, double *rho_g, FpropsError *err){
		double T;
		fprops_sat_p(p, &T, rho_f, rho_g, $self, err);
		return T;
	}

	// raise exception if user attempts to write to these variables
	//%typemap(in) double{
	//	SWIG_exception(SWIG_ValueError,"Read-only attribute");
	//	return NULL;
	//}
	%immutable;
	double T_t;
	double T_c;
	double rho_c;
	double p_c;
	double omega;
	double M;
	double R;
	%immutable;
	char *name;
	int type;
	const char *typename;
	char *source;
}

%{
void delete_PureFluid(PureFluid *P){
	fprintf(stderr,"DESTROY\n");
	fprops_fluid_destroy(P);
}

// TODO trim this stuff down using some macro magic

double PureFluid_T_t_get(const PureFluid *fluid){
	return fluid->data->T_t;
}
double PureFluid_T_c_get(PureFluid *fluid){
	return fluid->data->T_c;
}
double PureFluid_p_c_get(PureFluid *fluid){
	return fluid->data->p_c;
}
double PureFluid_rho_c_get(PureFluid *fluid){
	return fluid->data->rho_c;
}
double PureFluid_omega_get(PureFluid *fluid){
	return fluid->data->omega;
}
double PureFluid_M_get(PureFluid *fluid){
	return fluid->data->M;
}
double PureFluid_R_get(PureFluid *fluid){
	return fluid->data->R;
}
const char *PureFluid_name_get(const PureFluid *fluid){
	return fluid->name;
}
const char *PureFluid_source_get(const PureFluid *fluid){
	return fluid->source;
}
int PureFluid_type_get(PureFluid *fluid){
	return fluid->type;
}

// FIXME get enumerations working more 'natively' in Python to avoid needing these string functions.
const char *PureFluid_typename_get(PureFluid *fluid){
	return fprops_corr_type(fluid->type);
}
%}

/*----------------------- FLUID STATE OBJECT ------------------*/

%{
static __thread FpropsError _fprops_fluidstate_err = 0;
%}

/* TODO be more sophisticated about the types of error returned to Python 
eg ValueError for FPROPS_VALUE_UNDEFINED, perhaps, or some FPROPS-specific
exception types? */
%exception{
	_fprops_fluidstate_err = 0;
	$action
	if(_fprops_fluidstate_err){
	    SWIG_exception(SWIG_RuntimeError,fprops_error(_fprops_fluidstate_err));
		return NULL;
	}
}

%extend FluidState2{
	// use a local _fprops___err variable to catch and raise errors from FPROPS
	%typemap(in,numinputs=0) FpropsError *err (FpropsError _fprops___err = 0) {
		$1 = &_fprops___err;
	}
	%typemap(argout) FpropsError *err {
		if(*$1 != 0) {
		    SWIG_exception(SWIG_ValueError,fprops_error(*$1));
		}
	}
	
	double deriv(char *spec, FpropsError *err){
		return fprops_deriv(*$self, spec, err);
	}
	%immutable;
	double T, rho, v;
	double x, p, u, h, s, a, cv, cp, w, g, alphap, betap, cp0, dpdT_rho;
	double mu, lam;
}

%{
#define FNS(G,X) G(T) X G(rho) X G(v) X G(x) X G(p) X G(u) X G(h) X G(s) X G(a) X G(cv) \
	X G(cp) X G(w) X G(g) X G(alphap) X G(betap) X G(cp0) X G(dpdT_rho) \
	X G(mu) X G(lam)
#define GETTER(N) \
	double FluidState2_##N##_get(FluidState2 *state){\
		return fprops_##N(*state,&_fprops_fluidstate_err);\
	}
#define SPACE
FNS(GETTER,SPACE)
#undef GETTER
#undef SPACE

%}

