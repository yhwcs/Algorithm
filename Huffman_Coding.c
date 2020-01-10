#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node{
    char symbol;            // Character received.
    int freq;               // Frequency of the character.
    struct _node *left;     // The tree's left child.
    struct _node *right;    // The tree's right child.
}NODE;

NODE *u, *v, *w;            // Used to merge nodes when making a tree.

NODE* newnode(char symbol, int freq);
void adjust(NODE** tree, int root, int size);
NODE* pop(NODE** tree, int size);
NODE* push(NODE** tree, NODE* w, int size);
void make_code(NODE* w, char str[], int len, char* code[], int code_len[]);

    int main(int argc, char *argv[]){
        if(argc != 3){                  // Not a valid input.
            printf("Usage: %s Option(-c|-d) InputFileName\n", argv[0]);
            return 0;
        }

        FILE* fp;
        char filename[200];             // Name of the input file to read.

        strcpy(filename, argv[2]);
        fp = fopen(filename, "r");
        if(!fp){                        // File does not exist
            printf("ERROR : input file does not exist\n");
            return 0;
        }

        //**********************************************************************************************//

        if(strcmp(argv[1], "-c")==0){   // Compression option selected.

            int size=0;                 // Total number of characters in the input file to read. (=input file size)
            int nsize=1;                // Number of ASCII Code types of read characters.
            char temp;                  // Used to read characters from input file.
            char* code[130];            // A two-demensional array for assigning the Huffman code to each ASCII code.
            char* arr;                  // Array for storing characters read from input file.
            int frequency[130] = {0};   // Frequency of appearance of each character.
            arr=(char*)malloc(sizeof(char));
            while(1){
                fscanf(fp, "%c", &temp);
                if(feof(fp)) break;     // Stop when read the file to the end.
                arr[size++] = temp;     // Store the characters read in the array.
                arr = (char*)realloc(arr, sizeof(char)*(size+1));
            }
            if(size<0){                 // File exists but empty.
                printf("ERROR : input file is empty\n");
                return 0;
            }
            for(int i=0; i<128; i++)
                code[i]=(char*)malloc(sizeof(char)*150);

            for(int i=0; i<size; i++)   // Count the frequency of the appearance of characters read.
                frequency[arr[i]]++;

            //*********************************** Make Huffman code **************************************//

            NODE** tree = (NODE**)malloc(129*sizeof(NODE*));    // Array for storing nodes that make up tree.
            NODE** buf = (NODE**)malloc(129*sizeof(NODE*));     // Array for storing information for use with decompress.
            for(int i=0; i<129; i++){                           // Initialization.
                tree[i]=NULL;
                buf[i]=NULL;
            }

            for(int i=0; i<128; i++)
                if(frequency[i] != 0){                          // Create nodes with the frequency and characters in file.
                    tree[nsize] = newnode(i, frequency[i]);
                    buf[nsize++] = newnode(i, frequency[i]);
                }
            nsize--;

            for(int i=nsize/2; i>0; i--)
                adjust(tree, i, nsize);            // Make a min heap based on frequency.
            for(int i=nsize-1; i>0; i--){
                u=pop(tree, i);                    // Extract the node that has the smallest frequency from the tree. 
                v=pop(tree, i-1);                  // Extract the node that has second smallest frequency from the tree.
                w=newnode(1,u->freq+v->freq);      // Create a new node that combines the frequency of the two nodes. 
                w->left=u;                         // Connect the node u to the left child of the new node.
                w->right=v;                        // Connect the node v to the right child of the new node.
                push(tree, w, i);                  // Add a new node to the original tree.
            }

            int code_len[129]={0};                 // Array for storing the length of the Huffman codes.
            char str[150];                         // Array that are temporarily saved when Huffman code is created.
            make_code(w,str,0,code, code_len);     // Generates Huffman code.

            //**********************************************************************************************//

            char output[210]=".zz";
            fp=fopen(strcat(filename,output), "w");         //Open the file to print.

            unsigned char result=0;                         // Character for bit unit operation.
            int now_len=0;                                  // The order of the bits currently in operation.
            int padding_len;                                // Number of padded bits.
            int fsize=0;                                    // Total number of characters printed to the outfut file.

            for(int i=0; i<size; i++){                       
                for(int j=0; j<code_len[arr[i]]; j++){
                    now_len++;                              // Move the position of the bit operating.
                    if(code[arr[i]][j] == '1'){             // A bitwise operation, in accordance with the value
                        result=result|(1<<(8-now_len));     // of the current bit position of Huffman code.
                    }
                    if(now_len==8){                         // If the operation was finished by the last bit, 
                        fprintf(fp,"%c",result);            // Write a character in the output file.
                        fsize++;
                        result=0;                           // Initialize the character and position of bits.
                        now_len=0;                          
                    }
                }
            }
            if(now_len!=0){                                 // Padding required.
                fprintf(fp,"%c",result);
                fsize++;
            }

            fclose(fp);
            if(now_len!=0)                                  // a number of paddings
                padding_len=8-now_len;                      // = 8 - the number of bits that have finished operation.
            else if(now_len==0)
                padding_len=0;

            //*********************************************************************************************//

            fp=fopen(filename, "w");                                    // Open the file to print.
            fprintf(fp, "%d\n", nsize);                     
            for(int i=1; i<=nsize; i++){    
                fprintf(fp, "%c %d ", buf[i]->symbol, buf[i]->freq);    // Print the characters read and their frequency.
            }
            fprintf(fp, "\n");                                          // Print number of character wrriten to the file
            fprintf(fp, "%d %d\n", fsize, padding_len);                 // and number of padded bits.

            result=0;                                                   // Initialize the character and position of bits.
            now_len=0;
            for(int i=0; i<size; i++){                                  // Print characters in the file
                for(int j=0; j<code_len[arr[i]]; j++){                  // in the same way as above.
                    now_len++;
                    if(code[arr[i]][j] == '1'){
                        result=result|(1<<(8-now_len));
                    }
                    if(now_len==8){
                        fprintf(fp,"%c",result);
                        result=0;
                        now_len=0;
                    }
                }
            }
            if(now_len!=0){
                fprintf(fp,"%c",result);
            }
            fclose(fp);
        }

        //***********************************************************************************************//

        else if(strcmp(argv[1], "-d")==0){              // Decompression option selected.
            int nsize;                                  // Number of ASCII Code types in the original input file.
            char sym;                                   // Used to temporarily store written characters and frequency.
            int frequency;
            int fsize;
            int padding_len;                            // Stored number of padded bits.
            NODE** tree = (NODE**)malloc(129*sizeof(NODE*));    // Array for storing nodes that make up tree.

            fp = fopen(filename, "r");
            fscanf(fp,"%d%*c", &nsize);

            for(int i=1; i<=nsize; i++){
                fscanf(fp, "%c %d%*c", &sym, &frequency);
                tree[i] = newnode(sym, frequency);          // Create nodes with the frequency and characters in file.
            }
            fscanf(fp, "%d %d%*c", &fsize, &padding_len);

            for(int i=nsize/2; i>0; i--)
                adjust(tree, i, nsize);                     // Make a min heap based on frequency.
            for(int i=nsize-1; i>0; i--){                   // Construct a tree as shown in compression option selected.
                u=pop(tree,i);
                v=pop(tree,i-1);
                w=newnode(1,u->freq+v->freq);
                w->left=u;
                w->right=v;
                push(tree,w,i);
            }

            char* code[130];                                // Array for assigning the Huffman codes to each ASCII code.
            for(int i=0; i<128; i++)
                code[i]=(char*)malloc(sizeof(char)*150);
            int code_len[129]={0};                          // Array for storing the length of the Huffman codes.
            char str[150];                                  // Array that are temporarily saved when code is created.
            make_code(w, str, 0, code, code_len);           // Generated Huffman code.

            //*****************************************************************************************//

            unsigned char result;                           // Used to save written characters in the file.
            int result_int;                                 // Int for bit unit operation.
            int end_check=8*fsize-padding_len;              // Total number of bits taht require operation.
            int echeck=0;                                   // Flag to check operation exit conditions.
            FILE* fpout;
            char output[210]=".yy";                         
            fpout = fopen(strcat(filename,output), "w");    // Open the file to print.
            for(int i=0; i<fsize; i++){
                if(echeck==end_check) break;                // If done the operation until the last bit, exit.
                fscanf(fp,"%c", &result);
                
                result_int=(int)result;
                for(int j=1; j<=8; j++){                    
                    if(echeck==end_check) break;            // If done the operation until the last bit, exit.
                    if(result_int&(1<<(8-j))){              // Move nodes along the tree according to the bit operation.
                        w=w->right;
                    }
                    else if((result_int&(1<<(8-j)))==0){
                        w=w->left;
                    }
                    if((w->left==NULL)&&(w->right==NULL)){  // If arrive at a leaf node,
                        fprintf(fpout, "%c",w->symbol);     // print the character stored on that node.
                        w=tree[1];                          // Move the starting point back to the root of the tree.
                    }
                    echeck++;

                }
            }
            fclose(fp);
            fclose(fpout);
        }

        //***************************************************************************************//

        else{                                               // The option is selected incorrectly.
            printf("Usage: %s Option(-c|-d) InputFileName\n", argv[0]);
            return 0;
        }
    }

NODE* newnode(char symbol, int freq){                       // Create a new node by taking character and frequency.
    NODE* temp;
    temp = (NODE*)malloc(sizeof(NODE));
    temp->left = NULL;
    temp->right = NULL;
    temp->symbol = symbol;
    temp->freq = freq;

    return temp;
}

void adjust(NODE** tree, int root, int size){
    int child = root*2;
    NODE* temp;
    temp = newnode(tree[root]->symbol, tree[root]->freq);               // Save the value of the root node in advance.
    temp->left=tree[root]->left;                            
    temp->right=tree[root]->right;
    while(child<=size){
        if((child<size) && (tree[child]->freq > tree[child+1]->freq))   // Compare the frequency stored on two nodes
            child++;                                                    // and move to the smaller side.
        if(temp->freq <= tree[child]->freq) break;                      // If the frequency of temp node is smaller, stop.
        tree[child/2]->symbol = tree[child]->symbol;                    // tree[child/2] = tree[child]
        tree[child/2]->freq = tree[child]->freq;
        tree[child/2]->left = tree[child]->left;
        tree[child/2]->right = tree[child]->right;
        child*=2;                                                       // Proceeds with child nodes below.
    }
    tree[child/2]->symbol=temp->symbol;                                 // Store the value of root node
    tree[child/2]->freq=temp->freq;                                     // that was saved in temp in advance
    tree[child/2]->left=temp->left;                                     // on the node.
    tree[child/2]->right=temp->right;
}

NODE* pop(NODE** tree, int size){                                       // Take a node out of the tree.
    NODE* item;
    item = newnode(tree[1]->symbol, tree[1]->freq);                     // Save the value of the root node in advance.
    item->left=tree[1]->left;
    item->right=tree[1]->right;
    tree[1]->symbol=tree[size+1]->symbol;                               // Store the value of the last node
    tree[1]->freq=tree[size+1]->freq;                                   // in the top node of the heap.
    tree[1]->left=tree[size+1]->left;
    tree[1]->right=tree[size+1]->right;
    adjust(tree, 1, size);                                              // Align again to min heap.
    return item;
}

NODE* push(NODE** tree, NODE* w, int size){                             // Add a node to the tree.
    int i=size;
    while((i!=1) && (w->freq < tree[i/2]->freq)){                       // If it's less than the frequency
        tree[i]->symbol=tree[i/2]->symbol;                              // stored on the parent's node,
        tree[i]->freq=tree[i/2]->freq;                                  // Move node up.
        tree[i]->left=tree[i/2]->left;                                  // Find a location to save the node.
        tree[i]->right=tree[i/2]->right;
        i/=2;
    }
    tree[i]->symbol=w->symbol;                                          // Store the node in current location.
    tree[i]->freq=w->freq;
    tree[i]->left=w->left;
    tree[i]->right=w->right;
}

void make_code(NODE* w, char str[], int len, char* code[], int code_len[]){
    if((w->left==NULL)&&(w->right==NULL)){                              // If arrive at leaf node,
        str[len]='\0';                                                  // complete code generation.
        strcpy(code[w->symbol], str);                                   // Copy temporarily stored Huffman code to array.
        code_len[w->symbol]=len;                                        // Store the length of the Huffman code.
    }
    else{
        if(w->left!=NULL){                                              // If the tree's left child exists,
            str[len]='0';                                               // Add 0 to the code.
            make_code(w->left, str, len+1, code, code_len);             // Call function to make next character of code.
        }
        if(w->right!=NULL){                                             // If the tree's right child exists,
            str[len]='1';                                               // Add 1 to the code.
            make_code(w->right, str, len+1, code, code_len);            // Call function to make next character of code.
        }
    }
}
