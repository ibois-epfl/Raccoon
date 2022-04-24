Author: Petras Vestartas
Previous developments: Christopher Robeller, Benjamin Hahn

# Install
Download latest build from the release directory of github.

# Interface
For security reasons all tools are defined in Tools.txt file.
Follow the same order to define new tools.
In order to use custom user defined tools from Tools.txt add Tools Component to Grasshopper canvas and assign the correct tool-path. After this all components will know about the set of tools, because we set a static dictionary.

<img width="1440" alt="Screenshot 2022-04-24 at 18 45 45" src="https://user-images.githubusercontent.com/18013985/164987154-4a0c4a6b-b400-4037-ac68-a8d8b56c3e66.png">




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
- [ ] air supply
- [ ] text
- [ ] too deep angle cut
