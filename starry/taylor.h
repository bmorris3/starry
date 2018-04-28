/**
Taylor expansions for the solver module.

Not for the faint of heart.

*/

#ifndef _STARRY_TAYLOR_H_
#define _STARRY_TAYLOR_H_

#include <iostream>
#include <cmath>
#include <Eigen/Core>
#include "errors.h"
#include "fact.h"
#include "ellip.h"

// Largest value of l for large occultors
// (analytic expressions are numerically unstable above this)
#define STARRY_LMAX_LARGE_OCC                   8

// Taylor expansion of the s2 term above this radius
#define STARRY_RADIUS_THRESH_S2                 2.0

// Taylor expansion of the M integral at large radius
#define STARRY_RADIUS_THRESH_M                  2.0
#define STARRY_R_ORDER                          12
#define STARRY_R_MAXPQ                          12
#define STARRY_R_NPQ                            7
static const double STARRY_R_COEFF[STARRY_R_NPQ][STARRY_R_NPQ][STARRY_R_ORDER] =
{{{0,0,2.356194490192345,0.19634954084936207,0.055223308363883075,0.023009711818284616,
0.011744540407249439,0.006795055521337176,0.004282300615009366,0.002871860333081678,
0.0020192767966980547,0.0014736641268831765}, {0,0,2.356194490192345,-0.19634954084936207,
-0.018407769454627694,-0.0046019423636569235, -0.00167779148674992,-0.0007550061690374639,
-0.00038930005590994234,-0.00022091233331397522, -0.00013461845311320365,-0.00008668612511077508},
{0,0,2.356194490192345,-0.5890486225480862,0.05522330836388308,0.0046019423636569235,
0.001006674892049952,0.0003235740724446274,0.00012976668530331411,0.0000602488181765387,
0.00003106579687227777,0.000017337225022155016}, {0,0,2.356194490192345,-0.9817477042468103,
0.2761165418194154,-0.023009711818284616, -0.00167779148674992,-0.0003235740724446274,
-0.00009269048950236724,-0.00003347156565363261, -0.000014120816760126258,-6.668163470059622e-6},
{0,0,2.356194490192345,-1.3744467859455345,0.6442719309119693,-0.16106798272799233,
0.01174454040724944,0.000755006169037464,0.00012976668530331414,0.000033471565653632616,
0.000010982857480098202,4.243376753674305e-6}, {0,0,2.356194490192345,-1.7671458676442586,
1.1596894756415448,-0.48320394818397694,0.10570086366524496,-0.006795055521337176,
-0.0003893000559099424,-0.0000602488181765387,-0.000014120816760126258,-4.243376753674305e-6},
{0,0,2.356194490192345,-2.159844949342983,1.8223691760081417,-1.0630486860047494,
0.38756983343923157,-0.07474561073470894,0.0042823006150093665,0.00022091233331397524,
0.00003106579687227777,6.6681634700596224e-6}},{{0,0,0,0.39269908169872414,0.07363107781851078,
0.027611654181941538,0.01342233189399936,0.00755006169037464,0.0046716006709193085,
0.003092772666395653,0.0021538952498112583,0.0015603502519939514},{0,0,0,0.39269908169872414,
-0.07363107781851078,-0.009203884727313847,-0.002684466378799872,-0.0010785802414820915,
-0.0005190667412132565,-0.0002811611514905139,-0.0001656842499854814,-0.0001040233501329301},
{0,0,0,0.39269908169872414,-0.22089323345553233,0.02761165418194154,0.002684466378799872,
0.0006471481448892548,0.00022245717480568137,0.00009372038383017132,0.00004518661363240403,
0.00002400538849221464},{0,0,0,0.39269908169872414,-0.3681553890925539,0.1380582709097077,
-0.013422331893999362,-0.0010785802414820915,-0.00022245717480568137,-0.00006694313130726523,
-0.00002510367424022446,-0.000010911540223733927},{0,0,0,0.39269908169872414,-0.5154175447295755,
0.32213596545598466,-0.09395632325799552,0.0075500616903746405,0.0005190667412132566,
0.00009372038383017132,0.00002510367424022446,8.48675350734861e-6},{0,0,0,0.39269908169872414,
-0.6626797003665971,0.5798447378207724,-0.28186896977398657,0.06795055521337176,
-0.0046716006709193085,-0.00028116115149051397,-0.00004518661363240403,-0.000010911540223733927},
{0,0,0,0.39269908169872414,-0.8099418560036186,0.9111845880040709,-0.6201117335027705,
0.24915203578236314,-0.051387607380112395,0.0030927726663956536,0.00016568424998548143,
0.00002400538849221464}},{{0,0,0,0,0.14726215563702155,0.03681553890925539,0.016106798272799233,
0.008628641931856732,0.005190667412132565,0.003373933817886167,0.00231957949979674,
0.0016643736021268816},{0,0,0,0,0.14726215563702155,-0.03681553890925539,-0.005368932757599744,
-0.0017257283863713463,-0.0007415239160189379,-0.00037488153532068527,-0.00021087086361788545,
-0.00012802873862514475},{0,0,0,0,0.14726215563702155,-0.11044661672776616,0.016106798272799233,
0.0017257283863713463,0.00044491434961136273,0.00016066351513743652,0.00007029028787262848,
0.00003491692871594856},{0,0,0,0,0.14726215563702155,-0.18407769454627693,0.08053399136399617,
-0.008628641931856732,-0.0007415239160189378,-0.00016066351513743652,-0.00005020734848044892,
-0.000019398293731082537},{0,0,0,0,0.14726215563702155,-0.25770877236478773,0.18791264651599104,
-0.060400493522997124,0.005190667412132565,0.00037488153532068527,0.00007029028787262849,
0.000019398293731082537},{0,0,0,0,0.14726215563702155,-0.3313398501832985,0.33824276372878387,
-0.18120148056899135,0.046716006709193084,-0.003373933817886167,-0.00021087086361788545,
-0.00003491692871594856},{0,0,0,0,0.14726215563702155,-0.4049709280018093,0.5315243430023746,
-0.398643257251781,0.17129202460037465,-0.03711327199674784,0.00231957949979674,
0.00012802873862514475}},{{0,0,0,0,0,0.07363107781851078,0.021475731030398976,
0.010354370318228077,0.005932191328151503,0.0037488153532068526,0.0025304503634146253,
0.0017924023407520264},{0,0,0,0,0,0.07363107781851078,-0.021475731030398976,
-0.0034514567727426926,-0.0011864382656303007,-0.0005355450504581218,-0.00028116115149051397,
-0.0001629456673410933},{0,0,0,0,0,0.07363107781851078,-0.06442719309119693,
0.010354370318228077,0.0011864382656303005,0.00032132703027487304,0.0001204976363530774,
0.0000543152224470311},{0,0,0,0,0,0.07363107781851078,-0.10737865515199489,0.051771851591140394,
-0.005932191328151503,-0.0005355450504581218,-0.0001204976363530774,-0.000038796587462165074},
{0,0,0,0,0,0.07363107781851078,-0.15033011721279285,0.12080098704599425,-0.04152533929706052,
0.0037488153532068526,0.00028116115149051397,0.0000543152224470311},{0,0,0,0,0,
0.07363107781851078,-0.19328157927359077,0.21744177668278963,-0.12457601789118156,
0.03373933817886167,-0.0025304503634146253,-0.0001629456673410933},{0,0,0,0,0,
0.07363107781851078,-0.23623304133438874,0.3416942205015266,-0.27406723936059946,
0.12371090665582614,-0.02783495399756088,0.0017924023407520264}},{{0,0,0,0,0,0,
0.04295146206079795,0.01380582709097077,0.007118629593781804,0.004284360403664975,
0.0028116115149051397,0.00195534800809312},{0,0,0,0,0,0,0.04295146206079795,
-0.01380582709097077,-0.0023728765312606014,-0.0008568720807329949,-0.00040165878784359136,
-0.0002172608897881244},{0,0,0,0,0,0,0.04295146206079795,-0.04141748127291232,
0.007118629593781804,0.0008568720807329949,0.0002409952727061548,0.00009311180990919618},{0,
0,0,0,0,0,0.04295146206079795,-0.06902913545485385,0.03559314796890902,-0.004284360403664975,
-0.00040165878784359136,-0.00009311180990919618},{0,0,0,0,0,0,0.04295146206079795,
-0.09664078963679539,0.08305067859412103,-0.029990522825654817,0.0028116115149051392,
0.0002172608897881244},{0,0,0,0,0,0,0.04295146206079795,-0.12425244381873694,
0.14949122146941787,-0.08997156847696447,0.025304503634146255,-0.00195534800809312},{0,0,0,0,
0,0,0.04295146206079795,-0.1518640980006785,0.23491477659479953,-0.19793745064932183,
0.09278317999186961,-0.021508828089024316}},{{0,0,0,0,0,0,0,0.02761165418194154,
0.009491506125042404,0.005141232484397969,0.003213270302748731,0.002172608897881244},{0,0,0,0,
0,0,0,0.02761165418194154,-0.009491506125042405,-0.0017137441614659898,-0.0006426540605497462,
-0.0003103726996973206},{0,0,0,0,0,0,0,0.02761165418194154,-0.028474518375127215,
0.005141232484397969,0.0006426540605497461,0.00018622361981839234},{0,0,0,0,0,0,0,
0.02761165418194154,-0.04745753062521202,0.025706162421989847,-0.003213270302748731,
-0.0003103726996973206},{0,0,0,0,0,0,0,0.02761165418194154,-0.06644054287529684,
0.05998104565130964,-0.022492892119241117,0.002172608897881244},{0,0,0,0,0,0,0,
0.02761165418194154,-0.08542355512538165,0.10796588217235735,-0.06747867635772335,
0.019553480080931197},{0,0,0,0,0,0,0,0.02761165418194154,-0.10440656737546646,0.169660671985133,
-0.14845308798699136,0.07169609363008106}},{{0,0,0,0,0,0,0,0,0.01898301225008481,
0.006854976645863959,0.003855924363298477,0.0024829815975785647},{0,0,0,0,0,0,0,0,
0.01898301225008481,-0.006854976645863959,-0.0012853081210994924,-0.0004965963195157129},{0,0,0,
0,0,0,0,0,0.018983012250084808,-0.020564929937591875,0.003855924363298477,0.0004965963195157129},
{0,0,0,0,0,0,0,0,0.01898301225008481,-0.0342748832293198,0.019279621816492384,
-0.0024829815975785647},{0,0,0,0,0,0,0,0,0.01898301225008481,-0.047984836521047715,
0.044985784238482235,-0.017380871183049953},{0,0,0,0,0,0,0,0,0.01898301225008481,
-0.06169478981277563,0.08097441162926802,-0.05214261354914986},{0,0,0,0,0,0,0,0,
0.018983012250084808,-0.07540474310450354,0.12724550398884973,-0.11471374980812968}}};

// Re-parametrize s2() when |b-r| < this value
#define STARRY_BMINUSR_THRESH_S2                1.e-2

// Default value of the radius threshold for quartic expansion of the occultor limb
#define STARRY_QUARTIC_MAXL                     8
const vector<double> STARRY_RADIUS_THRESH_QUARTIC_VEC({100, 30, 30, 20, 15, 10, 8, 6, 5});
template <typename T>
T STARRY_RADIUS_THRESH_QUARTIC(int l) {
    if (l <= STARRY_QUARTIC_MAXL)
        return STARRY_RADIUS_THRESH_QUARTIC_VEC[l];
    else
        return 1;
}

// Taylor expansion of J() for small impact parameter
#define STARRY_B_ORDER                          12
template <typename T>
T STARRY_B_THRESH_J(T r) {return 0.1 * (1 - r);}
static const double STARRY_B_COEFF[STARRY_B_ORDER][STARRY_B_ORDER / 2] =
{{1.,0,0,0,0,0}, {-3.,0,0,0,0,0}, {-1.5,1.5,0,0,0,0}, {1.5,0.5,0,0,0,0},
 {0.375,0.75,0.375,0,0,0},{0.375,0.75,0.375,0,0,0},
 {0.0625,0.5625,0.9375,0.4375,0,0}, {0.1875,0.9375,1.3125,0.5625,0,0},
 {0.0234375,0.46875,1.640625,1.96875,0.7734375,0},
 {0.1171875,1.09375,2.953125,3.09375,1.1171875,0},
 {0.01171875,0.41015625,2.4609375,5.4140625,5.02734375,1.67578125},
 {0.08203125,1.23046875,5.4140625,10.0546875,8.37890625,2.58984375}};

// Taylor expansion of E() - K() when r >= 1
#define STARRY_EMINUSK_ORDER                          13
static const double STARRY_EMINUSK_COEFF[STARRY_EMINUSK_ORDER] =
{0, 0.25, 0.09375, 0.05859375, 0.042724609375, 0.0336456298828125,
 0.027757644653320312, 0.023627042770385742, 0.020568184554576874,
 0.018211413407698274, 0.016339684807462618, 0.01481712326858542,
 0.013554300262740071};


template <typename T>
using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
template <typename T>
using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

namespace solver
{
    template <class T>
    class Greens;
    bool is_even(int n, int ntimes);
    template <typename T>
    inline T step(T x);
};

namespace taylor {

    using std::abs;

    // Taylor expand the difference between the elliptic integrals
    // for the s2 term when r >= 1; much more numerically stable!
    template <typename T>
    inline T s2(solver::Greens<T>& G) {
        T x = 1. / G.b_r(1);
        T eps = x - 1;
        T EP, goodterm;
        if (abs(G.b() - G.r()) > 1e-8) {
            EP = ellip::PI(1 - 1. / ((G.b() - G.r()) * (G.b() - G.r())), G.ksq());
            goodterm = (3 * (G.b() + G.r()) / (G.b() - G.r()) * EP) / sqrt(G.br);
        } else {
            // Numerically stable first order expansion when b = r
            goodterm = 6 * G.pi * G.r() * (0.5 - solver::step(G.r() - G.b())) / sqrt(G.br);
        }
        T EminusK = 0;
        for (int i = 0; i < STARRY_EMINUSK_ORDER; i++)
            EminusK += STARRY_EMINUSK_COEFF[i] * G.ksq(i);
        EminusK *= -G.pi;
        T taylor = 2 * G.b(3) * sqrt(x) * (EminusK * (16 + 28 * eps + 14 * eps * eps) - eps * eps * (2 + 3 * eps) * G.ELL.K());
        T badterm = taylor + sqrt(G.br) * ((8 - 3 / G.br + 12 / G.b_r(1)) * G.ELL.K() - 16 * G.ELL.E());
        T Lambda = (badterm + goodterm) / (9 * G.pi);
        return (2. * G.pi / 3.) * (1 - 1.5 * Lambda - solver::step(G.r() - G.b()));
    }

    // Taylor expansion about b = 0
    template <typename T>
    inline T computeJ(solver::Greens<T>& G, int u, int v) {
        int i, j, k, l;
        T res = 0;
        T foo;
        T r1 = 1 - G.r(2);
        Vector<T> fac;
        fac.resize(STARRY_B_ORDER);
        fac(0) = r1 * sqrt(r1);
        for (i = 1; i < STARRY_B_ORDER; i++) fac(i) = fac(i - 1) / r1;
        l = 0;
        for (i = 0; i < STARRY_B_ORDER; i++) {
            foo = 0;
            for (j = 0; j < i / 2 + 1; j++) {
                k = (solver::is_even(i, 1) ? 2 * j : 2 * j + 1);
                foo += STARRY_B_COEFF[i][j] * G.r(k) * G.I(u, v + k) * fac(l + j);
            }
            l += (int) solver::is_even(i, 1);
            res += G.b(i) * foo;
        }
        return res;
    }

    // Approximate occultor limb as a quartic function
    template <typename T>
    inline T P(solver::Greens<T>& G) {
        if (!solver::is_even(G.mu / 2, 1))
            return 0;
        T res = 0;
        T amp = pow(G.b() - G.r(), G.nu / 2) / G.r();
        T jamp, jsum;
        T frac = (G.mu + 2.) / 4.;
        int twofrac = 1 + G.mu / 2;
        for (int i = 0; i <= G.nu / 2; i++) {
            jamp = fact::choose(G.nu / 2, i) * pow(2 * G.r() * (G.b() - G.r()), -i);
            jsum = 0;
            for (int j = 0; j <= i; j++) {
                jsum += fact::choose(i, j)
                      * pow(4, -j) / G.r(2 * j)
                      * G.coslam(twofrac + 2 * (i + j + 1))
                      * (1. / (frac + i + j + 1) + 0.5 * G.coslam(2) / (G.r(2) * (frac + i + j + 2)));
            }
            res += jamp * jsum;
        }
        return amp * res;

    }

    // Taylor expand M() at large k^2
    template <typename T>
    inline T computeM(solver::Greens<T>& G, int p, int q) {
        if ((p > STARRY_R_MAXPQ) || (q > STARRY_R_MAXPQ)) throw errors::BadTaylor();
        T res = 0;
        for (int i = 0; i < STARRY_R_ORDER; i++)
            res += STARRY_R_COEFF[p / 2][q / 2][i] * G.ksq(i);
        return res;
    }


}; // namespace taylor

#endif
