#ifndef _offset_h_
#define _offset_h_

#include <vector>
#include "geo2d.h"

// simplify e simplify_contained sono state spostate in simplify.h/.cpp, ma molti client
// si aspettano che siano ancora in offset.h; per intanto aggiungiamo l'#include qui
#include "simplify.h"

typedef std::vector<P2d>    p2d_array;
typedef p2d_array::iterator  p2d_array_ite;

typedef std::vector<double> dist_array;
typedef dist_array::iterator dist_array_ite;

typedef std::vector<Path2d> path2d_array;
typedef path2d_array::iterator path2d_array_ite;

typedef std::vector<p2d_array> vectorp2d_array;
typedef vectorp2d_array::iterator vectorp2d_array_ite;

enum TipoOffset
{
    SPIGOLO_VIVO = 0,
    SMUSSATA = 1
};

Path2d raccordo(P2d o, P2d p1, P2d p2, double delta);

Path2d para_smussa(const Path2d &pts_in, double dist, bool chiusa);
Path2d para(const Path2d &pts, double dist, bool chiusa);
Path2d para_external(const Path2d &pts, double dist, bool chiusa);
Path2d para_internal(const Path2d &pts, double dist, bool chiusa);

path2d_array offset(Path2d &pts, const dist_array& d, int tipo, double soglia_unibili);

P2d nd(const Path2d &pts, int i, double d, bool chiusa);

/**
 * @brief Performs an enlargement on the given area
 *
 * The enlargement is performed by expanding external borders by %dist using
 * %para_external and shrinking internal ones with %para_internal.
 *
 * This has its shortcomings (it may generate holes inside the area if the
 * new paths come in touch), but should work reasonably well for "normal" geometries
 * and small offsets.
 *
 * @remark Paths in %area must not intersecate.
 *
 * @param[in]   paths       Paths describing the area (odd-even rule)
 * @param[in]   dist        Expansion distance
 * @param[in]   simplifyEps Epsilon for the preventive %simplify applied to the paths; set
 *                          to a negative value to disable the simplification
 */
std::vector<Path2d> enlarge_area(std::vector<Path2d> paths, double dist, double simplifyEps=0.01);

#endif
