Author: Petras Vestartas

# Install
Download latest build from the release directory of github.

To run CNC G-Code download the Build folder and place in Grasshopper Libraries folder.

Important: add all tools parameters from the current CNC machine to Tools.txt file.


# Interface
For security reasons all tools are defined in Tools.txt file.

In order to use custom user defined tools from Tools.txt add Tools Component to Grasshopper canvas and assign the correct tool-path. After this all components will know about the set of tools, because we set a static dictionary.

<img width="1440" alt="Screenshot 2022-04-24 at 18 45 45" src="https://user-images.githubusercontent.com/18013985/164987154-4a0c4a6b-b400-4037-ac68-a8d8b56c3e66.png">

# Example Files
![CNC_Plugin](https://user-images.githubusercontent.com/18013985/164996185-eb7d612d-bc5b-4e32-94a5-09fef6dd9750.png)
https://user-images.githubusercontent.com/18013985/164996235-c1af4799-1e8e-488e-8755-79c49566c315.mp4



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
- [x] simulation update in Visual Studio Cardan
- [ ] air supply
- [x] text use OpenNest 
- [ ] too deep angle cut
