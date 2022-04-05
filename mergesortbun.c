// CPP Program to implement merge sort using 
// multi-threading 
#include <pthread.h> 
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>
  
// number of elements in array 
#define MAX 100 
  
// number of threads 
#define THREAD_MAX 4 

#define MAX_THREAD_COUNT 4
  
// array of size MAX 
int highs[THREAD_MAX];
int lows[THREAD_MAX];
int a[MAX], result[MAX];


void displayResult(){
    printf("Sorted array:\n");
    for (int i = 0; i < MAX; i++) 
        printf("%d ", result[i]); 
    printf("\n");
}

// merge function for merging two parts 
void merge(int low, int mid, int high) { 
  
    int leftSize = mid - low + 1;
    int rightSize = high - mid, i, j; 
  
    int* left = malloc((mid-low+1)*sizeof(int));
    int* right = malloc((high-mid)*sizeof(int)); 
    // storing values in right part 
    for (i = 0; i < rightSize; i++) {
        right[i] = a[i + mid + 1]; 
    }
    // storing values in left part 
    for (i = 0; i <leftSize; i++) {

        left[i] = a[i + low]; 
    }
  
  
    i = 0;
    j = 0; 
    int k = low; 
  
    // merge left and right in ascending order 
    while (i <leftSize && j < rightSize) { 
        if (left[i] <= right[j]){ 
            a[k] = left[i];
            k++;
            i++; 
        }
        else{
            a[k] = right[j]; 
            k++;
            j++;
        }
    } 
  
    // insert remaining values from left 
    while (i <leftSize) { 
        a[k] = left[i];
        k++;
        i++; 
    } 
  
    // insert remaining values from right 
    while (j < rightSize) { 
        a[k] = right[j]; 
        k++;
        j++;
    } 
}


  


void initArray(){
    srand(time(0));
    for (int i = 0; i < MAX; i++) 
        a[i] = rand() % 100; 
}






pthread_t threads[MAX_THREAD_COUNT];
typedef struct{
    int left;
    int right;
    int depth;
}interval;

typedef struct intervalStackNode intervalStackNode;

struct intervalStackNode{
    intervalStackNode* next;
    intervalStackNode* prev;
    interval value;
};

typedef struct{
    intervalStackNode* first;
    intervalStackNode* last;
    int size;
}intervalStackHead;

void pushIntervalInStack(intervalStackHead* head, interval* input){
    intervalStackNode* inter = malloc(sizeof(intervalStackNode));
    inter->value = *input;

    if(head->size==0){
        head->first = inter;
        head->last = inter;
        head->size = 1;
        inter->next = NULL;
        inter->prev = NULL;
    }
    else{
        head->size++;
        head->last->next = inter;
        inter->prev = head->last;
        head->last = inter;
        inter->next = NULL;
    }
}

void printStack(intervalStackHead* head){
    intervalStackNode* iter = head->first;
    while(iter!=NULL){
        printf("left:%d right:%d\n", iter->value.left, iter->value.right);
        iter = iter->next;
    }

}

intervalStackNode* popIntervalNodeFromStack(intervalStackHead* head){
    if(head->size == 0 || head->last ==NULL){
        return NULL;
    }
    intervalStackNode* result = head->last;

    if(head->size>1){
        result->prev->next = NULL;
        head->last = result->prev;
    }else{
        head->last = NULL;  
    }
    head->size--;  
    return result;
}

intervalStackHead layers[100];
intervalStackHead work[MAX_THREAD_COUNT];

void merge_sort(int low, int high, int depth)//sparge array-ul in intervale
{ 
    // calculating mid point of array 
    int mid = low + (high - low) / 2; 
    if (low < high) { 
        // calling first half 
        merge_sort(low, mid, depth+1); 
        // calling second half 
        merge_sort(mid + 1, high, depth+1);

        interval* inter = malloc(sizeof(interval));
        inter->left = low;
        inter->right = high;
        inter->depth = depth;
        pushIntervalInStack(&layers[depth], inter);
    } 
}

void* do_tasks(void* args){//this should recieve &work[thread_no];
    intervalStackHead* head = (intervalStackHead*)args;
    while(head->size!=0){
        intervalStackNode* interStackNode = popIntervalNodeFromStack(head);
        interval inter = interStackNode->value;
        merge(inter.left, inter.left+(inter.right-inter.left)/2, inter.right);
    }

}

void merge_layer(int layer){
    int index = 0;
    while(1){
        intervalStackNode* interStNode = popIntervalNodeFromStack(&layers[layer]);
        if(interStNode == NULL)
            break;
        pushIntervalInStack(&work[index%MAX_THREAD_COUNT], &interStNode->value);//give work to thread
        index++;

    }
    pthread_t threads[MAX_THREAD_COUNT];
    for(int i=0; i<MAX_THREAD_COUNT; i++){
        pthread_create(threads+i, NULL, do_tasks, &work[i]);
    }

    for(int i=0; i<MAX_THREAD_COUNT; i++){
        pthread_join(threads[i], NULL);
    }

}


intervalStackHead testStack;


int main() 
{
    //test insertion
    // for(int i=0; i<10; i++){
    //     interval* inter = malloc(sizeof(inter));
    //     inter->left = i;
    //     inter->right = i+1;
    //     inter->depth = 0;
    //     pushIntervalInStack(&testStack, inter);
    //     printStack(&testStack);
    //     printf("\n\n");
    // }

    // for(int i=0; i<10; i++){
    //     intervalStackNode* node = popIntervalNodeFromStack(&testStack);
    //     interval inter = node->value;
    //     printf("interval: %d %d\n", inter.left, inter.right);
    // }

    // printf("testStackSize:  %d\n", testStack.size);

    initArray();
    merge_sort(0,MAX-1, 0);

    int size = MAX/2, layer = 0;
    while(size!=0){
        size = size/2;
        layer++;
    }
    while(layer>=0){
        merge_layer(layer);
        layer--;
    }

    for(int i=0; i<MAX; i++)
        printf("%d\n", a[i]);

    // initArray();
    // run();
    // displayResult();
    return 0; 
}