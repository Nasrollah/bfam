#include <bfam.h>

int
check_approx_eq(bfam_long_real_t eps, size_t nx, bfam_long_real_t *P,
    bfam_long_real_t *Pexact)
{
  int failure = 0;
  bfam_long_real_t diff = 0;
  for (size_t i=0; i<nx; ++i)
  {
    diff = BFAM_MAX(BFAM_LONG_REAL_ABS(P[i] - Pexact[i]), diff);
  }

  if (diff > eps)
  {
    failure = 1;
    BFAM_LERROR("Failed Test: %"BFAM_LONG_REAL_PRIe, diff);
  }

  return failure;
}

bfam_long_real_t P_0_0_0[] = {
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
  7.071067811865475e-01,
};

bfam_long_real_t dP_0_0_0[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};

bfam_long_real_t P_0_0_1[] = {
  -1.224744871391589e+00,
  -9.525793444156804e-01,
  -6.804138174397717e-01,
  -4.082482904638631e-01,
  -1.360827634879544e-01,
   1.360827634879544e-01,
   4.082482904638629e-01,
   6.804138174397717e-01,
   9.525793444156803e-01,
   1.224744871391589e+00,
};

bfam_long_real_t dP_0_0_1[] = {
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
  1.224744871391589e+00,
};

bfam_long_real_t P_0_0_2[] = {
   1.581138830084190e+00,
   6.441676715157812e-01,
  -5.856069741052537e-02,
  -5.270462766947297e-01,
  -7.612890663368320e-01,
  -7.612890663368320e-01,
  -5.270462766947299e-01,
  -5.856069741052537e-02,
   6.441676715157808e-01,
   1.581138830084190e+00,
};

bfam_long_real_t dP_0_0_2[] = {
  -4.743416490252569e+00,
  -3.689323936863109e+00,
  -2.635231383473650e+00,
  -1.581138830084190e+00,
  -5.270462766947301e-01,
   5.270462766947301e-01,
   1.581138830084189e+00,
   2.635231383473650e+00,
   3.689323936863109e+00,
   4.743416490252569e+00,
};

bfam_long_real_t P_0_0_3[] = {
  -1.870828693386972e+00,
  -1.796406152772192e-02,
   7.570568786682527e-01,
   7.621894676761732e-01,
   3.053890459712615e-01,
  -3.053890459712615e-01,
  -7.621894676761731e-01,
  -7.570568786682527e-01,
   1.796406152772115e-02,
   1.870828693386972e+00,
};

bfam_long_real_t dP_0_0_3[] = {
   1.122497216032182e+01,
   5.681776031767837e+00,
   1.524378935352347e+00,
  -1.247219128924647e+00,
  -2.633018161063144e+00,
  -2.633018161063144e+00,
  -1.247219128924648e+00,
   1.524378935352347e+00,
   5.681776031767836e+00,
   1.122497216032182e+01,
};

bfam_long_real_t P_2_1_3[] = {
  -3.872983346207417e+00,
  -5.817444120846532e-01,
   7.012809351157466e-01,
   6.454972243679030e-01,
  -7.969101535406189e-02,
  -8.048792550760273e-01,
  -8.606629658238709e-01,
   4.223623813765293e-01,
   3.713601315499292e+00,
   9.682458365518544e+00,
};

bfam_long_real_t dP_2_1_3[] = {
   2.033316256758894e+01,
   9.790041236246527e+00,
   2.259240285287661e+00,
  -2.259240285287660e+00,
  -3.765400475479434e+00,
  -2.259240285287660e+00,
   2.259240285287657e+00,
   9.790041236246529e+00,
   2.033316256758894e+01,
   3.388860427931490e+01,
};

bfam_long_real_t P_m025_2_5[] = {
  -2.779196835777356e+01,
  -3.496214727847906e+00,
   1.840744719386286e+00,
   7.110677426982215e-01,
  -6.853585653210824e-01,
  -5.691663775602251e-01,
   2.902442683111531e-01,
   4.464568705409493e-01,
  -3.522101360068282e-01,
   7.090480208465177e-01,
};

bfam_long_real_t dP_m025_2_5[] = {
   1.794897956439543e+02,
   5.452374053955874e+01,
   2.737631449573485e+00,
  -8.395190275375201e+00,
  -2.978501981517381e+00,
   3.306800288060472e+00,
   3.202701793415466e+00,
  -2.125930902251689e+00,
  -3.091349932596684e+00,
   1.831707387186839e+01,
};

bfam_long_real_t x_0_0_0[] = {
  0,
};

bfam_long_real_t w_0_0_0[] = {
  2,
};

bfam_long_real_t x_0_0_1[] = {
  -5.773502691896257e-01,
   5.773502691896257e-01,
};

bfam_long_real_t w_0_0_1[] = {
  1,
  1,
};

bfam_long_real_t x_0_0_2[] = {
  -7.745966692414833e-01,
  -4.510281037539698e-17,
   7.745966692414834e-01,
};

bfam_long_real_t w_0_0_2[] = {
  5.555555555555558e-01,
  8.888888888888888e-01,
  5.555555555555558e-01,
};

bfam_long_real_t x_0_0_3[] = {
  -8.611363115940526e-01,
  -3.399810435848562e-01,
   3.399810435848562e-01,
   8.611363115940526e-01,
};

bfam_long_real_t w_0_0_3[] = {
  3.478548451374542e-01,
  6.521451548625463e-01,
  6.521451548625460e-01,
  3.478548451374541e-01,
};

bfam_long_real_t x_2_1_3[] = {
  -7.972962734001832e-01,
  -3.734893787362535e-01,
   1.563704318081081e-01,
   6.507788566919653e-01,
};

bfam_long_real_t w_2_1_3[] = {
   2.152837051737072e-01,
   5.901533609926285e-01,
   4.412333545929730e-01,
   8.666291257402459e-02,
};

bfam_long_real_t x_m025_2_5[] = {
  -7.660861640239679e-01,
  -4.138362888665464e-01,
   8.651447739158399e-03,
   4.262081219957594e-01,
   7.642887387206505e-01,
   9.625923262531283e-01,
};

bfam_long_real_t w_m025_2_5[] = {
   1.399672365675630e-02,
   1.257921701397209e-01,
   4.414593129590954e-01,
   9.102572180512539e-01,
   1.234921457628804e+00,
   1.001183200766983e+00,
};

bfam_long_real_t x_53_42_5[] = {
  -7.507512819352168e-01,
  -5.013308638210570e-01,
  -2.133612113479640e-01,
   9.274200753161091e-02,
   3.944407269823559e-01,
   6.712838784042245e-01,
};

bfam_long_real_t w_53_42_5[] = {
   1.280567792431395e-02,
   1.259542220360614e-01,
   3.059197468129359e-01,
   2.665632830269100e-01,
   8.271746649287130e-02,
   6.176833476425957e-03,
};

bfam_long_real_t xl_0_0_1[] = {
  -1,
   1,
};

bfam_long_real_t wl_0_0_1[] = {
  1,
  1,
};

bfam_long_real_t xl_0_0_2[] = {
  -1,
   0,
   1,
};

bfam_long_real_t wl_0_0_2[] = {
  3.333333333333333e-01,
  1.333333333333333e+00,
  3.333333333333333e-01,
};

bfam_long_real_t xl_0_0_3[] = {
  -1.000000000000000e+00,
  -4.472135954999579e-01,
   4.472135954999579e-01,
   1.000000000000000e+00,
};

bfam_long_real_t wl_0_0_3[] = {
   1.666666666666665e-01,
   8.333333333333333e-01,
   8.333333333333333e-01,
   1.666666666666665e-01,
};

bfam_long_real_t xl_2_1_4[] = {
  -1.000000000000000e+00,
  -6.295006461249313e-01,
  -1.043853303832391e-01,
   4.611587037808977e-01,
   1.000000000000000e+00,
};

bfam_long_real_t wl_2_1_4[] = {
  2.857142857142852e-02,
  4.494303739909380e-01,
  6.227419917318147e-01,
  2.278276342772477e-01,
  4.761904761904756e-03,
};

bfam_long_real_t xl_m025_2_5[] = {
  -1.000000000000000e+00,
  -5.307980844806093e-01,
  -2.306266853114286e-02,
   4.744619081112718e-01,
   8.453562917089907e-01,
   1.000000000000000e+00,
};

bfam_long_real_t wl_m025_2_5[] = {
   1.277971354690001e-03,
   9.372630724544165e-02,
   4.962293308306567e-01,
   1.154571944617246e+00,
   1.490954927280840e+00,
   4.908496018737385e-01,
};

bfam_long_real_t xl_53_42_5[] = {
  -1.000000000000000e+00,
  -5.890484263175890e-01,
  -2.454557217954551e-01,
   1.217715902667813e-01,
   4.870915322052373e-01,
   1.000000000000000e+00,
};

bfam_long_real_t wl_53_42_5[] = {
   4.780465708855393e-04,
   9.031675630663250e-02,
   3.526252636979745e-01,
   3.014454480230028e-01,
   5.513887100666661e-02,
   1.328441643536374e-04,
};


void print_quadrature(bfam_long_real_t alpha, bfam_long_real_t beta,
    int N, bfam_long_real_t *restrict x, bfam_long_real_t *restrict w)
{
  BFAM_INFO("Quadrature for %"BFAM_LONG_REAL_PRIe
      ":%"BFAM_LONG_REAL_PRIe ":%d", alpha, beta, N);

  for (int n=0; n <=N; ++n)
  {
    BFAM_INFO("  %"BFAM_LONG_REAL_PRIe "  %"BFAM_LONG_REAL_PRIe, x[n], w[n]);
  }
}


int
main (int argc, char *argv[])
{
  int failures = 0;

#define LIST_OF_TESTS  \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 0, 10, P_0_0_0,    dP_0_0_0)   \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 1, 10, P_0_0_1,    dP_0_0_1)   \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 2, 10, P_0_0_2,    dP_0_0_2)   \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 3, 10, P_0_0_3,    dP_0_0_3)   \
  X(BFAM_LONG_REAL( 2.0),  BFAM_LONG_REAL(1.0), 3, 10, P_2_1_3,    dP_2_1_3)   \
  X(BFAM_LONG_REAL(-0.25), BFAM_LONG_REAL(2.0), 5, 10, P_m025_2_5, dP_m025_2_5)

#define X(alpha, beta, N, nx, Pexact, dPexact)                       \
  {                                                                  \
    BFAM_INFO("Testing " #alpha " : " #beta " : " #N " : " #nx       \
              " : " #Pexact " : " #dPexact);                         \
    BFAM_ALIGN(32) bfam_long_real_t x[nx];                           \
    BFAM_ALIGN(32) bfam_long_real_t P[nx];                           \
    BFAM_ALIGN(32) bfam_long_real_t dP[nx];                          \
    for (size_t i=0; i<(nx); ++i)                                    \
    {                                                                \
      x[i] = BFAM_LONG_REAL(-1.0) + i*BFAM_LONG_REAL(2.0)/(nx-1);    \
    }                                                                \
    bfam_jacobi_p(alpha, beta, N, nx, x, P);                         \
    bfam_grad_jacobi_p(alpha, beta, N, nx, x, dP);                   \
    failures += check_approx_eq(BFAM_REAL_EPS*100, nx,  P,  Pexact); \
    failures += check_approx_eq(BFAM_REAL_EPS*100, nx, dP, dPexact); \
  }
  LIST_OF_TESTS
#undef X
#undef LIST_OF_TESTS

#define LIST_OF_TESTS  \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 0, x_0_0_0,    w_0_0_0)    \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 1, x_0_0_1,    w_0_0_1)    \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 2, x_0_0_2,    w_0_0_2)    \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 3, x_0_0_3,    w_0_0_3)    \
  X(BFAM_LONG_REAL( 2.0),  BFAM_LONG_REAL(1.0), 3, x_2_1_3,    w_2_1_3)    \
  X(BFAM_LONG_REAL(-0.25), BFAM_LONG_REAL(2.0), 5, x_m025_2_5, w_m025_2_5) \
  X(BFAM_LONG_REAL( 5.3),  BFAM_LONG_REAL(4.2), 5, x_53_42_5,  w_53_42_5)

#define X(alpha, beta, N, xexact, wexact)                            \
  {                                                                  \
    BFAM_INFO("Testing Gauss Quadrature " #alpha " : " #beta         \
        " : " #N " : " #xexact " : " #wexact);                       \
    BFAM_ALIGN(32) bfam_long_real_t x[N+1];                          \
    BFAM_ALIGN(32) bfam_long_real_t w[N+1];                          \
    bfam_jacobi_gauss_quadrature(alpha, beta, N, x, w);              \
    print_quadrature(alpha, beta, N, x, w);                          \
    failures += check_approx_eq(BFAM_REAL_EPS*100, N+1, x, xexact);  \
    failures += check_approx_eq(BFAM_REAL_EPS*100, N+1, w, wexact);  \
  }
  LIST_OF_TESTS
#undef X
#undef LIST_OF_TESTS

#define LIST_OF_TESTS  \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 1, xl_0_0_1,    wl_0_0_1)    \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 2, xl_0_0_2,    wl_0_0_2)    \
  X(BFAM_LONG_REAL( 0.0),  BFAM_LONG_REAL(0.0), 3, xl_0_0_3,    wl_0_0_3)    \
  X(BFAM_LONG_REAL( 2.0),  BFAM_LONG_REAL(1.0), 4, xl_2_1_4,    wl_2_1_4)    \
  X(BFAM_LONG_REAL(-0.25), BFAM_LONG_REAL(2.0), 5, xl_m025_2_5, wl_m025_2_5) \
  X(BFAM_LONG_REAL( 5.3),  BFAM_LONG_REAL(4.2), 5, xl_53_42_5,  wl_53_42_5)

#define X(alpha, beta, N, xexact, wexact)                            \
  {                                                                  \
    BFAM_INFO("Testing Gauss Quadrature " #alpha " : " #beta         \
        " : " #N " : " #xexact " : " #wexact);                       \
    BFAM_ALIGN(32) bfam_long_real_t x[N+1];                          \
    BFAM_ALIGN(32) bfam_long_real_t w[N+1];                          \
    bfam_jacobi_gauss_lobatto_quadrature(alpha, beta, N, x, w);              \
    print_quadrature(alpha, beta, N, x, w);                          \
    failures += check_approx_eq(BFAM_REAL_EPS*100, N+1, x, xexact);  \
    failures += check_approx_eq(BFAM_REAL_EPS*100, N+1, w, wexact);  \
  }
  LIST_OF_TESTS
#undef X
#undef LIST_OF_TESTS

  if(failures)
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}