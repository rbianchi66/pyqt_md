%ModuleHeaderCode
    #include "inside.h"
%End

bool inside(const P2d &pt, const Path2d& pts);
bool inside(const P2d &pt, const std::vector<Path2d>& paths);
bool inside_equal(const P2d &pt, const Path2d& pts, const bool overlap = true, const double eps = 0.0000001);

class InsideTester
{
public:
    InsideTester(std::vector<Path2d>& paths, double eps=0.5);
%MethodCode
	// Genera eccezioni Python per alcune condizioni limite non gestite dal
	// codice di InsideTester (che causerebbero un assert).
        if (a0->size() == 0)
        {
	    PyErr_Format(PyExc_ValueError, "paths list is empty");
	    sipIsErr = 1;
	}
	else
	{
	    for (size_t i=0; i < a0->size(); ++i)
	    {
	        if ((*a0)[i].size() == 0)
	        {
		    PyErr_Format(PyExc_ValueError, "empty path in list");
		    sipIsErr = 1;
		    break;
	        }
	    }

	    if (!sipIsErr)
		sipCpp = new InsideTester(*a0, a1);
	}
%End
    bool operator()(const P2d &pt) const;
    bool contains(const Path2d &path, bool partial = false) const;
};
