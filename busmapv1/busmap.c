#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jrb.h"
#include "dllist.h"
#include "jval.h"
#define INF 999999
#include <dirent.h>

typedef struct{
    JRB edges;
    JRB vertices;

    JRB route;
    JRB plan;
}Graph;
//==============================================================
Graph createGraph();
void dropGraph(Graph g);
void addVertex(Graph g, int id, char *name,int *res);
char *getVertex(Graph g, int id);

void addEdge(Graph g, int v1, int v2, double weight);
double getEdge(Graph g, int v1, int v2);

int indegree(Graph g, int v, int *output);
int outdegree(Graph g, int v, int *output);

double shortestPath(Graph g, int s, int t, int *path, int *length);
//===============================================================
Graph createGraph(){
    Graph g;
    g.edges = make_jrb();
    g.vertices = make_jrb();
    g.route = make_jrb();
    g.plan = make_jrb();
    return g;
}
void dropGraph(Graph g){
    jrb_free_tree(g.edges);
    jrb_free_tree(g.vertices);
    jrb_free_tree(g.route);
    jrb_free_tree(g.plan);
    printf("\nGraph deleted\n");
}
//================================================================
void addVertex(Graph g, int id, char *name,int *res){
    JRB node;
    JRB temp;
    int check = 0;
    jrb_traverse(temp,g.vertices){
        if (strcmp(name,jval_s(temp->val)) == 0 ){
            *res = jval_i(temp->key);
            check = 1;
            break;  
        }
    }
    if (check == 1){
        return NULL;
    }
    if((node = jrb_find_int(g.vertices,id)) == NULL){
        jrb_insert_int(g.vertices, id, new_jval_s(name));
        *res = id;
    }
    else{
        printf("--------------- vao day  : %s\n",jval_s(node->val));
        *res = jval_i(node->key);
    }
}
void addEdge(Graph g, int v1, int v2, double weight){
    JRB node = jrb_find_int(g.edges,v1);
    JRB tree = make_jrb();
    
    if(node != NULL){
        tree = (JRB) jval_v(node->val);
        JRB temp = jrb_find_int(tree,v2);
        if (temp == NULL){
            jrb_insert_int(tree,v2,new_jval_d(weight));
        }
    }
    else{
        jrb_insert_int(g.edges,v1,new_jval_v(tree));
        jrb_insert_int(tree,v2,new_jval_d(weight));
    }
}
char *getVertex(Graph g, int id){
    JRB node =jrb_find_int(g.vertices, id);
    if(node != NULL){
        return jval_s(node->val);
    }
    else return NULL;
}
void addRoute(Graph g, char *name, int *array,int size){
    // JRB node = jrb_find_str(g.route, name);

    // if(node == NULL){
    //     jrb_insert_str(g.route,name, new_jval_v(array));
    // }
    JRB tree = make_jrb();
    for(int i = 0;i<size;i++){
        jrb_insert_int(tree,array[i],new_jval_i(i));
    }
    jrb_insert_str(g.route,name,new_jval_v(tree));
}
double getEdge(Graph g, int v1, int v2) {
	JRB node1 = jrb_find_int(g.edges, v1);
	if(node1 != NULL) {
		JRB tree = jval_v(node1->val);
		JRB node2 = jrb_find_int(tree, v2);
		if(node2 != NULL) return jval_d(node2->val);
		else return INF;
	}
	else return INF;
}
//================================================================
int indegree(Graph g, int v, int *output){
    JRB tree,node;
    int total = 0;
    jrb_traverse(node,g.edges){
        tree = (JRB) jval_v(node->val);
        if(jrb_find_int(tree,v)){
            output[total] = jval_i(node->key);
            total++;
        }
    }
    return total;
}
int outdegree(Graph g, int v, int *output){
    JRB tree;
    JRB node = jrb_find_int(g.edges,v);
    int total = 0;
    if(node != NULL){
        tree = (JRB) jval_v(node->val);
    }
    else{
        return total;
    }
    jrb_traverse(node,tree){
        output[total] = jval_i(node->key);
        total++;
    }
    return total;
}
//================================================================
//print edges
void printGraph(Graph g){
	JRB node,node_2;
	jrb_traverse(node,g.edges){
		printf("%d -> ",jval_i(node->key));
        JRB tree = (JRB) jval_v(node->val);
        jrb_traverse(node_2,tree){
            printf("%d  ",jval_i(node_2->key));
        }
        printf("\n");
	}
}
//print vertices
void printGraph2(Graph g){
    FILE *p;
    p = fopen("test.txt","w");
    JRB node; 
    jrb_traverse(node, g.vertices){
        printf("%s : %d\n",jval_s(node->val),jval_i(node->key));
        fprintf(p,"%s : %d\n",jval_s(node->val),jval_i(node->key));
    }
    fclose(p);
}
void printVertex(int v){
	printf("%4d ",v );
}
//=================================================================
double shortestPath(Graph g, int s, int t, int *path, int *length) {
	double distance[1000];
	int previous[1000], u, visit[1000];

	for (int i = 0; i < 1000; ++i){
		distance[i] = INF;
		visit[i] = 0;
		previous[i] = 0;
	}

	distance[s] = 0;
	previous[s] = s;
	visit[s] = 1;

	Dllist ptr, queue, node;
	queue = new_dllist();
	dll_append(queue, new_jval_i(s));

	while(!dll_empty(queue)) {
		node = dll_first(queue);
		int u = jval_i(node->val);
		dll_delete_node(node);	

		int output[100];
		int number = outdegree(g, u, output);


		for (int i = 0; i < number; i++){
			if (visit[output[i]] == 0){
				visit[output[i]] = 1;
				dll_append(queue, new_jval_i(output[i]));
			}
			if((getEdge(g, u, output[i]) + distance[u]) < distance[output[i]]) {
				distance[output[i]] = getEdge(g, u, output[i]) + distance[u];
				previous[output[i]] = u;
			}
		}
	}	
	path[0] = t;
	*length = 1;
	int cur = t;
	while(previous[cur] != s) {
		path[*length] = previous[cur];
		*length = *length + 1;
		cur = previous[cur];
	}
	path[*length] = s;
	return distance[t];

}
int findPoint(Graph g, char name[]){
    JRB node = make_jrb();
    jrb_traverse(node,g.vertices){
        if(strcmp(name,jval_s(node->val))== 0){
            return jval_i(node->key);
        }
    }
    return -1;
}
void findPointName(Graph g, int v, char *res){
    JRB node = make_jrb();
    node = jrb_find_int(g.vertices,v);
    strcpy(res,jval_s(node->val));
}
//=================================================================
int main(){
    //Graph g
    Graph g = createGraph();
    int count = 0;

    printf("Test load graph : \n");
    struct dirent *de;
    DIR *dr = opendir("data");

    if(dr == NULL){
        printf("Could not open current data directory .\n");
        return 0;
    }
    char checkpoint;

    char *routeName[50];
    int arr[50][256];
    int size[100];
    int number = 0;

    while ((de = readdir(dr)) != NULL){
        printf("===================\n");
        int weight;
        char c;
        char str[256];
        char point[256];
        
        int *pointArray = (int*)malloc(sizeof(int)*256);
        
        int index = -1;
        int res;

        char add[256] = "data/";

        
        if (strcmp(".",de->d_name) == 0 || strcmp("..",de->d_name) == 0){
            continue;
        }
        printf("%s\n",de->d_name);
        strcat(add,de->d_name);
        printf("add : %s\n",add);
        FILE *p = fopen(add,"r");
        if (p == NULL){
            printf("fail to open\n");
            break;
        }
        while((fgets(str,256,p)) != NULL){
            //===============================
            //weight right 
            if(strcmp(str,"\n")==0) continue;
            char w[10];
            int m = 0;
            while(str[m]!=':'){
                w[m] = str[m];
                m++;
            }
            weight = atoi(w);
            //===============================
            //point
            char *p = strstr(str," : ");
            strcpy(point,p+3);
            point[strlen(point)-1]='\0';
            printf("%d : %s.\n",weight,point);
            //===============================
            //add to graph vertices
            char *ptr = strdup(point);
            addVertex(g,count,ptr,&res);
            printf("========%d\n",res);
            pointArray[++index] = res;
            //Double check
            JRB temp = jrb_find_int(g.vertices,count);
            if(res < count){
                // printf("Input fail ... existing ... \n");
                continue;
            }
            else {
                printf("Input sucessful : %s - %d\n",temp->val,jval_i(temp->key));
                count++;
            }
        }
        printf("======================================\n");
        printf("test trinh tu : ");
        for(int i = 0;i<index+1;i++){
            printf("%d\t",pointArray[i]);
        }
        printf("\n");
        
        
        //================================================
        //Add to the graph edges
        for(int i = 0; i < index;i++){
            printf("==================================================%d - %d\n",pointArray[i],pointArray[i+1]);
            addEdge(g,pointArray[i],pointArray[i+1],1);
        }
        // scanf("%c",&checkpoint);
        //================================================
        //Add to the graph routes 
        char *name = strdup(de->d_name);
        name[strlen(name)-4] = '\0';
        FILE *r;
        if((r = fopen("route.txt","a")) == NULL){
            r = fopen("route.txt","w");
        }
        addRoute(g, name, pointArray,index+1);
        fprintf(r,"%s\n",name);
        for(int i = 0;i<index+1;i++){
            fprintf(r,"%d ",pointArray[i]);
            arr[number][i]=pointArray[i];
        }
        routeName[number] = name;
        size[number] = index+1;
        number++;
        fprintf(r,"\n");
        fclose(r);
        fclose(p);
    }
    closedir(dr);
    printf("============================\n");
    printf("Test read graph : \n");
    printGraph2(g);
    printf("*****************************************\n");
    printf("check add edges : \n");
    printGraph(g);


    //=================================================
    //Input starting point and destination
    printf("================================================\n");
    printf("************************************************\n");
    char start[256],end[256];
    int p1, p2;
    printf("Test, Input name for starting point : ");
    // fgets(name,256,stdin);
    // name[strlen(name)-1] = '\0';
    fflush(stdin);
    scanf("%[^\n]s",start);
    scanf("%*c");
    printf("Starting point : %s.\n",start);

    p1 = findPoint(g,start);
    printf("Starting point vertex  : %d\n",p1);

    printf("Test, Input name for destination : ");
    // fgets(name,256,stdin);
    // name[strlen(name)-1] = '\0';
    fflush(stdin);
    scanf("%[^\n]s",end);
    scanf("%*c");
    printf("Destination : %s.\n",end);

    p2 = findPoint(g,end);
    printf("Destination : %d\n",p2);


    //====================================================
    //Trigger shortestPath 
    printf("************************************************\n");
    int path[1000],length;
    int pathn[1000];
    int e = 0;
    double weight = shortestPath(g,p1,p2,path,&length);
    if(weight == INF){
        printf("No path bw _%s_ and _%s_\n",start,end);
    }
    else{
        char xxx[256];
        printf("Path bw _%s_ and _%s_ :\n",start,end);
        for(int i = length; i>=0; i--){
            findPointName(g,path[i],xxx);
            printf("%s * %d -> ",xxx,path[i]);    
            // printf("%d  ",path[i]);
            pathn[e] = path[i];
            e++; 
        }
        printf("\n");
        printf("Total weight : %lf.\n",weight);
    }

    // printf("=================================\n");
    // printf("Test : \n");
    // int xx[100];
    // int x = indegree(g,102,xx);
    // for(int i = 0;i<x;i++){
    //     printf("%d\t",xx[i]);
    // }

    //=============================================================
    printf("===============================================\n");
    printf("***********************************************\n");
    printf("Track route : \n");
    // printf("first, test input : \n");
    // printf("routes : \n");
    // for(int i = 0; i<number; i++){
    //     printf("%s\n",routeName[i]);
    //     for(int j = 0;j<size[i];j++){
    //         printf("%d->",arr[i][j]);
    //     }
    //     printf("\n");
    // }
    //Greedy alg
    int top = 0 ;
    int maxp = 0;
    char *plan[50];
    int countp = 0;
    while(top < length -1){
        // printf("top : %d",top);
        for(int i = 0; i<number; i++){
            for(int j = 0;j<size[i]-1;j++){
                int temp = top;
                int count = 0;
                if(arr[i][j] == pathn[temp]){
                    while(j<size[i]-1&&arr[i][j+1] == pathn[temp+1]){
                        j++;
                        // printf("cai gi bang cai gi : %d -%d - %d\n",j+1,size[i],pathn[temp+1]);
                        count++;
                        temp++;
                    }
                    if(count>maxp){
                        // printf("%s-%d\n",routeName[i],count);
                        plan[countp] = strdup(routeName[i]);
                        maxp = count;
                    }
                }
            }
        }
        countp++;
        top+=maxp;
        // printf("maxp: %d",maxp);
        maxp = 0;
    }
    printf("In ra route : \n");
    for(int i = 0;i<countp;i++){
        printf("%s->",plan[i]);
    }
    printf("\n");
    printf("===============================================\n");


    dropGraph(g);
    return 0;
}