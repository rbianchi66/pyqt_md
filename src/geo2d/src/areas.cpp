#include <algorithm>
#include <math.h>
#include "areas.h"
#include <stdlib.h>  // abs
#include <assert.h>
#include "poly2d.h"
#include <stdio.h>
#include <functional>
#include <cfloat>

static const int K = 65535;


// Disegna il path <p> nell'immagine <img> di larghezza <w> la cui origine
// corrisponde alle coordinate (<xx0>,<yy0>) usando il colore <color>
static void draw_line(const Path2d& p, int color, int xx0, int yy0,
    std::vector<int>& img, int w, bool w3 = false) {
    for (int j = 0, nj = p.size(); j < nj - 1; j++) {
        int ix0 = int(floor(p[j].x + 0.5));
        int iy0 = int(floor(p[j].y + 0.5));
        int ix1 = int(floor(p[j+1].x + 0.5));
        int iy1 = int(floor(p[j+1].y + 0.5));
        int ix = ix0 < ix1 ? 1 : -1;
        int iy = iy0 < iy1 ? 1 : -1;
        int dx = abs(ix1 - ix0);
        int dy = abs(iy1 - iy0);
        int m = dx > dy ? dx : dy;
        int cx = m / 2;
        int cy = m / 2;
        for (int ii = 0; ii <= m; ++ii) {
            int a = (iy0 - yy0) * w + (ix0 - xx0);
            if (img[a-w-1] < color) img[a-w-1] = color;
            if (img[a-w] < color) img[a-w] = color;
            if (img[a-w+1] < color) img[a-w+1] = color;
            if (img[a-1] < color) img[a-1] = color;
            if (img[a] < color) img[a] = color;
            if (w3) {
                if (img[a+1] < color) img[a+1] = color;
                if (img[a+w-1] < color) img[a+w-1] = color;
                if (img[a+w] < color) img[a+w] = color;
                if (img[a+w+1] < color) img[a+w+1] = color;
            }

            if ((cx += dx) >= m) { ix0 += ix; cx -= m; }
            if ((cy += dy) >= m) { iy0 += iy; cy -= m; }
        }
    }
}

// Esegue il flood_fill nell'immagine <img> di larghezza <w> a partire dal
// punto (<x>,<y>) e usando per il riempimento il colore <color>
void flood_fill(int x, int y, int color, std::vector<int>& img, int w) {
    std::vector<int> tx, ty;
    tx.push_back(x);
    ty.push_back(y);

    int h = img.size() / w;
    int ta = 0;
    int to_fill = img[y*w+x];
    while (tx.size() > 0) {
        int xx = tx.back();
        int yy = ty.back();
        tx.pop_back();
        ty.pop_back();
        if (img[yy*w+xx] == to_fill) {
            while (xx > 0 && img[yy*w+xx-1] == to_fill)
                --xx;
            bool check_above = true;
            bool check_below = true;
            ta -= xx;

            if (xx > 0) {
                if (yy > 0 && img[(yy-1)*w+xx-1] == to_fill) {
                    if (check_above) {
                       check_above = false;
                       tx.push_back(xx-1);
                       ty.push_back(yy-1);
                    } else {
                        // Nop
                    }
                } else {
                    check_above = true;
                }
                if (yy < h-1 && img[(yy+1)*w+xx-1] == to_fill) {
                    if (check_below) {
                       check_below = false;
                       tx.push_back(xx-1);
                       ty.push_back(yy+1);
                    } else {
                        // Nop
                    }
                } else {
                    check_below = true;
                }
            }

            while (xx < w && img[yy*w+xx] == to_fill) {
                if (yy > 0) {
                    if (img[(yy-1)*w+xx] == to_fill) {
                        if (check_above) {
                           check_above = false;
                           tx.push_back(xx);
                           ty.push_back(yy-1);
                        } else {
                            // Nop
                        }
                    } else {
                        check_above = true;
                    }
                }
                if (yy < h-1) {
                    if (img[(yy+1)*w+xx] == to_fill) {
                        if (check_below) {
                           check_below = false;
                           tx.push_back(xx);
                           ty.push_back(yy+1);
                        } else {
                            // Nop
                        }
                    } else {
                        check_below = true;
                    }
                }
                img[yy*w+xx] = color;
                xx++;
            }

            if (xx < w-1) {
                if (yy > 0 && img[(yy-1)*w+xx+1] == to_fill) {
                    if (check_above) {
                       check_above = false;
                       tx.push_back(xx+1);
                       ty.push_back(yy-1);
                    } else {
                        // Nop
                    }
                } else {
                    check_above = true;
                }
                if (yy < h-1 && img[(yy+1)*w+xx+1] == to_fill) {
                    if (check_below) {
                       check_below = false;
                       tx.push_back(xx+1);
                       ty.push_back(yy+1);
                    } else {
                        // Nop
                    }
                } else {
                    check_below = true;
                }
            }

            ta += xx;
        }
    }
}

static int cl(int x) {
    return x < K ? x : x / K;
}

static int dst(int x) {
    return x < K ? 0 : x % K;
}

static int cod(int col, int dst) {
    return col * K + dst;
}

// Erode il colore <color> nell'immagine <img> di larghezza <w>
// L'algoritmo lavora in place e assegna ai pixel di colore <color> il colore
// del pixel che ha la distanza manhattan piu' vicina. Per portarsi in giro
// queste due informazioni e salvarle in place usa le funzioni "cod" per
// codificarle in un intero e "cl" e "dst" per rileggerle.
// N.B.: Non valuta una cornice di 2 pixel attorno all'immagine: questo serve
// perchè la successiva marching_squares accede fino a 2 pixel prima e dopo
// il bounding box del colore
static void erode(int color, std::vector<int>& img, int w) {
    int h = int(img.size() / w);
    for(int i = 1; i < h - 2; ++i) {
        int rowidx = i * w;
        for(int j = 1; j < w - 2; ++j) {
            int idx = rowidx + j;
            if (img[idx] == color){
                // Colore 1, distanza w + h
                img[idx] = cod(1, w + h);
                if (dst(img[idx]) > dst(img[(i - 1) * w + j]) + 1) {
                    img[idx] = cod(cl(img[(i - 1) * w + j]),
                                   dst(img[(i - 1) * w + j]) + 1);
                }
                if (dst(img[idx]) > dst(img[i * w + j - 1]) + 1) {
                    img[idx] = cod(cl(img[i * w + j - 1]),
                                   dst(img[i * w + j - 1]) + 1);
                }
            }
        }
    }

    for(int i = h - 3; i >= 1; --i) {
        int rowidx = i * w;
        for(int j = w - 3; j >= 1; --j) {
            int idx = rowidx + j;
            if (img[idx] == color){
                if (dst(img[idx]) > dst(img[(i + 1) * w + j]) + 1) {
                    img[idx] = cod(cl(img[(i + 1) * w + j]),
                                   dst(img[(i + 1) * w + j]) + 1);
                }
                if (dst(img[idx]) > dst(img[i * w + j + 1]) + 1) {
                    img[idx] = cod(cl(img[i * w + j + 1]),
                                   dst(img[i * w + j + 1]) + 1);
                }
            }
        }
    }

    // Decodifica il colore (cioe' butta via la distanza)
    for(int i = 1; i < h - 2; ++i) {
        int rowidx = i * w;
        for(int j = 1; j < w - 2; ++j) {
            int idx = rowidx + j;
            img[idx] = cl(img[idx]);
        }
    }
}

/* Disegna su un'immagine i path ricevuti, usando i soli valori 0 e 1 (un certo
 * pixel ha valore 1 se e' contenuto in uno dei path, 0 altrimenti). Aggiunge
 * all'immagine una cornice di dimensione configurabile. */
template <typename ImgBaseTypeT>
inline Image<ImgBaseTypeT>* drawPathsImage(const std::vector<std::vector<Path2d> >& regions,
                                           int imgBorderSize, P2d* bboxMinOut = 0)
{
    // bounding box computation
    P2d bboxMin(INT_MAX, INT_MAX), bboxMax(INT_MIN, INT_MIN);
    boundingBox(regions, false, &bboxMin, &bboxMax);

    if (bboxMinOut)
        *bboxMinOut = bboxMin;

    if (bboxMin.x > bboxMax.x || bboxMin.y > bboxMax.y) // no input
        return NULL;

    // create the image and set its background color
    int x0 = int(bboxMin.x - imgBorderSize);
    int y0 = int(bboxMin.y - imgBorderSize);
    int x1 = int(bboxMax.x + imgBorderSize);
    int y1 = int(bboxMax.y + imgBorderSize);
    Image<ImgBaseTypeT>* img = new Image<ImgBaseTypeT>(x1 - x0, y1 - y0);
    img->fastBox(0, 0, img->w, img->h, (ImgBaseTypeT)0);
    X2d xf = X2d::xlate(P2d(-x0, -y0));

    // draw quality areas
    for (int i = 0; i < int(regions.size()); ++i)
    {
        const std::vector<Path2d>& paths = regions[i];
        std::vector<Path2d> transformedPaths;
        for (int j = 0; j < int(paths.size()); ++j)
            transformedPaths.push_back(paths[j] * xf);

        polyFill<ImgBaseTypeT>(*img, (ImgBaseTypeT)1, transformedPaths);
    }

    return img;
}

/* Riceve in input un vector di vector di path; ad ogni vector di path e'
 * associata un'area di qualita' diversa. Calcola un vector di path che
 * rappresenti l'unione dei path di tutte le aree di qualita'. */
std::vector<Path2d> unionAreaPaths(const std::vector<std::vector<Path2d> >& regions)
{
    std::vector<Path2d> result;
    P2d bboxMin;
    Image<int>* img = drawPathsImage<int>(regions, 8, &bboxMin);

    if (img)
    {
        marching_squares<std::equal_to<int> >(1, int(bboxMin.x - 8), int(bboxMin.y - 8),
                                              img->data, img->w, result);
        delete img;
    }

    return result;
}

/* Riceve in input un vector di vector di path; ad ogni vector di path e'
 * associata un'area di qualita' diversa. Calcola il valore di area dell'unione
 * dei path di tutte le aree di qualita'. */
double unionAreaValue(const std::vector<std::vector<Path2d> >& regions)
{
    double result = 0.0;
    Image<unsigned char>* img = drawPathsImage<unsigned char>(regions, 1);

    if (img)
    {
        for (int i = 0, n = img->w * img->h; i < n; ++i)
            result += img->data[i];

        delete img;
    }

    return result;
}

std::vector< std::vector<Path2d> > areas(
                          const std::vector<Path2d>& boundary,
                          const std::vector<Path2d>& lines,
                          const std::vector<QualityMarker>& markers,
                          const std::vector<Defect>& defects,
                          int qa, int qb,
                          double k,
                          bool dilate)
{
    std::vector< std::vector<Path2d> > result;

    // Calcolo bounding box
    double x0=1E30, y0=1E30, x1=-1E30, y1=-1E30;
    for (int i=0,ls=lines.size(),bs=boundary.size(),n=ls+bs+defects.size();
         i<n; i++)
    {
        const Path2d& p = i < ls ? lines[i] :
                          (i < ls+bs ? boundary[i-ls] :
                           defects[i-ls-bs].p);
        for (int j=0,m=p.size(); j<m; j++) {
            const P2d& pt = p[j];
            if (x0 > pt.x) x0 = pt.x;
            if (y0 > pt.y) y0 = pt.y;
            if (x1 < pt.x) x1 = pt.x;
            if (y1 < pt.y) y1 = pt.y;
        }
    }
    if (x0 > x1 || y0 > y1)
        // No input!
        return result;

    // Allocazione mappa inizializzata a 999
    int xx0 = int(x0 - 8);
    int yy0 = int(y0 - 8);
    int w = int(x1 - x0 + 16);
    int h = int(y1 - y0 + 16);
    Image<int> image(w, h);
    std::vector<int>& img = image.data;
    for (int i=0; i<w*h; i++) img[i] = 999;

    // Crea il pezzo con colore 0 partendo dal boundary
    std::vector<Path2d> tb;
    X2d xf = X2d::xlate(P2d(-xx0, -yy0));
    for (int i=0,n=boundary.size(); i<n; i++)
        tb.push_back(boundary[i] * xf);
    polyFill(image, 0, tb);

    // Disegno linee di confine usando colore 1000
    for (int i=0,n=lines.size(); i<n; i++) {
        draw_line(lines[i], 1000, xx0, yy0, img, w);
    }

    int mxq = 1;
    // Flood fill aree contrassegnate da un marker
    // NOTA: Un marker esterno al pezzo (o in un buco) non ha nessun
    //       effetto perche' la qualita' dell'esterno e' 999
    for (int i=0,nm=markers.size(); i<nm; i++)
    {
        const QualityMarker& m = markers[i];
        int ix = int(floor(m.p.x + 0.5)) - xx0;
        int iy = int(floor(m.p.y + 0.5)) - yy0;
        if (ix >= 0 && ix < w && iy >= 0 && iy < h && img[iy*w+ix] < m.q) {
            flood_fill(ix, iy, m.q, img, w);
            if (m.q > mxq) mxq = m.q;
        }
    }

    // Cerca aree chiuse restanti (hanno colore 0) e calcola il valore di
    // merito per classificazione automatica come A o B
    {
        std::vector<int> seed_x, seed_y;
        int best = -1;
        double best_value = -1;
        int kcol = -3;
        for (int y=0; y<h; y++)
            for (int x=0; x<w; x++)
                if (img[y*w+x] == 0)
                {
                    seed_x.push_back(x);
                    seed_y.push_back(y);
                    int aa = 0, pp = 0;
                    flood_fill(x, y, kcol, img, w);
                    for (int yy=1; yy<h-1; yy++)
                        for (int xx=1; xx<w-1; xx++)
                            if (img[yy*w+xx] == kcol)
                            {
                                ++aa;
                                if (img[(yy-1)*w+xx] != kcol ||
                                    img[(yy+1)*w+xx] != kcol ||
                                    img[yy*w+xx-1] != kcol ||
                                    img[yy*w+xx+1] != kcol)
                                    ++pp;
                            }
                    double v = aa - k*pp;
                    if (best == -1 || v > best_value)
                    {
                        best_value = v;
                        best = seed_x.size() - 1;
                    }
                    kcol--;
                }
        // Ok, la best area e' di qualita' A, le altre sono di qualita' B
        for (int i=0,n=seed_x.size(); i<n; i++) {
            int q = i == best ? qa : qb;
            flood_fill(seed_x[i], seed_y[i], q, img, w);
            if (q > mxq) mxq = q;
        }
    }
    // dilate delle aree colorate (ovvero != 1000)
    if (dilate) {
        erode(1000, img, w);
    }

    // Disegno difetti
    for (int i=0,n=defects.size(); i<n; i++) {
        const Defect& d = defects[i];
        draw_line(d.p, d.q, xx0, yy0, img, w, true);
    }

    // Generazione output per tutti i colori da 1 a max(q)
    for (int i=0,n=markers.size(); i<n; i++)
        if (mxq < markers[i].q) mxq = markers[i].q;
    for (int i=0,n=defects.size(); i<n; i++)
        if (mxq < defects[i].q) mxq = defects[i].q;
    for (int i = 1; i <= mxq; ++i)
    {
        result.push_back(std::vector<Path2d>());
        marching_squares<std::equal_to<int> >(i, xx0, yy0, img, w, result.back());
    }
    return result;
}
