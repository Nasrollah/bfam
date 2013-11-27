#ifndef BFAM_DOMAIN_P8EST_H
#define BFAM_DOMAIN_P8EST_H

#include <bfam_base.h>
#include <bfam_domain.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#include <p4est_to_p8est.h>
#include <p8est.h>
#include <p8est_bits.h>
#include <p8est_extended.h>
#include <p8est_ghost.h>
#include <p8est_lnodes.h>
#include <p8est_mesh.h>
#include <p8est_vtk.h>
#pragma clang diagnostic pop

/**
 * structure containing a domain managed by p8est
 */
typedef struct bfam_domain_p8est
{
  bfam_domain_t         base;     /** parent domain */
  p8est_connectivity_t *conn;  /** connectivity for p8est */
  p8est_t              *p8est; /** forest of quadtrees */
} bfam_domain_p8est_t;

/* Domain managed by p8est based functions */
/** create a p8est managed domain
 *
 * \warning It is the callers responsibility to ensure that
 *          \a domComm and \a conn are freed after this domain is.
 *
 * \param [in]  domComm   pointer to the communicator for the domain
 * \param [in]  conn      pointer to the p8est connectivity for the domain
 *
 * \return the newly created p8est managed domain
 */
bfam_domain_p8est_t*
bfam_domain_p8est_new(MPI_Comm domComm,
                      p8est_connectivity_t *conn);

/** initializes a domain
 *
 * \param [in,out] domain  pointer to the p8est managed domain
 * \param [in]     domComm pointer to the communicator for the domain
 * \param [in]     conn    pointer to the p8est connectivity for the domain
 */
void
bfam_domain_p8est_init(bfam_domain_p8est_t *domain, MPI_Comm domComm,
                       p8est_connectivity_t *conn);

/** Clean up domain
 *
 * frees any memory allocated by the domain and calls free command on all
 * subdomains
 *
 * \param [in,out] domain domain to clean up
 */
void
bfam_domain_p8est_free(bfam_domain_p8est_t *domain);

/** Takes an initialized domain and generates a DG hex mesh
 *
 * \param [in,out] domain        pointer to the initialized p8est managed
 *                               domain
 * \param [in]     numSubdomains number of volume subdomains to generate
 * \param [in]     subdomainID   array of length \c p8est->local_num_quadrants
 *                               which indicates the subdomain id for each
 *                               element
 * \param [in]     N             array of orders for the volume subdomains
 */
void
bfam_domain_p8est_split_dgx_subdomains(bfam_domain_p8est_t *domain,
    bfam_locidx_t numSubdomains, bfam_locidx_t *subdomainID, int *N);

#endif
