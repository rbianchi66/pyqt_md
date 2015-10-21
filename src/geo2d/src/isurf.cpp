#include "isurf.h"

namespace {
    // Uniform cubic B-Spline quasi-interpolation
    void qibspline(std::vector<P2d>& pts, int levels)
    {
        int n = pts.size();
        int new_n = ((n - 1) << levels) + 1;

        // The result is allocated once at full resolution,
        // computation will fill the gaps
        std::vector<P2d> result(new_n);
        int step = 1 << levels;

        // Initial quasi-interpolation setup with - 1/6 + 4/3 - 1/6
        result[0] = pts[0];
        result[(n-1)*step] = pts[n-1];
        for (int i=1; i<n-1; i++)
            result[i*step] = pts[i]*1.333333 - (pts[i-1] + pts[i+1])*0.1666666;

        // Uniform B-spline interpolation
        while (step > 1)
        {
            int hstep = step >> 1;
            P2d oldp = result[0];
            for (int i=step; i<new_n-1; i+=step)
            {
                P2d newp = (oldp + 6*result[i] + result[i+step]) * 0.125;
                result[i - hstep] = (oldp + result[i]) * 0.5;
                oldp = result[i]; result[i] = newp;
            }
            result[new_n -1 - hstep] = (oldp + result[new_n - 1]) * 0.5;
            step = hstep;
        }
        pts.swap(result);
    }

    // Uniform cubic B-Spline interpolation
    void ibspline(std::vector<P2d>& pts, int levels, int passes)
    {
        int n = pts.size();
        std::vector<P2d> delta(n);
        for (int p=0; p<passes; p++)
        {
            std::vector<P2d> result(n);
            for (int i=0; i<n; i++)
                result[i] = pts[i] + delta[i];
            qibspline(result, levels);
            int k = (result.size() - 1) / (pts.size() - 1);
            for (int i=0; i<n; i++)
                delta[i] += pts[i] - result[i*k];
        }
        for (int i=0; i<n; i++)
            pts[i] += delta[i];
        qibspline(pts, levels);
    }
}

ISurf::ISurf(const std::vector< std::vector<P2d> >& pts,
             int ext_mode, int levels, int passes)
    : levels(levels), passes(passes)
{
    //
    // Quadratic extrapolation scheme
    //
    // x0 + (x0 - x1) + ((x0 - x1) - (x1 - x2))
    // 3*x0 - 3*x1 + x2
    //
    // Linear extrapolation scheme
    //
    // x0 + (x0 - x1)
    // 2*x0 - x1
    //
    int rows = pts.size();
    int cols = pts[0].size();
    this->pts.resize(rows + 2);
    for (int i=0; i<rows+2; i++)
        this->pts[i].resize(cols+2);

    // Fill internal part
    for (int i=0; i<rows; i++)
        for (int j=0; j<cols; j++)
            this->pts[i+1][j+1] = pts[i][j];

    // Extrapolate first and last row
    for (int i=0; i<cols; i++)
        if (ext_mode == 2)
        {
            this->pts[0][i+1] = (pts[0][i]*3
                                 - pts[1][i]*3
                                 + pts[2][i]);
            this->pts[rows+1][i+1] = (pts[rows-1][i]*3
                                      - pts[rows-2][i]*3
                                      + pts[rows-3][i]);
        }
        else
        {
            this->pts[0][i+1] = (pts[0][i]*2
                                 - pts[1][i]);
            this->pts[rows+1][i+1] = (pts[rows-1][i]*2
                                      - pts[rows-2][i]);
        }

    // Extrapolate first and last column
    for (int i=0; i<rows+2; i++)
        if (ext_mode == 2)
        {
            this->pts[i][0] = (this->pts[i][1]*3
                               - this->pts[i][2]*3
                               + this->pts[i][3]);
            this->pts[i][cols+1] = (this->pts[i][cols]*3
                                    - this->pts[i][cols-1]*3
                                    + this->pts[i][cols-2]);
        }
        else
        {
            this->pts[i][0] = (this->pts[i][1]*2
                               - this->pts[i][2]);
            this->pts[i][cols+1] = (this->pts[i][cols]*2
                                    - this->pts[i][cols-1]);
        }

    if (idata.size() == 0)
    {
        // Row spline interpolation
        idata.resize(rows+2);
        for (int i=0; i<rows+2; i++)
        {
            idata[i] = this->pts[i];
            ibspline(idata[i], levels, passes);
        }

        // Column spline interpolation
        int csz = idata[0].size();
        for (int j=0; j<csz; j++)
        {
            std::vector<P2d> col(rows+2);
            for (int i=0; i<rows+2; i++)
                col[i] = idata[i][j];
            ibspline(col, levels, passes);
            int nr = col.size();
            if (int(idata.size()) < nr)
            {
                // Matrix extension
                idata.resize(nr);
                for (int i=rows+2; i<nr; i++)
                    idata[i].resize(csz);
            }
            for (int i=0,n=col.size(); i<n; i++)
                idata[i][j] = col[i];
        }
    }
}

P2d ISurf::operator()(double ii, double jj) const
{
    int n = pts.size();
    int m = pts[0].size();
    int nn = idata.size();
    int mm = idata[0].size();

    double i = (ii + 1) / (n - 1) * (nn - 1);
    double j = (jj + 1) / (m - 1) * (mm - 1);
    int i0 = int(floor(i)); if (i0 < 0) i0 = 0; if (i0 > nn-2) i0 = nn-2;
    int j0 = int(floor(j)); if (j0 < 0) j0 = 0; if (j0 > mm-2) j0 = mm-2;
    double si = i - i0, sj = j - j0;
    P2d v0 = idata[i0][j0]*(1 - si) + idata[i0+1][j0]*si;
    P2d v1 = idata[i0][j0+1]*(1 - si) + idata[i0+1][j0+1]*si;
    return v0*(1 - sj) + v1*sj;
}
