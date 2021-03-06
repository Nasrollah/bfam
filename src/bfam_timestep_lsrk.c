#include <bfam_timestep_lsrk.h>
#include <bfam_log.h>

#define BFAM_LSKR_PREFIX ("_lsrk_rate_")

typedef struct bfam_ts_allprefix
{
  bfam_ts_lsrk_t *ts;
  bfam_long_real_t arg;
  const char *rate_prefix;
  const char *field_prefix_lhs;
  const char *field_prefix_rhs;
} bfam_ts_lsrk_allprefix_t;

static int bfam_ts_lsrk_scale_rates(const char *key, void *val, void *arg)
{
  bfam_ts_lsrk_allprefix_t *data = (bfam_ts_lsrk_allprefix_t *)arg;
  bfam_subdomain_t *sub = (bfam_subdomain_t *)val;
  data->ts->scale_rates(sub, data->rate_prefix, data->arg);
  return 1;
}

static int bfam_ts_lsrk_intra_rhs(const char *key, void *val, void *arg)
{
  bfam_ts_lsrk_allprefix_t *data = (bfam_ts_lsrk_allprefix_t *)arg;
  bfam_subdomain_t *sub = (bfam_subdomain_t *)val;
  data->ts->intra_rhs(sub, data->rate_prefix, data->rate_prefix,
                      data->field_prefix_rhs, data->arg);
  return 1;
}

static int bfam_ts_lsrk_inter_rhs(const char *key, void *val, void *arg)
{
  bfam_ts_lsrk_allprefix_t *data = (bfam_ts_lsrk_allprefix_t *)arg;
  bfam_subdomain_t *sub = (bfam_subdomain_t *)val;
  data->ts->inter_rhs(sub, data->rate_prefix, data->rate_prefix,
                      data->field_prefix_rhs, data->arg);
  return 1;
}

static int bfam_ts_lsrk_add_rates(const char *key, void *val, void *arg)
{
  bfam_ts_lsrk_allprefix_t *data = (bfam_ts_lsrk_allprefix_t *)arg;
  bfam_subdomain_t *sub = (bfam_subdomain_t *)val;
  data->ts->add_rates(sub, data->field_prefix_lhs, data->field_prefix_rhs,
                      data->rate_prefix, data->arg);
  return 1;
}

void bfam_ts_lsrk_step_extended(bfam_ts_t *a_ts, bfam_long_real_t dt,
                                const char *rate_prefix,
                                const char *field_prefix_lhs,
                                const char *field_prefix_rhs)
{
  bfam_ts_lsrk_t *ts = (bfam_ts_lsrk_t *)a_ts;
  bfam_ts_lsrk_allprefix_t data;
  data.ts = ts;
  data.rate_prefix = rate_prefix;
  data.field_prefix_lhs = field_prefix_lhs;
  data.field_prefix_rhs = field_prefix_rhs;

  for (int s = 0; s < ts->nStages; s++)
  {
    /* set the stage time */
    bfam_long_real_t t = ts->t + ts->C[s] * dt;

    /*
     * start the communication
     */
    bfam_communicator_start(ts->comm);

    /*
     * scale the rate
     */
    data.arg = ts->A[s];
    bfam_dictionary_allprefixed_ptr(&ts->elems, "", &bfam_ts_lsrk_scale_rates,
                                    &data);

    /*
     * do the intra work
     */
    data.arg = t;
    bfam_dictionary_allprefixed_ptr(&ts->elems, "", &bfam_ts_lsrk_intra_rhs,
                                    &data);

    /*
     * finish the communication
     */
    bfam_communicator_finish(ts->comm);

    /*
     * do the inter work
     */
    data.arg = t;
    bfam_dictionary_allprefixed_ptr(&ts->elems, "", &bfam_ts_lsrk_inter_rhs,
                                    &data);

    /*
     * add the rate
     */
    data.arg = ts->B[s] * dt;
    bfam_dictionary_allprefixed_ptr(&ts->elems, "", &bfam_ts_lsrk_add_rates,
                                    &data);
  }
  ts->t += ts->C[ts->nStages] * dt;
}

void bfam_ts_lsrk_step(bfam_ts_t *a_ts, bfam_long_real_t dt)
{
  bfam_ts_lsrk_step_extended(a_ts, dt, BFAM_LSKR_PREFIX, "", "");
}

bfam_ts_lsrk_t *bfam_ts_lsrk_new(
    bfam_domain_t *dom, bfam_ts_lsrk_method_t method,
    bfam_domain_match_t subdom_match, const char **subdom_tags,
    bfam_domain_match_t comm_match, const char **comm_tags, MPI_Comm mpicomm,
    int mpitag, void *comm_data,
    void (*aux_rates)(bfam_subdomain_t *thisSubdomain, const char *prefix),
    void (*scale_rates)(bfam_subdomain_t *thisSubdomain,
                        const char *rate_prefix, const bfam_long_real_t a),
    void (*intra_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*inter_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*add_rates)(bfam_subdomain_t *thisSubdomain,
                      const char *field_prefix_lhs,
                      const char *field_prefix_rhs, const char *rate_prefix,
                      const bfam_long_real_t a))
{
  bfam_ts_lsrk_t *newTS = bfam_malloc(sizeof(bfam_ts_lsrk_t));
  bfam_ts_lsrk_init(newTS, dom, method, subdom_match, subdom_tags, comm_match,
                    comm_tags, mpicomm, mpitag, comm_data, aux_rates,
                    scale_rates, intra_rhs, inter_rhs, add_rates);
  return newTS;
}

void bfam_ts_lsrk_init(
    bfam_ts_lsrk_t *ts, bfam_domain_t *dom, bfam_ts_lsrk_method_t method,
    bfam_domain_match_t subdom_match, const char **subdom_tags,
    bfam_domain_match_t comm_match, const char **comm_tags, MPI_Comm mpicomm,
    int mpitag, void *comm_data,
    void (*aux_rates)(bfam_subdomain_t *thisSubdomain, const char *prefix),
    void (*scale_rates)(bfam_subdomain_t *thisSubdomain,
                        const char *rate_prefix, const bfam_long_real_t a),
    void (*intra_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*inter_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*add_rates)(bfam_subdomain_t *thisSubdomain,
                      const char *field_prefix_lhs,
                      const char *field_prefix_rhs, const char *rate_prefix,
                      const bfam_long_real_t a))
{
  bfam_ts_lsrk_init_extended(ts, dom, method, subdom_match, subdom_tags,
                             comm_match, comm_tags, mpicomm, mpitag, comm_data,
                             aux_rates, scale_rates, intra_rhs, inter_rhs,
                             add_rates, 1);
}

bfam_ts_lsrk_t *bfam_ts_lsrk_new_extended(
    bfam_domain_t *dom, bfam_ts_lsrk_method_t method,
    bfam_domain_match_t subdom_match, const char **subdom_tags,
    bfam_domain_match_t comm_match, const char **comm_tags, MPI_Comm mpicomm,
    int mpitag, void *comm_data,
    void (*aux_rates)(bfam_subdomain_t *thisSubdomain, const char *prefix),
    void (*scale_rates)(bfam_subdomain_t *thisSubdomain,
                        const char *rate_prefix, const bfam_long_real_t a),
    void (*intra_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*inter_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*add_rates)(bfam_subdomain_t *thisSubdomain,
                      const char *field_prefix_lhs,
                      const char *field_prefix_rhs, const char *rate_prefix,
                      const bfam_long_real_t a),
    int make_rates)
{
  bfam_ts_lsrk_t *newTS = bfam_malloc(sizeof(bfam_ts_lsrk_t));
  bfam_ts_lsrk_init_extended(newTS, dom, method, subdom_match, subdom_tags,
                             comm_match, comm_tags, mpicomm, mpitag, comm_data,
                             aux_rates, scale_rates, intra_rhs, inter_rhs,
                             add_rates, make_rates);
  return newTS;
}

void bfam_ts_lsrk_init_extended(
    bfam_ts_lsrk_t *ts, bfam_domain_t *dom, bfam_ts_lsrk_method_t method,
    bfam_domain_match_t subdom_match, const char **subdom_tags,
    bfam_domain_match_t comm_match, const char **comm_tags, MPI_Comm mpicomm,
    int mpitag, void *comm_data,
    void (*aux_rates)(bfam_subdomain_t *thisSubdomain, const char *prefix),
    void (*scale_rates)(bfam_subdomain_t *thisSubdomain,
                        const char *rate_prefix, const bfam_long_real_t a),
    void (*intra_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*inter_rhs)(bfam_subdomain_t *thisSubdomain, const char *rate_prefix,
                      const char *minus_rate_prefix, const char *field_prefix,
                      const bfam_long_real_t t),
    void (*add_rates)(bfam_subdomain_t *thisSubdomain,
                      const char *field_prefix_lhs,
                      const char *field_prefix_rhs, const char *rate_prefix,
                      const bfam_long_real_t a),
    int make_rates)
{
  BFAM_LDEBUG("LSRK INIT");

  /*
   * set up some preliminaries
   */
  bfam_ts_init(&ts->base, dom);
  bfam_dictionary_init(&ts->elems);
  ts->t = BFAM_LONG_REAL(0.0);
  if (make_rates)
  {
    ts->base.step = &bfam_ts_lsrk_step;
    ts->step_extended = NULL;
  }
  else
  {
    ts->base.step = NULL;
    ts->step_extended = bfam_ts_lsrk_step_extended;
  }

  /*
   * store the function calls
   */
  ts->scale_rates = scale_rates;
  ts->intra_rhs = intra_rhs;
  ts->inter_rhs = inter_rhs;
  ts->add_rates = add_rates;

  /*
   * get the subdomains and create rates we will need
   */
  bfam_subdomain_t *subs[dom->numSubdomains + 1];
  bfam_locidx_t numSubs = 0;
  bfam_domain_get_subdomains(dom, subdom_match, subdom_tags, dom->numSubdomains,
                             subs, &numSubs);
  for (int s = 0; s < numSubs; s++)
  {
    int rval = bfam_dictionary_insert_ptr(&ts->elems, subs[s]->name, subs[s]);
    BFAM_ABORT_IF_NOT(rval != 1, "Issue adding subdomain %s", subs[s]->name);

    if (make_rates)
      aux_rates(subs[s], BFAM_LSKR_PREFIX);
  }

  /*
   * Set up the communicator we will use
   */
  ts->comm = bfam_communicator_new(dom, comm_match, comm_tags, mpicomm, mpitag,
                                   comm_data);

  switch (method)
  {
  default:
    BFAM_WARNING("Invalid LSRK scheme, using KC54");
  case BFAM_TS_LSRK_KC54:
    ts->nStages = 5;
    ts->A = bfam_malloc_aligned(ts->nStages * sizeof(bfam_long_real_t));
    ts->B = bfam_malloc_aligned(ts->nStages * sizeof(bfam_long_real_t));
    ts->C = bfam_malloc_aligned((ts->nStages + 1) * sizeof(bfam_long_real_t));

    ts->A[0] = BFAM_LONG_REAL(0.0);
    ts->A[1] =
        BFAM_LONG_REAL(-567301805773.0) / BFAM_LONG_REAL(1357537059087.0);
    ts->A[2] =
        BFAM_LONG_REAL(-2404267990393.0) / BFAM_LONG_REAL(2016746695238.0);
    ts->A[3] =
        BFAM_LONG_REAL(-3550918686646.0) / BFAM_LONG_REAL(2091501179385.0);
    ts->A[4] =
        BFAM_LONG_REAL(-1275806237668.0) / BFAM_LONG_REAL(842570457699.0);

    ts->B[0] =
        BFAM_LONG_REAL(1432997174477.0) / BFAM_LONG_REAL(9575080441755.0);
    ts->B[1] =
        BFAM_LONG_REAL(5161836677717.0) / BFAM_LONG_REAL(13612068292357.0);
    ts->B[2] =
        BFAM_LONG_REAL(1720146321549.0) / BFAM_LONG_REAL(2090206949498.0);
    ts->B[3] =
        BFAM_LONG_REAL(3134564353537.0) / BFAM_LONG_REAL(4481467310338.0);
    ts->B[4] =
        BFAM_LONG_REAL(2277821191437.0) / BFAM_LONG_REAL(14882151754819.0);

    ts->C[0] = BFAM_LONG_REAL(0.0);
    ts->C[1] =
        BFAM_LONG_REAL(1432997174477.0) / BFAM_LONG_REAL(9575080441755.0);
    ts->C[2] =
        BFAM_LONG_REAL(2526269341429.0) / BFAM_LONG_REAL(6820363962896.0);
    ts->C[3] =
        BFAM_LONG_REAL(2006345519317.0) / BFAM_LONG_REAL(3224310063776.0);
    ts->C[4] =
        BFAM_LONG_REAL(2802321613138.0) / BFAM_LONG_REAL(2924317926251.0);
    ts->C[5] = BFAM_LONG_REAL(1.0);
    break;
  case BFAM_TS_LSRK_W33:
    ts->nStages = 3;
    ts->A = bfam_malloc_aligned(ts->nStages * sizeof(bfam_long_real_t));
    ts->B = bfam_malloc_aligned(ts->nStages * sizeof(bfam_long_real_t));
    ts->C = bfam_malloc_aligned((ts->nStages + 1) * sizeof(bfam_long_real_t));

    ts->A[0] = BFAM_LONG_REAL(0.0);
    ts->A[1] = BFAM_LONG_REAL(-5.0) / BFAM_LONG_REAL(9.0);
    ts->A[2] = BFAM_LONG_REAL(-153.0) / BFAM_LONG_REAL(128.0);

    ts->B[0] = BFAM_LONG_REAL(1.0) / BFAM_LONG_REAL(3.0);
    ts->B[1] = BFAM_LONG_REAL(15.0) / BFAM_LONG_REAL(16.0);
    ts->B[2] = BFAM_LONG_REAL(8.0) / BFAM_LONG_REAL(15.0);

    ts->C[0] = BFAM_LONG_REAL(0.0);
    ts->C[1] = BFAM_LONG_REAL(1.0) / BFAM_LONG_REAL(3.0);
    ts->C[2] = BFAM_LONG_REAL(3.0) / BFAM_LONG_REAL(4.0);
    ts->C[3] = BFAM_LONG_REAL(1.0);
    break;
  case BFAM_TS_LSRK_HEUN:
    ts->nStages = 2;
    ts->A = bfam_malloc_aligned(ts->nStages * sizeof(bfam_long_real_t));
    ts->B = bfam_malloc_aligned(ts->nStages * sizeof(bfam_long_real_t));
    ts->C = bfam_malloc_aligned((ts->nStages + 1) * sizeof(bfam_long_real_t));

    ts->A[0] = BFAM_LONG_REAL(0.0);
    ts->A[1] = -BFAM_LONG_REAL(1.0);

    ts->B[0] = BFAM_LONG_REAL(1.0);
    ts->B[1] = BFAM_LONG_REAL(1.0) / BFAM_LONG_REAL(2.0);

    ts->C[0] = BFAM_LONG_REAL(0.0);
    ts->C[1] = BFAM_LONG_REAL(1.0);
    ts->C[2] = BFAM_LONG_REAL(1.0);
    break;
  case BFAM_TS_LSRK_FE:
    ts->nStages = 1;
    ts->A = bfam_malloc_aligned(sizeof(bfam_long_real_t));
    ts->B = bfam_malloc_aligned(sizeof(bfam_long_real_t));
    ts->C = bfam_malloc_aligned((ts->nStages + 1) * sizeof(bfam_long_real_t));

    ts->A[0] = BFAM_LONG_REAL(0.0);

    ts->B[0] = BFAM_LONG_REAL(1.0);

    ts->C[0] = BFAM_LONG_REAL(0.0);
    ts->C[1] = BFAM_LONG_REAL(1.0);
    break;
  }
}

void bfam_ts_lsrk_free(bfam_ts_lsrk_t *ts)
{
  BFAM_LDEBUG("LSRK FREE");
  bfam_communicator_free(ts->comm);
  bfam_free(ts->comm);
  ts->comm = NULL;
  bfam_dictionary_clear(&ts->elems);
  bfam_free_aligned(ts->A);
  ts->A = NULL;
  bfam_free_aligned(ts->B);
  ts->B = NULL;
  bfam_free_aligned(ts->C);
  ts->C = NULL;
  ts->nStages = 0;
  ts->t = NAN;
  bfam_ts_free(&ts->base);
}

void bfam_ts_lsrk_set_time(bfam_ts_lsrk_t *ts, bfam_long_real_t time)
{
  ts->t = time;
}

bfam_long_real_t bfam_ts_lsrk_get_time(bfam_ts_lsrk_t *ts) { return ts->t; }
