#include "tetris.h"

static struct sigaction act, oact;
int B,count;
long long space = 0;

int main(){
	int exit=0;

	createRankList();

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: {
			rank(); 
			break;
		}
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	writeRankFile();

	// 동적 할당된 메모리 해제
	Node *ptr = head;
	Node *target = head;
	for(ptr=head->link; ptr!=NULL; target=ptr, ptr=ptr->link) {
		free(target);
	}
	free(target);

	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	for(int i=0; i<VISIBLE_BLOCKS; i++) {
		nextBlock[i] = rand()%7;
	}
	// nextBlock[0]=rand()%7;
	// nextBlock[1]=rand()%7;
	// nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	
	// Recommand 기능 관련
	RecNode root;
	root.lv = 0;
	root.score = 0;
	memcpy(root.f, field, sizeof(field));
	field_to_minifield(root.f, &root);

	modified_recommend(&root);
	// recommend(&root);

	// 그림자가 그려지도록 DrawBlock 함수를 DrawBlockWithFeatures 함수로 교체하였다.
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(10+i, WIDTH+13);
		for(j=0; j<4; j++) {
			if(block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) { 
				// 게임판 범위 내에 블록이 존재한다면
				if(i+blockY<0 || HEIGHT <= i+blockY || j+blockX<0 || WIDTH <= j+blockX || f[i+blockY][j+blockX] == 1) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	int ypos = 0, xpos = 0, rot = blockRotate;
	switch(command) {
		case KEY_LEFT:
			xpos = 1; 
			break;
		case KEY_RIGHT:
			xpos = -1; 
			break;
		case KEY_UP:
			rot = (rot+3)%4;
			break;
		case KEY_DOWN:
			ypos = -1;
			break;
		default:
			break;
	}

	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	// attroff(A_REVERSE);
	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][rot][i][j] == 1 && i+blockY>=0) { 
				// 이전 블록의 위치 정보를 지운다.
				move(i+blockY+ypos+1, j+blockX+xpos+1);
				printw(".");
				
				// 다음 방법으로도 똑같이 구현가능하지만, 
				// 전체 field를 다시 출력하는 것은 비효율적이므로
				// 달라진 부분만을 갱신하는 위의 코드를 채택하였다.
				// f[blockY+i+ypos][blockX+j+xpos] = 0;
				// DrawField();
			}
		}
	}

	int shadow_ypos;
	for(shadow_ypos=(blockY+ypos); shadow_ypos<HEIGHT; shadow_ypos++) {
		if(CheckToMove(field, currentBlock, rot, shadow_ypos+1, blockX+xpos)==0) break;
	}

	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(shadow_ypos+i>=0 && block[currentBlock][rot][i][j]==1) {
				move(shadow_ypos+i+1, blockX+xpos+j+1);
				printw(".");
			}
		}
	}

	//3. 새로운 블록 정보를 그린다. 
	// 그림자가 그려지도록 DrawBlock 함수를 DrawBlockWithFeatures 함수로 교체하였다.
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
}

void BlockDown(int sig){
	// user code

	//강의자료 p26-27의 플로우차트를 참고한다.
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX) == 1) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	}
	else {
		if(blockY == -1) {
			gameOver = 1;
		}
		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);

		for(int i=0;i<VISIBLE_BLOCKS-1;i++) {
			nextBlock[i] = nextBlock[i+1];
		}
		nextBlock[VISIBLE_BLOCKS-1] = rand() % 7;

		RecNode root;
		root.lv = 0;
		root.score = 0;
		memcpy(root.f, field, sizeof(field));
		field_to_minifield(root.f, &root);
		// FILE* test = fopen("test.txt", "w");
		// for(int i=0; i<WIDTH; i++) {
		// 	fprintf(test, "%d ", root.minifield[i]);
		// }
		// fclose(test);
		modified_recommend(&root);
		// recommend(&root);

		blockRotate = 0;
		blockY=-1;
		blockX=WIDTH/2-2;
		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
	}
	//BlockDown(sig);
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	//Block이 추가된 영역의 필드값을 바꾼다.
	int bottom_cnt = 0;
	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				if(blockY+i == HEIGHT-1) bottom_cnt++;
				else if(f[blockY+i+1][blockX+j]==1) {
					bottom_cnt++;
				}
				f[blockY+i][blockX+j] = 1;
			}
		}
	}
	return bottom_cnt*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	char isfull; // 한 줄이 모두 채워졌는지 확인하는 변수(1: 모두 채워짐, 0: 모두 채워지지 않음)
	int delete_cnt = 0; // 삭제한 줄의 개수
	int delete_ypos[HEIGHT+1]; // 추가 배열을 이용해 시간복잡도 최소화
	for(int i=HEIGHT-1; i>=0; i--) {
		isfull = 1;
		for(int j=0; j<WIDTH; j++) {
			if(f[i][j] == 0) {
				isfull = 0;
				break;
			}
		}
		if(isfull) { // i번째 줄이 블록으로 꽉 차있다면
			delete_ypos[delete_cnt] = i; // i를 배열에 저장
			delete_cnt++; // 삭제한 줄의 개수 증가
		}
	}
	delete_ypos[delete_cnt] = 0;

	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	// 꽉 찬 줄이 발견될 때마다 한 칸씩 아래로 이동시키는 것이 아니라,
	// 지워야 할 줄을 delete_ypos[]에 저장해 두었다가, 한 번에 아래로 이동시킨다.
	for(int k=0; k<delete_cnt; k++) {
		int target = delete_ypos[k];
		for(int i=target-1; i>delete_ypos[k+1]; i--) {
			for(int j=0; j<WIDTH; j++) {
				f[i+k+1][j] = f[i][j];
			}
		}
		for(int j=0; j<WIDTH; j++) {
			f[0][j] = 0; 
		}
	}

	// 삭제한 줄의 수를 점수로 환산하여 return
	return delete_cnt*delete_cnt*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int shadow_ypos;
	for(shadow_ypos=y; shadow_ypos<HEIGHT; shadow_ypos++) {
		if(CheckToMove(field, blockID, blockRotate, shadow_ypos+1, x)==0) break;
	}
	DrawBlock(shadow_ypos, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawRecommend(y, x, blockID, blockRotate);
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i, j;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	if(fp == NULL) return;

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if(fscanf(fp, "%d", &score_number) == EOF) 
		score_number = 0;

	for(int i=0; i<score_number; i++) {
		char *_name = (char*)malloc(sizeof(char)*(NAMELEN+1));
		int _score;
		if (fscanf(fp, "%s %d", _name, &_score) != EOF) {
			Node* new = (Node*)malloc(sizeof(Node));
			strcpy(new->name, _name);
			new->score = _score;
			new->link = NULL;
			if(head == NULL) {
				head = new;
				tail = new;
			}
			else {
				tail->link = new;
				tail = tail->link;
			}
		}
		else {
			return;
		}
		free(_name);
	}
	
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=score_number, ch, i, j;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		echo();
		printw("X: ");
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		noecho();

		if(X>Y || X<1 || score_number < Y) {
			printw("search failure: no rank in the list\n");
			// return;
		}
		else {
			int count = 1;
			printw("      name      |   score\n");
			printw("----------------------------\n");
			for(Node *ptr = head; ptr != NULL; ptr = ptr->link) {
				if(X<=count && count<=Y) {
					printw("%-16s| %d\n", ptr->name, ptr->score);
					
				}
				else if(Y<count) break;
				count++;
			}
		}

	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;

		echo();
		printw("input the name: ");
		scanw("%s", str);
		noecho();

		printw("      name      |   score\n");
		printw("----------------------------\n");

		Node *ptr = head;
		int finddata = 0;
		for(; ptr!=NULL; ptr=ptr->link) {
			// ptr이 가리키는 노드의 이름과 입력받은 이름이 동일하다면
			if(strcmp(ptr->name, str) == 0) {
				printw("%-16s| %d\n", ptr->name, ptr->score);
				finddata = 1;
			}
		}
		if(finddata == 0) {
			printw("search failure: no name in the list\n");
		}
	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num;
		echo();
		printw("input the rank: ");
		scanw("%d", &num);
		noecho();

		if(num<1 || num>score_number) {
			printw("\nsearch failure: the rank not in the list\n");
		}
		else {
			Node *ptr = head;
			Node *pre = NULL;
			for(int i=1; i<num; i++) {
				if(ptr==NULL) break; // Err
				pre = ptr;
				ptr = ptr->link;
			}
			if(pre == NULL) { // linked list의 맨 첫 노드를 삭제하는 경우
				head = ptr->link;
			}
			else {
				pre->link = ptr->link;
			}
			free(ptr);
			score_number--;
			printw("\nresult: the rank deleted\n");
		}
	}
	// writeRankFile(); test에서 바로바로 rank.txt의 값을 보고 싶을 경우 주석 해제
	getch();
}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn, i;
	//1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_number);

	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	Node *ptr = head;
	for(int i=0; i<score_number; i++) {
		if(ptr == NULL) printw("Error\n");
		else {
			fprintf(fp, "%s %d\n", ptr->name, ptr->score);
			ptr = ptr->link;
		}
	}

	fclose(fp);
	// for ( i= 1; i < score_number+1 ; i++) {
	// 	free(a.rank_name[i]);
	// }
	// free(a.rank_name);
	// free(a.rank_score);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
	echo();
	printw("Input a user name : ");
	scanw("%s", str);
	noecho();

	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	Node *new = (Node*)malloc(sizeof(Node));
	Node *pre = NULL;
	Node *ptr = head;
	strcpy(new->name, str);
	new->score = score;
	new->link = NULL;
	if(head == NULL) {
		head = new;
		tail = new;
	}
	else {
		// 대소비교 후 삽입
		for(; ptr!=NULL; pre=ptr, ptr=ptr->link) {
			if(ptr->score < score) {
				new->link = pre->link;
				pre->link = new;
				break;
			}
		}
		if(pre == NULL) { // linked list의 맨 첫번째 노드로 삽입되는 경우
			new->link = head;
			head = new;
		}
		if(ptr == NULL) { // linked list의 맨 마지막 노드로 삽입되는 경우
			pre->link = new;
			tail = new;
		}
	}
	score_number++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(recY, recX, blockID, recRotate, 'R');
}

// int recommend(char fieldOri[HEIGHT][WIDTH],int lv) {
int recommend(RecNode *root) {
	int child_idx = 0;
	int maxscore = -1;

	
	// child node 메모리 할당
	if(root->lv < VISIBLE_BLOCKS) { 
		for(int i=0; i<CHILDREN_MAX; i++) {
			root->c[i] = (RecNode*)malloc(sizeof(RecNode)); 
			root->c[i]->lv = root->lv + 1;
			root->c[i]->score = root->score;
		}
	}

	// 각 rotation의 경우에 대해 child의 경우들 계산
	for(int r=0; r<NUM_OF_ROTATE; r++) {
		for(int j=-1; j<WIDTH; j++) {
			if(child_idx >= CHILDREN_MAX) break;

			// 블럭을 놓는 것이 불가능한 경우
			// if(CheckToMove(root->f, nextBlock[root->lv], r, 0, j)==0) {
			// 	root->c[child_idx++]->score = -1;
			// 	continue;
			// }
			
			// child에 field 복사
			memcpy(root->c[child_idx]->f, root->f, sizeof(field));

			int i;
			for(i=0; i<HEIGHT; i++) {
				// x = j, y = i+1 자리에 블럭을 놓을 수 없을 때까지 i 증가
				if(CheckToMove(root->c[child_idx]->f, nextBlock[root->lv], r, i+1, j)==0) break;
			}			

			// 위치 저장
			root->c[child_idx]->recBlockRotate = r;
			root->c[child_idx]->recBlockX = j;
			root->c[child_idx]->recBlockY = i;
			
			// 점수 계산
			root->c[child_idx]->score += AddBlockToField(root->c[child_idx]->f, nextBlock[root->lv], r, i, j);
			root->c[child_idx]->score += DeleteLine(root->c[child_idx]->f);
			
			// 다음 child node를 계산하도록 child_idx 1 증가
			child_idx++;
		}
	}

	// 아직 탐색 목표 depth에 도달하지 않은 경우
	if(root->lv < VISIBLE_BLOCKS-1) {
		for(int i=0; i<CHILDREN_MAX; i++) {
			// child node 존재?? 걍 0점 제외??
			if(root->c[i]->score > 0) {
				root->c[i]->score += recommend(root->c[i]);
			}
		}
	}

	for(int i=0; i<CHILDREN_MAX; i++) {
		if(maxscore <= root->c[i]->score) {
			if(root->lv == 0) { // 최상위 노드인 경우
				// 블록이 더 아래에 놓이거나 score가 더 높은 경우
				// recY < root->c[i]->recBlockY || 
				if(maxscore < root->c[i]->score || (maxscore == root->c[i]->score && recY < root->c[i]->recBlockY)) {
					recX = root->c[i]->recBlockX;
					recY = root->c[i]->recBlockY;
					recRotate = root->c[i]->recBlockRotate;
				}
			}
			maxscore=root->c[i]->score;
		}
		// 할당한 메모리 해제
		free(root->c[i]);
	}
	
	return maxscore;
}

void field_to_minifield(char f[HEIGHT][WIDTH], RecNode *root) {
	// 공간복잡도 측정
	space += sizeof(*root);

	for(int j=0; j<WIDTH; j++) {
		int i=0;
		for(i=0; i<HEIGHT && f[i][j]==0; i++);
		root->minifield[j] = i;
	}
}

int recommend_CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX, char minif[WIDTH]) {
	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				// 블록이 게임판 밖에 위치하는 경우 또는 이미 블럭이 존재하는 경우
				if(i+blockY<0 || HEIGHT <= i+blockY || j+blockX<0 || WIDTH <= j+blockX || f[i+blockY][j+blockX] == 1) {
					return 0;
				}
				// 블록이 사이에 존재하는 경우
				if(minif[blockX+j] <= blockY+i) return 0;
			}
		}
	}
	return 1;
}

int modified_recommend(RecNode *root) {
	int child_idx = 0;
	int maxscore = -1;
	int rotatemax = 4; // 해당 블럭의 고려해야 하는 회전량  디ㅏ거 
	int sumscore = 0; // child 경우의 score 합
	int childcnt = 0; // 유효한 child의 갯수

	switch(nextBlock[root->lv]) {
		case 6: rotatemax = 2; break;
		case 4: rotatemax = 1; break;
	}
	// child node 메모리 할당
	if(root->lv < VISIBLE_BLOCKS) { 
		for(int i=0; i<CHILDREN_MAX; i++) {
			root->c[i] = (RecNode*)malloc(sizeof(RecNode)); 
			root->c[i]->lv = root->lv + 1;
			root->c[i]->score = root->score;
		}
	}

	for(int r=0; r<rotatemax; r++) {
		for(int j=-1; j<WIDTH; j++) {
			if(child_idx >= CHILDREN_MAX) break;			

			// child에 field 복사
			memcpy(root->c[child_idx]->f, root->f, sizeof(root->f));
			memcpy(root->c[child_idx]->minifield, root->minifield, sizeof(root->minifield));

			char flag = 1;
			int i;
			if(j<0) i = 21;
			else i = root->c[child_idx]->minifield[j] - 1;
			for(int tmpi = 0; i-tmpi>=0; tmpi++) {
				if(recommend_CheckToMove(root->c[child_idx]->f, nextBlock[root->lv], r, i-tmpi, j, root->c[child_idx]->minifield)==1) {
					flag = 0;
					i = i-tmpi;
					break;
				}
			}
			if(flag==1) continue;
			
			// 위치 저장
			childcnt++;
			root->c[child_idx]->recBlockRotate = r;
			root->c[child_idx]->recBlockX = j;
			root->c[child_idx]->recBlockY = i;
			
			// 점수 계산
			root->c[child_idx]->score += AddBlockToField(root->c[child_idx]->f, nextBlock[root->lv], r, i, j);
			root->c[child_idx]->score += DeleteLine(root->c[child_idx]->f);
			sumscore += root->c[child_idx]->score;
			
			// minifield에 변경사항 저장
			//field_to_minifield(root->c[child_idx]->f, root->c[child_idx]);
			for(int tmpj=0; tmpj<BLOCK_WIDTH; tmpj++) {
				for(int tmpi=0; tmpi<BLOCK_HEIGHT; tmpi++) {
					if(block[nextBlock[root->lv]][r][tmpi][tmpj] == 1) {
						root->c[child_idx]->minifield[tmpj+j] = tmpi+i;
						break;
					}
					// if(root->c[child_idx]->f[tmpi+i][tmpj+j]==1) {
					// 	root->c[child_idx]->minifield[tmpj+j] = tmpi+i;
					// 	break;
					// }
				}
			}

			// 다음 child node를 계산하도록 child_idx 1 증가
			child_idx++;
		}
	}

	// 아직 탐색 목표 depth에 도달하지 않은 경우
	if(root->lv < VISIBLE_BLOCKS-1) {
		for(int i=0; i<CHILDREN_MAX; i++) {
			// score가 child들의 평균 score보다 큰 경우
			if(childcnt == 0) return 0;
			if(root->c[i]->score >= sumscore/childcnt) {
				root->c[i]->score += modified_recommend(root->c[i]);
			}
		}
	}

	for(int i=0; i<CHILDREN_MAX; i++) {
		if(maxscore <= root->c[i]->score) {
			if(root->lv == 0) { // 최상위 노드인 경우
				// 블록이 더 아래에 놓이거나 score가 더 높은 경우
				// recY < root->c[i]->recBlockY || 
				if(maxscore < root->c[i]->score || (maxscore == root->c[i]->score && recY < root->c[i]->recBlockY)) {
					recX = root->c[i]->recBlockX;
					recY = root->c[i]->recBlockY;
					recRotate = root->c[i]->recBlockRotate;
				}
			}
			maxscore=root->c[i]->score;
		}
		// 할당한 메모리 해제
		free(root->c[i]);
	}
	return maxscore;
}

void recommend_BlockDown(int sig) {
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX) == 0) {
		gameOver = 1;
		return;
	}
	DrawField();
	DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	//blockY++;
	
	// recommend된 블럭을 추가
	score += AddBlockToField(field, nextBlock[0], recRotate, recY, recX);
	score += DeleteLine(field);

	for(int i=0;i<VISIBLE_BLOCKS-1;i++) {
		nextBlock[i] = nextBlock[i+1];
	}
	nextBlock[VISIBLE_BLOCKS-1] = rand() % 7;

	RecNode root;
	root.lv = 0;
	root.score = 0;
	memcpy(root.f, field, sizeof(field));
	field_to_minifield(root.f, &root);
	modified_recommend(&root);

	blockRotate = 0;
	blockY=-1;
	blockX=WIDTH/2-2;
	DrawNextBlock(nextBlock);
	PrintScore(score);
	
	
	//BlockDown(sig);
	timed_out = 0;
}

void recommendedPlay(){
	// user code
	time_t start_t, finish_t;
	double play_t;

	start_t = time(NULL);

	int command;
	clear();
	act.sa_handler = recommend_BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(command==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	finish_t = time(NULL);
	play_t = (double)difftime(finish_t, start_t);

	move(HEIGHT+5, 0);
	printw("score(t): %d\n", score);
	printw("time(t): %lf\n", play_t);
	printw("space(t): %lld\n", space);
	printw("Time Efficiency(score(t)/time(t)): %lf\n", (double)score/play_t);
	printw("Space Efficiency(score(t)/space(t)): %lf\n", (double)score/space);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	// newRank(score);
}
