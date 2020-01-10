#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void swap(int* arr, int i, int j);
void selection_sort(int* arr, int size);
void quick_sort(int* arr, int left, int right);
int partition(int* arr, int left, int right);
void merge_sort(int* arr, int* buffer, int left, int right);
void merge(int* arr, int* buffer, int left, int middle, int right);
int log2(int x);
void sort(int* arr, int size, int left, int right);
void insertion_sort(int* arr, int size);
void introsort(int* arr, int maxdepth, int size, int left, int right);
void heapsort(int* arr, int size);
void adjust(int* arr, int root, int size);

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("usage: %s InputFileName AlgorithmIndex\n", argv[0]);
        return 0;
    }

    int index;
    index = atoi(argv[2]);

    int size;
    int* arr;

    //read input file
    FILE* fp;
    char filename[200];
    strcpy(filename, argv[1]);
    fp = fopen(filename, "r");
    if(fp == NULL){
        printf("ERROR : Input File is empty\n");
        return 0;
    }
    fscanf(fp, "%d", &size);
    arr = (int*)malloc(sizeof(int)*(size+1));
    
    for(int i=0; i<size; i++)
        fscanf(fp, "%d", &arr[i]);

    fclose(fp);
    
    //record program start time
    clock_t start_time = clock();

    if(index == 1)
        selection_sort(arr, size);

    else if(index == 2)
        quick_sort(arr, 0, size-1);

    else if(index == 3){
        int* buffer;    //extra space for merge sort
        buffer = (int*)malloc(sizeof(int)*size);
        merge_sort(arr, buffer, 0, size-1);
    }
    else if(index == 4)
        sort(arr, size, 0, size-1);

    clock_t end_time = clock();

    //wrtie output file
    char output[210] = "result_";
    char underscore[2] = "_";
    strcat(output,argv[2]);
    strcat(output, underscore);
    strcat(output, filename);
    fp = fopen(output, "w");
    fprintf(fp, "%s\n%d\n%d\n%.3f\n", filename, index, size,(double)(end_time-start_time)/CLOCKS_PER_SEC*1000);
    for(int i=0; i<size; i++)
        fprintf(fp, "%d ", arr[i]);
    fprintf(fp,"\n");
    fclose(fp);

}

void swap(int* arr, int i, int j){
    int temp;
    temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
} 

void selection_sort(int* arr, int size){
    int cur;
    
    for(int i=0; i<size-1; i++){
        cur=i;
        for(int j=i+1; j<size; j++){
            if(arr[j] < arr[cur])
                cur=j;      //min = arr[j]
        }
        swap(arr, i, cur);
    }
}

void quick_sort(int* arr, int left, int right){
    int pivot;
    
    if(right-left>0){
        pivot = partition(arr, left, right);
        
        quick_sort(arr, left, pivot-1);
        quick_sort(arr, pivot+1, right);
    }
}

int partition(int* arr, int left, int right){
    int pivot;  //pivot=arr[right]
    
    pivot = left;
    for(int i=left; i<right; i++){
        if(arr[i] < arr[right]){
            swap(arr, i, pivot);
            pivot++;
        }
    }
    swap(arr, right, pivot);
    return pivot;
}

void merge_sort(int* arr, int* buffer, int left, int right){
    int middle;

    if(left < right){
        middle = (left+right)/2;

        merge_sort(arr, buffer, left, middle);
        merge_sort(arr, buffer, middle+1, right);

        merge(arr, buffer, left, middle, right);
    }
}

void merge(int* arr, int* buffer, int left, int middle, int right){
    int i, i_left, i_right;

    i_left = left;
    i_right = middle+1;
    i=left;

    while((i_left<=middle) && (i_right<=right)){
        if(arr[i_left]<=arr[i_right])   //compare and add to
            buffer[i++]=arr[i_left++];
        else
            buffer[i++]=arr[i_right++];
    }
    //move remainder
    while(i_left<=middle)
        buffer[i++]=arr[i_left++];
    while(i_right<=right)
        buffer[i++]=arr[i_right++];

    for(i=left; i<=right; i++)  //copy buffer to arr
        arr[i]=buffer[i];
}

int log2(int x){
    return (int)(log(x)/log(2));
}

void sort(int* arr, int size, int left, int right){
    int length = log2(size)-1;    //length of tree(arr)
    int maxdepth = (log2(length)-1)*2;
    
    introsort(arr, maxdepth, size, left, right);
}

void introsort(int* arr, int maxdepth, int size, int left, int right){
    int pivot;
    
    if((left-right)<16){
        insertion_sort(arr, size);
        return;
    }
    else if(maxdepth == 0)
        heapsort(arr, size);
    else{
        pivot = partition(arr, left, right);
        introsort(arr, maxdepth-1, size, left, pivot);
        introsort(arr, maxdepth-1, size, pivot+1, right);
    }
}

void insertion_sort(int* arr, int size){
    int i, j, temp;
    for(i=1; i<size; i++){
        temp = arr[i];
        j=i;
        while((j>0) && (temp<arr[j-1])){
            arr[j]=arr[j-1];
            j--;
        }
        arr[j]=temp;
    }
}

void heapsort(int* arr, int size){
    for(int i=size; i>0; i--)   //index starting from 1
        arr[i]=arr[i-1];
    for(int i=size/2; i>0; i--)     //make a max heap
        adjust(arr, i, size);
    for(int i=size-1; i>0; i--){    //extract max element sequentially
        swap(arr, 1, i+1);
        adjust(arr, 1, i);
    }
    for(int i=0; i<size; i++)   //index starting from 0
        arr[i]=arr[i+1];
}

void adjust(int* arr, int root, int size){
    int child = root*2;
    int temp = arr[root];
    while(child<=size){
        if((child<size) && (arr[child]<arr[child+1]))
            child++;
        if(temp>arr[child]) break;
        else{
            arr[child/2]=arr[child];    //move to parent
            child*=2;
        }
    }
    arr[child/2]=temp;
}
