#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
  
  // create a new oscillator which we'll use for the actual audio signal
  SineWave tone = SineWave();
  
  // create a sine wave we'll use for some vibrato
  SineWave vibratoOsc = SineWave();
  vibratoOsc.freq(10);
  
  // you can use the regular arithmatic operators on Generators and their subclasses (SineWave extends Generator)
  float basePitch = 400;
  Generator frequency = basePitch + (vibratoOsc * basePitch * 0.01);
  
  // plug that frequency generator into the frequency slot of the main audio-producing sine wave
  tone.freq(frequency);
  
  // let's also create a tremelo effect
  SineWave tremeloSine = SineWave().freq(1);
  
  // set the synth's final output generator
  synth.setOutputGen( tone * tremeloSine );
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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

void ofApp::audioRequested (float * output, int bufferSize, int nChannels){
  synth.fillBufferOfFloats(output, bufferSize, nChannels);
}