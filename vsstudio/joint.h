#pragma once
#include "compas.h"
#include <string>

//Container for cuts
class joint
{
	//Methods
	
	bool change_basis(CGAL_Polyline& rect0, CGAL_Polyline& rect1, CGAL::Aff_transformation_3<IK>& xform);//first get 2x ChangeBasis transformation matrices

public:

	/////////////////////////////////////////////////////////////////////////////////////////
	//Parameters from Search method
	/////////////////////////////////////////////////////////////////////////////////////////
	int id, f0, f1, e0, e1;
	CGAL_Polyline joint_area;//delete
	CGAL_Polyline joint_lines[2];//delete
	//CGAL_Polyline joint_quads[2];//delete
	CGAL_Polyline joint_volumes[4];//mostly 2, but can be 4 e.g. in-plane side-side

	/////////////////////////////////////////////////////////////////////////////////////////
	//Detailed parameters for geometry transfer from library or custom made
	/////////////////////////////////////////////////////////////////////////////////////////
	std::string name = "";
	int id_of_global_joint_list = -1;//Directs which joint applies where, -1 all cases
	std::vector<int> tile_parameters;//For rebuilding

	std::vector<CGAL_Polyline> m;
	std::vector<char> m_boolean_type;//-1 do not merge 0 edge 1 cut 2 hole

	std::vector<CGAL_Polyline> f;
	std::vector<char> f_boolean_type;//-1 do not merge 0 edge 1 cut 2 hole

	/////////////////////////////////////////////////////////////////////////////////////////
	//Constructors
	/////////////////////////////////////////////////////////////////////////////////////////
	joint(int, int, int, int, int, CGAL_Polyline(&)[4]);
	joint(int, int, int, int, int,  CGAL_Polyline(&), CGAL_Polyline(&)[2], CGAL_Polyline(&)[4] );

	//Operators
	std::vector<CGAL_Polyline>& operator[] (bool male_or_female) {
		if (male_or_female)
			return m;
		else
			return f;
	}


	
	void transform(CGAL::Aff_transformation_3<IK>& xform0, CGAL::Aff_transformation_3<IK>& xform1);//Custom user transformation
	bool orient_to_connection_area();//Orient to connection area if rectangles are set
};






