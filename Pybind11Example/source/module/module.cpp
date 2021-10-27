#include <pybind11/pybind11.h>
#include <stdio.h>

//#include "pch.h"
#include <iostream>
#include <deque>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <boost/foreach.hpp>

namespace bg = boost::geometry;

//void say_hello() {
//	printf("Hellow World \n");
//}

struct MyData {
	float x, y;
	MyData() : x(0), y(0) {}
	MyData(float x, float y) : x(x), y(y) {}

	void print() {
		printf("%f %f \n",x,y);
	}

};

void boostTest()
{
	std::cout << "Hello World!\n";

	typedef bg::model::polygon<boost::geometry::model::d2::point_xy<double>> polygon;

	bg::model::polygon<boost::geometry::model::d2::point_xy<double>> polygon0;
	bg::model::polygon<boost::geometry::model::d2::point_xy<double>> polygon1;


	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(2, 1.3));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(2.4, 1.7));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(2.8, 1.8));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(3.4, 1.2));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(3.7, 1.6));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(3.4, 2));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(4.1, 3));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(5.3, 2.6));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(5.4, 1.2));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(4.9, 0.8));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(2.9, 0.7));
	bg::append(polygon0.outer(), bg::model::d2::point_xy<double>(2, 1.3));

	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(4.0, -0.5));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(3.5, 1.0));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(2.0, 1.5));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(3.5, 2.0));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(4.0, 3.5));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(4.5, 2.0));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(6.0, 1.5));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(4.5, 1.0));
	bg::append(polygon1.outer(), bg::model::d2::point_xy<double>(4.0, -0.5));



	polygon green, blue;



	//boost::geometry::read_wkt(
	//	"POLYGON((2 1.3,2.4 1.7,2.8 1.8,3.4 1.2,3.7 1.6,3.4 2,4.1 3,5.3 2.6,5.4 1.2,4.9 0.8,2.9 0.7,2 1.3)"
	//	"(4.0 2.0, 4.2 1.4, 4.8 1.9, 4.4 2.2, 4.0 2.0))", green);

	//boost::geometry::read_wkt(
	//	"POLYGON((4.0 -0.5 , 3.5 1.0 , 2.0 1.5 , 3.5 2.0 , 4.0 3.5 , 4.5 2.0 , 6.0 1.5 , 4.5 1.0 , 4.0 -0.5))", blue);

	std::deque<polygon> output;
	//bg::intersection(green, blue, output);
	bg::intersection(polygon0, polygon1, output);

	int i = 0;
	//std::cout << "green && blue:" << std::endl;

	BOOST_FOREACH(polygon const& p, output)
	{
		std::cout << i++ << ": " << bg::area(p) << std::endl;

		//Iterate Polygon Points
		for (auto it = boost::begin(bg::exterior_ring(p)); it != boost::end(bg::exterior_ring(p)); ++it)
		{
			double x = bg::get<0>(*it);
			double y = bg::get<1>(*it);
			printf("%f %f \n", x, y);
		}
	}




}




//
//PYBIND11_MODULE(pybind11module, module) {
//	module.doc() = "Pybind11Module";//Module name
//	module.def("say_hello", &say_hello);//Function reference
//	module.def("boostTest", &boostTest);//Function reference
//
//
//	//expose a struct
//	pybind11::class_<MyData>(module, "MyData")
//		.def(pybind11::init<>())//constructor empty
//		.def(pybind11::init<float,float>(), "constructor 2", pybind11::arg("x"), pybind11::arg("y"))//constructor with x and y arguments
//		.def("print",&MyData::print)//function inside struct
//		.def_readwrite("x", &MyData::x) //fields
//		.def_readwrite("y", &MyData::y) //fields
//
//		;
//
//}
//

