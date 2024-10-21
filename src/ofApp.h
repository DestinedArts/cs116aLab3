#pragma once

#include "ofMain.h"
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/euler_angles.hpp>



//  General Purpose Ray class 
//
class Ray {
public:
	Ray(glm::vec3 p, glm::vec3 d) { this->p = p; this->d = d; }
	void draw(float t) { ofDrawLine(p, p + t * d); }

	glm::vec3 evalPoint(float t) {
		return (p + t * d);
	}

	glm::vec3 p, d;
};

//  Base class for any renderable object in the scene
//
class SceneObject {
public:
	virtual ~SceneObject() = default;
	virtual void draw() = 0;    // pure virtual funcs - must be overloaded
	virtual bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) { return false; }
	virtual glm::vec3 getNormal(const glm::vec3& p) { return glm::vec3(1, 0, 0); }

	glm::mat4 getMatrix() {
		glm::mat4 T = glm::translate(glm::mat4(1.0), glm::vec3(position));
		glm::mat4 R = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));   // yaw, pitch, roll 
		glm::mat4 S = glm::scale(glm::mat4(1.0), scale);      // added this scale if you want to change size of object
		//cout << scale[0] << "," << scale[1] << "," << scale[2] << endl;
		//return T * R * S;
		//return R * T * S;
		//return S * R * T;
		return T * S * R;
	}
	void setPosition(glm::vec3 p) {
		position = p;
		transform = getMatrix();
	}
	void resetTransform() {
		transform = getMatrix();
	}

	// material properties (we will ultimately replace this with a Material class - TBD)
	//
	ofColor diffuseColor = ofColor::grey;    // default colors - can be changed.
	ofColor specularColor = ofColor::lightGray;
	

	// any data common to all scene objects goes here
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);    // degrees
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::mat4 transform;

	// selection
	//
	bool isSelectable = true;
	bool isSelected = false;
	string name = "SceneObject";
};

//  General purpose sphere  (assume parametric)
//
class Sphere : public SceneObject {
public:
	Sphere(glm::vec3 p, float r, ofColor diffuse = ofColor::lightGray) { position = p; radius = r; diffuseColor = diffuse; }
	Sphere() {}
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
		return (glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal));
	}
	glm::vec3 getNormal(const glm::vec3& p) { return glm::normalize(p - position); }

	void draw() {
		//	ofDrawSphere(position, radius); 
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		spherePrim.setRadius(radius);
		spherePrim.draw();
		ofPopMatrix();
	}

	float radius = 1.0;
	ofSpherePrimitive spherePrim;

};

//  General purpose plane 
//
class Plane : public SceneObject {
public:
	Plane(glm::vec3 p, glm::vec3 n, ofColor diffuse = ofColor::green, float w = 20, float h = 20) {
		position = p; normal = n;
		width = w;
		height = h;
		diffuseColor = diffuse;
		if (normal == glm::vec3(0, 1, 0))
			plane.rotateDeg(-90, 1, 0, 0);
		else if (normal == glm::vec3(0, -1, 0))
			plane.rotateDeg(90, 1, 0, 0);
		else if (normal == glm::vec3(1, 0, 0))
			plane.rotateDeg(90, 0, 1, 0);
		else if (normal == glm::vec3(-1, 0, 0))
			plane.rotateDeg(-90, 0, 1, 0);
	}
	Plane() {
		normal = glm::vec3(0, 1, 0);
		plane.rotateDeg(90, 1, 0, 0);
		isSelectable = false;
	}

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);
	glm::vec3 getNormal(const glm::vec3& p) { return this->normal; }
	void draw() {
		plane.setPosition(position);
		plane.setWidth(width);
		plane.setHeight(height);
		plane.setResolution(4, 4);
		//	plane.drawWireframe();
		plane.draw();
	}
	ofPlanePrimitive plane;
	glm::vec3 normal;

	float width = 20;
	float height = 20;

};

class Triangle {
public:
	Triangle(int i, int j, int k) { this->i = i; this->j = j; this->k = k; }
	int i, j, k;
};

class Pyramid : public SceneObject {
public:
	Pyramid(ofColor color = ofColor::blue) {
		init();
		diffuseColor = color;
	}
	Pyramid(glm::vec3 tran, glm::vec3 rot, glm::vec3 sc, ofColor color = ofColor::blue) {
		init();
		position = tran;
		rotation = rot;
		scale = sc;
		diffuseColor = color;
	}
	void init();
	void draw();
	void setLength(float l) { len = l; init(); }
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);

	float radius = 1.0;
	float len = 2.0;

	vector<glm::vec3> v;
	vector<Triangle> tri;
};

class Cube : public SceneObject {
public:
	Cube(ofColor color = ofColor::blue) {
		init();
		diffuseColor = color;
	}
	Cube(glm::vec3 tran, glm::vec3 rot, glm::vec3 sc, ofColor color = ofColor::blue) {
		init();
		position = tran;
		rotation = rot;
		scale = sc;
		diffuseColor = color;
	}
	void init();
	void draw();
	void setLength(float l) { len = l; init(); }
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);

	float radius = 1.0;
	float len = 2.0;

	vector<glm::vec3> v;
	vector<Triangle> tri;
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		bool mouseToDragPlane(int x, int y, glm::vec3& point);

		static void drawAxis(glm::mat4 transform = glm::mat4(1.0), float len = 1.0);

		// scene components
		//
		vector<SceneObject*> scene;
		vector<SceneObject*> selected;
		ofPlanePrimitive plane;

		// selection
		//
		bool objSelected() { return (selected.size() ? true : false); };
		void removeObject(SceneObject*);
		void clearSelectionList() {
			for (int i = 0; i < selected.size(); i++) {
				selected[i]->isSelected = false;
			}
			selected.clear();
		}


		// Lights
		//
		ofLight light1;

		// Cameras
		//
		ofEasyCam  mainCam;
		ofCamera sideCam;
		ofCamera topCam;
		ofCamera previewCam;
		ofCamera* theCam;    // set to current camera either mainCam or sideCam

		// Materials
		//
		ofMaterial material;

		// state
		bool bDrag = false;
		bool bHide = true;
		bool bAltKeyDown = false;
		bool bSftKeyDown = false;
		bool bCtrlKeyDown = false;
		bool bRotateX = false;
		bool bRotateY = false;
		bool bRotateZ = false;
		bool bScaleX = false;
		bool bScaleY = false;
		bool bScaleZ = false;
		glm::vec3 lastPoint;

};

