/* This file is created by Hongke Zhu, 01-27-2010.
Chemical & Materials Engineering Department,
University of Alabama in Huntsville, United States.

LITERATURE REFERENCE
Dillon, H.E. and Penoncello, S.G., "A Fundamental Equation for Calculation of the Thermodynamic Properties of Ethanol"
Int. J. Thermophys., 25(2):321-335, 2004.

Triple point temperature comes from
http://webbook.nist.gov/cgi/cbook.cgi?ID=C64175&Units=SI&Mask=4#Thermo-Phase
*/

#include "../helmholtz.h"

#define ETHANOL_M 46.06844 /* kg/kmol */
#define ETHANOL_R (8314.472/ETHANOL_M) /* J/kg/K */
#define ETHANOL_TC 513.9 /* K */

static const IdealData ideal_data_ethanol = {
	IDEAL_CP0,{.cp0={
		ETHANOL_R /* cp0star */
		, 1. /* Tstar */
		, 1 /* power terms */
		, (const Cp0PowTerm[]){
		   /* d_i,		theta_i */
			{6.41129,	0.0}
		}
		, 4 /* exponential terms */
		, (const Cp0ExpTerm[]){
		   /* d_i,			theta_i */
			{1.95989,	694.0}
			,{7.60084,	1549.0}
			,{3.89583,	2911.0}
			,{4.23238,	4659.0}
		}
	}}
};

static const HelmholtzData helmholtz_data_ethanol = {
	/* R */ ETHANOL_R /* J/kg/K */
    , /* M */ ETHANOL_M /* kg/kmol */
    , /* rho_star */ 5.991*ETHANOL_M /* kg/m3(= rho_c for this model) */
    , /* T_star */ ETHANOL_TC /* K (= T_c for this model) */

    , /* T_c */ ETHANOL_TC
    , /* rho_c */ 5.991*ETHANOL_M /* kg/m3 */
    , /* T_t */ 159.0

	,{FPROPS_REF_IIR}

    , 0.644 /* acentric factor */
    , &ideal_data_ethanol
    , 23 /* power terms */
    , (const HelmholtzPowTerm[]){
        /* a_i, 		t_i, 	d_i, 	l_i */
        {0.114008942201E+2,	-0.5,	1.0,	0.0}
        , {-0.395227128302E+2,	0.0,	1.0,	0.0}
        , {0.413063408370E+2,	0.5,	1.0,	0.0}
        , {-0.188892923721E+2,	1.5,	1.0,	0.0}
        , {0.472310314140E+1,	2.0,	1.0,	0.0}
        , {-0.778322827052E-2,	5.0,	1.0,	0.0}
        , {0.171707850032E+0,	-0.5,	2.0,	0.0}
        , {-0.153758307602E+1,	1.0,	2.0,	0.0}
        , {0.142405508571E+1,	2.0,	2.0,	0.0}
        , {0.132732097050E+0,	0.0,	3.0,	0.0}
        , {-0.114231649761E+0,	2.5,	3.0,	0.0}
        , {0.327686088736E-5,	6.0,	6.0,	0.0}
        , {0.495699527725E-3,	2.0,	7.0,	0.0}
        , {-0.701090149558E-4,	2.0,	8.0,	0.0}
        , {-0.225019381648E-5,	4.0,	8.0,	0.0}
        , {-0.255406026981E+0,	5.0,	1.0,	2.0}
        , {-0.632036870646E-1,	3.0,	3.0,	2.0}
        , {-0.314882729522E-1,	7.0,	3.0,	2.0}
        , {0.256187828185E-1,	5.5,	6.0,	2.0}
        , {-0.308694499382E-1,	4.0,	7.0,	2.0}
        , {0.722046283076E-2,	1.0,	8.0,	2.0}
        , {0.299286406225E-2,	22.0,	2.0,	4.0}
        , {0.972795913095E-3,	23.0,	7.0,	4.0}
    }
};

EosData eos_ethanol = {
	"ethanol"
	,"H E Dillon and S G Penoncello, 2004. 'A Fundamental Equation for "
	"Calculation of the Thermodynamic Properties of Ethanol', Int. J. "
	"Thermophys., 25(2):321-335."
	,"http://dx.doi.org/10.1023/B:IJOT.0000028470.49774.14"
	,100
	,FPROPS_HELMHOLTZ
	,.data = {.helm = &helmholtz_data_ethanol}
};

/*
    Test suite. These tests attempt to validate the current code using a few sample figures output by REFPROP 8.0. To compile and run the test:

    ./test.py ethanol
*/

#ifdef TEST

#include "../test.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

const TestData td[]; const unsigned ntd;

int main(void){
	test_init();
	PureFluid *P = helmholtz_prepare(&eos_ethanol,NULL);
	return helm_run_test_cases(P, ntd, td, 'C');
}

/*
A small set of data points calculated using REFPROP 8.0, for validation.
*/

const TestData td[] = {
    /* Temperature, Pressure, Density, Int. Energy, Enthalpy, Entropy, Cv, Cp, Cp0, Helmholtz */
    /* (C), (MPa), (kg/m3), (kJ/kg), (kJ/kg), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg-K), (kJ/kg) */
	{-2.0E+1, 9.99999999996E-2, 8.22593570889E+2, 1.56784130211E+2, 1.56905696928E+2, 8.35800290706E-1, 1.69412559546E+0, 2.05871916841E+0, 1.46744709062E+0, -5.47987133811E+1}
	, {3.0E+1, 9.99999999995E-2, 7.81220862969E+2, 2.73484810315E+2, 2.73612815089E+2, 1.25493529443E+0, 2.24580041092E+0, 2.6351539005E+0, 1.61304730942E+0, -1.06948824192E+2}
	, {7.79030485448E+1, 9.99999999993E-2, 7.36778584856E+2, 4.1315454529E+2, 4.13290271276E+2, 1.68173479459E+0, 2.6614213364E+0, 3.18178133269E+0, 1.75928316176E+0, -1.77223581194E+2}
	, {7.79030485448E+1, 1.E-1, 1.62685787441E+0, 1.20234994404E+3, 1.26381812855E+3, 4.10452491364E+0, 1.58939265689E+0, 1.80441359339E+0, 1.75928316176E+0, -2.38556039721E+2}
	, {8.0E+1, 1.E-1, 1.61599906386E+0, 1.20573124413E+3, 1.2676124681E+3, 4.11530118172E+0, 1.59944126498E+0, 1.81435687674E+0, 1.76569328102E+0, -2.4758736819E+2}
	, {1.30E+2, 1.E-1, 1.39680036022E+0, 1.29075039124E+3, 1.36234258355E+3, 4.36596889683E+0, 1.75911141856E+0, 1.96146598566E+0, 1.91643014052E+0, -4.69389969512E+2}
	, {1.80E+2, 1.E-1, 1.23407458071E+0, 1.3825375066E+3, 1.4635698863E+3, 4.60252368114E+0, 1.89433566364E+0, 2.08788068384E+0, 2.06054367171E+0, -7.03096099507E+2}
	, {2.30E+2, 1.E-1, 1.10724998185E+0, 1.48080538567E+3, 1.57111922774E+3, 4.82754222162E+0, 2.02482466567E+0, 2.21357417019E+0, 2.19620111781E+0, -9.48172483144E+2}
	, {2.80E+2, 1.E-1, 1.00497049175E+0, 1.58534363629E+3, 1.68484904548E+3, 5.04294888393E+0, 2.14869032458E+0, 2.33466071948E+0, 2.32309368631E+0, -1.20416353885E+3}
	, {3.30E+2, 1.E-1, 9.20445061849E-1, 1.69583796321E+3, 1.80448105789E+3, 5.24992840747E+0, 2.26526087089E+0, 2.44953096385E+0, 2.44158323879E+0, -1.47065635576E+3}
	, {-2.0E+1, 9.99999999999E-1, 8.23227153653E+2, 1.56506015153E+2, 1.57720746707E+2, 8.34699642772E-1, 1.69483239479E+0, 2.05863176723E+0, 1.46744709062E+0, -5.47981994151E+1}
	, {3.0E+1, 1.E+0, 7.8205577451E+2, 2.73104898268E+2, 2.74383579447E+2, 1.25367960456E+0, 2.24542696581E+0, 2.63336562261E+0, 1.61304730942E+0, -1.06948073855E+2}
	, {8.0E+1, 1.E+0, 7.35803584537E+2, 4.19246884015E+2, 4.20605942357E+2, 1.69904040257E+0, 2.67425846805E+0, 3.19915481374E+0, 1.76569328102E+0, -1.80769234154E+2}
	, {1.30E+2, 1.E+0, 6.77388024094E+2, 5.91463570998E+2, 5.92939829752E+2, 2.15472205721E+0, 2.91970343416E+0, 3.69012492101E+0, 1.91643014052E+0, -2.77212626364E+2}
	, {1.50703933505E+2, 1.E+0, 6.47203686778E+2, 6.70012588811E+2, 6.71557697438E+2, 2.34484287627E+0, 2.98081117959E+0, 3.91039464772E+0, 1.97706093979E+0, -3.23858287747E+2}
	, {1.50703933505E+2, 1.E+0, 1.52463668971E+1, 1.29165372322E+3, 1.35724311949E+3, 3.9625827811E+0, 1.96481188568E+0, 2.42765017631E+0, 1.97706093979E+0, -3.87902575383E+2}
	, {1.80E+2, 1.E+0, 1.35838550972E+1, 1.35421620918E+3, 1.4278330133E+3, 4.12363612276E+0, 2.02680210201E+0, 2.39232883543E+0, 2.06054367171E+0, -5.14409499854E+2}
	, {2.30E+2, 1.E+0, 1.1679974707E+1, 1.46147766589E+3, 1.54709428965E+3, 4.37328615732E+0, 2.11016412331E+0, 2.39164994169E+0, 2.19620111781E+0, -7.38941264162E+2}
	, {2.80E+2, 1.E+0, 1.03680187166E+1, 1.57146715668E+3, 1.66791759985E+3, 4.60218259002E+0, 2.20463397552E+0, 2.4469610926E+0, 2.32309368631E+0, -9.74230142989E+2}
	, {3.30E+2, 1.E+0, 9.37567867954E+0, 1.68548765671E+3, 1.79214660207E+3, 4.81714160724E+0, 2.30302935411E+0, 2.52428792204E+0, 2.44158323879E+0, -1.2199713037E+3}
	, {-2.0E+1, 1.E+1, 8.29322519157E+2, 1.53848278312E+2, 1.65906313357E+2, 8.24009226028E-1, 1.70176218208E+0, 2.05834130701E+0, 1.46744709062E+0, -5.47496572571E+1}
	, {3.0E+1, 1.0E+1, 7.89971027254E+2, 2.69523377049E+2, 2.82182069146E+2, 1.24163645555E+0, 2.24216125867E+0, 2.6173458509E+0, 1.61304730942E+0, -1.0687871445E+2}
	, {8.0E+1, 1.E+1, 7.46698416743E+2, 4.13683635289E+2, 4.27075922959E+2, 1.68298530038E+0, 2.66315136912E+0, 3.15990988267E+0, 1.76569328102E+0, -1.80662623539E+2}
	, {1.30E+2, 1.E+1, 6.94103735629E+2, 5.81944176873E+2, 5.96351245279E+2, 2.13064218646E+0, 2.90033342825E+0, 3.59757382301E+0, 1.91643014052E+0, -2.77024220599E+2}
	, {1.80E+2, 1.E+1, 6.25850581071E+2, 7.70593751397E+2, 7.86572006115E+2, 2.57495117268E+0, 3.0062275009E+0, 4.02657038775E+0, 2.06054367171E+0, -3.96245372505E+2}
	, {2.30E+2, 1.E+1, 5.21968120562E+2, 9.86447791179E+2, 1.00560604933E+3, 3.03268102086E+0, 3.04813962438E+0, 4.9257279574E+0, 2.19620111781E+0, -5.39445664469E+2}
	, {2.80E+2, 1.E+1, 2.38090717553E+2, 1.31757877173E+3, 1.35957956917E+3, 3.69945445205E+0, 3.01750799947E+0, 8.68099551174E+0, 2.32309368631E+0, -7.2877445842E+2}
	, {3.30E+2, 1.0E+1, 1.2639395011E+2, 1.55804090338E+3, 1.63715861424E+3, 4.18272135057E+0, 2.72596358682E+0, 3.98368381139E+0, 2.44158323879E+0, -9.64767479221E+2}
	, {-2.0E+1, 1.E+2, 8.75444385368E+2, 1.35081609949E+2, 2.49309311527E+2, 7.37124463997E-1, 1.76249292957E+0, 2.08570492905E+0, 1.46744709062E+0, -5.15214481122E+1}
	, {3.0E+1, 1.E+2, 8.45297807369E+2, 2.46126336866E+2, 3.64427838572E+2, 1.15085860271E+0, 2.23199238974E+0, 2.54270533068E+0, 1.61304730942E+0, -1.02756448546E+2}
	, {8.0E+1, 1.00E+2, 8.15152991752E+2, 3.80367115217E+2, 5.03043472921E+2, 1.57323716413E+0, 2.60530192953E+0, 2.98221188055E+0, 1.76569328102E+0, -1.75221589294E+2}
	, {1.30E+2, 1.E+2, 7.82361385306E+2, 5.32567282401E+2, 6.603854517E+2, 1.9894798003E+0, 2.80887957114E+0, 3.28911059965E+0, 1.91643014052E+0, -2.69491499091E+2}
	, {1.80E+2, 1.E+2, 7.46301896872E+2, 6.95944731329E+2, 8.29938762992E+2, 2.38573258172E+0, 2.8803978606E+0, 3.47538727706E+0, 2.06054367171E+0, -3.85149988077E+2}
	, {2.30E+2, 1.E+2, 7.07315221791E+2, 8.6503346122E+2, 1.00641314162E+3, 2.75505991204E+0, 2.86723453606E+0, 3.57087400315E+0, 2.19620111781E+0, -5.21174933521E+2}
	, {2.80E+2, 1.00E+2, 6.66188484722E+2, 1.035861179E+3, 1.18596884717E+3, 3.09526153993E+0, 2.80655710093E+0, 3.60270787347E+0, 2.32309368631E+0, -6.7628274181E+2}
	, {3.30E+2, 1.E+2, 6.23911315961E+2, 1.20569216389E+3, 1.36597135332E+3, 3.40680509995E+0, 2.72418093594E+0, 3.59146901408E+0, 2.44158323879E+0, -8.49122332145E+2}
};

const unsigned ntd = sizeof(td)/sizeof(TestData);

#endif
