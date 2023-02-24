/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// dependency:
// NestPoint
//
// CheckList:
// check width_calculated and height_calculated functions results
// why arrays are used instead of lists?
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace opennest
{

    class NFP
    {

    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // properties
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        NFP *sheet;
        size_t id;
        size_t source;
        double rotation;
        std::string name;
        std::vector<NestPoint> points;

        bool is_bin;
        double x, y;
        std::vector<NFP> children;
        size_t id;
        double offset_x, offset_y = DBL_MAX;
        size_t source = SIZE_MAX;
        double rotation;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // constructors
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        NFP(); // declare default constructor

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // methods
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * check if the sheet was fully used
         *
         * @return return true if the class NFP instance "sheet" is not null
         */
        bool fitted();

        /**
         * convert class to string, like this to_string(my_nfp_instance)
         * https://stackoverflow.com/questions/33357480/is-there-a-standard-way-to-convert-a-class-to-a-string
         * https://cplusplus.com/reference/cstdio/printf/
         *
         * @return a string representation of the class
         */
        operator std::string() const
        {
            std::string point_count = (points.size() == 0) ? points.size() + "" : "null";
            return "nfp: id: %z; source: {%z}; rotation: {%f}; points: {%s}", this->id, this->source, this->rotation, point_count;
        }

        /**
         * add a point to the point list
         *
         * @param [in] point a point to add to the points vectos
         */
        void add_point(NestPoint &point);

        /**
         * reverse the point vector
         */
        void reverse();

        /**
         * get the width of the points
         */
        double width_calculated;

        /**
         * get the height of the points
         */
        double height_calculated;

        /**
         * get the height of the points
         *
         * @param [in] id get point in the points vector
         * @return a reference to this point
         */
        NestPoint &operator[](int id);

        /**
         * get count of the points
         *
         * @return a size_t value of points.size()
         */
        size_t length();

        /**
         * get area of the polygon
         *
         * @return a double
         */
        double area();

        /**
         * add a point to the points vector
         *
         * @param [in] point NestPoint
         */
        void push(NestPoint point);

        /**
         * get a list of points bettween current id and the end of the points vector
         *
         * @param [in] id start index
         */
        NFP slice(int id);


    };

}
