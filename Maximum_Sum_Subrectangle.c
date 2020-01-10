#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct point{
    int row;
    int col;
}point;

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("usage: %s InputFileName AlgorithmIndex\n", argv[0]);
        return 0;
    }
    
    //record program start time
    clock_t start_time = clock();

    int row, col;
    int ThisSum, MaxSum;
    int** arr;
    int index;
    index = atoi(argv[2]);

    //read input file
    FILE* fp;
    char filename[200];
    strcpy(filename, argv[1]);
    fp = fopen(filename,"r");
    fscanf(fp, "%d %d", &row, &col);
    arr = (int**)malloc(sizeof(int*)*row);
    for(int i=0; i<row; i++)
        arr[i] = (int*)malloc(sizeof(int)*col);
    for(int i=0; i<row; i++)
        for(int j=0; j<col; j++)
            fscanf(fp, "%d", &arr[i][j]);
    fclose(fp);
    

    MaxSum=arr[0][0];

    if(index==1){
        for(int si=0; si<row; si++){ //set start point
            for(int sj=0; sj<col; sj++){
                for(int ei=si; ei<row; ei++){ //set end point
                    for(int ej=sj; ej<col; ej++){
                        ThisSum=0;
                        for(int i=si; i<=ei; i++){
                            for(int j=sj; j<=ej; j++){
                                ThisSum += arr[i][j];
                            }
                        }
                        if(ThisSum > MaxSum)
                            MaxSum = ThisSum;
                    }
                }
            }
        }
    }

    else if(index==2){
        int RowSum[row]; //Sum for each row
        int TempSum; //Sum of rows
        for(int si=0; si<row; si++){
            for(int sj=0; sj<col; sj++){ //set start point
                for(int i=0; i<row; i++)
                    RowSum[i]=0;
                for(int j=sj; j<col; j++){
                    TempSum=0;
                    for(int i=si; i<row; i++){
                        RowSum[i] += arr[i][j];
                        TempSum += RowSum[i];
                        if(TempSum > MaxSum)
                            MaxSum = TempSum;
                    }

                }
            }
        }
    }

    else if(index==3){
        int RowSum[row]; //Sum for each row
        int TempSum; //Sum of rows
        for(int sj=0; sj<col; sj++){ //set start point
            for(int i=0; i<row; i++)
                RowSum[i]=0;
            for(int j=sj; j<col; j++){
                TempSum=0;
                for(int i=0; i<row; i++){
                    RowSum[i] += arr[i][j];
                }
                for(int i=0; i<row; i++){
                    TempSum += RowSum[i];
                    if(TempSum < RowSum[i])
                        TempSum = RowSum[i];
                    if(TempSum > MaxSum)
                        MaxSum = TempSum;
                }
            }
        }
    }

    //record program end time
    clock_t end_time = clock();

    //make output file
    char output[210] = "result_";
    fp = fopen(strcat(output,filename),"w");
    fprintf(fp, "%s\n%d\n%d\n%d\n%d\n%.3f\n",filename,index,row,col,MaxSum,(double)(end_time-start_time)/CLOCKS_PER_SEC*1000);
    fclose(fp);

}

