# MID3_Projet_Opengl

This repository brings together several projects developed during my coursework in computer graphics, primarily using OpenGL and Gkit in some cases. Its purpose is to centralize these projects for easy access and to showcase the practical application of the concepts studied throughout different courses.
Gkit3GL: https://forge.univ-lyon1.fr/JEAN-CLAUDE.IEHL/gkit3GL

## Courses and Associated Projects

## 1. Geometric modeling
The projects below explore various methods for representing and manipulating geometric shapes.

**Implicit Surfaces**:
This project explores shape modeling using signed distance functions (SDFs) and constructive solid geometry (CSG).
* Implemented a framework to define primitives using signed distance functions
* Added operators to combine and modify primitives through a CSG hierarchy
* Implemented an erosion algorithm to introduce surface irregularities
* Added a bounding volume hierarchy to accelerate distance evaluatio
* Mesh generated through Marching cubes

  WIP: ![image](https://github.com/user-attachments/assets/8f7428e9-2fee-4243-9e53-5835eae68c69)
 
  
**Deformations**
This practical work focuses on implementing several mesh deformation techniques:
* Twist deformation based on A. H. Barr, “Global and Local Deformations of Solid Primitives” 
* Free-Form Deformations (FFD) based on T. Sederberg, S. Parry, “Free-form deformation of solid geometric models” 
* Local deformations using spherical regions of influence for translation-based deformation 

**Bézier Surfaces**
This part of the project focuses on the implementation of Bézier curves and surfaces:
* Interactive rendering using OpenGL with visible control points
* Export and visualization of the resulting surfaces 
![image](https://github.com/user-attachments/assets/3c340c88-28df-4573-9909-6db80ac98b0b)

    
## 2. Image synthesis
The course covers the fundamentals of 3D image synthesis, including rendering techniques and real-time graphics using OpenGL, Global illumination and more.

#### World Generation
* Procedural generation through map of large-scale worlds composed of cubes and multiple object types with basic shadow mapping
* Implemented several optimization techniques:
  * Instanced rendering
  * Frustum culling
Frustum culling

<img width="882" height="463" alt="Capture d’écran 2025-12-18 071552" src="https://github.com/user-attachments/assets/d9dcdb66-ae3d-4007-accc-d349b8f72f74" />



### Path Tracing & Ray Tracing

These projects explore physically based rendering techniques through both CPU and GPU implementations of ray tracing.

#### Path Tracing (CPU – OpenGL Framework)

This project focuses on the implementation of a **basic path tracer**, capable of producing global illumination effects such as soft shadows and indirect lighting.
We rendered a Cornell box  using path tracing, showing indirect lighting effects and soft shadowing that emerge from multiple light bounces.

* Recursive ray tracing with multiple bounces
* Physically based light transport simulation
* **Bounding Volume Hierarchy (BVH)** to accelerate ray–scene intersection tests
* Support for simple materials and geometric primitives


<img width="438" height="327" alt="Capture d’écran 2025-12-18 071506" src="https://github.com/user-attachments/assets/ade161b2-845f-4350-a5cb-0f7d9bbb9f42" />

#### GPU Ray Tracing (ShaderToy)

This project implements **real-time ray tracing entirely in a fragment shader**, inspired by ShaderToy-style rendering.

Key features:

* Ray–object intersection computed in the shader
* Support for advanced material properties:
  * Reflection, Refraction, Metallic materials, Procedural texture generation
* **Blinn–Phong shading model** for direct illumination
* Support for multiple light types: Point lights, Directional lights
* **Shadow computation** including **soft shadows** using multiple shadow samples
* **Ambient Occlusion (AO)** to approximate indirect shadowing in crevices and contact areas
<img width="960" height="540" alt="Sans titre" src="https://github.com/user-attachments/assets/349721b7-498d-4aa7-9af1-1a284a71f0d9" />



