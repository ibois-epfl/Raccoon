#pragma once


inline Tile Cross_Simple() {



    std::vector<CGAL_Polyline> plines =

    {
    { CGAL_Point(0.5, -0.5, 0), CGAL_Point(0.5, 0.5, 0), CGAL_Point(0.5, 0.5, 0.55), CGAL_Point(0.5, -0.5, 0.55), CGAL_Point(0.5, -0.5, 0) },
    { CGAL_Point(-0.5,-0.5,0),CGAL_Point(-0.5,0.5,0),CGAL_Point(-0.5,0.5,0.55),CGAL_Point(-0.5,-0.5,0.55),CGAL_Point(-0.5,-0.5,0) },
    { CGAL_Point(0.5,0.5,-0.55),CGAL_Point(-0.5,0.5,-0.55),CGAL_Point(-0.5,0.5,0),CGAL_Point(0.5,0.5,0),CGAL_Point(0.5,0.5,-0.55) },
    { CGAL_Point(0.5,-0.5,-0.55),CGAL_Point(-0.5,-0.5,-0.55),CGAL_Point(-0.5,-0.5,0),CGAL_Point(0.5,-0.5,0),CGAL_Point(0.5,-0.5,-0.55) },

    };





    return plines;
}