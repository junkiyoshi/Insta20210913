#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openframeworks");

	ofBackground(239);
	ofSetColor(0);

	this->shader.load("shader/shader.vert", "shader/shader.frag");
	this->fbo1.allocate(ofGetWidth(), ofGetHeight());
	this->fbo2.allocate(ofGetWidth(), ofGetHeight());
	this->fbo3.allocate(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	vector<glm::vec2> location_list;
	vector<glm::vec2> next_list;

	this->fbo1.begin();
	ofClear(0);
	ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);
	
	for (int i = 0; i < 3; i++) {

		auto deg = ofGetFrameNum() + i * 120;
		auto location = glm::vec2(230 * cos(deg * DEG_TO_RAD), 230 * sin(deg * DEG_TO_RAD));
		auto next = glm::vec2(230 * cos((deg + 1) * DEG_TO_RAD), 230 * sin((deg + 1) *DEG_TO_RAD));

		location_list.push_back(location);
		next_list.push_back(next);
	}

	for (int i = 0; i < location_list.size(); i++) {

		this->draw_arrow(location_list[i], next_list[i], 120);
	}

	this->fbo1.end();

	this->fbo2.begin();
	ofClear(0);
	ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);

	for (int i = 0; i < location_list.size(); i++) {

		auto location = location_list[i];
		auto next = next_list[i];

		int range = 120;
		for (int k = 0; k < 10; k++) {

			auto child_noise_seed = glm::vec2(ofRandom(1000), ofRandom(1000));
			auto child_location = location + glm::vec2(ofMap(ofNoise(child_noise_seed.x, ofGetFrameNum() * 0.01), 0, 1, -range, range), ofMap(ofNoise(child_noise_seed.y, ofGetFrameNum() * 0.01), 0, 1, -range, range));
			auto child_next = next + glm::vec2(ofMap(ofNoise(child_noise_seed.x, (ofGetFrameNum() + 1) * 0.01), 0, 1, -range, range), ofMap(ofNoise(child_noise_seed.y, (ofGetFrameNum() + 1) * 0.01), 0, 1, -range, range));

			this->draw_arrow(child_location, child_next, 30);

		}
	}

	this->fbo2.end();

	this->fbo3.begin();
	ofClear(0);
	ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);

	int out = 600;
	int in = 500;

	ofBeginShape();
	ofVertex(out * -0.5, out * -0.5);
	ofVertex(out *  0.5, out * -0.5);
	ofVertex(out *  0.5, out *  0.5);
	ofVertex(out * -0.5, out *  0.5);
	ofNextContour(true);
	ofVertex(in * -0.5, in * -0.5);
	ofVertex(in *  0.5, in * -0.5);
	ofVertex(in *  0.5, in *  0.5);
	ofVertex(in * -0.5, in *  0.5);

	ofEndShape();

	this->fbo3.end();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofFill();
	this->shader.begin();
	this->shader.setUniform1f("time", ofGetElapsedTimef());
	this->shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	this->shader.setUniformTexture("tex1", this->fbo1.getTexture(), 1);
	this->shader.setUniformTexture("tex2", this->fbo2.getTexture(), 2);
	this->shader.setUniformTexture("tex3", this->fbo3.getTexture(), 3);

	ofDrawRectangle(glm::vec2(0, 0), ofGetWidth(), ofGetHeight());

	this->shader.end();
}

//--------------------------------------------------------------
void ofApp::draw_arrow(glm::vec2 location, glm::vec2 next_location, float size) {

	auto angle = std::atan2(next_location.y - location.y, next_location.x - location.x);

	ofFill();
	ofBeginShape();
	ofVertex(location + glm::vec2(size * cos(angle), size * sin(angle)));
	ofVertex(location + glm::vec2(size * 0.5 * cos(angle + PI * 0.5), size * 0.5 * sin(angle + PI * 0.5)));
	ofVertex(location + glm::vec2(size * 0.5 * cos(angle - PI * 0.5), size * 0.5 * sin(angle - PI * 0.5)));
	ofEndShape();

	ofBeginShape();
	ofVertex(location + glm::vec2(size * 0.5 * cos(angle + PI * 0.5), size * 0.5 * sin(angle + PI * 0.5)) * 0.25);
	ofVertex(location + glm::vec2(size * 0.5 * cos(angle + PI * 0.5), size * 0.5 * sin(angle + PI * 0.5)) * 0.25 - glm::vec2(size * cos(angle), size * sin(angle)) * 0.5);
	ofVertex(location + glm::vec2(size * 0.5 * cos(angle - PI * 0.5), size * 0.5 * sin(angle - PI * 0.5)) * 0.25 - glm::vec2(size * cos(angle), size * sin(angle)) * 0.5);
	ofVertex(location + glm::vec2(size * 0.5 * cos(angle - PI * 0.5), size * 0.5 * sin(angle - PI * 0.5)) * 0.25);
	ofEndShape();
}

//--------------------------------------------------------------
int main() {

	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(720, 720);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}