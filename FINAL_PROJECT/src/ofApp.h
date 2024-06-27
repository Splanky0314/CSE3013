#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup(); // 프로그램이 실행될 때 호출되는 함수이다. 전역변수에 대한 초기화를 수행한다. 
		void draw(); // gameState 값에 따라 아래 4개 함수를 호출하는 역할을 한다.
		void keyPressed(int key); // gameState 값을 기준으로 플레이어의 keyboard input을 읽어와 처리해주는 함수이다.
	
    private:
        enum GameState {
            MENU,
            GAME,
            RESULT,
            INSTRUCTIONS
        };

        ofTrueTypeFont font;
        ofImage deliveryImage; // main menu에 출력되는 이미지를 저장
        GameState gameState; // 현재 게임 진행 상태를 저장
        string errorMessage; // 에러가 발생한 경우 화면에 출력할 메세지를 저장
        string mapName; // 사용자가 플레이할 맵의 파일명을 저장
        
        vector<ofVec2f> nodePositions; // 노드 위치를 저장하는 벡터
        vector<vector<pair<int, int>>> graph; // city의 지도 정보를 저장
        int weightSum; // 지도의 모든 경로의 Power의 합(최종 점수 계산에 활용됨)
        int n; // 도시의 수

        int previousCity; // 직전에 거친 city
        int currentCity; // 현재 선택된 city
        vector<int> visitedCity; // 한 번 이상 방문한 cities

        int consumedPower; // 플레이어가 소비한 Power
        int minConsumedPower; // dijkstra로 계산한 배달에 드는 최소 Power

        int score; // 점수 변수 추가S
        time_t startTime; // 게임 시작 시간 
        time_t endTime; // 게임 종료 시간
        int timeLimit = 40; // 제한 시간



        //filename의 이름을 가지는 지도 파일을 읽어와 graph 자료구조에 저장한다.
        void readMap(string filename);
        // 각 노드(도시)의 지도에서의 위치를 지정한다.
        void initializeGraph();

        // gameState == INSTRUCTIONS인 경우 호출되는 함수이다. 게임 플레이 방법에 대한 설명이 있는 UI를 그린다.
        void drawInstructions();
        // graph 자료구조에 저장된 정보를 토대로 graph(지도)를 그린다.
        void drawGraph();
        //gameState == MENU인 경우 호출되는 함수이다. 메뉴 화면 UI를 그린다.
        void drawMenu();
        //gameState == GAME인 경우 호출되는 함수이다. 게임 화면 UI를 그린다.
        void drawGame();
        //gameState == RESULT인 경우 호출되는 함수이다. 게임이 종료되었을 때, 스코어와 안내 메세지를 포함한 UI를 그린다.
        void drawResult();
        
        
        //사용자가 다른 도시로 이동했을 때, 소모한 power 값을 알아내 consumedPower 변수의 값을 갱신한다.
        void updateConsumedPower(int preCity, int curCity);
        // 서로 이어져 있지 않은 도시를 이동하는 것인지 판별
        bool checkValidSelect();
        //스페이스바가 눌렸을 때, 배달기사의 city 이동을 처리하는 함수이다. 
        void selectCity();
        
        //graph의 모든 간선의 weight를 합하여 저장하는 함수이다. 이 합은 나중에 최종 스코어를 계산할 때 활용된다.
        void calculateWeightSum();
        // 출발 도시부터 목적지 도시까지 가는 경로 중 Power를 제일 적게 소모하는 경우의 Power 소모량을 리턴한다.
        int dijkstra(int start, int n);
        //gameState가 RESULT로 바뀌었을 때, 화면에 표시될 Score를 계산하는 함수이다. 
        void updateScore();
};
