# Raccoon_Litter_Box
Development code for Petras Vestartas

# C++ Project joinery_solver

### Interface
- [x] pybind11 + CGAL
- [ ] output: polylines, polylines vertex count, polylines ids (minus are joints), cut types (milling, drilling, cut, notches)
- [ ] simplify conversion (instead of creation of polyline vector, create directly elements)
- [ ] compas_wood C++ repository
- [ ] C# (Wrapper) + CGAL
- [ ] Grasshopper components (integration with NGon)
- [ ] C++ Visualizer

### Global Search Methods
- [x] RTree + OOB and ABB collision
- [ ] no search by user given index (must have input in the beginning)

### Local Search Methods
- [x] plane_to_face cross
- [x] face_to_face side-to-side non-parallel
- [x] face_to_face side-to-side parallel in-plane
- [x] face_to_face side-to-side parallel out-of-plane
- [x] face_to_face top-to-side 
- [ ] face_to_face top_to_top 


### Joints
- [ ] store each parameterized joint inside std::unordered_map<string, joint> that is not remapped yet

- [x] parametric ss_e_op_1 Nejiri Arigata 

![Nejiri Arigata](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_0.jpg) 

- [x] parametric ss_e_op_2 Dovetail

![Dovetail](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_1.jpg) 

- [x] parametric ss_e_ip_0 Dovetail 

![Dovetail](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_2.jpg) 

- [ ] parameterize tenon mortise ts_e_p_2 (ts_e_p_0 - original) (start and end point of the joint line are skipped)

![Tenon-mortise](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_3.jpg) 

- [ ] parameterize Annen joint - ts_e_p_3 (ts_e_p_2 - original)

![Tenon-mortise Annen](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_4.jpg) 

- [ ] :snowflake: parameterize cr_c_ip_1 conic cross 

### Merge
- [x] insert face joints inside edge
- [ ] :snowflake: insert between multiple edges (cross joint case)
- [ ] cut projection and insert in polylgon (case when side-side joints are rotated)
- [ ] mesh boolean: a) reference shapes, b) joints polygon pairs

### Element Grouping
- [ ] introduce unordered_map to track grouping like sorted lists x;x;x

### Insertion Vector
- [x] side-top connection
- [ ] side-side connection
- [ ] top-top connection

### Display
- [ ] Meshing in COMPAS

### Examples
- [x] Small samples
- [x] Annen
- [ ] CR
- [ ] VDA
- [ ] Vidy
- [ ] Students

# CSharp Project Raccoon (CNC G-Code)
To run CNC G-Code download the Build folder and place in Grasshopper Libraries folder: 
https://github.com/ibois-epfl/Raccoon_Litter_Box/tree/main/Raccoon/build

Important: add all tools parameters from the current CNC machine to Tools.txt file.

### Physical Work
- [x] Fabricate table
- [ ] camera holder, buy cameras

### Code

- [x] C A axis implementation
- [x] B-Axis integration ( a)find most top position, b) rotate b axis )
- [ ] calibration procedure
- [ ] camera
- [x] milling
- [x] slice
- [x] notches 
- [x] probe 
- [ ] drilling using I J (use G48 for cutting in a plane)
- [ ] rotation limits in 5 axis (did not popped yet)


