#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

int n, m;
int dx[] = {-1, 0, 0, 1};
int dy[] = {0, -1, 1, 0};

char **maze = NULL;
int **board = NULL;

void searchmaze(int x, int y, int target, int dst) {
    board[x][y] = dst;
    for(int k=0; k<4; k++) {
        int nx = x+dx[k];
        int ny = y+dy[k];
        if(0<=nx && nx<m && 0<=ny && ny<n && board[nx][ny] == target) {
            searchmaze(nx, ny, target, dst);
        } 
    }
}

void writefile(const char* filename) {
    ofstream fout;
    fout.open(filename, ios::out);

    for(int i=0; i<m*2+1; i++) {
        for(int j=0; j<n*2+1; j++) {
            fout << maze[i][j];
        } fout << "\n";
    }

    fout.close();
}

int main() {
    int number = 1;
    int randnum;
    srand((unsigned int)time(NULL));
    cin >> n >> m;
    
    // 메모리 동적 할당
    maze = new char*[m*2+1];
    for(int i=0; i<m*2+1; i++) {
        maze[i] = new char[n*2+1];
    }
    
    board = new int*[m];
    for(int i=0; i<m; i++) {
        board[i] = new int[n];
    }

    // 미로 뼈대 그리기
    for(int i=0; i<m*2+1; i++) {
        for(int j=0; j<n*2+1; j++) {
            if(i%2 == 0) {
                if(j%2 == 0) maze[i][j] = '+';
                else maze[i][j] = '-';
            }
            else {
                if(j%2 == 0) maze[i][j] = '|';
                else maze[i][j] = ' ';
            }
        }
    }
    // 0번째 row 초기화
    for(int j=0; j<n; j++) {
        board[0][j] = number++;
    }

    for(int i=0; i<m-1; i++) {
        // 오른쪽 벽 뚫기
        for(int j=0; j<n-1; j++) {
            randnum = rand()%2;
            if(randnum == 1 && board[i][j] != board[i][j+1]) {
                maze[i*2+1][(j+1)*2] = ' ';
                searchmaze(i, j+1, board[i][j+1], board[i][j]);
            }
        }

        // 아래 벽 뚫기
        for(int j=0; j<n; j++) {
            randnum = rand()%2;
            // 각 숫자마다 적어도 1칸은 아래 벽을 뚫어야 하므로
            if(board[i][j] != board[i][j+1]) {
                randnum = 1;
            }

            if(randnum == 1) {
                maze[(i+1)*2][j*2+1] = ' ';
                board[i+1][j] = board[i][j];
            }
            else {
                board[i+1][j] = number++;
            }
        }
    }

    for(int j=0; j<n-1; j++) {
        if(board[m-1][j] != board[m-1][j+1]) {
            maze[(m-1)*2+1][(j+1)*2] = ' ';
            searchmaze(m-1, j, board[m-1][j+1], board[m-1][j]);
        }
    }

    // for test
    // for(int i=0; i<m*2+1; i++) {
    //     for(int j=0; j<n*2+1; j++) {
    //         if(i%2==1 && j%2==1) cout << board[i/2][j/2];
    //         else cout << maze[i][j];
    //     } cout << endl;
    // } cout << endl;

    // 파일로 저장
    writefile("maze.maz");

    // 메모리 할당 해제
    for(int i=0; i<m*2+1; i++) {
        delete[] maze[i];
    }
    delete[] maze;
    
    for(int i=0; i<m; i++) {
        delete[] board[i];
    }
    delete[] board;
}