#include "ofApp.h"

//--------------------------------------------------------------
// ���α׷��� ����� �� ȣ��Ǵ� �Լ��̴�. ���������� ���� �ʱ�ȭ�� �����Ѵ�. 
void ofApp::setup() {
    font.load("verdana.ttf", 14);
    deliveryImage.load("delivery.png");
    deliveryImage.resize(350, 200);

    gameState = MENU;
    n = 10; // ������ ��
    initializeGraph();

    visitedCity.clear();
    vector<int>().swap(visitedCity);

    

    score = 0; // ���� ���� �ʱ�ȭ
    consumedPower = 0; // ��޿� �� Power �ʱ�ȭ
    weightSum = 0;
    mapName = "map1.txt"; // �⺻���� Map1����

    previousCity = 0; // 0�� ���ÿ������� ����
    currentCity = -1; 
    visitedCity.push_back(0); // 0�� ��� �湮ó��
}

// ��� ���� ������ �о���� �Լ�
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

// �� city�� ��ġ�� �����ϴ� �Լ�
void ofApp::initializeGraph() {
    nodePositions.resize(n);

    // �� City ����� ��ġ ����
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

// �� ����ġ(Power)�� ���� ����ϴ� �Լ�
void ofApp::calculateWeightSum() {
    for (int i = 0; i < graph.size(); i++) {
        for (auto y : graph[i]) {
            weightSum += y.second;
        }
    }
    weightSum /= 2;
}

//--------------------------------------------------------------
// gameState ������ �з��Ǿ� �Էµ� Ű���� ó���ϴ� �Լ�
void ofApp::keyPressed(int key) {
    switch (gameState) {
    case MENU:
        if (key == 's' || key == 'S') {
            readMap(mapName);
            startTime = time(0); // ���� �ð� ����
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
// gameState ���� ���� �˸��� draw �Լ��� ȣ���ϴ� �Լ�
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

// gameState == MENU�� ��� ȣ��Ǵ� �Լ��̴�. �޴� ȭ�� UI�� �׸���.
void ofApp::drawMenu() {
    ofSetColor(255);
    deliveryImage.draw(50, 200);
    font.drawString("<< Delivery Game >>", 450, 200);
    font.drawString("Choose Map  - Press '1' or '2' (default: 1)", 450, 250);
    font.drawString("Start Game - Press 'S'", 450, 350);
    font.drawString("Game Instruction - Press 'I'", 450, 400);
    font.drawString("Quit Game - Press 'Q'", 450, 450);
}

//gameState == GAME�� ��� ȣ��Ǵ� �Լ��̴�. ���� ȭ�� UI�� �׸���.
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
    // errorMessage�� �ִ� ��� ǥ��
    font.drawString(errorMessage, 400, 250);
}

// graph �ڷᱸ���� nodePosition�� ���� city ������ ȭ�鿡 �׸��� �Լ�
void ofApp::drawGraph() {
    for (int i = 0; i < n; ++i) {
        for (const auto& edge : graph[i]) {
            int j = edge.first;
            int weight = edge.second;
            // nodePositions�� ����� ��ǥ�� ���߾� city �׸���
            ofSetColor(255);
            ofDrawLine(nodePositions[i].x, nodePositions[i].y, nodePositions[j].x, nodePositions[j].y);
            font.drawString(std::to_string(weight), (nodePositions[i].x + nodePositions[j].x) / 2, (nodePositions[i].y + nodePositions[j].y) / 2);
        }
    }

    for (int i = 0; i < n; ++i) {
        ofSetColor(0, 0, 255); // �Ϲ� ���ô� �Ķ������� ǥ��
        for (int j = 0; j < visitedCity.size(); j++) {
            if (visitedCity[j] == i) { // visitedCity�� ���
                ofSetColor(204, 204, 0); // �����
            }
        }
        if (i == previousCity) { // PreviousCity�� ���
            ofSetColor(255, 128, 0); // ��Ȳ��
        }
        if (i == currentCity) { // ���� ������ City
            ofSetColor(255, 0, 0); // ������
        }

        ofDrawCircle(nodePositions[i].x, nodePositions[i].y, 30);
        ofSetColor(255);
        font.drawString(std::to_string(i), nodePositions[i].x - 5, nodePositions[i].y + 5);
    }
}

// Result ȭ���� UI�� �׸��� �Լ�
void ofApp::drawResult() {
    ofSetColor(255);
    font.drawString("<< GAME OVER >>", 450, 200);
    font.drawString("Final Score(it can be negative number): " + std::to_string(score), 450, 250);
    font.drawString("Optimal Consumed Power determined by Dijkstra's algorithm : " + std::to_string(minConsumedPower), 450, 300);
    font.drawString("Your Consumed Power : " + to_string(consumedPower), 450, 350);
    font.drawString("Your Play Time : " + to_string(int(endTime-startTime)), 450, 400);
    font.drawString("Back to Main Menu - Press 'M'", 450, 650);
}

// Instruction ȭ���� UI�� �׸��� �Լ�
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



// �̵������� city�� ���õǾ����� Ȯ���ϴ� �Լ�
bool ofApp::checkValidSelect() {
    // previousCity�� ����Ǿ� �ִ���
    for (auto y : graph[previousCity]) {
        if (y.first == currentCity) {
            errorMessage = "";
            return true;
        }
    }
    errorMessage = "This city cannot be visited from the current city!";
    return false;
}

// �����̽��ٰ� ������ ��, city�� �̵��� ó���ϴ� �Լ�
void ofApp::selectCity() {
    updateConsumedPower(previousCity, currentCity);
    // ���������� ������ ���
    if (currentCity == n - 1) {
        updateScore();
        gameState = RESULT;
    }
    
    visitedCity.push_back(currentCity);
    previousCity = currentCity;
    currentCity = -1;
}

// ����ڰ� �ٸ� ���÷� �̵����� ��, �Ҹ��� power ���� �˾Ƴ� consumedPower ������ ���� �����Ѵ�.
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

// ������ ����Ǿ��� �� ���ھ ����ϴ� �Լ� (���ͽ�Ʈ��� ��)
void ofApp::updateScore() {
    minConsumedPower = dijkstra(0, n);
    endTime = time(0);
    score = (weightSum - (consumedPower - minConsumedPower)) * 100;
    score += (timeLimit - (endTime - startTime)) * 100;
}

// ������������ �ִܰ�θ� ������ �����ϴ� �Լ�
int ofApp::dijkstra(int start, int n) {
    const int INF = numeric_limits<int>::max(); // ���Ѵ븦 ǥ���ϱ� ���� ���
    vector<int> dist(n, INF); // �� �������� power �Ҹ��� �����ϴ� �迭
    // weight�� ����ϱ� ���� pair ���·� priority queue ����
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq; 

    dist[start] = 0; // ���������� power �Ҹ� = 0
    pq.push({ 0, start }); // ���۳������� power = 0�� ���� ����� ��ȣ push

    while (!pq.empty()) {
        int d = pq.top().first;  // ���� ����� �Ÿ�
        int u = pq.top().second; // ���� ����� ��ȣ
        pq.pop();

        // dist�迭�� ����� ���� ����� ���� d���� ������ continue
        if (d > dist[u]) continue;

        // u�� ������ ��� ������ ���Ͽ�
        for (const auto& edge : graph[u]) {
            int v = edge.first; // �ش� ����� �Ÿ�
            int weight = edge.second; // �ش� ����� ��ȣ

            // u�� ���ļ� �ش� ���� ���� ���� �� ȿ������ ���
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({ dist[v], v });
            }
        }
    }

    return dist[n - 1];
}