#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup(); // ���α׷��� ����� �� ȣ��Ǵ� �Լ��̴�. ���������� ���� �ʱ�ȭ�� �����Ѵ�. 
		void draw(); // gameState ���� ���� �Ʒ� 4�� �Լ��� ȣ���ϴ� ������ �Ѵ�.
		void keyPressed(int key); // gameState ���� �������� �÷��̾��� keyboard input�� �о�� ó�����ִ� �Լ��̴�.
	
    private:
        enum GameState {
            MENU,
            GAME,
            RESULT,
            INSTRUCTIONS
        };

        ofTrueTypeFont font;
        ofImage deliveryImage; // main menu�� ��µǴ� �̹����� ����
        GameState gameState; // ���� ���� ���� ���¸� ����
        string errorMessage; // ������ �߻��� ��� ȭ�鿡 ����� �޼����� ����
        string mapName; // ����ڰ� �÷����� ���� ���ϸ��� ����
        
        vector<ofVec2f> nodePositions; // ��� ��ġ�� �����ϴ� ����
        vector<vector<pair<int, int>>> graph; // city�� ���� ������ ����
        int weightSum; // ������ ��� ����� Power�� ��(���� ���� ��꿡 Ȱ���)
        int n; // ������ ��

        int previousCity; // ������ ��ģ city
        int currentCity; // ���� ���õ� city
        vector<int> visitedCity; // �� �� �̻� �湮�� cities

        int consumedPower; // �÷��̾ �Һ��� Power
        int minConsumedPower; // dijkstra�� ����� ��޿� ��� �ּ� Power

        int score; // ���� ���� �߰�S
        time_t startTime; // ���� ���� �ð� 
        time_t endTime; // ���� ���� �ð�
        int timeLimit = 40; // ���� �ð�



        //filename�� �̸��� ������ ���� ������ �о�� graph �ڷᱸ���� �����Ѵ�.
        void readMap(string filename);
        // �� ���(����)�� ���������� ��ġ�� �����Ѵ�.
        void initializeGraph();

        // gameState == INSTRUCTIONS�� ��� ȣ��Ǵ� �Լ��̴�. ���� �÷��� ����� ���� ������ �ִ� UI�� �׸���.
        void drawInstructions();
        // graph �ڷᱸ���� ����� ������ ���� graph(����)�� �׸���.
        void drawGraph();
        //gameState == MENU�� ��� ȣ��Ǵ� �Լ��̴�. �޴� ȭ�� UI�� �׸���.
        void drawMenu();
        //gameState == GAME�� ��� ȣ��Ǵ� �Լ��̴�. ���� ȭ�� UI�� �׸���.
        void drawGame();
        //gameState == RESULT�� ��� ȣ��Ǵ� �Լ��̴�. ������ ����Ǿ��� ��, ���ھ�� �ȳ� �޼����� ������ UI�� �׸���.
        void drawResult();
        
        
        //����ڰ� �ٸ� ���÷� �̵����� ��, �Ҹ��� power ���� �˾Ƴ� consumedPower ������ ���� �����Ѵ�.
        void updateConsumedPower(int preCity, int curCity);
        // ���� �̾��� ���� ���� ���ø� �̵��ϴ� ������ �Ǻ�
        bool checkValidSelect();
        //�����̽��ٰ� ������ ��, ��ޱ���� city �̵��� ó���ϴ� �Լ��̴�. 
        void selectCity();
        
        //graph�� ��� ������ weight�� ���Ͽ� �����ϴ� �Լ��̴�. �� ���� ���߿� ���� ���ھ ����� �� Ȱ��ȴ�.
        void calculateWeightSum();
        // ��� ���ú��� ������ ���ñ��� ���� ��� �� Power�� ���� ���� �Ҹ��ϴ� ����� Power �Ҹ��� �����Ѵ�.
        int dijkstra(int start, int n);
        //gameState�� RESULT�� �ٲ���� ��, ȭ�鿡 ǥ�õ� Score�� ����ϴ� �Լ��̴�. 
        void updateScore();
};
