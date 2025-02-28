# MID3_Projet_Opengl

This repository consolidates multiple projects developed during my coursework related to computer graphics using OpenGL and the mainly the library Gkit. The goal is to centralize these projects for easy access and to demonstrate the practical applications of the concepts learned throughout various courses.
Gkit3GL: https://forge.univ-lyon1.fr/JEAN-CLAUDE.IEHL/gkit3GL

## Courses and Associated Projects

### 1. Geometric modeling
The projects below explore various methods for representing and manipulating geometric shapes.

- **Implicit Surfaces**:
- This first mini-project first goal was to implement a framework to define primitives through signed distance function, the operator used to modify them and the CSG hierarchy to connect them.
- The second goal is to add an erosion algorithm to add iregularity on the shape
- Third goal is simply a bounding box hierarchy to speed up the calculation of the shape.
  WIP: ![image](https://github.com/user-attachments/assets/8f7428e9-2fee-4243-9e53-5835eae68c69)
 
  
- **Deformations**: This practical work goal was to implement multiple mesh deformation algorithm. I chose to implement:
  - Twist based on A. H. Barr. Global and Local Deformations of Solid Primitives. (Done)
  - FreeForm Deformations based on T. Sederberg, S. Parry. Free-form deformation of solid geometric models. (Done)
  - Local Deformations: Translations with a sphere as deformations support. (Done)
- **Bezier Surface**: This last part of our project was the implementation of Bezier curves.
Rendering on OpengGl with the control point and on Blender ![image](https://github.com/user-attachments/assets/3c340c88-28df-4573-9909-6db80ac98b0b)

    
### 2. Image synthesis
The course covers the fundamentals of 3D image synthesis, including rendering techniques and real-time graphics using OpenGL, Global illumination and more.

- World Generation: Using opengl we generate massive world comprised of cube and multiples objects. We use some optimisations technique including instance drawing, frustrum culling.

The project in (done) can already be used but  they lack any sort of interactivity. You can only launch them and move around and parameter to alterate for example deformations can't be passed yet without modifying the code. No instructions for building till i go around making them more usable.
