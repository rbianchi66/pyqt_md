#include <utility>
#include "rectscover.h"

#ifdef _MSC_VER
#if _MSC_VER <= 1310
#define min std::_cpp_min
#define max std::_cpp_max
#else
using std::min;
using std::max;
#endif
#else
using std::min;
using std::max;
#endif

void rc_bBox(const std::vector<Path2d> &paths, P2d &minp, P2d &maxp, bool initp=true)
{
    for (unsigned int i=0; i<paths.size(); ++i)
    {
        const Path2d &path = paths[i];
        const int sz = path.size();
        for (int p=0; p < sz; ++p)
        {
            bool first = !(p|i) && initp;

            if (first || minp.x > path[p].x) minp.x = path[p].x;
            if (first || minp.y > path[p].y) minp.y = path[p].y;
            if (first || maxp.x < path[p].x) maxp.x = path[p].x;
            if (first || maxp.y < path[p].y) maxp.y = path[p].y;
        }
    }
}

coveringRect rect_in(Image<int>& orig, Image<int>& scan, int w, int h, int start_res)
{
    int resolution = start_res;
    int max_area = INT_MIN;
    int rect_x1 = 0, rect_x2 = 0, rect_y1 = 0, rect_y2 = 0;
    
    //Ricerca iniziale
    for(int x1 = 0; x1 < w; x1 += resolution)
    {
        for(int y1 = 0; y1 < h; y1 += resolution)
        {
            if (orig.pixel(x1, y1, 0) == 1)
                continue;
            
            for(int x2 = x1; x2 < w; x2 += resolution)
            {
                for(int y2 = y1; y2 < h; y2 += resolution)
                {
                    int area = (x2-x1) * (y2-y1);
                    
                    if ( area <= max_area )
                        continue;
                    
                    int diff = scan.pixel(x2, y2) - scan.pixel(x1-1, y2, 0) - scan.pixel(x2, y1-1 , 0) + scan.pixel(x1-1, y1-1 ,0);
                    
                    if (diff == 0)
                    {
                        rect_x1 = x1;
                        rect_x2 = x2;
                        rect_y1 = y1;
                        rect_y2 = y2;
                        max_area = area;
                    }
                }
            }
        }
    }
    
    // Aumento della risoluzione
    resolution = resolution/2;
    int extra = resolution % 2;
    while(resolution > 0)
    {
        int tmp = max(0, rect_x1-resolution);
        int diff = scan.pixel(rect_x2, rect_y2) - scan.pixel(tmp-1, rect_y2, 0) - scan.pixel(rect_x2, rect_y1-1 , 0) + scan.pixel(tmp-1, rect_y1-1 ,0);
        if (diff == 0)
            rect_x1 = tmp;
        
        tmp = max(0, rect_y1-resolution);
        diff = scan.pixel(rect_x2, rect_y2) - scan.pixel(rect_x1-1, rect_y2, 0) - scan.pixel(rect_x2, tmp-1 , 0) + scan.pixel(rect_x1-1, tmp-1 ,0);
        if (diff == 0)
            rect_y1 = tmp;
        
        tmp = min(w-1, rect_x2 + resolution);
        diff = scan.pixel(tmp, rect_y2) - scan.pixel(rect_x1-1, rect_y2, 0) - scan.pixel(tmp, rect_y1-1 , 0) + scan.pixel(rect_x1-1, rect_y1-1 ,0);
        if (diff == 0)
            rect_x2 = tmp;
        
        tmp = min(h-1, rect_y2 + resolution);
        diff = scan.pixel(rect_x2, tmp) - scan.pixel(rect_x1-1, tmp, 0) - scan.pixel(rect_x2, rect_y1-1 , 0) + scan.pixel(rect_x1-1, rect_y1-1 ,0);
        if (diff == 0)
            rect_y2 = tmp;
        
        resolution = resolution/2;
        extra += resolution%2;
    }
    
    // Recupero i passi saltati dovuti ai resti delle divisioni per 2
    resolution = 1;
    while(extra-- > 0)
    {
        int tmp = max(0, rect_x1-resolution);
        int diff = scan.pixel(rect_x2, rect_y2) - scan.pixel(tmp-1, rect_y2, 0) - scan.pixel(rect_x2, rect_y1-1 , 0) + scan.pixel(tmp-1, rect_y1-1 ,0);
        if (diff == 0)
            rect_x1 = tmp;
        
        tmp = max(0, rect_y1-resolution);
        diff = scan.pixel(rect_x2, rect_y2) - scan.pixel(rect_x1-1, rect_y2, 0) - scan.pixel(rect_x2, tmp-1 , 0) + scan.pixel(rect_x1-1, tmp-1 ,0);
        if (diff == 0)
            rect_y1 = tmp;
        
        tmp = min(w-1, rect_x2 + resolution);
        diff = scan.pixel(tmp, rect_y2) - scan.pixel(rect_x1-1, rect_y2, 0) - scan.pixel(tmp, rect_y1-1 , 0) + scan.pixel(rect_x1-1, rect_y1-1 ,0);
        if (diff == 0)
            rect_x2 = tmp;
        
        tmp = min(h-1, rect_y2 + resolution);
        diff = scan.pixel(rect_x2, tmp) - scan.pixel(rect_x1-1, tmp, 0) - scan.pixel(rect_x2, rect_y1-1 , 0) + scan.pixel(rect_x1-1, rect_y1-1 ,0);
        if (diff == 0)
            rect_y2 = tmp;
    }
    
    return coveringRect(rect_x1, rect_y1, rect_x2-rect_x1, rect_y2-rect_y1);
}

std::vector< coveringRect > rects_cover(const Path2d& p, int nrects, int start_res, bool safety)
{
    std::vector<Path2d> v;
    v.push_back(p);
    return rects_cover(v, nrects, start_res, safety);
}

std::vector< coveringRect > rects_cover(const std::vector<Path2d>& v, int nrects, int start_res, bool safety)
{
    std::vector< coveringRect > res;
    std::vector<Path2d> v_copy(v.size());
    
    P2d maxp, minp;
    
    rc_bBox(v, minp, maxp);
    for (int i = 0; i < int(v.size()); ++i)
        v_copy[i] = v[i] * X2d::xlate(-minp);
    
    int w = int(maxp.x-minp.x);
    int h = int(maxp.y-minp.y);
    
    start_res = min(start_res, min(w/2, h/2));
    
    for (int i = 0; i < nrects; ++i) // Ricerca nrects rettangoli
    {
        // Creo una bitmap dell'area
        Image<int> orig(w, h);
        polyFill(orig, 1, v_copy);
        orig.xorColor(1);
        
        // rasterizzazione dell'area "per eccesso"
        if(safety)
            polyDraw(orig, 1, v_copy);

        // Creo la sum table
        Image<int> scan(orig);
        for(int y = 0; y < h; ++y)
        {
            for(int x = 0; x < w; ++x)
            {
                int val = scan.pixel(x,y,0) + scan.pixel(x-1,y,0) + scan.pixel(x,y-1,0) - scan.pixel(x-1,y-1,0);
                scan.fastSetPixel(x,y, val);
            }
        }
        
        coveringRect tmp = rect_in(orig, scan, w, h, start_res);
        
        Path2d jf_rect(5); // just found rectangle
        jf_rect[0] = P2d(tmp.x, tmp.y);
        jf_rect[1] = P2d(tmp.x+tmp.w, tmp.y);
        jf_rect[2] = P2d(tmp.x+tmp.w, tmp.y+tmp.h);
        jf_rect[3] = P2d(tmp.x, tmp.y+tmp.h);
        jf_rect[4] = P2d(tmp.x, tmp.y);
        
        v_copy.push_back(jf_rect);
        
        // Quando ritorno il rettangolo coprente lo traslo alla posizione corretta
        tmp.x += minp.x;
        tmp.y += minp.y;
        res.push_back( tmp );
    }
    
    return res;
}
