#include "placement.h"
#include <math.h>
#include <stdio.h>

int main()
{
    const int N = 100;
    Path2d r;
    for (int i=0; i<N; i++)
    {
      double t = i*2*3.141592654/N;
      double rr = 70 + 30*cos(t*3);
      r.push_back(P2d(rr*cos(t), rr*sin(t)));
    }
    std::vector<Path2d> b;
    b.push_back(r);

    Placement::Part p(b);
    Placement::Sheet s(1000, 1500);
    FILE *f = fopen("out.plt", "w");

    fprintf(f, "IN;\n");

    X2d xf;
    int n = 0;
    //
    //  4_
    // 0|_|1
    // 2|_|3
    //  6
    //
    const char *digits[10] = { "012346", "13", "41526", "41536", "0513",
                               "40536", "023456", "413", "0123456", "013456" };
    const char *segment = "PU%0.3f,%0.3f;\nPD%0.3f,%0.3f;\n";
    const int H = 8;
    const int HS = 4;
    while (s.addPart(&p, xf))
    {
       n++;
       printf("."); fflush(stdout);
       fprintf(f, "SP1;\nPA;\n");
       double x0=0, y0=0, x1=0, y1=0;
       for (int i=0; i<=r.size(); i++)
       {
           P2d xp = r[i%r.size()]*xf;
           if (i==0) { x0 = x1 = xp.x; y0 = y1 = xp.y; }
           if (xp.x < x0) x0 = xp.x;
           if (xp.x > x1) x1 = xp.x;
           if (xp.y < y0) y0 = xp.y;
           if (xp.y > y1) y1 = xp.y;
           if (i == 0)
                fprintf(f, "PU%0.3f,%0.3f;\n", xp.x, xp.y);
           else
                fprintf(f, "PD%0.3f,%0.3f;\n", xp.x, xp.y);
       }
       double x = (x0 + x1)/2;
       double y = (y0 + y1)/2;
       x += H/2 + HS;
       for (int i=0,nn=n; i<3; i++)
       {
           const char *d = digits[nn%10];
           if (strchr(d, '0')) fprintf(f, segment, x, y, x, y-H);
           if (strchr(d, '1')) fprintf(f, segment, x+H, y, x+H, y-H);
           if (strchr(d, '2')) fprintf(f, segment, x, y, x, y+H);
           if (strchr(d, '3')) fprintf(f, segment, x+H, y, x+H, y+H);
           if (strchr(d, '4')) fprintf(f, segment, x, y-H, x+H, y-H);
           if (strchr(d, '5')) fprintf(f, segment, x, y, x+H, y);
           if (strchr(d, '6')) fprintf(f, segment, x, y+H, x+H, y+H);
           nn /= 10;
           x -= H + HS;
       }
       fprintf(f, "SP0;\n");
    }
    fclose(f);
    printf("\n");
    return 0;
}
