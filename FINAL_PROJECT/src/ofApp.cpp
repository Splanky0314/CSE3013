#include "ofApp.h"

//--------------------------------------------------------------
// 프로그램이 실행될 때 호출되는 함수이다. 전역변수에 대한 초기화를 수행한다. 
void ofApp::setup() {
    font.load("verdana.ttf", 14);
    deliveryImage.load("delivery.png");
    deliveryImage.resize(350, 200);

    gameState = MENU;
    n = 10; // 도시의 수
    initializeGraph();

    visitedCity.clear();
    vector<int>().swap(visitedCity);

    

    score = 0; // 게임 점수 초기화
    consumedPower = 0; // 배달에 든 Power 초기화
    weightSum = 0;
    mapName = "map1.txt"; // 기본값은 Map1으로

    previousCity = 0; // 0번 도시에서부터 시작
    currentCity = -1; 
    visitedCity.push_back(0); // 0번 노드 방문처리
}

// 배달 지도 파일을 읽어오는 함수
void ofApp::readMap(string filename) {
    cout << "READ!!";
    ifstream fin(ofToDataPath(filename).c_str());
    if (!fin.is_open()) {
        ofLogError("readMap") << "Failed to open fin: " << filename;
        return;
    }

    // graph.clear(); 
    if (!graph.empty()) {
        for (auto& y : graph) {
            vector<pair<int, int>>().swap(y);
        }
        vector<vector<pair<int, int>>>().swap(graph);
    }
    graph.resize(n);

    int a, b, wei;
    while (fin >> a >> b >> wei) {
        graph[a].emplace_back(b, wei);
        graph[b].emplace_back(a, wei);
    }

    fin.close(); 
}

// 각 city의 위치를 지정하는 함수
void ofApp::initializeGraph() {
    nodePositions.resize(n);

    // 각 City 노드의 위치 지정
    nodePositions[0] = ofVec2f(100, 450);
    nodePositions[1] = ofVec2f(200, 600);
    nodePositions[2] = ofVec2f(250, 450);
    nodePositions[3] = ofVec2f(200, 300);
    nodePositions[4] = ofVec2f(400, 550);
    nodePositions[5] = ofVec2f(400, 350);
    nodePositions[6] = ofVec2f(600, 650);
    nodePositions[7] = ofVec2f(650, 450);
    nodePositions[8] = ofVec2f(600, 300);
    nodePositions[9] = ofVec2f(900, 450);

    calculateWeightSum();
}

// 총 가중치(Power)의 합을 계산하는 함수
void ofApp::calculateWeightSum() {
    for (int i = 0; i < graph.size(); i++) {
        for (auto y : graph[i]) {
            weightSum += y.second;
        }
    }
    weightSum /= 2;
}

//--------------------------------------------------------------
// gameState 값으로 분류되어 입력된 키값을 처리하는 함수
void ofApp::keyPressed(int key) {
    switch (gameState) {
    case MENU:
        if (key == 's' || key == 'S') {
            readMap(mapName);
            startTime = time(0); // 시작 시간 저장
            gameState = GAME;
        }
        else if (key == 'i' || key == 'I') {
            gameState = INSTRUCTIONS;
        }
        else if (key == 'q' || key == 'Q') {
            ofExit();
        }
        else if (key == '1') {
            mapName = "map1.txt";
        }
        else if (key == '2') {
            mapName = "map2.txt";
        }
        break;

    case GAME:
        if (key >= '0' && key <= '9') {
            currentCity = key - '0';
        }
        else if (key == ' ') {
            if (checkValidSelect()) {
                selectCity();
            }
            else {
                currentCity = -1;
            }
        }
        break;

    case RESULT:
        if (key == 'm' || key == 'M') {
            setup();
        }
        break;
    case INSTRUCTIONS:
        if (key) {
            gameState = MENU;

        }
        break;
    }

}

//--------------------------------------------------------------
// gameState 값에 따라 알맞은 draw 함수를 호출하는 함수
void ofApp::draw() {
    switch (gameState) {
    case MENU:
        drawMenu();
        break;

    case GAME:
        drawGame();
        break;

    case RESULT:
        drawResult();
        break;

    case INSTRUCTIONS:
        drawInstructions();
        break;
    }
}

// gameState == MENU인 경우 호출되는 함수이다. 메뉴 화면 UI를 그린다.
void ofApp::drawMenu() {
    ofSetColor(255);
    deliveryImage.draw(50, 200);
    font.drawString("<< Delivery Game >>", 450, 200);
    font.drawString("Choose Map  - Press '1' or '2' (default: 1)", 450, 250);
    font.drawString("Start Game - Press 'S'", 450, 350);
    font.drawString("Game Instruction - Press 'I'", 450, 400);
    font.drawString("Quit Game - Press 'Q'", 450, 450);
}

//gameState == GAME인 경우 호출되는 함수이다. 게임 화면 UI를 그린다.
void ofApp::drawGame() {
    if (timeLimit < int(time(0) - startTime)) {
        updateScore();
        gameState = RESULT;
    }
    drawGraph();

    ofSetColor(255);
    font.drawString("Choose City: " + std::to_string(currentCity) + " - Press 1-9", 400, 100);
    font.drawString("Select City - Press SpaceBar", 400, 150);
    font.drawString("Time: " + std::to_string(int(time(0) - startTime))+"/"+std::to_string(timeLimit), 400, 200);
    font.drawString("Consumed Power: "+std::to_string(consumedPower), 600, 200);
    // errorMessage가 있는 경우 표시
    font.drawString(errorMessage, 400, 250);
}

// graph 자료구조와 nodePosition을 토대로 city 지도를 화면에 그리는 함수
void ofApp::drawGraph() {
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : graph[i]) {
            int j = edge.first;
            int weight = edge.second;
            // nodePositions에 저장된 좌표에 맞추어 city 그리기
            ofSetColor(255);
            ofDrawLine(nodePositions[i].x, nodePositions[i].y, nodePositions[j].x, nodePositions[j].y);
            font.drawString(std::to_string(weight), (nodePositions[i].x + nodePositions[j].x) / 2, (nodePositions[i].y + nodePositions[j].y) / 2);
        }
    }

    for (int i = 0; i < n; ++i) {
        ofSetColor(0, 0, 255); // 일반 도시는 파란색으로 표시
        for (int j = 0; j < visitedCity.size(); j++) {
            if (visitedCity[j] == i) { // visitedCity인 경우
                ofSetColor(204, 204, 0); // 노란색
            }
        }
        if (i == previousCity) { // PreviousCity인 경우
            ofSetColor(255, 128, 0); // 주황색
        }
        if (i == currentCity) { // 현재 선택한 City
            ofSetColor(255, 0, 0); // 빨간색
        }

        ofDrawCircle(nodePositions[i].x, nodePositions[i].y, 30);
        ofSetColor(255);
        font.drawString(std::to_string(i), nodePositions[i].x - 5, nodePositions[i].y + 5);
    }
}

// Result 화면의 UI를 그리는 함수
void ofApp::drawResult() {
    ofSetColor(255);
    font.drawString("<< GAME OVER >>", 450, 200);
    font.drawString("Final Score(it can be negative number): " + std::to_string(score), 450, 250);
    font.drawString("Optimal Consumed Power determined by Dijkstra's algorithm : " + std::to_string(minConsumedPower), 450, 300);
    font.drawString("Your Consumed Power : " + to_string(consumedPower), 450, 350);
    font.drawString("Your Play Time : " + to_string(int(endTime-startTime)), 450, 400);
    font.drawString("Back to Main Menu - Press 'M'", 450, 650);
}

// Instruction 화면의 UI를 그리는 함수
void ofApp::drawInstructions() {
    ofSetColor(255);
    
    font.drawString("<< GAME RULE >>", 450, 200);
    font.drawString("You need to deliver the package to City 9 as quickly as possible. ", 100, 250);
    font.drawString("However, the routes between each city are limited. ", 100, 300);
    font.drawString("Review the given paths and select the optimal route starting from City 0 to reach City 9.", 100, 350);
    font.drawString("It is possible to revisit nodes that have already been visited.", 100, 400);

    font.drawString("Choose the city to move to using the number keys 1-9.", 100, 450);
    font.drawString("Press Space Bar to move to the selected city.", 100, 500);
    font.drawString("The game ends when the time limit is reached or you arrive at City 9.", 100, 550);
    font.drawString("Your final score will be determined based on the time taken and To return to the main menu - Press Any Key!", 100, 600);
    font.drawString("whether you chose the optimal path.", 100, 650);

    font.drawString("To return to the main menu - Press Any Key!", 100, 700);
}



// 이동가능한 city가 선택되었는지 확인하는 함수
bool ofApp::checkValidSelect() {
    // previousCity와 연결되어 있는지
    for (auto y : graph[previousCity]) {
        if (y.first == currentCity) {
            errorMessage = "";
            return true;
        }
    }
    errorMessage = "This city cannot be visited from the current city!";
    return false;
}

// 스페이스바가 눌렸을 때, city의 이동을 처리하는 함수
void ofApp::selectCity() {
    updateConsumedPower(previousCity, currentCity);
    // 도착지점에 도달한 경우
    if (currentCity == n - 1) {
        updateScore();
        gameState = RESULT;
    }
    
    visitedCity.push_back(currentCity);
    previousCity = currentCity;
    currentCity = -1;
}

// 사용자가 다른 도시로 이동했을 때, 소모한 power 값을 알아내 consumedPower 변수의 값을 갱신한다.
void ofApp::updateConsumedPower(int preCity, int curCity) {
    int weight = 0;
    for (auto y : graph[preCity]) {
        if (y.first == curCity) {
            weight = y.second;
            break;
        }
    }
    consumedPower += weight;
}

// 게임이 종료되었을 때 스코어를 계산하는 함수 (다익스트라와 비교)
void ofApp::updateScore() {
    minConsumedPower = dijkstra(0, n);
    endTime = time(0);
    score = (weightSum - (consumedPower - minConsumedPower)) * 100;
    score += (timeLimit - (endTime - startTime)) * 100;
}

// 목적지까지의 최단경로를 산출해 리턴하는 함수
int ofApp::dijkstra(int start, int n) {
    const int INF = numeric_limits<int>::max(); // 무한대를 표현하기 위해 사용
    vector<int> dist(n, INF); // 각 노드까지의 power 소모량을 저장하는 배열
    // weight를 고려하기 위해 pair 형태로 priority queue 선언
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq; 

    dist[start] = 0; // 시작지점의 power 소모량 = 0
    pq.push({ 0, start }); // 시작노드까지의 power = 0과 시작 노드의 번호 push

    while (!pq.empty()) {
        int d = pq.top().first;  // 기준 노드의 거리
        int u = pq.top().second; // 기준 노드의 번호
        pq.pop();

        // dist배열에 저장된 기준 노드의 값이 d보다 작으면 continue
        if (d > dist[u]) continue;

        // u와 인접한 모든 간선에 대하여
        for (const auto& edge : graph[u]) {
            int v = edge.first; // 해당 노드의 거리
            int weight = edge.second; // 해당 노드의 번호

            // u를 거쳐서 해당 노드로 가는 것이 더 효율적인 경우
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({ dist[v], v });
            }
        }
    }

    return dist[n - 1];
}