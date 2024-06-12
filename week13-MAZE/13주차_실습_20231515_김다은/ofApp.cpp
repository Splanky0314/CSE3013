/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") {
		readFile();
	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if (title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			isdfs = DFS(); // 수정함
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;

	}

	if (title == "Show BFS") {
		doTopmost(bChecked); // Use the checked value directly

	}

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	// ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;

	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	int ratio = 20; // 미로 선의 크기 비율
	for (int i = 0; i < inputmaze.size(); i++) {
		for (int j = 0; j < inputmaze[0].size(); j++) {
			if (inputmaze[i][j] == '|') {
				ofDrawLine(ratio * j, ratio * (i - 1), ratio * j, (i + 1) * ratio);
			}
			else if (inputmaze[i][j] == '-') {
				ofDrawLine(ratio * (j - 1), ratio * i, ratio * (j + 1), ratio * i);
			}
		}
	}

	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if (bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			int input_flag = 0;

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;


			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
			// 이전에 할당한 메모리 해제
			freeMemory();
			isdfs = 0;
			for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
				string line = *it;
				inputmaze.push_back(line);
			}
			return true;
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() {

	//TO DO
	// malloc한 memory를 free해주는 함수

	inputmaze.clear(); // vector inputmaze에 저장된 값 제거
	vector<string>().swap(inputmaze); // vector에 할당된 메모리 해제
}

bool ofApp::DFS()//DFS탐색을 하는 함수
{
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	int x, y;
	// 미로 목적지 좌표
	int destx = inputmaze.size() - 2, desty = inputmaze[0].size() - 2;
	// printf("%d %d\n\n", destx, desty);
	int nx, ny, dir;
	bool success = 0; // 목적지 도착 여부
	// stack 초기화
	while (!st.empty()) st.pop();
	// anspath, path 초기화
	path.clear();
	anspath.clear();
	vector<pair<int, int>>().swap(path);
	vector<pair<int, int>>().swap(anspath);

	// maze 크기에 맞추어 visited 배열 동적할당
	visited = new bool* [inputmaze.size()];
	for (int i = 0; i < inputmaze.size(); i++) {
		visited[i] = new bool[inputmaze[0].size()];
		for (int j = 0; j < inputmaze[0].size(); j++) {
			visited[i][j] = 0; // 0으로 초기화
		}
	}

	// 출발지점
	st.push({ 1, 1, 0 });
	visited[1][1] = 1; // 방문 처리

	while (!st.empty()) {
		x = get<0>(st.top());
		y = get<1>(st.top());
		dir = get<2>(st.top());
		// st.pop();	
		// printf("%d %d %d\n", x, y, dir);
		path.push_back({ x, y });		

		// 벽에 막힌 경로인지 확인
		bool flag = 0;
		for (; dir < maxdir; dir++) {
			nx = x + dx[dir];
			ny = y + dy[dir];
			if (inputmaze[nx][ny] == '|' || inputmaze[nx][ny] == '-') {

				continue;
			}
			else {
				nx += dx[dir]; ny += dy[dir];
				if (!visited[nx][ny] && 0 <= nx && nx < inputmaze.size() && 0 <= ny && ny < inputmaze[0].size() && dir < maxdir) {
					st.push({ nx, ny, 0 });
					
					visited[nx][ny] = 1; // 방문처리
					flag = 1;
				
					if (nx == destx && ny == desty) {
						success = 1;
						break;
					}
					break;
				}
			}
			
		}
		if (!flag) {
			st.pop();
		}
		if (success) break;
	}
	
	// visited 동적 할당 메모리 해제
	for (int i = 0; i < inputmaze.size(); i++) {
		delete[] visited[i];
	}
	delete[] visited;

	// for test
	while (!st.empty()) {
		// printf("dfs 결과: %d %d %d\n", get<0>(st.top()), get<1>(st.top()), get<2>(st.top()));
		anspath.push_back({ get<0>(st.top()), get<1>(st.top()) });
		st.pop();
	}
	
	if (success) return true;
	return false;
}
void ofApp::dfsdraw()
{
	//TO DO 
	ofSetLineWidth(5);
	
	int ratio = 20; // 미로 정답 선의 길이 비율
	int prex, prey, x, y;
	
	// 탐색한 경로 그리기
	ofSetColor(0, 256, 0);
	prex = path[0].first; prey = path[0].second;
	for (int i = 1; i < path.size(); i++) {
		x = path[i].first; y = path[i].second;
		//printf("path: %d %d %d %d\n", prex, prey, x, y);
		ofDrawLine(ratio * prey, ratio * prex, ratio * y, ratio * x);
		prex = x; prey = y;
	}


	// 정답 경로 그리기
	ofSetColor(256, 0, 0);
	prex = anspath[0].first, prey = anspath[0].second, x, y;
	for (int i = 1; i < anspath.size(); i++) {
		x = anspath[i].first; y = anspath[i].second;
		//printf("anspath: %d %d %d %d\n", prex, prey, x, y);
		ofDrawLine(ratio * prey, ratio * prex, ratio * y, ratio * x);
		prex = x; prey = y;
	}

	
}

