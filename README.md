# Stardust Engine
 3D Game Engine
3D engine student project, made by [Ricardo Gutiérrez](https://twitter.com/reRikarudo) & [David Valdivia](https://twitter.com/ValdiviaDev), students at the CITM university (UPC affiliate) at Terrassa (Barcelona).

## 3rd party libraries used:
	
	* SDL 2.0.4
	* OpenGL 4.6
	* Assimp 3.3.0
	* DevIL 1.7.8
	* ImGui 1.72b
	* ImGuizmo 1.61
	* MathGeoLib 1.5
	* LGC RNG 1.5
	* Glew 2.1.0
	* Parson
	* par_shapes
	* mmgr memory manager
	* PhysFS 2.0.3

## Instalation Instructions
Open the "Stardust Engine.exe" file that you'll find inside this folder.

## Engine instructions

  ### Move arround
  	- With the right mouse button you can change the angle of the engine camera.
	- You can move forwards/backards and left/right with the right mouse button and WASD and you can move up and down with Q and E.
	
  ### GameObject interaction
  	- Using the hierarchy panel you can select and move gameobjects from parent to parent.
	- Pressing the left mouse button to a GameObject with a mesh in the scene, this GameObject will be selected.
	- With the "Supr"/"Delete" button you can delete the selected GameObject.
	- With the "F" button you can focus your camera to the selected GameObject.
	- By pressing "W", "E" or "R", you can use diferent translation, rotation and scale gizmos. By pressing "T" you can change the gizmo form global to local space and vice versa. You can hide the gizmos by pressing "Q".

  ### Asset panel
  	- In the asset panel you can charge mesh scenes by double clicking a .fbx/.obj/.dae and pressing the "Open" button.
	- You can open a scene by selecting the scene and pressing the "Open" button.
	- You can create a GameObject by pressing the "Put to GameObject"button with a selected mesh  or assign a new mesh to an already created GameObject.
	- You can assign a texture to an already created GameObject by selecting a texture and pressing the "Put to GameObject" button.

  ### Play mode
  	- Press "Play" in the edit panel to watch the engine in the perspective of the GameObject that has a camera and the main camera property activated. Press "Stop" to resume activity on the engine
	- Press "Pause" to pause the current simulation (only stops in-game clock for now).
	- Move the "Time Scale" slider to increase or decrease the simulation speed (only affects the in-game clock for now).

  ### Drag and drop
  	- You can drop a mesh scene file (fbx/.obj/.dae) in assets/meshes to charge the scene in the file.
	- You can drop a scene file (.json) that you can create in the engine to open a scene.
	- You can drop a texture file in assets/textures to apply said texture to the object.

## Features
	- GameObject tree structure
	- Functioning transform, mesh, material and camera components
	- Creating cube, sphere and plane primitives as GameObjects
	- Fbx scene charge by dragging .fbx, .obj or .dae files into the engine
	- Creating GameObjects selecting a mesh in the assets panel
	- Changing GameObjects mesh by selecting a mesh in the assets panel
	- Charging or changing textures to a GameObject by selecting one in the assets panel, (.png, .jpg, .jpeg, .dds, .bmp and .tga 	files supported)
	- Config window that includes options for the application, window, renderer and scene, and information of other modules of the engine
	- Save and load for the config options
	- Console that notifies the user of the various changes of the engine, warnings or errors
	- Scene serialization (save and load scenes)
	- Mouse picking of GameObjects with
	- Moving, rotating and scaling with Gizmos
	- Camera culling
	- Quadtree optimizations for static GameObjects
	- Change of game view and engine view with the edit panel
	- Importation of assets to own format into a library folder
	- Resource manager
	

## List of tasks done in this release (assignment)
Ricardo Gutiérrez
	
	- GameObject system
	- Transform system for the GameObjects
	- Material importer system
	- Camera component and camera culling
	- Scene serialization
	- Resource manager: offline part

David Valdivia

	- GameObject system
	- Mesh and material components systems
	- Mesh/scene importer system
	- Engine camera refactoring
	- Mouse picking and gizmos implementation
	- Quadtree and quadtree optimizations (camera culling and mouse picking)
	- Time manager (game and engine times)
	- Resource manager: runtime part


## [Github repository](https://github.com/ValdiviaDev/Stardust-Engine)

License:

MIT License

Copyright (c) 2019 Ricardo Gutiérrez & David Valdivia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
  
