%ModuleHeaderCode
    #include "placement.h"
%End

namespace Placement {

class Shape;

class Part
{
    public:
        Part(const std::vector<Path2d>);
        ~Part();

        Placement::Shape *shape(int);

    private:
        Part(const Placement::Part &);
};

class Sheet
{
    public:
        Sheet(double, double);

        bool addPart(Placement::Part *, X2d& /Out/ = NULL);

        double width();
        double height();

        int getHborder();
        void setHborder(int hb);
};

};
