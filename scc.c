#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/timeb.h>

#define MAX_VERTICES 875715 //12 //number of vertices + 1

unsigned int t = 0;
unsigned int s = 0;
unsigned int *leader = NULL;
unsigned int *explored = NULL;
unsigned int *f = NULL;

struct largeFive
{
	unsigned int m1, m2, m3, m4, m5, flag;
};

struct edge
{
	unsigned int v;
	struct edge *previous;
	struct edge *next;
};

int createEdge(struct edge **root, unsigned int ver);
void printArrayOfPointers(struct edge **a, int len);
void freeArrayOfPointers(struct edge **a, int len);
int reverseGraph(struct edge **array, struct edge **arrayRev, int len);
void printArray(unsigned int *A, int len);
void dfsLoop(struct edge **array, int len, int flag);
void dfs(struct edge **array, unsigned int ver, int flag);
int modifyArray(struct edge **array, int len);
struct largeFive *extractLargeFive(unsigned int *count, int len);
void mergeSort(unsigned int *a, int l, int r);
void merger(unsigned int *a, int l1, int r1, int l2, int r2);

int main(int argc, char **argv)
{
	struct timeb stop1, stop2, stop3, start1, start2, start3;
        clock_t start11,stop11, start22, stop22, start33, stop33;
        ftime(&start1);
        start11 = clock();
	struct stat sb;
	off_t len;
	char *p;
	int fd;
	fd = open("SCC.txt", O_RDONLY);
	
	if(fd == -1)
	{
		perror("open");
		return 1;
	}
	
	if(fstat(fd, &sb) == -1)
	{
		perror("fstat");
		return 1;
	}

	if(!S_ISREG(sb.st_mode))
	{
		fprintf(stderr,"%s is not a regular file\n","SCC.txt");
		return 1;
	}

	p = mmap(NULL, sb.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
	if(p == MAP_FAILED)
	{
		perror("mmap");
		return 1;
	}

	if(close(fd) == -1)
	{
		perror("close");
		return 1;
	}
	int maxVertices = MAX_VERTICES;
	struct edge **array = NULL;
	// array consists of 875715 elements.. array[0] is set to NULL.. so array[1] corresponds to vertex 1 and so on..
	array = calloc(maxVertices, sizeof(struct edge *));
	if(array == NULL)
	{
		fprintf(stderr,"%s\n","calloc failed to create array of struct pointers");
		return 1;
	}
	int i = 0;
	for(; i < maxVertices; ++i)
	{
		array[i] = NULL;
	}
	char *token = NULL, *subtoken = NULL, *str1 = NULL, *str2 = NULL, *saveptr1 = NULL, *saveptr2 = NULL;
	unsigned int v1 = 0, v2 = 0, flag = 0;
	int r = 0;
	for(str1 = p; ; str1 = NULL)
	{
		token = strtok_r(str1,"\n\0",&saveptr1);
		if(token == NULL)
			break;
		for(str2 = token; ; str2 = NULL)
		{
			subtoken = strtok_r(str2," ", &saveptr2);
			if(subtoken == NULL)
				break;
			if(flag == 0)
			{
				flag = 1;
				if(sscanf(subtoken, "%u", &v1) == EOF)
				{
					fprintf(stderr,"%s\n", "sscanf failed to create v1");
					free(array);
					return 1;
				}
			}
			else
			{
				flag = 0;
				if(sscanf(subtoken, "%u", &v2) == EOF)
				{
					fprintf(stderr,"%s\n", "sscanf failed to create v2");
					free(array);
					return 1;
				}
				else
				{
					r = createEdge(&array[v1], v2);
					if(r == 1)
					{
						fprintf(stderr,"%s\n","malloc failed in createEdge routine");
						free(array);
						return r;
					}
				}
			}
		}
	}
	if(munmap(p, sb.st_size) == -1)
	{
		perror("munmap");
		return 1;
	}
	//printArrayOfPointers(array, maxVertices);
	struct edge **arrayRev = calloc(maxVertices, sizeof(struct edge *));
	if(arrayRev == NULL)
	{
		fprintf(stderr,"%s\n","calloc failed to create arrayRev");
		freeArrayOfPointers(array, maxVertices);
		return 1;
	}
	for(i = 0; i < maxVertices; ++i)
		arrayRev[i] = NULL;
	r = reverseGraph(array, arrayRev, maxVertices);
	if(r == 1)
	{
		freeArrayOfPointers(array, maxVertices);
		return 1;
	}
	//printf("%s\n","---------------");
	//printArrayOfPointers(arrayRev, maxVertices);
	
	leader = calloc(maxVertices, sizeof(unsigned int));
	explored = calloc(maxVertices, sizeof(unsigned int));
	f = calloc(maxVertices, sizeof(unsigned int));
	if(leader == NULL || explored == NULL || f == NULL)
	{
		fprintf(stderr,"%s\n","calloc failed to create leader, f or explored array");
		freeArrayOfPointers(arrayRev, maxVertices);
	        freeArrayOfPointers(array, maxVertices);
		return 1;
	}
	
	dfsLoop(arrayRev, maxVertices, 0); //flag = 0 for first dfsLoop call; 1 for second dfsLoop call
	freeArrayOfPointers(arrayRev, maxVertices);
	//printArray(f, maxVertices);
	//printf("%s\n","--------------");
	r = modifyArray(array, maxVertices);
	if(r == 1)
	{
		fprintf(stderr,"%s\n","calloc failed to create auxArray array in modifyArray subroutine");
		free(f); free(leader); free(explored);
	        freeArrayOfPointers(array, maxVertices);
		return 1;
	}
	//printArrayOfPointers(array, maxVertices);
	for(i = 0; i < maxVertices; ++i)
		explored[i] = 0;
	dfsLoop(array, maxVertices, 1);
	free(f); free(explored);
	freeArrayOfPointers(array, maxVertices);
	//printf("%s\n","-----------");
        //printArray(leader, maxVertices);
	
	unsigned int *count = calloc(maxVertices, sizeof(unsigned int));
	if(count == NULL)
	{
		fprintf(stderr,"%s\n","calloc failed to create count array");
		free(leader);
		return 1;
	}
	for(i = 1; i < maxVertices; ++i)
	{
		count[leader[i]]++;
	}
	free(leader);
	//printf("%s\n","-----------");
	//printArray(count, maxVertices);
	/*struct largeFive *lf = NULL;
	lf = extractLargeFive(count, maxVertices);*/
	mergeSort(count, 0, maxVertices-1);
	printf("%u %u %u %u %u\n",count[0], count[1], count[2], count[3], count[4]);
	free(count);
	/*if(lf->flag == 1)
	{
		fprintf(stderr,"%s\n","calloc failed to create largeFive struct");
		free(lf);
		return 1;
	}
	//printf("%s\n","-----------");
	printf("%u %u %u %u %u\n",lf->m1,lf->m2,lf->m3,lf->m4,lf->m5);
	free(lf);*/
	stop11 = clock();
        ftime(&stop1);
        double cpu_time1 = ((double) (stop11 - start11)) / CLOCKS_PER_SEC;
        int diff1 = (int) (1000.0 * (stop1.time - start1.time) + (stop1.millitm - start1.millitm));
	printf("The number of items, CPU and wall clock time are \'%d\', \'%f\' seconds and \'%u\' seconds",maxVertices-1, cpu_time1,diff1/1000);
        printf("\n");
	return 0;
}

void mergeSort(unsigned int *a, int l, int r)
{
	if(r - l == 0)
		return;
	else
	{
		mergeSort(a, l, l + ((r - l)/2));
		mergeSort(a, l + ((r - l)/2) + 1, r);
		merger(a, l, l + ((r - l)/2), l + ((r - l)/2) + 1, r);
	}
}

void merger(unsigned int *a, int l1, int r1, int l2, int r2)
{
	int i = l1, j = l2, z = 0;
	unsigned int *auxArray = (unsigned int *)calloc(r2 - l1 + 1, sizeof(unsigned int));
	assert(auxArray != NULL);
	while(i <= r1 && j <= r2)
	{
		if(a[i] >= a[j])
			auxArray[z++] = a[i++];
		else
		{
			auxArray[z++] = a[j++];
		}
	}
	while(i <= r1 && j > r2)
	{
		auxArray[z++] = a[i++];
	}
	while(i > r1 && j <= r2)
		auxArray[z++] = a[j++];
	i = l1; z = 0;
	for(; i <= r2; ++i)
		a[i] = auxArray[z++];
	free(auxArray);
}

struct largeFive *extractLargeFive(unsigned int *count, int len)
{
	struct largeFive *lf = calloc(1, sizeof(struct largeFive));
	if(lf == NULL)
	{
		lf->flag == 1;
		return lf;
	}
	int i = 1;
	for(; i < len; ++i)
	{
		if(count[i] > 0)
		{
			if(count[i] > lf->m1)
			{
				lf->m5 = lf->m4;
				lf->m4 = lf->m3;
				lf->m3 = lf->m2;
				lf->m2 = lf->m1;
				lf->m1 = count[i];
			}
			else if(count[i] > lf->m2)
			{
				lf->m5 = lf->m4;
                                lf->m4 = lf->m3;
                                lf->m3 = lf->m2;
                                lf->m2 = count[i];
			}
			else if(count[i] > lf->m3)
			{
				lf->m5 = lf->m4;
                                lf->m4 = lf->m3;
                                lf->m3 = count[i];
			}
			else if(count[i] > lf->m4)
			{
				lf->m5 = lf->m4;
                                lf->m4 = count[i];
			}
			else if(count[i] > lf->m5)
				lf->m5 = count[i];
		}
	}
	return lf;
}

int modifyArray(struct edge **array, int len)
{
	struct edge **auxArray = calloc(len, sizeof(struct edge *));
	if(auxArray == NULL)
		return 1;
	int i = 1;
	for(; i < len; ++i)
	{
		auxArray[f[i]] = array[i];
	}
	for(i = 1; i < len; ++i)
		array[i] = auxArray[i];
	free(auxArray);
	struct edge *temp = NULL;
	unsigned int x = 0;
	for(i = 1; i < len; ++i)
	{
		temp = array[i];
		while(temp != NULL)
		{
			x = temp->v;
			temp->v = f[x];
			temp = temp->next;
		}
	}
	return 0;
}

void printArray(unsigned int *A, int len)
{
	int i = 1;
	for(; i < len; ++i)
		printf("%d ",A[i]); 
	printf("\n");
}

void dfsLoop(struct edge **array, int len, int flag)
{
	t = 0; s = 0;
	unsigned int i = len - 1;
	for(; i > 0; --i)
	{
		if(explored[i] != 1)
		{
			s = i;
			dfs(array, i, flag);
		}
	}
}

void dfs(struct edge **array, unsigned int ver, int flag)
{
	explored[ver] = 1;
	if(flag == 1)
		leader[ver] = s;
	struct edge *temp = array[ver];
	while(temp != NULL)
	{
		if(explored[temp->v] != 1)
			dfs(array, temp->v, flag);
		temp = temp->next;
	}
	if(flag == 0)
	{
		t++; f[ver] = t;
	}
	return;
}

int reverseGraph(struct edge **array, struct edge **arrayRev, int len)
{
	int i = 1, r = 0;
	for(; i < len; ++i)
	{
		struct edge *temp = array[i];
		while(temp != NULL)
		{
			r = createEdge(&arrayRev[temp->v], i);
			if(r == 1)
			{
				freeArrayOfPointers(arrayRev, i + 1);
				return r;
			}
			temp = temp->next;
		}
	}
	return r;
}

int createEdge(struct edge **root, unsigned int ver)
{
	if(*root == NULL)
	{
		*root = calloc(1, sizeof(struct edge));
		if(*root == NULL)
			return 1;
		(*root)->v = ver;
		(*root)->previous = NULL;
		(*root)->next = NULL;
	}
	else
	{
		struct edge *temp1 = *root;
		struct edge *temp2 = NULL;
		temp2 = calloc(1, sizeof(struct edge));
		if(temp2 == NULL)
		       return 1;	
		while(temp1->next != NULL)
			temp1 = temp1->next;
		temp1->next = temp2;
		temp2->v = ver;
		temp2->previous = temp1;
		temp2->next = NULL;
	}
	return 0;
}

void printArrayOfPointers(struct edge **a, int len)
{
	int i = 0;
	struct edge *temp = NULL;
	if(a[i++] == NULL)
		printf("%s\n", "NULL");
	for(; i < len; ++i)
	{
		temp = a[i];
		while(temp != NULL)
		{
			printf("%u-->", temp->v);
			temp = temp->next;
		}
		printf("%s\n", "NULL");
	}
}

void freeArrayOfPointers(struct edge **a, int len)
{
	int i = 1;
	struct edge *temp1 = NULL, *temp2 = NULL;
	for(; i < len; ++i)
	{
		temp1 = a[i];
		if(temp1 != NULL)
		{
			while(temp1->next != NULL)
			{	
				temp2 = temp1;
				while(temp2->next != NULL)
					temp2 = temp2->next;
				(temp2->previous)->next = NULL;
				free(temp2);
			}
			free(temp1);
		}
	}
	free(a);
}
