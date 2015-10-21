#include "unione_path.h"
#include <math.h>
#include <limits.h>
#include <assert.h>

path2d_array unione_path(path2d_array &apts, double eps)
{
    path2d_array ret;
    int as = apts.size();
    std::vector<bool> visited(as, false);    
    
    int removed = 0;
    while (apts.size()-removed > 1)
    {
        int mmin_i1 = -1;
        int mmin_i2 = -1;
        double mmin_v = 1E99;
        bool mrev_l1 = false;
        bool mrev_l2 = false;
        
        for (int ind_l1=0; ind_l1<int(apts.size())-1; ++ind_l1)
        {
            if (visited[ind_l1])
                continue;
                
            const P2d &p1_a = apts[ind_l1][0];
            const P2d &p1_b = apts[ind_l1].back();
            
            for (int ind_l2=ind_l1+1; ind_l2<int(apts.size()); ++ind_l2)
            {
                if (visited[ind_l2])
                    continue;
                    
                const P2d &p2_a = apts[ind_l2][0];
                const P2d &p2_b = apts[ind_l2].back();
                
                double min1 = len(p1_a - p2_a);
                double min2 = len(p1_a - p2_b);
                double min3 = len(p1_b - p2_a);
                double min4 = len(p1_b - p2_b);
                double mmin;
                                
                bool rev_l1 = false;
                bool rev_l2 = false;
                
                if (min1 <= min2)
                {
                    if (min1 <= min3)
                    {
                        if (min1 <= min4)
                        {
                            // min1
                            rev_l1 = true;
                            mmin = min1;
                        }
                        else
                        {
                            // min4
                            rev_l2 = true;
                            mmin = min4;
                        }
                    }
                    else
                    {
                        if (min3 <= min4)
                        {
                            // min3
                            mmin = min3;
                        }
                        else
                        {
                            // min4
                            rev_l2 = true;
                            mmin = min4;
                        }
                    }
                }
                else
                {
                    if (min2 <= min3)
                    {
                        if (min2 <= min4)
                        {
                            // min2
                            rev_l1 = true;
                            rev_l2 = true;
                            mmin = min2;
                        }
                        else
                        {
                            // min4
                            rev_l2 = true;
                            mmin = min4;
                        }
                    }
                    else
                    {
                        if (min3 <= min4)
                        {
                            // min3
                            mmin = min3;
                        }
                        else
                        {
                            // min4
                            rev_l2 = true;
                            mmin = min4;
                        }
                    }
                }
                
                if (mmin < mmin_v)
                {
                    mmin_v = mmin;
                    mrev_l1 = rev_l1;
                    mrev_l2 = rev_l2;
                    mmin_i1 = ind_l1;
                    mmin_i2 = ind_l2;
                }
            }
        }
        
        if (mmin_i1 != -1)
        {
            if (mmin_v < eps)
            {               
                visited[mmin_i1] = true;
                visited[mmin_i2] = true;
                visited.push_back(false);

                apts.push_back(Path2d());
                Path2d &t = apts.back();
                
                P2d p1, p2;
                
                if (!mrev_l1)
                {
                    int s = apts[mmin_i1].size()-1;

                    for (int i=0; i<s; ++i)
                       t.push_back(apts[mmin_i1][i]);

                    p1 = apts[mmin_i1][s];
                }
                else
                {
                    for (int i=apts[mmin_i1].size()-1; i>0; --i)
                       t.push_back(apts[mmin_i1][i]);

                    p1 = apts[mmin_i1][0];
                }

                if (!mrev_l2)
                    p2 = apts[mmin_i2][0];
                else
                    p2 = apts[mmin_i2].back();

                t.push_back((p1+p2)*0.5);

                if (!mrev_l2)
                {
                    for (int i=1; i<apts[mmin_i2].size(); ++i)
                       t.push_back(apts[mmin_i2][i]);
                }
                else
                {
                    for (int i=apts[mmin_i2].size()-2; i>=0; --i)
                       t.push_back(apts[mmin_i2][i]);
                }

                removed += 2;
            }
            else
                break;
        }
        else
            break;
    }
    
    for (int i=0,z=0; i<int(apts.size()); ++i)
        if (!visited[i])
        {
            ret.push_back(Path2d());

            Path2d &v = ret[z++];
            
            v.swap(apts[i]);
        }
        
    return ret;
}

