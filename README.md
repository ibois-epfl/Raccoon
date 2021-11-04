# Raccoon_Litter_Box
Development code for Petras Vestartas

# Interface
- [x] pybind11 + CGAL
- [ ] compas_wood C++ repository
- [ ] C# (Wrapper) + CGAL
- [ ] Grasshopper components (integration with NGon)

# Global Search Methods
- [x] RTree + OOB and ABB collision
- [ ] no search by user given index (must have input in the beginning)

# Local Search Methods
- [x] plane_to_face cross
- [x] face_to_face side-to-side non-parallel
- [x] face_to_face side-to-side parallel in-plane
- [x] face_to_face side-to-side parallel out-of-plane
- [x] face_to_face top-to-side 
- [ ] face_to_face top_to_top 


# Joints
- [ ] parameterize ss_e_ip_0 to changing number of tenons, method ss_e_ip_1
- [ ] parameterize ss_e_op_0 to changing number of tenons, method ss_e_op_1
- [ ] parameterize ts_e_p_1 to changing number of tenons, method ts_e_p_2
- [ ] parameterize cr_c_ip_1 conic cross 

# Merge
- [x] insert face joints inside edge
- [ ] insert between multiple edges (cross joint case)
- [ ] cut projection and insert in polylgon (case when side-side joints are rotated)
- [ ] mesh boolean: a) reference shapes, b) joints polygon pairs

# Element Grouping
- [ ] introduce unordered_map to track grouping like sorted lists x;x;x

# Insertion Vector
- [x] side-top connection
- [ ] side-side connection
- [ ] top-top connection

# Display
- [ ] Meshing in COMPAS

# CNC
- [x] C A axis implementation
- [ ] B-Axis integration ( a)find most top position, b) rotate b axis)
- [ ] calibration procedure
- [ ] milling
- [ ] camera
- [ ] slice
- [ ] notches 
- [ ] drilling using I J  
- [ ] rotation limits in 5 axis

# Examples
- [x] Small samples
- [x] Annen
- [ ] CR
- [ ] VDA
- [ ] Vidy
- [ ] Students