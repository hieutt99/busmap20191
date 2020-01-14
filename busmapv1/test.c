#include <stdio.h>
#include <stdlib.h>
#include "jrb.h"
#include "jval.h"
#include "dllist.h"

typedef struct{
    JRB vertices;
    JRB edges;    
    

}Graph;

Graph createGraph(){
    Graph g;
    g.vertices = make_jrb();
    g.edges = make_jrb();

    return g;
}
void dropGraph(Graph g){
    free(g.vertices);
    free(g.edges);
    
    printf("Graph deleted!\n");
}
void addVertex(Graph g, int v1, int value){
    JRB node = jrb_find_int(g.vertices, v1);
    
    if(node != NULL){
        printf("Vertex already exists.\n");
    }
    else{
        char temp[20] = "Vertex ";
        char temptemp[20];
        snprintf(temptemp,sizeof(20),"%d",value);
        strcat(temp,temptemp);
        jrb_insert_int(g.vertices,v1,new_jval_s(temp));
        printf("Input successfully : %d - %s.\n",v1,temp);
    }
}
void addEdge(Graph g, int v1, int v2, double weight){
    JRB node;
    JRB tree = make_jrb;
    node = jrb_find_int(g.edges,v1);
    
    if(node == NULL){
        //not exist yet
        jrb_insert_int(g.edges,v1,new_jval_v(tree));
        jrb_insert_int(tree,v2,new_jval_d(weight));
    }
    else{
        //exist already
        tree = (JRB) jval_v(node->val);
        JRB temp = jrb_find_int(tree,v2);
        if(temp == NULL){
            jrb_insert_int(tree,v2,new_jval_d(weight));
        }
    }
}
int main(){

    // JRB test = make_jrb();

    // char name[] = "hello world";

    // int *array = (int*)malloc(sizeof(int)*256);
    // for(int i =0;i<256;i++){
    //     array[i] = i;
    // }
    // for(int i = 0 ;i<256;i++){
    //     printf("%d \t",array[i]);
    // }
    // printf("\n=====================================\n");
    // jrb_insert_str(test,name,new_jval_v(array));
    // JRB node = jrb_find_str(test,name);
    // printf("ket qua : \n");

    // int *temp = jval_v(node->val);
    // for(int i = 0;i<256;i++){
    //     printf("%d\t",temp[i]);
    // }

    int *array = (int *)malloc(sizeof(int)*256);
    int arraytemp[100] = { 111, 112, 113, 114, 115, 116, 
    117, 51, 118, 119, 120, 121, 68, 153, 154, 31, 99, 98, 103, 102};
    for(int i = 0;i<20;i++){
        array[i] = arraytemp[i];
    }
    printf("test array : \n");
    for(int i = 0;i<20;i++){
        printf("%d\n",array[i]);
    }
    //================================================
    //Trying to track the route 
    Graph g = createGraph();
    int routes[50][256];
    int size[50];
    for(int i = 0;i<50;i++){
        size[i] = -1;
    }
    char *routeName[50];
    int xp = 0;

    FILE *p;
    if((p = fopen("route.txt","r"))==NULL){
        printf("Error reading file route.");
        return -1;
    }
    char *str = (char*)malloc(256);
    int count = 0;

    while(fgets(str,256,p)!=NULL){
        int a[100];
        int index = 0;
        if(count%2==0){
            str[strlen(str)-1] = '\0';
            printf("%s:\n",str);
            // routeName[xp] = strdup(str);
            memset(str,0,256);
        }
        else if(count%2==1){
            char tocop[5];
            char c;
            int x = 0,ind = 0;
            while((c = str[x])!='\n'){
                if(c == ' '){
                    int temp = atoi(tocop);
                    // printf("%d\n",temp);
                    a[index++] = temp;
                    memset(tocop,0,5);
                    ind = 0; 
                }
                else{
                    tocop[ind] = c;
                    ind++;
                }
                x++;
            }
            for(int i = 0; i<index;i++){
                printf("%d--",a[i]);
                routes[xp][i] = a[i];
            }
            size[xp] = index;
            xp++;
            // printf("%d.",a[index-1]);
            printf("\n");
        }
        count++;

    }

    //======================================
    //Greedy Alg
    int top = 0;
    int  maxp = 0;
    char *plan[50];
    int countp = 0;
    while(top!=19){
        for(int i = 0;i < xp;i++){
            for(int j = 0;j<size[i];j++){
                int temp = top;
                int count = 0;
                if(routes[i][j] == arraytemp[temp]){
                    while(routes[i][++j] == arraytemp[++temp]){
                        count++;
                    }
                    if(count>maxp){
                        plan[countp] = strdup(routeName[i]);
                        maxp = count;
                    }
                }
            }
 
        }
        countp++;
        top +=maxp;
        maxp = 0;
    }
    dropGraph(g);

    return 0;
}