# Raccoon_Litter_Box
<strong style="color: red; opacity: 0.80;">Development code for Petras Vestartas.</strong>



# C++ Project joinery_solver
Build directory: https://github.com/ibois-epfl/Raccoon_Litter_Box/tree/main/joinery_solver/build

________________________________________________________
## Interface compas_wood
- [x] pybind11 + CGAL
- [ ] output: polylines, polylines vertex count, polylines ids (minus are joints), cut types (milling, drilling, cut, notches)
- [ ] simplify conversion (instead of creation of polyline vector, create directly elements)
- [ ] compas_wood C++ repository
- [ ] Meshing in COMPAS

## Interface Rhino C++ Grasshopper
- [ ] C# (Wrapper) + CGAL
- [ ] Grasshopper components (integration with NGon)

## Interface Rhino C++ Plugin
### Installation Steps:
- Rhino requires stdafx files referenced to .cpp files. If you are using another project for building this one, create stdafx.h in that project to stop complaining.
- And underfine following variables in rhino stdafx.h file, #define NOMINMAX #undef min #undef max.
- (Headers) C/C++ -> General -> Additional Include Directions :

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$(RhinoIncDir); \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\Python\Pybind11Example\externals\pybind11\include; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\Python\Pybind11Example\source\module; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\Users\petra\AppData\Local\Programs\Python\Python38\include; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\include; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\auxiliary\gmp\include; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\CPP\Eigen\eigen-3.3.9; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\CPP\Boost\;C:\IBOIS57\_Code\Software\Raccoon_Litter_Box\joinery_solver; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\Raccoon_Litter_Box\joinery_solver\joinery_solver_rhino7; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\Raccoon_Litter_Box\joinery_solver\joinery_solver_rhino6; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%(AdditionalIncludeDirectories)

- (Libraries) Linker -> Input -> Additional Dependencies :

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\Users\petra\AppData\Local\Programs\Python\Python38\libs\python38.lib; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\auxiliary\gmp\lib\libgmp-10.lib; \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;C:\IBOIS57\_Code\Software\CPP\CGAL\CGAL-5.3\auxiliary\gmp\lib\libmpfr-4.lib; 

-  Also, add .cpp files clipper.cpp and connection_zones.cpp as existing items to the new project
-  CGAL Builds with C++14, not 17 
 
### Command-line interface:
- [x] Menu 1 : Select Polylines pairs
- [x] Menu 2 : Select Insertion vectors
- [x] Menu 3 : Skip not needed joints, by dots 
- [x] Menu 4 : search type | division distance | shift | 
- [x] Menu 5 : Joint parameters per category 

 [Video](https://vimeo.com/650968401 "Crash test")


![rhino_command_line](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/rhino_interface_1.gif)

- [x] Loft polyline groups with holes, 2d cgal constrained delaunay triangulations
- [ ] MAC - only possible through NET or CPython wrappers

## Interface Stand-alone
- [ ] C++ Visualizer
________________________________________________________

## Search Methods
### Global
- [x] Closest Object Query + OOB and ABB collision
- [ ] no search by user given index (must have input in the beginning)

### Local

- [x] face_to_face side-to-side parallel in-plane | **joinery_library 1-9** | type 12
- [x] face_to_face side-to-side parallel out-of-plane | **joinery_library 10-19** | type 11
- [x] face_to_face top-to-side | **joinery_library 20-29** | type 20
- [x] plane_to_face cross | **joinery_library 30-39** | type 30
- [ ] face_to_face top_to_top | **joinery_library 40-49** |  type 40 :tomato: currently only output joint_area with 0 local search
- [ ] face_to_face side-to-side | **joinery_library non-parallel 50-59** |  type 50



### Local Search and Insertion Vector
Description: a vector on an element edge that is equal to a plane normal on an edge. 
- [x] side-top connection insertion vector is currently defined by plane normals, not insertion direction because it is equal what is inside code
![Tenon-mortise Annen](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/insertion_vectors_0.jpg) 
![Tenon-mortise Annen2](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/insertion_vectors_0.jpg) 

- [ ] side-side connection, not finished for out of plane connections
- [ ] side-side connection in rotaiton insertion vector not tested 
- [ ] top-top connection

________________________________________________________

## Element 

### Code implementation
- [x] element is specified as a pair of polylines, with planes for each side
 
### Merge
- [x] insert face joints inside edge
- [x] insert between multiple edges (open polylines + closed polygons) ![Merge closed polygons and open edges](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/merge_1.jpg) 
- [ ] cut projection and insert in polygon (case when side-side joints are rotated)
- [x] mesh boolean: a) reference shapes, b) joints polygon pairs

### Element Grouping
- [ ] introduce unordered_map to track grouping like sorted lists x;x;x
________________________________________________________

## Joint 

### Code implementation
- [x] joint class that stores adjacency information
- [x] change-basis transformation from unit tile to joint volume defined by two rectangles 
- [x] assignment of joints based and search categories and connection types
- [ ] store each parameterized joint inside std::unordered_map<string, joint> that is not remapped yet

- [x] parametric ss_e_op_1 Nejiri Arigata 

![Nejiri Arigata](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_0.jpg) 

- [x] parametric ss_e_op_2 Dovetail

![Dovetail](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_1.jpg) 

- [x] parametric ss_e_ip_0 Dovetail 

![Dovetail](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_2.jpg) 

- [x] parameterize tenon mortise ts_e_p_2 (ts_e_p_0 - original) (start and end point of the joint line are skipped)

![Tenon-mortise](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_3.jpg) 

- [x] parameterize Annen joint - ts_e_p_3 (ts_e_p_2 - original)

![Tenon-mortise Annen](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_4.jpg) 

- [x] parameterize cr_c_ip_0 cross 

![Cross Joint](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_5.jpg) 

- [x] parameterize cr_c_ip_1 conic cross 

![Conic Joint](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_6.jpg) 

- [ ] snap-fit joint
- [ ] keys
- [ ] tenon-mortise beams
- [ ] tenon-mortise half beam
- [ ] scarf

## Joints Parameters

    nothing = '0',

    //plates
    edge_insertion = '1',
    hole = '2',
    insert_between_multiple_edges = '3',

    //beams
    slice = '4',//project and make rectangle
    mill = '5',
    mill_project = '6', //project
    cut = '7',
    cut_project = '8', //project
    binary_slice_mill = '9' //project and make rectangle

________________________________________________________

### Examples
- [x] Small samples
- [x] Annen
- [ ] CR
- [ ] VDA
- [ ] Vidy
- [ ] Students

________________________________________________________
________________________________________________________
________________________________________________________

# CSharp Project Raccoon (CNC G-Code)
To run CNC G-Code download the Build folder and place in Grasshopper Libraries folder: 
https://github.com/ibois-epfl/Raccoon_Litter_Box/tree/main/Raccoon/build

Important: add all tools parameters from the current CNC machine to Tools.txt file.

### Physical Work
- [x] Fabricate table [Video](https://vimeo.com/645880001 "Fabricate table - Click to Watch!")
- [ ] camera holder, buy cameras

### Code

- [x] C A axis implementation
- [x] B-Axis integration ( a)find most top position, b) rotate b axis ) [Video](https://vimeo.com/645879445 "B-Axis integration - Click to Watch!")
- [ ] calibration procedure
- [ ] camera
- [x] milling 
- [x] slice
- [x] notches [Video](https://vimeo.com/645882287 "notches - Click to Watch!")
- [x] probe 
- [ ] drilling using I J (use G48 for cutting in a plane)
- [ ] rotation limits in 5 axis (did not popped yet)
- [x] collision detection model for the Cardan 5-Axis CNC machine [Video](https://vimeo.com/647108247 "Cardan Axis Approximation - Click to Watch!")
- [ ] simulation update in Visual Studio 


### Installation - Known issues

Windows 7 does not find C++ file resulting in HResult error. Install x64 VCRuntime libraries: https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0
