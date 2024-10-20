#include "ofApp.h"

// Intersect Ray with Plane  (wrapper on glm::intersect*)
//
bool Plane::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normalAtIntersect) {
	float dist;
	bool insidePlane = false;
	bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
	if (hit) {
		Ray r = ray;
		point = r.evalPoint(dist);
		normalAtIntersect = this->normal;
		glm::vec2 xrange = glm::vec2(position.x - width / 2, position.x + width / 2);
		glm::vec2 yrange = glm::vec2(position.y - width / 2, position.y + width / 2);
		glm::vec2 zrange = glm::vec2(position.z - height / 2, position.z + height / 2);

		// horizontal 
		//
		if (normal == glm::vec3(0, 1, 0) || normal == glm::vec3(0, -1, 0)) {
			if (point.x < xrange[1] && point.x > xrange[0] && point.z < zrange[1] && point.z > zrange[0]) {
				insidePlane = true;
			}
		}
		// front or back
		//
		else if (normal == glm::vec3(0, 0, 1) || normal == glm::vec3(0, 0, -1)) {
			if (point.x < xrange[1] && point.x > xrange[0] && point.y < yrange[1] && point.y > yrange[0]) {
				insidePlane = true;
			}
		}
		// left or right
		//
		else if (normal == glm::vec3(1, 0, 0) || normal == glm::vec3(-1, 0, 0)) {
			if (point.y < yrange[1] && point.y > yrange[0] && point.z < zrange[1] && point.z > zrange[0]) {
				insidePlane = true;
			}
		}
	}

	ofImage background;
	float w = background.getWidth();
	float h = background.getHeight();
	return insidePlane;
}

void Pyramid::init() {

	v.clear();
	tri.clear();
	//
	// verts (local space)
	//
	v.push_back(glm::vec3(-radius, len / 2.0, radius));
	v.push_back(glm::vec3(radius, len / 2.0, radius));
	v.push_back(glm::vec3(radius, len / 2.0, -radius));
	v.push_back(glm::vec3(-radius, len / 2.0, -radius));
	v.push_back(glm::vec3(0, -len / 2.0, 0));

	// triangles (base)
	//
	tri.push_back(Triangle(0, 1, 3));
	tri.push_back(Triangle(1, 2, 3));

	// sides
	//
	tri.push_back(Triangle(0, 4, 1));
	tri.push_back(Triangle(1, 4, 2));
	tri.push_back(Triangle(2, 4, 3));
	tri.push_back(Triangle(3, 4, 0));

}

// Draw a Unit cube (size = 2) transformed 
//
void Pyramid::draw() {

	//rotateToVector(glm::vec3(0, 1, 0), glm::vec3(1, 1, 1));

	glm::mat4 m = getMatrix();

	//   push the current stack matrix and multiply by this object's
	//   matrix. now all vertices will be transformed by this matrix
	//
	ofPushMatrix();
	ofMultMatrix(m);
	for (int i = 0; i < tri.size(); i++) {
		glm::vec3 v1 = v[tri[i].i];
		glm::vec3 v2 = v[tri[i].j];
		glm::vec3 v3 = v[tri[i].k];
		ofMaterial material;
		material.setDiffuseColor(diffuseColor);
		material.begin();
		ofDrawTriangle(v1, v2, v3);
		material.end();

		// draw normal (one each at vertex)
		//
		glm::vec3 normal = glm::cross(glm::normalize(v3 - v2), glm::normalize(v1 - v2));
		ofDrawLine(v1, v1 + (.2 * normal));
		ofDrawLine(v2, v2 + (.2 * normal));
		ofDrawLine(v3, v3 + (.2 * normal));
	}
	ofPopMatrix();


	// draw axis
	//
//	ofApp::drawAxis(m, 1.5);

}


bool Pyramid::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {

	glm::mat4 m = getMatrix();
	for (int i = 0; i < tri.size(); i++) {

		// Compute normal (take cross product of two sides)
		//
		glm::vec3 v1 = m * glm::vec4(v[tri[i].i], 1.0);
		glm::vec3 v2 = m * glm::vec4(v[tri[i].j], 1.0);
		glm::vec3 v3 = m * glm::vec4(v[tri[i].k], 1.0);
		normal = glm::cross(glm::normalize(v3 - v2), glm::normalize(v1 - v2));

		// first check if ray is on correct side of triangle
		//
		if (glm::dot(ray.d, normal) >= 0.0)
			continue;

		// check ray/triangle intersection
		//
		glm::vec2 baryPoint;
		float t;
		if (glm::intersectRayTriangle(ray.p, ray.d, v1, v2, v3, baryPoint, t)) {
			point = ray.p + (ray.d * t);
			return true;
		}
	}
	return false;
}




//--------------------------------------------------------------
void ofApp::setup(){

	// setup background and cameras
	//
	ofSetBackgroundColor(ofColor::black);
	ofEnableDepthTest();
	mainCam.setDistance(15);
	mainCam.setNearClip(.1);
	mainCam.disableMouseInput();

	sideCam.setPosition(12, 0, 0);
	sideCam.lookAt(glm::vec3(0, 0, 0));
	topCam.setNearClip(.1);
	topCam.setPosition(0, 12, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));
	ofSetFrameRate(24);

	ofSetSmoothLighting(true);

	// setup simple scene
	//
	scene.push_back(new Plane(glm::vec3(0, -1, 0), glm::vec3(0, 1, 0), ofColor::darkGreen));

	// setup one point light
	// 
	// 
	//
	light1.enable();
	light1.setPosition(0, 20, 0);
	light1.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	light1.setSpecularColor(ofColor(255.f, 255.f, 255.f));
	light1.setAmbientColor(ofColor(150, 150, 150));

	theCam = &mainCam;


}

//--------------------------------------------------------------
void ofApp::update(){
	// rotate the object when selected
	if (objSelected())
		selected[0]->rotation.x += 1.0;	// rotates on its x-axis
	// selected[0]->rotation.y += 1.0;	// rotates on its y-axis
	// selected[0]->rotation.z += 1.0;	// rotates on its z-axis
}

//--------------------------------------------------------------
void ofApp::draw(){

	theCam->begin();
	ofNoFill();
	drawAxis();
	ofEnableLighting();
	//  draw the objects in scene
	//
//	material.begin();
	ofFill();

	// draw the scene.  If an object is selected, draw in white - otwherwise
	// draw in it's diffuse color.
	//
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i]->isSelected)
			ofSetColor(ofColor::white);
		else ofSetColor(scene[i]->diffuseColor);
		scene[i]->draw();
	}

	// draw the default light
	light1.setScale(.1, .1, .1);
	ofSetColor(ofColor::grey);
	light1.draw();

	//	material.end();
	ofDisableLighting();
	theCam->end();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
//		if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
//		else mainCam.enableMouseInput();
//		break;
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bHide = !bHide;
		break;
	case 'i':
		break;
	
	case 'p':
	{
		Pyramid* pyramid = new Pyramid();
		// pyramid->scale = glm::vec3(5, 1, 1);
		scene.push_back(pyramid);
		break;
	}
	case 's':
	{
		Sphere *sphere = new Sphere();
		scene.push_back(sphere);
		break;
	}
	break;
	
	case 'x':
		bRotateX = true;
		break;
	case 'y':
		bRotateY = true;
		break;
	case 'z':
		bRotateZ = true;
		break;

	case OF_KEY_F1:
		theCam = &mainCam;
		break;
	case OF_KEY_F2:
		theCam = &sideCam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_F4:
		theCam = &previewCam;
		break;
	case OF_KEY_ALT:		// was not working in openframeworks11, now works in 12
		bAltKeyDown = true;
		if (!mainCam.getMouseInputEnabled()) mainCam.enableMouseInput();
		break;
	case OF_KEY_SHIFT:
		bSftKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_BACKSPACE:
		if (objSelected()) {
			removeObject(selected[0]);
			clearSelectionList();
		}
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
		case OF_KEY_ALT:
			bAltKeyDown = false;
			mainCam.disableMouseInput();
			break;
		case OF_KEY_CONTROL:
			bCtrlKeyDown = false;
			break;
		case 'x':
			bRotateX = false;
			break;
		case 'y':
			bRotateY = false;
			break;
		case 'z':
			bRotateZ = false;
			break;

		default:
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::removeObject(SceneObject* obj) {

	// remove from selection list
	//
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i] == obj) {
			scene.erase(scene.begin() + i);
			break;
		}
	}

	// remove from scene list;
	//
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i] == obj) {
			scene.erase(scene.begin() + i);
			break;
		}
	}

	// delete object;  sceneObject destructor will handle cleaning up parent and child lists.
	//
	delete obj;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (objSelected() && bDrag) {
		glm::vec3 point;
		mouseToDragPlane(x, y, point);
		if (bRotateX) {
			selected[0]->rotation += glm::vec3((point.x - lastPoint.x) * 20.0, 0, 0);
		}
		else if (bRotateY) {
			selected[0]->rotation += glm::vec3(0, (point.x - lastPoint.x) * 20.0, 0);
		}
		else if (bRotateZ) {
			selected[0]->rotation += glm::vec3(0, 0, (point.x - lastPoint.x) * 20.0);
		}
		else {
			selected[0]->position += (point - lastPoint);

		}
		lastPoint = point;
	}

}

bool ofApp::mouseToDragPlane(int x, int y, glm::vec3& point) {
	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	float dist;
	glm::vec3 pos;
	if (objSelected()) {
		pos = selected[0]->position;
	}
	else pos = glm::vec3(0, 0, 0);
	if (glm::intersectRayPlane(p, dn, pos, glm::normalize(theCam->getZAxis()), dist)) {
		point = p + dn * dist;
		return true;
	}
	return false;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if we are moving the camera around, don't allow selection
	//
	if (mainCam.getMouseInputEnabled()) return;

	// clear selection list
	//
	if (!bSftKeyDown) clearSelectionList();


	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	// check for selection of scene objects
	//
	SceneObject* selectedObj = NULL;

	for (int i = 0; i < scene.size(); i++) {

		glm::vec3 point, norm;

		float nearestDist = std::numeric_limits<float>::infinity();

		// it is possible to hit multple objects with one mouse ray. If so , select
		// the closet one.
		//
		if (scene[i]->isSelectable && scene[i]->intersect(Ray(p, dn), point, norm)) {
			selectedObj = scene[i];
			float dist = glm::length(point - theCam->getPosition());
			if (dist < nearestDist) {
				nearestDist = dist;
				selectedObj = scene[i];
			}
		}
	}

	if (selectedObj) {
		selectedObj->isSelected = true;
		selected.push_back(selectedObj);
		bDrag = true;
		mouseToDragPlane(x, y, lastPoint);
	}
	else {
		clearSelectionList();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bDrag = false;

}

// 
// Draw an XYZ axis in RGB at transform
//
void ofApp::drawAxis(glm::mat4 m, float len) {

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(len, 0, 0, 1)));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(0, len, 0, 1)));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(0, 0, len, 1)));
}


