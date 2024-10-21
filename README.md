# About
Lab 3 - Transformations
Play around with the starter file that demonstrations matrix transformations in GLM (and openframeworks).
Create two animations:
  a. an object spinning around it's local origin (with the object translated/moved to some arbitrary point)
  b. an object spinning around the world origin
Create your own Primitive.

**Code Editor**: Visual Studio<br>
**Library**: OpenGL (openFrameworks)<br>
**Addons**: ofxGUI

# Notes
random notes taken during class

* operator overloading glm::mat4: glm library knows trans rotation scale matrix is 4x4
* glm matrix operators have more support in 3D than 2D
* glm has a matrix stack
* ofPushMatrix();
* ofMultMatrix();
* ofDrawTriangle(verts[], verts[], verts[]);
* ofPopMatrix();  // if do not pop, every triangle has same transformation

use simple numbers then scale up

### TO-DO:
* make an abstract/inherit class for primitive classes
