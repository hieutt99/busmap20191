#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jrb.h"
#include "dllist.h"

typedef struct {
    JRB edges;
    JRB vertices;
    JRB bus;
} Graph;

Graph createGraph() {
    Graph g;
    g.edges = make_jrb();
    g.vertices = make_jrb();
	g.bus = make_jrb();
    return g;
}

void dropGraph(Graph g) {
	jrb_free_tree(g.edges);
	jrb_free_tree(g.vertices);
	jrb_free_tree(g.bus);
	printf("\n\nGraph deleted\nClosed!\n");
}

// --------------------------------------------------

void addVertex(Graph g, int id, char *name) {
    JRB node = jrb_find_int(g.vertices, id);

    if(node == NULL){
        jrb_insert_int(g.vertices, id, new_jval_s(name));
    }
    else{
        node->val = new_jval_s(name);
    }
}

char *getVertex(Graph g, int id) {
    JRB node = jrb_find_int(g.vertices, id);
    if(node != NULL)
        return jval_s(node->val);
    else return NULL;
}

int hasVertex(Graph g, char *stop) {
	int flag = 0;
	JRB node;
	jrb_traverse(node, g.vertices){
		int id = jval_i(node->key);
		if(strcmp(jval_s(node->val), stop) == 0)
			flag = 1;
	}
	if(flag == 1)
		return 1;
	else return 0;
}

void addEdge(Graph g, int v1, int v2) {
	JRB tree = make_jrb();
	JRB tree2 = make_jrb();
	JRB node = jrb_find_int(g.edges, v1);
	JRB node2 = jrb_find_int(g.edges, v2);

	if(node != NULL) {
		tree = (JRB) jval_v(node->val);
		if(jrb_find_int(tree, v2) == NULL)
			jrb_insert_int(tree, v2, new_jval_i(1));	
	} else {
		jrb_insert_int(g.edges, v1, new_jval_v(tree));
		jrb_insert_int(tree, v2, new_jval_i(1));
	}

	if(node2 != NULL) {
		tree2 = (JRB) jval_v(node2->val);
		if(jrb_find_int(tree2, v1) == NULL)
			jrb_insert_int(tree2, v1, new_jval_i(1));	
	} else {
		jrb_insert_int(g.edges, v2, new_jval_v(tree2));
		jrb_insert_int(tree2, v1, new_jval_i(1));
	}
}

int hasEdge(Graph g, int v1, int v2) {
	JRB node1 = jrb_find_int(g.edges, v1);
	if(node1 != NULL) {
		JRB tree = jval_v(node1->val);
		JRB node2 = jrb_find_int(tree, v2);
		if(node2 != NULL) return 1;
		else return 0;
	} 
	else return 0;
}

void addBus(Graph g, int id, char *bustmp) {
	JRB node = jrb_find_int(g.bus, id);
    JRB tree = make_jrb();

    if(node != NULL) {
        tree = (JRB) jval_v(node->val);
        jrb_insert_str(tree, bustmp, new_jval_v(NULL));
    } else {
        jrb_insert_int(g.bus, id, new_jval_v(tree));
        jrb_insert_str(tree, bustmp, new_jval_v(NULL));
    }
}

int hasBus(Graph g, int id, char *bustmp) {
    JRB node1 = jrb_find_int(g.bus, id);
	if(node1 != NULL) {
		JRB tree = (JRB) jval_v(node1->val);
		JRB node2 = jrb_find_str(tree, bustmp);

		if(node2 != NULL) return 1;
		else return 0;
	} 
	else return 0;
}

// --------------------------------------------------

void deleteNewLine(char *str){ // Delete \n character
    int i = 0;
    while(str[i] != '\0'){
        if(str[i] == '\n'){
            str[i] = '\0';
            break;
        }
        i++;
    }
}

int readfromfile2(Graph g) { //replace busstops by id (write to routemapid2)
	FILE *fin2 = fopen("routemap2.txt", "r");
	FILE *fout = fopen("routemapidx.txt", "w");
	if(fin2 == NULL || fout == NULL) {
		printf("Cant open file to read\n");
		return -1;
	}

	printf("\n");
	char buf[10000];
	while(fgets(buf, 10000, fin2) != NULL) {
		//buf[strlen(buf) - 1] = '\0';
		char route[10];
		strcpy(route, buf);
		deleteNewLine(route);
		printf("\nroute '%s': ", route);

		char *tmp = (char *)malloc(sizeof(char) * 1000);
		strcpy(tmp, route);
		fputs(tmp, fout); // write bus route to fout
		fputs("\n", fout);

		fgets(buf, 10000, fin2); // get busstop
		//buf[strlen(buf) - 1] = '\0';
		deleteNewLine(buf);

		char arr[200][200];
        int j = 0, ctr = 0;
        for(int i = 0; i <= strlen(buf); i++) {
            if(buf[i] == '-') {
                arr[ctr][j] = '\0';
                ctr++;
                j = 0;
            }
            else {
                arr[ctr][j] = buf[i];
                j++;
            }
        }

		for(int i = 0; i < ctr; i++) {
			//printf("%s ", arr[i]);
			char *d = (char *)malloc(sizeof(char) * 1000);
			strcpy(d, arr[i]);
			
			JRB node;
			jrb_traverse(node, g.vertices) {
				if(strcmp(jval_s(node->val), d) == 0) {
					printf("%d ", jval_i(node->key));
					fprintf(fout, "%d", jval_i(node->key));
					fputs(" ",fout);
				}
			}
			printf("-> ");
		}
		printf("\n");
		fputs("\n",fout);
	}

	fclose(fin2);
}

int readfromfile(Graph g) { //addBus and addVertices by id from busmap4.txt
	FILE *fin = fopen("busmap4.txt", "r");
	if(fin == NULL) {
		printf("Cant open file to read\n");
		return -1;
	}

	char buf[10000];
	int id = 0;

	while(fgets(buf, 10000, fin) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		//printf("stop %d: %s -> ", id, buf);
		char *stop = (char*)malloc(sizeof(char) * 10000);
		strcpy(stop, buf);

		addVertex(g, id, stop);

		char *bus = (char *)malloc(sizeof(char) * 10000);
		fgets(buf, 10000, fin);
		buf[strlen(buf) - 1] = '\0';
		strcpy(bus, buf);
		//printf("-> %s\n", bus);

		char arr[200][200];
        int j = 0, ctr = 0;
        for(int i = 0; i <= strlen(bus); i++) {
            if(bus[i] == ' ' || bus[i] == '\0') {
                arr[ctr][j] = '\0';
                ctr++;
                j = 0;
            }
            else {
                arr[ctr][j] = bus[i];
                j++;
            }
        }

		for(int i = 0; i < ctr; i++) {
			//printf("%s ", arr[i]);
			char *d = (char *)malloc(sizeof(char) * 10000);
			strcpy(d, arr[i]);
			addBus(g, id, d);
		}
		id++;
		//printf("\n");
	}
	close(fin);
}

int readfromfile3(Graph g) { //addEdge from routemapid2
	FILE *fin3 = fopen("routemapid2.txt", "r");
	if(fin3 == NULL) {
		printf("Cant open file to read\n");
		return -1;
	}

	char buf[10000];
	while(fgets(buf, 10000, fin3) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		//printf("bus %s: ", buf);

		fgets(buf, 10000, fin3);
		buf[strlen(buf) - 1] = '\0';
		//printf("%s \n", buf);

		char arr[200][200];
        int j = 0, ctr = 0;
        for(int i = 0; i <= strlen(buf); i++) {
            if(buf[i] == ' ' || buf[i] == '\0') {
                arr[ctr][j] = '\0';
                ctr++;
                j = 0;
            }
            else {
                arr[ctr][j] = buf[i];
                j++;
            }
        }

		for(int i = 0; i < ctr - 1; i++) {
			//printf("%s ", arr[i]);
			int id1 = atoi(arr[i]);
			int id2 = atoi(arr[i+1]);
			addEdge(g, id1, id2);
		}
		//printf("%s ", arr[ctr-1]);
		//printf("\n");
	}

	fclose(fin3);
}

// --------------------------------------------------

void printMap(Graph g) { //print adjacent details of all locations
	JRB node1, node2;
	printf("\nHanoi Bus Map adjacent details: \n");
	jrb_traverse(node1, g.edges) {
		int id = jval_i(node1->key);
        printf("id %d - %s --> ", id, getVertex(g, id));

        JRB tree = jval_v(node1->val);
    	jrb_traverse(node2, tree) {
    		int id2 = jval_i(node2->key);
			printf("%s, ", getVertex(g, id2));
    	}
    	printf("\n");
	}
}

void printMapByID(Graph g) { //print adjacent details of all locations by ID
	JRB node1, node2;
	printf("\nHanoi Bus Map details by ID: \n");
	jrb_traverse(node1, g.edges) {
		int id = jval_i(node1->key);
        //printf("%s: ", getVertex(g, id));
		printf("id %d: ", id);

        JRB tree = jval_v(node1->val);
    	jrb_traverse(node2, tree) {
    		int id2 = jval_i(node2->key);
			//printf("%s - ", getVertex(g, id2));
			printf("%d ", id2);
    	}
    	printf("\n");
	}
}

void printBus(Graph g) { //print buses through locations
	printf("\n\nHanoi Bus through locations:\n");
	JRB node, node2, node3;
	jrb_traverse(node, g.bus) {
		int id = jval_i(node->key);
		printf("id %d - ", id);
		printf("%s --> ", getVertex(g, id));

		JRB tree = jval_v(node->val);
		jrb_traverse(node2, tree) {
			printf("%s ", jval_s(node2->key));
		}
		printf("\n");
	}
}

// --------------------------------------------------------

char *inputLocation(Graph g, char *stop1, int s) { // input a location
	int check;
	char *s1 = (char *)malloc(sizeof(char) * 1000);
	do {
		printf("Input location %d: ", s); getchar();
		scanf("%[^\n]s", stop1);
		strcpy(s1, stop1);

		check = hasVertex(g, s1);
		if(check == 0)
			printf("--> Invalid location %d! Try again\n", s);
	} while (check == 0);
	return s1;
}

void findLocationID(Graph g) { //find location through an id
	int id, check;
	do {
		printf("Input ID: ");
		scanf("%d", &id);
	
		check = jrb_find_int(g.vertices, id);
		if (check == NULL)
			printf("Invalid id! Try again\n");
		else {
			printf("--> ID %d: %s\n", id, getVertex(g, id));
		}	
	} while(check == NULL);	
}

int locationToID(Graph g, char *stop) { // convert from location to id
	int id1;
	JRB node1;
	jrb_traverse(node1, g.vertices) { //take id from 2 locations
		if(strcmp(jval_s(node1->val), stop) == 0)
			return jval_i(node1->key);
	}
}

void findID(Graph g) {
	char *stop1 = (char *)malloc(sizeof(char) * 1000);
	stop1 = inputLocation(g, stop1, 1);

	int id1 = locationToID(g, stop1);
	printf("%s --> %d\n", stop1, id1);

}

void checkConnection(Graph g) { //check connection between 2 locations
	int check, check2;
	char *stop1 = (char *)malloc(sizeof(char) * 1000);
	char *stop2 = (char *)malloc(sizeof(char) * 1000);

	stop1 = inputLocation(g, stop1, 1);
	stop2 = inputLocation(g, stop2, 2);	

	int id1 = locationToID(g, stop1);
	int id2 = locationToID(g, stop2);
	JRB node2, node3;

	jrb_traverse(node2, g.edges) { //find connection 
		if(jval_i(node2->key) == id1) {
			int flag = 0;
			JRB tree = jval_v(node2->val);
			jrb_traverse(node3, tree) {
				if(jval_i(node3->key) == id2) {
					printf("--> Locations connected!\n");
					flag = 1;
				}
			}
			if(flag == 0)
				printf("--> Locations not connected!\n");
		}
	}
}

void adjacentLocation(Graph g) { //adjacent ids of an id
	int check;
	char *stop1 = (char *)malloc(sizeof(char) * 1000);

	stop1 = inputLocation(g, stop1, 1);

	int id = locationToID(g, stop1);

	JRB node1, node2;
	jrb_traverse(node1, g.edges) {
		int idtmp = jval_i(node1->key);
        if(idtmp == id) {
			printf("id %d: %s --> ", id, getVertex(g, id));

        	JRB tree = jval_v(node1->val);
    		jrb_traverse(node2, tree) {
    			int id2 = jval_i(node2->key);
				printf("%s, ", getVertex(g, id2));
    		}
    		printf("\n");
		}
	}
}

// ------ shortest path Dijktra-------------------------------------------

int getAdjacent(Graph g, int id, int *output) { //adjacent ids of an id
	JRB node1 = jrb_find_int(g.edges, id);
	JRB tree = make_jrb(); 
	tree = jval_v(node1->val);

	JRB node2;
	int total = 0;
	jrb_traverse(node2, tree) {
		output[total] = jval_i(node2->key);
		total++;
	}
	return total;
} 

int getEdge(Graph g, int v1, int v2) { //get Edge value of 2 ids
	JRB node1 = jrb_find_int(g.edges, v1);
	if(node1 != NULL) {
		JRB tree = jval_v(node1->val);
		JRB node2 = jrb_find_int(tree, v2);
		if(node2 != NULL) return jval_i(node2->val);
		else return 99999;
	}
	else return 99999;
}

int busSuggest(Graph g, int v1, int v2) { //find bus from paths
	JRB node1, node2;
    JRB nodev1 = jrb_find_int(g.bus, v1);
    JRB nodev2 = jrb_find_int(g.bus, v2);
	JRB tree = make_jrb(), tree2 = make_jrb();
	tree = jval_v(nodev1->val); // all buses of vertex 1
	tree2 = jval_v(nodev2->val); // all buses of vertex 2
	
	printf("\n%s -> %s: ", getVertex(g, v1), getVertex(g, v2));

    char temp[1000]; 
    strcpy(temp, "");
    strcat(temp, getVertex(g, v1));
    strcat(temp,"-");
    strcat(temp, getVertex(g, v2)); // -> example: "Nguyen Trai-Chu Van An"

    char temp2[1000];
    strcpy(temp2, "");
    strcat(temp2, getVertex(g, v2));
    strcat(temp2,"-");
    strcat(temp2, getVertex(g, v1)); // -> example: "Chu Van An-Nguyen Trai"

	jrb_traverse(node1, tree) {
		if(jrb_find_str(tree2, jval_s(node1->key))) { //if (a bus of v1) exists in tree2 
            char *bus = (char *)malloc(sizeof(char) * 100);
            strcpy(bus, jval_s(node1->key));

            // read route of a bus
            FILE *f = fopen("routemap2.txt", "r");
	        if(f == NULL) {
		        printf("Cant open file to read! \n");
		        return -1;
        	}

	        char buf[1000];
	        while(fgets(buf, 1000, f) != NULL) {
		        deleteNewLine(buf);
		        if(strcmp(buf, bus) == 0) {
			        char route[2000];
			        fgets(route, 2000, f);
			        deleteNewLine(route);
			        //printf("%s \n", route);
                    //printf("\nbus %s checktemp %d ", bus, is_substr(route, temp));
					//printf("checktemp2 %d\n", is_substr(route, temp2));
					
					//char *str, *str2;
					char *str = strstr(route, temp);
					char *str2 = strstr(route, temp2);
					if(str != NULL || str2 != NULL) {
					    printf("%s ", jval_s(node1->key));
                    }
		        }
	        }
	        fclose(f);
        }
	}
}

int Dijktra(Graph g, int s, int t, int *path, int *length ) { // length = 1
	int distance[1000];
	int previous[1000], u, visit[1000];

	for (int i = 0; i < 1000; ++i){
		distance[i] = 99999;
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
		int number = getAdjacent(g, u, output);

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

int readRouteBus(int res[], char *bus) { //read all stops of a bus by id
	FILE *f = fopen("routemapid2.txt", "r");
	if(f == NULL) {
   	   	printf("Cant open file to read! \n");
 	   	return -1;
	}

	char buf[1000];
	int ctr;
	while(fgets(buf, 1000, f) != NULL) {
		deleteNewLine(buf);
		if(strcmp(buf, bus) == 0) {
			char route[1000];
			fgets(route, 2000, f);
			deleteNewLine(route);

			char a[200][200]; 
	        int j = 0;
			ctr = 0;
	        for(int i = 0; i <= strlen(route); i++) {
	            if(route[i] == ' ' || route[i] == '\0') {
	    			a[ctr][j] = '\0';
	                ctr++;
	                j = 0;
	        	}
	   			else {
       			   	a[ctr][j] = route[i];
	              	j++;
	        	}	
			}
						
			for(int i = 0; i < ctr; i++) { //id int array of that bus
				res[i] = atoi(a[i]);
			}
			int m = ctr - 2;
			for(int i = ctr; i < 2*ctr - 1; i++) {
				res[i] = res[m];
				m--;
			}
		}
	}
	fclose(f);
	return 2*ctr - 1;
}

void shortestPath2(Graph g) { // using Dijkstra to find shortest path and bus suggestions on that path
	int check, check2;
	char *stop1 = (char *)malloc(sizeof(char) * 1000);
	char *stop2 = (char *)malloc(sizeof(char) * 1000);

	stop1 = inputLocation(g, stop1, 1);
	stop2 = inputLocation(g, stop2, 2);

	int s = locationToID(g, stop1);
	int t = locationToID(g, stop2);

	int length, path[1000];
	int weight = Dijktra(g, s, t, path, &length);
	if(weight == 99999)
		printf("\nNo path between %s and %s\n", getVertex(g, s), getVertex(g, t));
	else {
		printf("\nPaths between %s and %s: \n", getVertex(g,s), getVertex(g,t));
		for (int i = length; i >= 0; i--){
			if(i != 0)
				printf("%s -> ", getVertex(g, path[i]));
			else printf("%s ", getVertex(g, path[i]));
		}
		printf("\n\nPossible bus details:");

		for (int i = length; i >= 0; i--) {
			if(i > 0) {
				int j = i - 1;
				busSuggest(g, path[i], path[j]);
			}
		}

		//printf("\n");
		int len = 0, path2[100];
		for (int i = length; i >= 0; i--) {
			path2[len] = path[i];
			//printf("%d -> ", path2[len]);
			len++;
		}
		printf("\n");

		printf("\n\nBus Suggestion: \n");
		int max = 0;
		int i = 0, countp = 0;
		char *plan[100];
		while (i < len - 1) { //traverse each stop in path
			//printf("Stop %d id %d - %s\n", i, path2[i], getVertex(g, path2[i]));
			max = 0;
			JRB node1, node = jrb_find_int(g.bus, path2[i]);
			JRB tree = make_jrb();
			tree = jval_v(node->val);
			
			char arr[200][200];
			int ctr = 0;
			jrb_traverse(node1, tree) {
				strcpy(arr[ctr], jval_s(node1->key)); //all bus of that stop
				ctr++;
			}

			for(int j = 0; j < ctr; j++) { //traverse each bus 
				int id[200]; // all id of that bus
				int total = readRouteBus(id, arr[j]);

				// printf("\n** route %s: ", arr[j], total);
				// for(int k = 0; k < total; k++) {
				// 	printf("%d - ", id[k]);
				// }
				// printf("\n");

				for(int k = 0; k < total; k++) {
					int count = 0;
					int temp = i;
					if(id[k] == path2[temp]) {
						//printf("find at %d: %d\n", k, path2[temp]);
						while(k < total && id[k+1] == path2[temp+1]) {
							k++;
							count++;
							temp++;
						}
						// printf("-> To %d: %d\n", k, id[k], path2[temp]);
						// printf("count: %d\n\n", count);
						if (count > max) {
							//printf("Bus %s (%d stops) -> ", arr[j], count);
							plan[countp] = strdup(arr[j]);
							max = count;
						}
					}
				}
			}
			i += max;
			countp++;
		}
		
    	printf("\n");
		printf("In ra route : \n");
    	for(int i = 0; i<countp; i++){
        	printf("%s ->", plan[i]);
    	}

		printf("\nTotal stops: %d\n", weight);
	}
} 

//--------------------------------------------------------------------

void busThroughLocation(Graph g) { //All buses through a location
	int check;
	char *stop1 = (char *)malloc(sizeof(char) * 1000);

	stop1 = inputLocation(g, stop1, 1);
	int id = locationToID(g, stop1);

	JRB node1, node2;
	jrb_traverse(node1, g.bus) {
		int idtmp = jval_i(node1->key);
        if(idtmp == id) {
			printf("id %d: %s --> ", id, getVertex(g, id));

        	JRB tree = jval_v(node1->val);
    		jrb_traverse(node2, tree) {
				printf("%s ", jval_s(node2->key));
    		}
    		printf("\n");
		}
	}
}	

int busLocations(Graph g) { //All locations of a bus
	int check = 0;
	char bus[1000];

	do {
		printf("Input Bus: "); getchar();
		scanf("%[^\n]s", bus);
		char *s1 = (char *)malloc(sizeof(char) * 1000);
		strcpy(s1, bus);

		JRB node, node2;
		jrb_traverse(node, g.bus) {
			JRB tree = jval_v(node->val);
			if(jrb_find_str(tree, s1) != NULL) {
				check = 1;
				break;
			}
		}
		if(check == 0)
			printf("Invalid bus! Try again\n");
	} while (check == 0);

	printf("-> Route %s: ", bus);

	FILE *f = fopen("routemap2.txt", "r");
	if(f == NULL) {
		printf("Cant open file to read! \n");
		return -1;
	}

	char buf[1000];
	while(fgets(buf, 1000, f) != NULL) {
		deleteNewLine(buf);
		if(strcmp(buf, bus) == 0) {
			char route[2000];
			fgets(route, 2000, f);
			deleteNewLine(route);
			printf("%s \n", route);
		}
	}

	fclose(f);
}

// --------------------------------------------------------------------

void isContinue() { // (Y/N option)
	printf("\n---------------------------------\nDo you want to continue? (y/n): ");
	getchar(); 
	char c; scanf("%c", &c);
	if(c != 'y' && c != 'Y')
		exit(0);
}

void showMenu() {
	printf("\n----------------------------------------\n");
	printf("\n\t---HANOI BUSGRAPH---\n\n");
	printf("0. Find ID of a location\n");
	printf("1. Find location of an ID\n");
	printf("2. Check connection between 2 locations\n");
	printf("3. Adjacent locations\n");
	printf("4. Shortest path between 2 locations\n");
    printf("5. Show all buses through a location\n");
    printf("6. Show all locations of a bus\n");
	printf("7. Show busmap connections\n");
	printf("8. Show buses through all locations\n");
    printf("9. Exit\n");
}


int main() {
	Graph g = createGraph();
    readfromfile(g); //addBus and addVertices by id from busmap4.txt
	//readfromfile2(g);	
	readfromfile3(g); //addEdge from routemapid2

	//printMapByID(g);

	// MENU
  	int choice;

		do {
			printf("\033[H\033[J");
			showMenu();
			printf("\n----------------------------------------\n\nInput a choice: ");
			scanf("%d", &choice);

			switch(choice){
				case 0:
					findID(g);
					isContinue();
					break;

				case 1: 
					findLocationID(g);
					isContinue();
					break;

				case 2: 
					checkConnection(g);
					isContinue();
					break;

				case 3: 
					adjacentLocation(g);
					isContinue();
					break;

                case 4: 
					shortestPath2(g);
					//sleep(5);
					isContinue();
					break;

				case 5: 
					busThroughLocation(g);
					isContinue();
					break;

				case 6: 
					busLocations(g);
					isContinue();
					break;

				case 7: 
					printMap(g);
					isContinue();
					break;
				
				case 8: 
					printBus(g);
					isContinue();
					break;

				case 9:
					printf("Program closing...\n");
					break;

				default:
                	printf("Invalid task! Try again!\n\n");
					isContinue();
                	break;
			}

		}	while (choice != 9);

    dropGraph(g);
    return 0;
}