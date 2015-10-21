%Module geo2d.marshaller

%ModuleHeaderCode
#include "marshaller_geo2d.h"
%End

%Import sip/_geo2dcpp.sip
%Import ../comcom2/sip/_comcom2.sip

void ___write_P2d(C2Stream& s, const P2d& p);
%MethodCode
    *a0 << *a1;
%End

P2d ___read_P2d(C2Stream& s);
%MethodCode
    sipRes = new P2d;
    *a0 >> *sipRes;
%End

void ___write_X2d(C2Stream& s, const X2d& p);
%MethodCode
    *a0 << *a1;
%End
X2d ___read_X2d(C2Stream& s);
%MethodCode
    sipRes = new X2d;
    *a0 >> *sipRes;
%End


void ___write_Path2d(C2Stream& s, const Path2d& p);
%MethodCode
    *a0 << *a1;
%End
Path2d ___read_Path2d(C2Stream& s);
%MethodCode
    sipRes = new Path2d;
    *a0 >> *sipRes;
%End


void ___write_std_vector_Path2d(C2Stream &s, const std::vector<Path2d>& v);
%MethodCode
    *a0 << *a1;
%End
std::vector<Path2d> ___read_std_vector_Path2d(C2Stream &s);
%MethodCode
    sipRes = new std::vector<Path2d>;
    *a0 >> *sipRes;
%End


void ___write_std_vector_P2d(C2Stream &s, const std::vector<P2d>& v);
%MethodCode
    *a0 << *a1;
%End
std::vector<P2d> ___read_std_vector_P2d(C2Stream &s);
%MethodCode
    sipRes = new std::vector<P2d>();
    *a0 >> *sipRes;
%End

void ___write_std_map_double_std_vector_Path2d(C2Stream &s, const std::map<double, std::vector<Path2d> >& v);
%MethodCode
    *a0 << *a1;
%End
std::map<double, std::vector<Path2d> > ___read_std_map_double_std_vector_Path2d(C2Stream &s);
%MethodCode
    sipRes = new std::map<double, std::vector<Path2d> >;
    *a0 >> *sipRes;
%End

void ___write_std_map_int_std_vector_Path2d(C2Stream &s, const std::map<int, std::vector<Path2d> >& v);
%MethodCode
    *a0 << *a1;
%End
std::map<int, std::vector<Path2d> > ___read_std_map_int_std_vector_Path2d(C2Stream &s);
%MethodCode
    sipRes = new std::map<int, std::vector<Path2d> >;
    *a0 >> *sipRes;
%End

void ___write_std_vector_std_vector_Path2d(C2Stream &s, const std::vector<std::vector<Path2d> >& v);
%MethodCode
    *a0 << *a1;
%End
std::vector<std::vector<Path2d> > ___read_std_vector_std_vector_Path2d(C2Stream &s);
%MethodCode
    sipRes = new std::vector<std::vector<Path2d> >;
    *a0 >> *sipRes;
%End
