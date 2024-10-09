#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <vector>
#include <unistd.h>
using namespace std;

#define W 51
#define H 31
#define BLOCK 219
#define KEY_SIZE 11 // Size of the key, bluecactus plus null character.


void endScreen(int ms);
void enter();
void endingScreen();
void title();


//struct User
struct User{
	char name[100];
	char password[100];
	int highscore;
	
	User* left;
	User* right;
	int height;
}*root = NULL;

User* currUser = NULL;
void writeAllUser(User* curr);

// constructor
User* createNewUser(char name[100], char password[100], int highscore) {
	User* newUser = (User*)malloc(sizeof(User));
	strcpy(newUser->name, name);
	strcpy(newUser->password, password);
	newUser->highscore = highscore;
	newUser->left = newUser->right = NULL;
	newUser->height = 1;
	
	return newUser;
}

//game logic & maze generation

struct PlayerNode{
	int x;
	int y;
	char symbol;
	
	PlayerNode(int x, int y, char symbol) {
		this->x = x;
		this->y = y;
		this->symbol = symbol;
	}
};

char map[H][W];
int vision = 3;
//time utk hitung udah brp detik (timer buat flashlight)
time_t flashLightTimer = 0;
PlayerNode currPlayer(1, 1, 'P');

struct Map{
	int X;
	int Y;
	char Symbol;
	Map* backtrackPath;
	Map* parent;
	bool visit;
	Map(int X,int Y){
		this->X = X;
		this->Y = Y;
		this->Symbol = 219;
		this->backtrackPath = NULL;
		this->visit = false;
	}
};

struct Graph{
	Map *src;
	Map *des;
	int value;
	Graph(Map *src,Map *des,int value){
		this->src = src;
		this->des = des;
		this->value = value;
	}
};

Map* Maze[H+1][W+1];

void initMaze(){
	for(int i =0;i<H;i++){
		for(int j =0;j<W;j++){
			Maze[i][j] = new Map(i,j);
			Maze[i][j]->parent = Maze[i][j];
		}
	}
}

void printMap(){
	for(int i =0;i<H;i++){
		for(int j =0;j<W;j++){
			printf("%c",Maze[i][j]->Symbol);
		}
		printf("\n");
	}	
}

vector<Graph*> primQueue;
vector<Graph*> primFinal;
vector <Graph*> kruskalQueue;

void addQueue(){
	for(int i =0;i<H;i++){
		for(int j =0;j<W;j++){
			if(i %2 == 1 && j%2 == 1) {
				if(j != W-2){
					kruskalQueue.push_back(new Graph(Maze[i][j],Maze[i][j+2],rand()));
				}
				if(i != H-2){
					kruskalQueue.push_back(new Graph(Maze[i][j],Maze[i+2][j],rand()));
				}
			} 
		}
	}
}

void swap(int i,int j){
	Graph *temp = kruskalQueue[i];
	kruskalQueue[i] = kruskalQueue[j];
	kruskalQueue[j] = temp;
}

void queueSort(){
	for(int i =0;i<kruskalQueue.size()-1;i++){
		for(int j =i;j<kruskalQueue.size();j++){
			Graph *a = kruskalQueue[i];
			Graph *b = kruskalQueue[j];
			if(a->value > b->value){
				swap(i,j);
			}
		}
	}
	
}



Map *findParent(Map *curr){
	if(curr->parent == curr){
		return curr;
	} 
	findParent(curr->parent);
}

void kruskalMaze(){

	for(int i =0;i<kruskalQueue.size();i++){
		Graph *temp = kruskalQueue.at(i);
		Map *x = findParent(temp->src);
		Map *y = findParent(temp->des);
		if(x->parent != y->parent){
			int x1,y1;
			x1 = abs(temp->des->X - temp->src->X);
			y1 = abs(temp->des->Y - temp->src->Y);
			
			temp->src->Symbol = ' ';
			
			Map *temp1;
			if(x1 == 0){
				temp1 = Maze[temp->src->X][temp->src->Y + 1];
				Maze[temp->src->X][temp->src->Y + 1]->Symbol = ' ';
			} 
			else if(y1 == 0){
				temp1 = Maze[temp->src->X + 1][temp->src->Y];
				Maze[temp->src->X + 1][temp->src->Y]->Symbol = ' ';
			} 
			
			temp->des->Symbol = ' '; 
			
	
			x->parent = y;
		}
	}
}

void KruskalMaze(){
	addQueue();
	queueSort();
	kruskalMaze();
}


Graph *minMaze(){
	int min = 999999;
	Graph *curr = NULL;
	for(int i =0;i<primQueue.size();i++){
		Graph *temp = primQueue[i];
		if(temp->value < min && temp->des->visit == false){
			min = temp->value;
			curr = temp;
		}
	}
	return curr;
}


void PrimMaze(){
	int xVector[] = {2,0,-2,0};
	int yVector[] = {0,2,0,-2};
	int curri=1,currj=1;
	Graph *temp;
	Maze[1][1]->visit = true;
	
	while(true){
		for(int i =0;i<4;i++){
			int itemp = curri+xVector[i],jtemp = currj+yVector[i];
			if(itemp >= 1 && itemp <= H - 2 && jtemp >= 1 && jtemp <= W - 2 && Maze[itemp][jtemp]->visit == false){
				primQueue.push_back(new Graph(Maze[curri][currj],Maze[itemp][jtemp],rand()));
			}
		}
		
		temp = minMaze();
		if(temp == NULL) break;
		else{
			Map *mazetemp;
			temp->src->Symbol = ' ';
			int xfinal = temp->des->X - temp->src->X;
			int yfinal = temp->des->Y - temp->src->Y;
			
			if(xfinal == 2){
				mazetemp = Maze[temp->src->X + 1][temp->src->Y];
				Maze[temp->src->X + 1][temp->src->Y]->Symbol = ' ';	
			} 
			else if(yfinal == 2){
				mazetemp = Maze[temp->src->X][temp->src->Y + 1];
				Maze[temp->src->X][temp->src->Y + 1]->Symbol = ' ';
			}else if(xfinal == -2){
				mazetemp = Maze[temp->src->X - 1][temp->src->Y];
				Maze[temp->src->X - 1][temp->src->Y]->Symbol = ' ';
			} 
			else if(yfinal == -2){
				mazetemp = Maze[temp->src->X][temp->src->Y - 1];
				Maze[temp->src->X][temp->src->Y - 1]->Symbol = ' ';
			}  
			temp->des->Symbol = ' ';
			primFinal.push_back(temp);
			temp->des->visit = true;
			curri = temp->des->X,currj = temp->des->Y;
		}	
	}
	
	
}

//kiri, kanan, atas, bawah
int moveX[] = {0, 0, -1, 1};
int moveY []= {-1, 1, 0, 0};

bool flag;
void moveAction(int idxMove) {
	
	//cek apakah user bisa ke tempat yg dituju
	if (Maze[currPlayer.x + moveX[idxMove]][currPlayer.y + moveY[idxMove]]->Symbol == ' ' || Maze[currPlayer.x + moveX[idxMove]][currPlayer.y + moveY[idxMove]]->Symbol == '*' || Maze[currPlayer.x + moveX[idxMove]][currPlayer.y + moveY[idxMove]]->Symbol == 'E') {
		//kalau ketemu item
		if (Maze[currPlayer.x + moveX[idxMove]][currPlayer.y + moveY[idxMove]]->Symbol == '*') {
			//set timer menjadi time sekarang
			flashLightTimer = time(NULL);
			//tambahin vision
			vision = 5;
		}
		
		else if (Maze[currPlayer.x + moveX[idxMove]][currPlayer.y + moveY[idxMove]]->Symbol == 'E') {
			//ending screen
			flag = false;
//			endScreen(100);
				
			
		}
		
		Maze[currPlayer.x][currPlayer.y]->Symbol = ' ';
		Maze[currPlayer.x + moveX[idxMove]][currPlayer.y + moveY[idxMove]]->Symbol = currPlayer.symbol;
		currPlayer.x = currPlayer.x + moveX[idxMove];
		currPlayer.y = currPlayer.y + moveY[idxMove];
	}
	

}

//3x3 pos = 2, 5x5 pos = 3, 7x7 pos = 4
void movePrint(int pos) {
	for (int i = currPlayer.x - pos; i<=(currPlayer.x + pos); i++) {
		for (int j = currPlayer.y - pos; j<=(currPlayer.y + pos); j++) {
			if (i == currPlayer.x - pos || j == currPlayer.y - pos || i == currPlayer.x + pos || j == currPlayer.y + pos || i >= H || j >= W || i < 0 || j < 0) {
				printf("#");
			} else {
				printf("%c", Maze[i][j]->Symbol);	
			}
		}
		puts("");
	}
}

time_t bigStart;
time_t currTime = NULL;

void generateFlashlights() {
	int count = 0;
	for(int i =0;i<H;i++){
		for(int j =0;j<W;j++){
			//kalau kosong
			if (Maze[i][j]->Symbol == ' ') {
				//1 in 15 space
				if ((rand() % 17) == 0) {
					Maze[i][j]->Symbol = '*';
				} 
//				if (count == 17) {
//					Maze[i][j]->Symbol = '*';	
//					count = 0;
//				}
//				else count++;
			}
		}
	}
}

void generatePrimMaze() {
	initMaze();
	srand(time(NULL));
	PrimMaze();
	Maze[1][1]->Symbol = currPlayer.symbol;
	Maze[H-2][W-2]->Symbol = 'E';
	generateFlashlights();
	
}

void generateKruskalMaze() {
	initMaze();
	srand(time(NULL));
	KruskalMaze();
	Maze[1][1]->Symbol = currPlayer.symbol;
	Maze[H-2][W-2]->Symbol = 'E';
	generateFlashlights();
}



void gamePlay() {
	srand(time(NULL));
	system("cls");
	printf("Ready ? Press enter to start...\n");
	getchar();
	system("cls");
	movePrint(vision);
	flag = true;
	char keyInput[KEY_SIZE] = {0};
	
	//insert code here
	srand(time(NULL));
	
	
	currPlayer.x = 1;
	currPlayer.y = 1;
	
	bigStart = time(NULL);
	
	
	while(flag) {
		char input;
//		scanf("%c", &input); getchar();
		
		if (kbhit()) {
			input = _getch();
			
			 // Shift  inputted chars left
            memmove(keyInput, keyInput+1, KEY_SIZE - 1);

            // masukin ke akhir array, -2 karena last spot utk null terminator
            keyInput[KEY_SIZE - 2] = input;

            // null terminate
            keyInput[KEY_SIZE - 1] = '\0';

            // cek apakah match
            if (strcmp(keyInput, "bluecactus") == 0) {
                printf("Cheat activated! (Press Enter to continue)"); getchar();
                vision = 10;
            }
//			
//			if (input == 'q') {
//				printMap(); getchar();
//			}
			switch(input) {
				case 'a':
					moveAction(0);
					break;
				case 'd':
					moveAction(1);
					break;
				case 'w':
					moveAction(2);
					break;
				case 's':
					moveAction(3);
					break;
			}
			
			system("cls");
			movePrint(vision);
		} 
		
		currTime = time(NULL);
		
		if (flashLightTimer != -1) {
			if (currTime - flashLightTimer >= 5) {
				//set vision back to default
				vision = 3;
				//set reset timer
				flashLightTimer = -1;
				system("cls");
				movePrint(vision);	
			}
		}
		
	}
	
	endingScreen();
}

void mazeMenu() {
	
	int opt;
	
	do {
		system("cls");
		printf("Hello %s !\n", currUser->name);
		printf("Choose your preferred maze generator,\n");
		printf("--------------\n");
		printf("1. Prim\n");
		printf("2. Kruskal\n");
		printf("3. Exit\n");
		printf(">> ");
		
		scanf("%d", &opt); getchar();
		
		opt == 1 ? generatePrimMaze() : generateKruskalMaze();
		
	} while (opt < 1 || opt > 3);
	
	if (opt != 3) gamePlay();
}


int totalPointMenu(int interval) {
	system("cls");
	
	int initialPoint = 100;
	double multiplier;
	int pointClass;
	
	if (interval <= 60) {
		multiplier = 3;
		pointClass = 60;
	}
	else if (interval <= 120) {
		multiplier = 2;
		pointClass = 120;
	}
	else if (interval <= 180) {
		multiplier = 1.5;
		pointClass = 180;
	} 
	else {
		multiplier = 1;	
		pointClass = interval;
	}

	int mainPoint = multiplier * initialPoint;
	int bonusPoint = (pointClass - interval) * multiplier;
	
	int totalPoint = mainPoint + bonusPoint;
	
	char temp[100];
	sprintf(temp, "Main point: %d\n", mainPoint);
	
	char temp1[100];
	sprintf(temp1, "Bonus point: %d\n", bonusPoint);
	
	char temp2[] = "------------------\n";
	
	char temp3[100];
	sprintf(temp3, "Total point: %d\n", totalPoint);
	
	for (int i = 0; i<strlen(temp); i++) {
		printf("%c", temp[i]);
		Sleep(20);
	}
	
	for (int i = 0; i<strlen(temp1); i++) {
		printf("%c", temp1[i]);
		Sleep(20);
	}
	
	for (int i = 0; i<strlen(temp2); i++) {
		printf("%c", temp2[i]);
		Sleep(20);
	}
	
	for (int i = 0; i<strlen(temp3); i++) {
		printf("%c", temp3[i]);
		Sleep(20);
	}

	getchar();
	return totalPoint;
	
	
}

void endingScreen() {
	system("cls");
	int interval = currTime - bigStart;
	char temp[100];
	sprintf(temp, "Congratulation %s ! You did it in %d seconds !\n", currUser->name, interval);
	
	for (int i = 0; i<strlen(temp); i++) {
		printf("%c", temp[i]);
		Sleep(20);
	}
	
	getchar();
	
	int tempp = totalPointMenu(interval);
	
	if (tempp > currUser->highscore) {
		//update 
		currUser->highscore = tempp;
		
		FILE *fptr = fopen("user.txt", "w");
		fprintf(fptr, "");
		fclose(fptr);
		
		writeAllUser(root);
	}
	
}

struct UserHeap{
	char name[100];
	int highscore;
};

UserHeap* createNewUserHeap(char name[100], int highscore) {
	UserHeap *newUser = (UserHeap*)malloc(sizeof(UserHeap));
	strcpy(newUser->name, name);
	newUser->highscore = highscore;
	return newUser;
}

//heap sort
void swapForHeap(UserHeap** a, UserHeap** b)
{
    UserHeap temp = **a;
    **a = **b;
    **b = temp;
}

void heapify(UserHeap* arr[], int N, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < N && arr[left]->highscore < arr[largest]->highscore) {
    	 largest = left;
	}

    if (right < N && arr[right]->highscore < arr[largest]->highscore) {
    	largest = right;
	}

    if (largest != i) {

        swapForHeap(&arr[i], &arr[largest]);
        heapify(arr, N, largest);
    }
}

void heapSort(UserHeap* arr[], int N)
{
    for (int i = N / 2 - 1; i >= 0; i--) {
    	heapify(arr, N, i);
	}
    for (int i = N - 1; i >= 0; i--) {
        swapForHeap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

void getHighscore() {
	system("cls");
	// array
	// n utk len array
	
	//read file dlu cari len total user
	FILE *fptr = fopen("user.txt", "r");
	int count = 0;
	while (!feof(fptr)) {
		char name[100];
		char password[100];
		int highscore;
		fscanf(fptr, "%[^#]#%[^#]#%d\n", name, password, &highscore);
		count++;
	}
	fclose(fptr);
	
	//init array dgn size yg didapat
	UserHeap* arrayHeap[count];
	
	FILE *fptr1 = fopen("user.txt", "r");
	
	//masukin smua ke array
	int count2 = 0;
	while (!feof(fptr)) {
		char name[100];
		char password[100];
		int highscore;
		fscanf(fptr1, "%[^#]#%[^#]#%d\n", name, password, &highscore);
		arrayHeap[count2] = createNewUserHeap(name, highscore);
		count2++;
	}
	fclose(fptr1);
	
	heapSort(arrayHeap, count);
	printf("-------------------------------\n");
	printf("|%-4s |%-10s |%-10s |\n", "No.", "Name", "Highscore");
	printf("-------------------------------\n");
	
	for (int i = 0; i<count; i++) {
		printf("|%d.%-2s |%-10s |%-10d |\n", i+1, "", arrayHeap[i]->name, arrayHeap[i]->highscore);
		Sleep(100);
	}
	puts("");
	enter();
	
}


//AVL util
int getHeight(User* curr) {
	if (!curr) return 0;
	return curr->height;
}

int max(int a, int b) {
	return a > b ? a : b;
}

int getBalance(User* curr) {
	// > 0 artinya condong ke kiri
	return getHeight(curr->left) - getHeight(curr->right);
}

User* rotateRight(User* curr) {
	User* x = curr->left;
	User* y = x->right;
	
	x->right = curr;
	curr->left = y;
	
	//update height
	x->height = 1 + max(getHeight(x->left), getHeight(x->right));
	curr->height = 1 + max(getHeight(curr->left), getHeight(curr->right));
	
	return x;
}

User* rotateLeft(User* curr) {
	User* x = curr->right;
	User* y = x->left;
	
	x->left = curr;
	curr->right = y;
	
	//update height
	x->height = 1 + max(getHeight(x->left), getHeight(x->right));
	curr->height = 1 + max(getHeight(curr->left), getHeight(curr->right));
	
	return x;
}

User* rebalance(User* curr) {
	curr->height = 1 + max(getHeight(curr->left), getHeight(curr->right));
	
	//LL
	if(getBalance(curr) > 1 && getBalance(curr->left) >= 0){
		curr = rotateRight(curr);
	}
	//RR
	else if(getBalance(curr) < -1 && getBalance(curr->right) <= 0){
		curr = rotateLeft(curr);
	}
	//LR
	else if(getBalance(curr) > 1 && getBalance(curr->left) < 0){
		curr->left = rotateLeft(curr->left);
		curr = rotateRight(curr);
	}
	//RL
	else if(getBalance(curr) < -1 && getBalance(curr->right) > 0){
		curr->right = rotateRight(curr->right);
		curr = rotateLeft(curr);
	}
	
	return curr;
	
}

//AVL insert by name
User* insertAVL(User* curr, User* newUser) {
	if (curr == NULL) {
		return newUser;
	}
	
	if (strcmp(newUser->name, curr->name) < 0) {
		curr->left = insertAVL(curr->left, newUser);
	} else if (strcmp(newUser->name, curr->name) > 0) {
		curr->right = insertAVL(curr->right, newUser);
	}
	
	curr = rebalance(curr);	
		
	return curr;
}

//search by name
User* searchAVL(User* curr, char name[100]) {
	//kalau ga ketemu
	if (curr == NULL) {
		return NULL;
	}
	
	if (strcmp(name, curr->name) < 0) {
		return searchAVL(curr->left, name);
	} else if (strcmp(name, curr->name) > 0) {
		return searchAVL(curr->right, name);
	} 
	//found
	else {
		return curr;
	}	
}

bool validateLogin(char name[100], char password[100]) {
	//search di avl (kalau g null artinya name exist)
	User* temp = searchAVL(root, name);
	
	if (temp == NULL) {
		return false;
	} else {
		//kalau password match
		if (strcmp(password, temp->password) == 0) return true;
		return false;
	}
}

bool validateRegisterName(char name[100]) {
	User* temp = searchAVL(root, name);
	
	//kalau NULL alias gaada artinya unique
	if (temp == NULL) return true;
	else return false;
}
char tempRegisterPassword[100];
int tempRegisterI = 0;

void writeUser(char name[100], char password[100], int score) {
	FILE *fptr = fopen("user.txt", "a");
	
	fprintf(fptr, "%s#%s#%d\n", name, password, score);
	
	fclose(fptr);
}

void writeAllUser(User* curr) {
	if (curr != NULL) {
		writeAllUser(curr->left);
		
		FILE *fptr = fopen("user.txt", "a");
		fprintf(fptr, "%s#%s#%d\n", curr->name, curr->password, curr->highscore);
		fclose(fptr);
		
		writeAllUser(curr->right);
	}
}

void inOrder(User* curr) {
	if (curr != NULL) {
		inOrder(curr->left);
		printf("Name: %s, Pass: %s, Score: %d\n", curr->name, curr->password, curr->highscore);
		inOrder(curr->right);
	}
}

void readUser() {
	FILE *fptr = fopen("user.txt", "r");
	
	while (!feof(fptr)) {
		char name[100];
		char password[100];
		int highscore;
		fscanf(fptr, "%[^#]#%[^#]#%d\n", name, password, &highscore);
		root = insertAVL(root, createNewUser(name, password, highscore));
	}
	
	fclose(fptr);
}

void registerPage() {
	
	char name[100];
	char password[100];
	int highscore = 0;
	
	bool registerValid = false;
	do {
		system("cls");
		printf("Register\n");
		printf("--------\n");
		
		do {
			printf("Input name [must be unique]: ");
			scanf("%s", &name); getchar();
			if (!validateRegisterName(name)) {
				printf("Name is not unique !\n");
			}
		} while (!validateRegisterName(name));
		
		int i = 0;
		printf("Input password [minimal 5 char]: ");
		while (i < 99) {
			char temp;
			temp = _getch();
			
			if (temp == '\r') {
				strcpy(tempRegisterPassword, password);
				tempRegisterI = i;
				strcpy(password, "");
				i = 0;
				break;
			} else {
				password[i] = temp;
				password[i + 1] = '\0';
			}
			
			i++;
		}
		
		if (strlen(tempRegisterPassword) >= 5) {
			registerValid = true;
		} else {
			printf("\nInvalid password !\n"); getchar();
		}
	} while(!registerValid); 
	
	//insert ke avl dan masukin write ke file
	root = insertAVL(root, createNewUser(name, tempRegisterPassword, 0));
	writeUser(name, tempRegisterPassword, 0);
	
	
	printf("\nRegister Success !\n");
	enter();
}

//logic password
char tempPassword[100];
int tempI = 0;


void loginPage() {
	
	char name[100];
	char password[100];
	int i = 0;
	
	do {
		system("cls");
		printf("Login\n");
		printf("--------\n");
		
		
		printf("Input name ['q' to go back]: ");
		scanf("%s", &name); getchar();
		
		if (strcmp(name, "q") == 0) {
			return;
		}
		
		//input password tpi g keliatan di console nya
		printf("Input password: "); 
		while (i < 99) {
			char temp;
			temp = _getch();
			
			if (temp == '\r') {
				strcpy(tempPassword, password);
				tempI = i;
				strcpy(password, "");
				i = 0;
				break;
			} else {
				password[i] = temp;
				password[i + 1] = '\0';
			}
			
			i++;
		}
		if (!validateLogin(name, tempPassword)) {
			printf("\nInvalid Credential!\n");
			enter();
		}
	} while (!validateLogin(name, tempPassword));
	
	//lanjut ke choose map
	currUser = searchAVL(root, name);
	
	mazeMenu();
	
}



void homeScreen() {
	
	int opt;
	
	do {
		system("cls");
		title();
		printf("Welcome to GrocktunneL,\n");
		printf("----------------------\n");
		printf("1. Login\n");
		printf("2. Register\n");
		printf("3. Highscore\n");
		printf("4. Quit\n");
		printf(">> ");
		
		scanf("%d", &opt); getchar();
		
		switch(opt) {
			case 1:
				loginPage();
				break;
			case 2:
				registerPage();
				break;
			case 3:
				getHighscore();
				break;
			case 4:
				endScreen(100);
				break;
				
		}
	} while (opt != 4);
}

int main() {
	
	//read data dlu
	readUser();
	
	homeScreen();

	return 0;
}

void title() {

puts(" .--.            .    .                     .    ");       
puts(":                |   _|_                    |   ");        
puts("| --..--..-.  .-.|.-. |  .  . .--. .--. .-. | ");        
puts(":   ||  (   )(   |-.' |  |  | |  | |  |(.-' |    ");
puts(" `--''   `-'  `-''  `-`-'`--`-'  `-'  `-`--''---'");
puts("");
                                                                                                                                     
}

void enter() {
	printf("Enter to continue...");
	getchar();
}

void endScreen(int ms) {
	system("cls");
	puts("\t\t	                      .                             ");
	Sleep(ms);
	puts("\t\t                             ...                            ");
	Sleep(ms);
	puts("\t\t                            ..,..                           ");
	Sleep(ms);
	puts("\t\t                          ....*...                          ");
	Sleep(ms);
	puts("\t\t                         ..,..*.....                        ");
	Sleep(ms);
	puts("\t\t                       ....*****,,....                      ");
	Sleep(ms);
	puts("\t\t                     ..*,**.****,,****..                    ");
	Sleep(ms);
	puts("\t\t .....         ..... ...***...*..,***..  ......         ....");
	Sleep(ms);
	puts("\t\t   ..*,....*,.,*,*,..    ...,*,*,..      .,*,*,.,*,.,..*... ");
	Sleep(ms);
	puts("\t\t    ...,,***...*,*,.  ...,******,.%.. .  ..***...****,...   ");
	Sleep(ms);
	puts("\t\t      ..,**.**,......,**,***,,&&(*******......,*,****...    ");
	Sleep(ms);
	puts("\t\t       ...*.**..,*******,, &&&*,,**,,,*******..*,,,...  ");
	Sleep(ms);
	puts("\t\t        .***,*..,***,,*,&&&&**,*,*,,,**& ***...*,***.   ");
	Sleep(ms);
	puts("\t\t        ...... ..,***,&&&&/,,***,&&&&&& ****.  ......       ");
	Sleep(ms);
	puts("\t\t          .     ..***&&&&(*,****,*/&&&//,**..               ");
	Sleep(ms);
	puts("\t\t        ..**,....,**&&&&&%/,,***,,&&&&/*,**,....,**.  ");
	Sleep(ms);
	puts("\t\t        .,,,,,,.,,,&#**,,,,,,,,*&&&&/*,,,,,,,.,,,,,,.     ");
	Sleep(ms);
	puts("\t\t      ..,...**.*,*******,,***,&& /*,,********.,*...*..     ");
	Sleep(ms);
	puts("\t\t     ....*******......**,**%&(*,,******......***,***...     ");
	Sleep(ms);
	puts("\t\t   ....**.***..*,**.   .. ******,**..   ..****..,*..**...   ");
	Sleep(ms);
	puts("\t\t .............**,*,.   .....,***.....   ..**,*............. ");
	Sleep(ms);
	puts("\t\t                 .   ..******.,.,****,..  .              ");
	Sleep(ms);
	puts("\t\t                     ..*,*,..**,..****.      ");
	Sleep(ms);
	puts("\t\t                        ..******,*..      ");
	Sleep(ms);
	puts("\t\t                         ...,*,*...        ");
	Sleep(ms);
	puts("\t\t                           ...*..       ");
	Sleep(ms);
	puts("\t\t                            ....        ");
	Sleep(ms);
	puts("\t\t                             ..           ");
	Sleep(ms);
	puts("");
	Sleep(ms);
	puts("");
	Sleep(ms);
	printf("Wonderful ");
	Sleep(ms);
	printf("things ");
	Sleep(ms);
	printf("can ");
	Sleep(ms);
	printf("be ");
	Sleep(ms);
	printf("achieved ");
	Sleep(ms);
	printf("when ");
	Sleep(ms);
	printf("there ");
	Sleep(ms);
	printf("is ");
	Sleep(ms);
	printf("a ");
	Sleep(ms);
	printf("teamwork ");
	Sleep(ms);
	printf(", hardwork ");
	Sleep(ms);
	printf(", and ");
	Sleep(ms);
	printf("perseverance. \n");
	Sleep(ms);
	puts("22-2                                            ");
	
	_getch();
}
