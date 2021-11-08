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
- [ ] :snowflake: parameterize ss_e_ip_0 to changing number of tenons, method ss_e_ip_1
- [ ] :snowflake: parameterize ss_e_op_0 to changing number of tenons, method ss_e_op_1
- [x] parameteric ss_e_op_1 Nejiri Arigata 
![Nejiri Arigata](https://github.com/ibois-epfl/Raccoon_Litter_Box/blob/main/joinery_solver/documentation/joint_documentation_0.jpg) 
- [ ] :snowflake: parameterize ts_e_p_1 to changing number of tenons, method ts_e_p_2
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
- [ ] Fabricate table
- [ ] camera holder, buy cameras

### Code

- [x] C A axis implementation
- [ ] B-Axis integration ( a)find most top position, b) rotate b axis)
- [ ] calibration procedure
- [x] milling
- [ ] camera
- [x] slice
- [x] notches 
- [ ] drilling using I J  
- [ ] rotation limits in 5 axis


