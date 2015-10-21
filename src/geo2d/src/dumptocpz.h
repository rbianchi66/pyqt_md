#ifndef DUMPTOCPZ_H
#define DUMPTOCPZ_H
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include "geo2d.h"

/**
 * The CPZStream class provides a quick&dirty method to dump paths/points to a
 * CPZ file
 */
class CPZStream
{
    /**
     * The underlying stream
     */
    std::ostream & os;
    /**
     * Pieces dumped so far
     */
    unsigned int pzCounter;
    /**
     * Bounding box so far
     */
    P2d ul, lr;
    /**
     * Bounding box of "cuts" (G01) so far
     * Used to track if we need to add a bounding box to the current piece
     * (CalCon & co. expect all other lines to live inside a cut bound)
     */
    P2d cutUL, cutLR;
    /**
     * true if the Y must be negated when writing the data to file; useful 
     * since often paths inside GL/nester1/... have a reversed Y compared to
     * the direction used in CalCon
     */
    bool invertY;

    /**
     * the default pen if none is specified (used by the operator<<)
     */
    unsigned int defaultPen;
    /**
     * Bounding box hook; updates the bounding boxes
     *
     * @param[in]   p       point
     * @param[in]   pen     pen used for the point
     */
    void bbHook(P2d p, unsigned int pen)
    {
        if(!ul.isValid())
            ul=p;
        if(!lr.isValid())
            lr=p;
        ul.x=std::min(ul.x, p.x);
        ul.y=std::min(ul.y, p.y);
        lr.x=std::max(lr.x, p.x);
        lr.y=std::max(lr.y, p.y);
        if(pen==1 || pen==99)
            cutBBHook(p);
    }

    /**
     * Bounding box hook for "cutting" pens; do not call directly
     * 
     * @param[in]   p   point
     */
    void cutBBHook(P2d p)
    {
        if(!cutUL.isValid())
            cutUL=p;
        if(!cutLR.isValid())
            cutLR=p;
        cutUL.x=std::min(cutUL.x, p.x);
        cutUL.y=std::min(cutUL.y, p.y);
        cutLR.x=std::max(cutLR.x, p.x);
        cutLR.y=std::max(cutLR.y, p.y);
    }
public:
    /**
     * Construcs a CPZStream
     *
     * @param[in]   baseStream  underlying stream; must remain valid as far as
     *                          long as the object lives (CPZStream may even
     *                          write in the destructor)
     * @param[in]   invertY     true if the Y must be inverted in the output file
     */
    CPZStream(std::ostream &baseStream, bool invertY=false) :
        os(baseStream),
        pzCounter(0),
        ul(P2d::none()), lr(P2d::none()),
        cutUL(P2d::none()), cutLR(P2d::none()),
        invertY(invertY), defaultPen(5)
    { }

    /**
     * Destructor; automatically closes the last piece if it's still open
     */
    ~CPZStream()
    {
        if(ul!=P2d() || lr!=P2d())
            closePiece("autoclose");
    }

    /**
     * Writes a path on the file with the given pen
     *
     * @param[in]   p       path to write
     * @param[in]   pen     pen to use
     */
    void writePath2d(const Path2d & p, int pen=-1)
    {
        if(pen<0)
            pen=defaultPen;
        os<<"G"<<std::setw(2)<<std::setfill('0')<<pen<<" ";
        for(int i=0; i<p.size(); ++i)
        {
            os<<"X"<<p[i].x<<" Y"<<(invertY?-1:1)*p[i].y<<"\n";
            bbHook(p[i], pen);
        }
    }

    /**
     * Closes the current piece, adding the necessary LBN and LBS; if there's
     * no cut path (or if it's not big enough) it's automatically added.
     *
     * @param[in]   name        name of the piece; if left unspecified, it's
     *                          autogenerated
     * @param[in]   measure     the measure of the piece
     *
     */
    void closePiece(const char * name=NULL, double measure=1.)
    {
        if(ul!=cutUL || lr!=cutLR)
            addAutoBBoxPath();
        if(name!=NULL && *name)
            os<<"LBN "<<name<<"\n";
        else
            os<<"LBN AUTO_"<<pzCounter<<"\n";
        os<<"LBS "<<measure<<"\n";
        ++pzCounter;
        cutUL=cutLR=ul=lr=P2d();
    }

    /**
     * Writes a hole at the given position
     *
     * @param[in]   pos         position of the hole
     * @param[in]   diameter    diameter of the hole
     */
    void writeHole(P2d pos, double diameter=1.)
    {
        os<<"N10 X"<<pos.x<<" Y"<<(invertY?-1:1)*pos.y<<" D"<<diameter<<"\n";
        P2d delta(diameter, diameter);
        bbHook(pos-delta, 0);
        bbHook(pos+delta, 0);
    }

    /**
     * Cuts an automatic bounding box around the paths given so far for the
     * current piece.
     *
     * @param[in]   epsilon     extra gap around the bounding box
     */
    void addAutoBBoxPath(double epsilon=1.)
    {
        Path2d p;
        P2d eps(epsilon, epsilon);
        p.push_back(ul-eps);
        p.push_back(P2d(lr.x+epsilon, ul.y-epsilon));
        p.push_back(lr+eps);
        p.push_back(P2d(ul.x-epsilon, lr.y+epsilon));
        p.push_back(ul-eps);
        writePath2d(p, 1);
    }

    void setDefaultPen(unsigned int pen)
    {
        defaultPen=pen;
    }

    unsigned int getDefaultPen()
    {
        return defaultPen;
    }
};

/**
 * Output operator for CPZStream; writes a Path2d with the default pen
 *
 * @param[in]   s   target stream
 * @param[in]   p   path to write
 * @return          the passed stream
 */
inline CPZStream & operator<<(CPZStream &s, const Path2d &p)
{
    s.writePath2d(p);
    return s;
}

/**
 * Output operator for CPZStream; writes all the passed Path2d with the default
 * pen
 *
 * @param[in]   s   target stream
 * @param[in]   v   paths to write
 * @return          the passed stream
 */
inline CPZStream & operator<<(CPZStream &s, const std::vector<Path2d> &v)
{
    for(unsigned int i=0; i<v.size(); ++i)
        s.writePath2d(v[i]);
    return s;
}

/**
 * Output operator for CPZStream; punches a hole at the specified point with
 * the default hole size
 *
 * @param[in]   s   target stream
 * @param[in]   p   location of the hole
 * @return          the passed stream
 */
inline CPZStream & operator<<(CPZStream &s, P2d p)
{
    s.writeHole(p);
    return s;
}

#endif
