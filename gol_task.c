#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>


int done;
pthread_mutex_t mutex;
typedef struct Coord{
    int row;
	int column;
	struct Coord * next;
} Coord;

typedef struct queue{
	Coord * head;
	Coord * tail;
	int length;
} Queue;

Queue * aliveQueue;
Queue * deadQueue;

int ** curGrid;
int ** futureGrid;

int done;
int gridSize=0;
void initArrays(int gridSize);
void setArray(int gridSize);
void freeArrays(int gridSize);
Coord * initPoint(int col, int row);
void deletePoint(Coord * toDelete);
Queue* initQueue();
void deleteQueue(Queue * toDelete);
void addBack(Coord * pt, Queue * addTo);
Coord * removeFront(Queue * queue);
int isEmpty(Queue * queue);
void * neighbourCount(void * size);
void * writeAlive(void *);
void * writeDead(void *);
void swapGrids(int gridSize);

void initArrays(int gridSize){
	int i = 0;
	curGrid = malloc(sizeof(int *)*gridSize);
    for(i=0;i<gridSize;i++){
        curGrid[i] = malloc(sizeof(int)*gridSize);
    }

    i = 0;
	futureGrid = malloc(sizeof(int *)*gridSize);
    for(i=0;i<gridSize;i++){
        futureGrid[i] = malloc(sizeof(int)*gridSize);
    }
    setArray(gridSize);
}

void makeArray(int gridSize){
	int i = 0;
	curGrid = malloc(sizeof(int *)*gridSize);
    for(i=0;i<gridSize;i++){
        curGrid[i] = malloc(sizeof(int)*gridSize);
    }

    i = 0;
	futureGrid = malloc(sizeof(int *)*gridSize);
    for(i=0;i<gridSize;i++){
        futureGrid[i] = malloc(sizeof(int)*gridSize);
    }

    int k=0, j=0;
	srand(time(NULL));
	for (k=0;k<gridSize;k++){
		for (j=0; j<gridSize;++j){
			int num = rand()%5;
			if (num < 2){
                printf("%d\n",num);
				curGrid[k][j] = 1;
			} else {
				curGrid[k][j] = 0;
			}
		}
	}
}

void setArray(int gridSize){
	int i=0, j=0;
	srand(time(NULL));
	for (i=0;i<gridSize;i++){
		for (j=0; j<gridSize;++j){
			int num = rand()%5;
			if (num == 1){
				curGrid[i][j] = 1;
			} else {
				curGrid[i][j] = 0;
			}
		}
	}
}

void freeArrays(int gridSize){
	int i = 0;
	for(i=0;i<gridSize;i++){
		free(curGrid[i]);
	}
	for(i=0;i<gridSize;i++){
		free(futureGrid[i]);
	}
	free(curGrid);
	free(futureGrid);
}

Coord * initPoint(int col, int row){
	Coord * newPoint = malloc(sizeof(Coord));
	newPoint->column = col;
	newPoint->row = row;
	newPoint->next = NULL;
	return newPoint;
}



Queue* initQueue(){
	Queue * newQueue = malloc(sizeof(Queue));
	newQueue->head = NULL;
	newQueue->tail = NULL;
	return newQueue;
}
void deleteQueue(Queue * queue){
	Coord * coordxy = queue->head;
	while (coordxy != NULL){
		Coord *temp = coordxy;
		coordxy = coordxy->next;
		free(temp);
	}
	free(queue);
}



Coord * removeFront(Queue * queue){
	if (queue->head == NULL){ /* there are no elements */
		return NULL;
	} else if (queue->head->next == NULL){ /* there is only one element*/
		Coord * hold = queue-> head;
		queue->head = NULL;
		queue->tail = NULL;
		return hold; 
	} else {
		Coord * hold = queue-> head;
		queue->head = queue->head->next;
		return hold;
	}
}


void printGrid(int gridSize){
	int i=0, j=0,h=0;
  for (h=0;h<=gridSize;h++){
    printf("-");
  }
  printf("\n");
	for (i=0;i<gridSize;i++){
    printf("|");
		for (j=0; j<gridSize;++j){
			if (curGrid[i][j] == 1) {
        		printf("X");
        	} else {
        		printf(" ");
        	}
		}
    printf("|\n");
	}
  for (h=0;h<=gridSize;h++){
    printf("-");
  }
	printf("\n");
}



void swapGrids(int gridSize){
	int i=0, j=0;
	for (i=0;i<gridSize;i++){
		for (j=0; j<gridSize;++j){
			curGrid[i][j] = futureGrid[i][j];
		}
	}
}

void *cgol() {
 
  int i,j=0;
  int alive = 0;
  int dead = 0;
  int neighbours=0;
  
  for(i = 0; i < gridSize; i++) {
    for(j = 0; j < gridSize; ++j) {
        neighbours = 0;
			if (i > 0){
				
				if (curGrid[i-1][j] == 1)
					neighbours++;
				if (j > 0){
					
					if (curGrid[i-1][j-1] == 1)
						neighbours++;
				}
				if (j < (gridSize-1)){
					
					if (curGrid[i-1][j+1] == 1)
						neighbours++;
				}
			}
			if (i < (gridSize-1)){
				
				if (curGrid[i+1][j] == 1)
					neighbours++;
				if (j > 0){
					
					if (curGrid[i+1][j-1] == 1)
						neighbours++;
				}
				if (j < (gridSize-1)){
				
					if (curGrid[i+1][j+1] == 1)
						neighbours++;
				}	
			}
			if (j > 0){
				
				if (curGrid[i][j-1] == 1)
					neighbours++;
			}
			if (j < (gridSize-1)){
				
				if (curGrid[i][j+1] == 1)
					neighbours++;
			}
	  /*i row j col*/
	  Coord * xyCoord = malloc(sizeof(Coord)*100);
	  xyCoord->column = i;
	  xyCoord->row = j;
	  xyCoord->next = NULL;
	  
	  if(curGrid[i][j] == 1){
		if(neighbours==2 || neighbours == 3){
			if (aliveQueue->tail != NULL){ /* the list already has elements*/
				aliveQueue->tail->next = xyCoord;
				aliveQueue->tail = xyCoord;
			} else { /* there are no elements */
				aliveQueue->head = xyCoord;
				aliveQueue->tail = xyCoord;
			}
			alive++;
		} else {
			if (deadQueue->tail != NULL){ /* the list already has elements*/
				deadQueue->tail->next = xyCoord;
				deadQueue->tail = xyCoord;
			} else { /* there are no elements */
				deadQueue->head = xyCoord;
				deadQueue->tail = xyCoord;
			}
			dead++;
		} 
	  } else {
		if (neighbours == 3){
			if (aliveQueue->tail != NULL){ /* the list already has elements*/
				aliveQueue->tail->next = xyCoord;
				aliveQueue->tail = xyCoord;
			} else { /* there are no elements */
				aliveQueue->head = xyCoord;
				aliveQueue->tail = xyCoord;
			}
			alive++;
		} else {
			if (deadQueue->tail != NULL){ /* the list already has elements*/
				deadQueue->tail->next = xyCoord;
				deadQueue->tail = xyCoord;
			} else { /* there are no elements */
				deadQueue->head = xyCoord;
				deadQueue->tail = xyCoord;
			}
			dead++;
		}
	  }
    }
	
  }
  done=1;
  return NULL;
}



void * checkAliveCells(void * blank){
	while (done == 0 || aliveQueue->head != NULL|| deadQueue->head != NULL){
		Coord * pt = removeFront(aliveQueue);
		if (pt != NULL){ 
			futureGrid[pt->column][pt->row] = 1;
			free(pt);
		}
	}
	return NULL;
}

void * checkDeadCells(void * blank){
	while (done == 0 || aliveQueue->head != NULL|| deadQueue->head != NULL){
		Coord * pt = removeFront(deadQueue);
		if (pt != NULL){ 
			futureGrid[pt->column][pt->row] = 0;
			free(pt);
		}
	}
	return NULL;
}



int main(int argc, char const *argv[]) {
	
	if (argc < 3){
		printf("Please enter the number of threads, the size of the\ngrid and the number of iterations as command line arguments\n");
		exit(0);
	}

	gridSize = atol(argv[1]);
	int numIter = atol(argv[2]);
	int display = 0;

	if (argc == 4){
		if (strcmp(argv[3], "-d")==0){
			display = 1;
		}
	}

	makeArray(gridSize);
	if (display == 1)
		printGrid(gridSize);

	aliveQueue = malloc(sizeof(Queue));
	aliveQueue->head = NULL;
	aliveQueue->tail = NULL;

    deadQueue = malloc(sizeof(Queue));
	deadQueue->head = NULL;
	deadQueue->tail = NULL;

	int i;
	pthread_t t0,t1,t2;
	for (i=0;i<numIter;i++){
		done = 0;
		pthread_create(&t0,NULL,cgol, (void*)&gridSize);
		pthread_create(&t1,NULL,checkAliveCells,(void*)&gridSize);
		pthread_create(&t2,NULL,checkDeadCells,(void*)&gridSize);
		
      	pthread_join(t0, NULL);
      	pthread_join(t1, NULL);
      	pthread_join(t2, NULL); 

		swapGrids(gridSize);
		if (display == 1)
			printGrid(gridSize);		
	}
  
	deleteQueue(aliveQueue);
	deleteQueue(deadQueue);
    freeArrays(gridSize);
	return 0;
}
