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
	int id, f0, f1, e0, e1, type; //10 - SS Rotate 11 - SS OUT OF PLANE 12 - SS IN Plane,  20 Top-Side, 30 - Cross
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

	std::vector<CGAL_Polyline> m[2];
	std::vector<char> m_boolean_type;//-1 do not merge 0 edge 1 cut 2 hole

	std::vector<CGAL_Polyline> f[2];
	std::vector<char> f_boolean_type;//-1 do not merge 0 edge 1 cut 2 hole

	/////////////////////////////////////////////////////////////////////////////////////////
	//Constructors
	/////////////////////////////////////////////////////////////////////////////////////////
	joint(int, int, int, int, int, CGAL_Polyline(&)[4]);
	joint(int, int, int, int, int,  CGAL_Polyline(&), CGAL_Polyline(&)[2], CGAL_Polyline(&)[4],int );

	//Operators
   // IK::Vector_3(&input)[4]
    std::vector<CGAL_Polyline>& operator() (bool male_or_female, bool first_or_second) {
        if (male_or_female)
            if (first_or_second)
                return m[0];
            else
                return m[1];
		else
            if (first_or_second)
                return f[0];
            else
                return f[1];
	}

    void reverse(bool male_or_female) {
     
        if (male_or_female) 
            std::swap(m[0],m[1]);
        else 
            std::swap(f[0], f[1]);
        
    }



    


	
	void transform(CGAL::Aff_transformation_3<IK>& xform0, CGAL::Aff_transformation_3<IK>& xform1);//Custom user transformation
	bool orient_to_connection_area();//Orient to connection area if rectangles are set
};


joint::joint(int _id, int _f0, int _f1, int _e0, int _e1, CGAL_Polyline(&_joint_volumes)[4]) {

    this->id = _id;
    this->f0 = _f0;
    this->f1 = _f1;
    this->e0 = _e0;
    this->e1 = _e1;
    this->type = -1;

    for (int i = 0; i < 4; i++) {
        this->joint_volumes[i].reserve(_joint_volumes[i].size());
        for (int j = 0; j < _joint_volumes[i].size(); j++) {
            IK::Point_3 p = _joint_volumes[i][j];
            this->joint_volumes[i].push_back(p);
        }
    }


}

joint::joint(int _id, int _f0, int _f1, int _e0, int _e1, CGAL_Polyline(&_joint_area), CGAL_Polyline(&_joint_lines)[2], CGAL_Polyline(&_joint_volumes)[4], int _type) {

    this->id = _id;
    this->f0 = _f0;
    this->f1 = _f1;
    this->e0 = _e0;
    this->e1 = _e1;
    this->type = _type;

    for (int i = 0; i < 4; i++) {
        this->joint_volumes[i].reserve(_joint_volumes[i].size());
        for (int j = 0; j < _joint_volumes[i].size(); j++) {
            this->joint_volumes[i].emplace_back(_joint_volumes[i][j]);
        }
    }

    for (int i = 0; i < 2; i++) {
        this->joint_lines[i].reserve(_joint_lines[i].size());
        for (int j = 0; j < _joint_lines[i].size(); j++) {
            this->joint_lines[i].emplace_back(_joint_lines[i][j]);
        }
    }

    this->joint_area.reserve(_joint_area.size());
    for (int j = 0; j < _joint_area.size(); j++) {
        this->joint_area.emplace_back(_joint_area[j]);
    }

}


//Methods



//https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp //Line 2153
inline bool joint::change_basis(CGAL_Polyline& rect0, CGAL_Polyline& rect1, CGAL::Aff_transformation_3<IK>& xform)
{

    IK::Point_3 O1(-0.5, -0.5, -0.5);
    IK::Vector_3 X1(1, 0, 0); // final frame X (X,Y,Z = arbitrary basis)
    IK::Vector_3 Y1(0, 1, 0); // final frame Y
    IK::Vector_3 Z1(0, 0, 1);  // final frame Z

    IK::Point_3 O0 = rect0[0];
    IK::Vector_3 X0 = (rect0[1] - rect0[0]); // initial frame X (X,Y,Z = arbitrary basis)
    IK::Vector_3 Y0 = (rect0[3] - rect0[0]); // initial frame Y
    IK::Vector_3 Z0 = (rect1[0] - rect0[0]); // initial frame Z

    // Q = a0*X0 + b0*Y0 + c0*Z0 = a1*X1 + b1*Y1 + c1*Z1
    // then this transform will map the point (a0,b0,c0) to (a1,b1,c1)


    //*this = ON_Xform::ZeroTransformation;

    double a, b, c, d;
    a = X1 * Y1;
    b = X1 * Z1;
    c = Y1 * Z1;
    double R[3][6] = { {X1 * X1,      a,      b,       X1 * X0, X1 * Y0, X1 * Z0},
                      {    a,  Y1 * Y1,      c,       Y1 * X0, Y1 * Y0, Y1 * Z0},
                      {    b,      c,  Z1 * Z1,       Z1 * X0, Z1 * Y0, Z1 * Z0} };

    // row reduce R
    int i0 = (R[0][0] >= R[1][1]) ? 0 : 1;
    if (R[2][2] > R[i0][i0])
        i0 = 2;
    int i1 = (i0 + 1) % 3;
    int i2 = (i1 + 1) % 3;
    if (R[i0][i0] == 0.0)
        return false;
    d = 1.0 / R[i0][i0];
    R[i0][0] *= d;
    R[i0][1] *= d;
    R[i0][2] *= d;
    R[i0][3] *= d;
    R[i0][4] *= d;
    R[i0][5] *= d;
    R[i0][i0] = 1.0;
    if (R[i1][i0] != 0.0) {
        d = -R[i1][i0];
        R[i1][0] += d * R[i0][0];
        R[i1][1] += d * R[i0][1];
        R[i1][2] += d * R[i0][2];
        R[i1][3] += d * R[i0][3];
        R[i1][4] += d * R[i0][4];
        R[i1][5] += d * R[i0][5];
        R[i1][i0] = 0.0;
    }
    if (R[i2][i0] != 0.0) {
        d = -R[i2][i0];
        R[i2][0] += d * R[i0][0];
        R[i2][1] += d * R[i0][1];
        R[i2][2] += d * R[i0][2];
        R[i2][3] += d * R[i0][3];
        R[i2][4] += d * R[i0][4];
        R[i2][5] += d * R[i0][5];
        R[i2][i0] = 0.0;
    }

    if (fabs(R[i1][i1]) < fabs(R[i2][i2])) {
        int i = i1; i1 = i2; i2 = i;
    }
    if (R[i1][i1] == 0.0)
        return false;
    d = 1.0 / R[i1][i1];
    R[i1][0] *= d;
    R[i1][1] *= d;
    R[i1][2] *= d;
    R[i1][3] *= d;
    R[i1][4] *= d;
    R[i1][5] *= d;
    R[i1][i1] = 1.0;
    if (R[i0][i1] != 0.0) {
        d = -R[i0][i1];
        R[i0][0] += d * R[i1][0];
        R[i0][1] += d * R[i1][1];
        R[i0][2] += d * R[i1][2];
        R[i0][3] += d * R[i1][3];
        R[i0][4] += d * R[i1][4];
        R[i0][5] += d * R[i1][5];
        R[i0][i1] = 0.0;
    }
    if (R[i2][i1] != 0.0) {
        d = -R[i2][i1];
        R[i2][0] += d * R[i1][0];
        R[i2][1] += d * R[i1][1];
        R[i2][2] += d * R[i1][2];
        R[i2][3] += d * R[i1][3];
        R[i2][4] += d * R[i1][4];
        R[i2][5] += d * R[i1][5];
        R[i2][i1] = 0.0;
    }

    if (R[i2][i2] == 0.0)
        return false;
    d = 1.0 / R[i2][i2];
    R[i2][0] *= d;
    R[i2][1] *= d;
    R[i2][2] *= d;
    R[i2][3] *= d;
    R[i2][4] *= d;
    R[i2][5] *= d;
    R[i2][i2] = 1.0;
    if (R[i0][i2] != 0.0) {
        d = -R[i0][i2];
        R[i0][0] += d * R[i2][0];
        R[i0][1] += d * R[i2][1];
        R[i0][2] += d * R[i2][2];
        R[i0][3] += d * R[i2][3];
        R[i0][4] += d * R[i2][4];
        R[i0][5] += d * R[i2][5];
        R[i0][i2] = 0.0;
    }
    if (R[i1][i2] != 0.0) {
        d = -R[i1][i2];
        R[i1][0] += d * R[i2][0];
        R[i1][1] += d * R[i2][1];
        R[i1][2] += d * R[i2][2];
        R[i1][3] += d * R[i2][3];
        R[i1][4] += d * R[i2][4];
        R[i1][5] += d * R[i2][5];
        R[i1][i2] = 0.0;
    }

    CGAL::Aff_transformation_3<IK> m_xform(
        R[0][3], R[0][4], R[0][5],
        R[1][3], R[1][4], R[1][5],
        R[2][3], R[2][4], R[2][5]
    );
    //m_xform.cartesian(0,0) = R[0][3];
    //m_xform[0][1] = R[0][4];
    //m_xform[0][2] = R[0][5];

    //m_xform[1][0] = R[1][3];
    //m_xform[1][1] = R[1][4];
    //m_xform[1][2] = R[1][5];

    //m_xform[2][0] = R[2][3];
    //m_xform[2][1] = R[2][4];
    //m_xform[2][2] = R[2][5];

    CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O1.x(), 0 - O1.y(), 0 - O1.z()));
    CGAL::Aff_transformation_3<IK> T2(CGAL::TRANSLATION, IK::Vector_3(O0.x(), O0.y(), O0.z()));


    xform = (T2 * m_xform * T0);

    return true;

}

inline void joint::transform(CGAL::Aff_transformation_3<IK>& xform0, CGAL::Aff_transformation_3<IK>& xform1)
{


    
    for (int i = 0; i < m[0].size(); i++)
        for (auto it = m[0][i].begin(); it != m[0][i].end(); ++it)
            *it = it->transform(xform0);

    for (int i = 0; i < m[1].size(); i++)
        for (auto it = m[1][i].begin(); it != m[1][i].end(); ++it)
            *it = it->transform(xform0);

    for (int i = 0; i < f[0].size(); i++)
        for (auto it = f[0][i].begin(); it != f[0][i].end(); ++it)
            *it = it->transform(xform1);


    for (int i = 0; i < f[1].size(); i++)
        for (auto it = f[1][i].begin(); it != f[1][i].end(); ++it)
            *it = it->transform(xform1);

}


inline bool joint::orient_to_connection_area() {
    CGAL::Aff_transformation_3<IK> xform0;
    bool flag0 = change_basis(joint_volumes[0], joint_volumes[1], xform0);

    CGAL::Aff_transformation_3<IK> xform1;
    bool flag1 = joint_volumes[2].size() > 0 ? change_basis(joint_volumes[2], joint_volumes[3], xform1) : change_basis(joint_volumes[0], joint_volumes[1], xform1);

    transform(xform0, xform1);

    return flag0 && flag1;
}




