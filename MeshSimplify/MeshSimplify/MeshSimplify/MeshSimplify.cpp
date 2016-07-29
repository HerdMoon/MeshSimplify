#include "stdafx.h"
#include"SimpleObject.h"
#include <iostream>
#include <vector>
#include "stdafx.h"
#include "SimpleObject.h"
#include <cmath>

using namespace SimpleOBJ;
using namespace std;



struct node
{
	int x;
	int y;
	node *next;
	node(){next = NULL;}
};
//void rearrage(node * t,node * v_graph,Vec3f* v_array,float* v_power,int num);
void save_obj(char* f_name,int v_num,int num,int * heap,Vec3f* v_array,node* v_graph);
void setup_node(node* target_arr,int n,int x,int y);

void setup_node(node* target_arr,int n,int x,int y)
{
	node* temp1 = new node;
	temp1->x = x;
	temp1->y = y;
	temp1->next = target_arr[n].next;
	target_arr[n].next = temp1;	
}


float dist(float x_1,float x_2,float x_3,float y_1,float y_2,float y_3)
{
	return sqrt((x_1-y_1)*(x_1-y_1) + (x_2 - y_2) * (x_2 - y_2) + (x_3 - y_3) * (x_3 - y_3));
}

void save_obj(char* f_name,int v_num,int num,int * heap,Vec3f* v_array,node* v_graph)
{
	FILE* fp;
	bool used[1000000];
	for (int i=0;i<1000000;i++)
	{
		used[i] = false;
	}
	fopen_s(&fp,f_name,"w");

	fprintf(fp,"# %d vertices\n",num+1);

	for (int i=0; i<v_num; i++)
	{
		fprintf(fp,"v %f %f %f\n",v_array[i].x,v_array[i].y,v_array[i].z);
	}
	int sum=0;
	for(int i=0; i<num+1; i++)
	{
		node* t = v_graph[heap[i]].next;
		while(t != NULL)
		{
			if (used[t->x] || used[t->y])
			{
				t = t->next;
				continue;
			}
			sum ++;
			t = t->next;
		}
		used[heap[i]] = true;
	}
	for (int i=0;i<1000000;i++)
	{
		used[i] = false;
	}
	fprintf(fp,"# %d triangles\n",sum);
	for(int i=0;i<=num;i++)
	{
		node* t = v_graph[heap[i]].next;
		while (t != NULL)
		{
			if ((!used[t->x]) && (!used[t->y]))
			{
				fprintf(fp,"f %d %d %d\n",heap[i]+1, t->x + 1,t->y + 1);
			}
			t = t->next;
		}
		used[heap[i]] = true;
	}
	fclose(fp);
	printf("done.");
}


int _tmain(int argc, _TCHAR* argv[])
{
	CSimpleObject obj;

	while(true)
	{
		char filenames[50];
	cout<<"name?"<<endl;
	cin >> filenames;
	obj.LoadFromObj(filenames);
	float bilv;
	cout <<"t?"<<endl;
	cin >> bilv;

	int mode;
	cout <<"mode?"<<endl;
	cin >>mode;

	int v_nums = obj.m_nVertices; 
	int* heap = new int[v_nums];
	int p_nums = obj.m_nTriangles;
	Vec3f* v_array = new Vec3f[v_nums]; 
	Array<int,3>* fs = new Array<int,3>[p_nums]; 

	node* v_graph = new node[v_nums];

	float* v_power = new float[v_nums];
	for(int i=0; i<v_nums; i++) heap[i]=100000;

	for (int i=0; i<v_nums; i++)
	{
		v_array[i].x = obj.m_pVertexList[i].x;
		v_array[i].y = obj.m_pVertexList[i].y;
		v_array[i].z = obj.m_pVertexList[i].z;
	}
	
	for (int i=0; i<p_nums; i++)
	{
		fs[i][0] = obj.m_pTriangleList[i][0];
		fs[i][1] = obj.m_pTriangleList[i][1];
		fs[i][2] = obj.m_pTriangleList[i][2];
	}

	for(int i=0; i<p_nums; i++)
	{
		setup_node(v_graph,fs[i][0],fs[i][1],fs[i][2]);
		setup_node(v_graph,fs[i][1],fs[i][0],fs[i][2]);
		setup_node(v_graph,fs[i][2],fs[i][0],fs[i][1]);
	}



	for(int i=0; i<v_nums; i++)
	{
		node *t = v_graph[i].next;
		float sum = 0; 
		int count = 0;
		while(t != NULL) 
		{

			count += 2;
			sum += dist(v_array[i].x, v_array[i].y, v_array[i].z, v_array[t->x].x, v_array[t->x].y, v_array[t->x].z);
			sum += dist(v_array[i].x, v_array[i].y, v_array[i].z, v_array[t->y].x, v_array[t->y].y, v_array[t->y].z);
			t = t->next;
		}
		v_power[i] = sum/count;

	}

	for(int j=0; j<v_nums; j++)
	{
		heap[j] = j;
		int i=j;
		if(i != 0) //上滤
		{
			while(i > 0)
			{
				if(v_power[heap[(i-1)/2]] > v_power[heap[i]])
				{
					int tt = heap[i];
					heap[i] = heap[(i-1)/2];
					heap[(i-1)/2] = tt;
					i = (i-1)/2;
				}
				else
					break;
			}
		}
	}
	int num = v_nums-1; 
for(int pp=0; pp<v_nums*(1-bilv);pp++)
{	
	int cur_nod = heap[0];
	heap[0] = heap[num];
	num--;  
	int spe_ptr = v_graph[cur_nod].next->x;

	float mindis = dist(v_array[cur_nod].x, v_array[cur_nod].y, v_array[cur_nod].z, 
		v_array[v_graph[cur_nod].next->x].x, v_array[v_graph[cur_nod].next->x].y, v_array[v_graph[cur_nod].next->x].z);
	node* smin = v_graph[cur_nod].next;
	while(smin != NULL)
	{
		if (mindis > dist(v_array[cur_nod].x, v_array[cur_nod].y, v_array[cur_nod].z, 
			v_array[smin->x].x, v_array[smin->x].y, v_array[smin->x].z))
		{
			spe_ptr = smin->x;
			mindis = dist(v_array[cur_nod].x, v_array[cur_nod].y, v_array[cur_nod].z, 
				v_array[smin->x].x, v_array[smin->x].y, v_array[smin->x].z);
		}
		if (mindis > dist(v_array[cur_nod].x, v_array[cur_nod].y, v_array[cur_nod].z, 
			v_array[smin->y].x, v_array[smin->y].y, v_array[smin->y].z))
		{
			spe_ptr = smin->y;
			mindis = dist(v_array[cur_nod].x, v_array[cur_nod].y, v_array[cur_nod].z, 
				v_array[smin->y].x, v_array[smin->y].y, v_array[smin->y].z);
		}
		smin = smin->next;
	}
	
	
	node* de = v_graph[spe_ptr].next;
	while(de->next != NULL)
	{

		if(de->next->x == cur_nod || de->next->y == cur_nod)
		{
			de->next = de->next->next;
		}
		else
			de = de->next;
	}

	if(v_graph[spe_ptr].next->x == cur_nod || v_graph[spe_ptr].next->y == cur_nod)
		v_graph[spe_ptr].next = v_graph[spe_ptr].next->next;

	node* temp_ptr1 = v_graph[cur_nod].next;
	while(temp_ptr1 != NULL)
	{
		if(temp_ptr1->x != spe_ptr && temp_ptr1->y != spe_ptr)
		{			
			setup_node(v_graph,spe_ptr,temp_ptr1->x,temp_ptr1->y);
		}
		temp_ptr1 = temp_ptr1->next;
	}

	node* temp_ptr2 = v_graph[cur_nod].next;
	while (temp_ptr2 != NULL)
	{

		node* ptr_1 = v_graph[temp_ptr2->x].next;
		while (ptr_1 != NULL)
		{
			if(ptr_1->x == cur_nod) ptr_1->x = spe_ptr;
			if(ptr_1->y == cur_nod) ptr_1->y = spe_ptr;
			ptr_1 = ptr_1->next;
		}
		node* ptr_2 = v_graph[temp_ptr2->y].next;
		while (ptr_2 != NULL)
		{
			if(ptr_2->x == cur_nod) ptr_2->x = spe_ptr;
			if(ptr_2->y == cur_nod) ptr_2->y = spe_ptr;
			ptr_2 = ptr_2->next;
		}		
		temp_ptr2 = temp_ptr2->next;
	}
	if (mode==1)
	{
		v_array[spe_ptr].x = (v_array[spe_ptr].x + v_array[cur_nod].x)/2;
		v_array[spe_ptr].y = (v_array[spe_ptr].y + v_array[cur_nod].y)/2;
		v_array[spe_ptr].z = (v_array[spe_ptr].z + v_array[cur_nod].z)/2;
	}

	node* local_ptr = v_graph[cur_nod].next;

//	rearrage(local_ptr,v_graph,v_array,v_power,num);



	while (local_ptr != NULL) //local_ptr.m, local_ptr.n是需要遍历修改的点;
	{
		
		node* tt = v_graph[local_ptr->x].next;
		float sum = 0; //为节点i对应的值;
		int count = 0;
		while(tt != NULL) 
		{
			count += 2;
			sum += dist(v_array[local_ptr->x].x, v_array[local_ptr->x].y, v_array[local_ptr->x].z, v_array[tt->x].x, v_array[tt->x].y, v_array[tt->x].z);
			sum += dist(v_array[local_ptr->x].x, v_array[local_ptr->x].y, v_array[local_ptr->x].z, v_array[tt->y].x, v_array[tt->y].y, v_array[tt->y].z);
			tt = tt->next;
		}
		v_power[local_ptr->x] = sum/count;
		int k;
		for (k=0; k<num; k++)
		{
			if(heap[k] == local_ptr->x)
				break;
		}
		while( k>0) //上滤;
		{
			if (v_power[heap[k]] < v_power[heap[(k-1)/2]])
			{
				int ttt = heap[k];
				heap[k] = heap[(k-1)/2];
				heap[(k-1)/2] = ttt;
			}
			k = (k-1)/2;
		}
		while( k < num) //下滤;
		{
			int minnum=0;
			if (2*k+1 <= num)
			{
				if(v_power[heap[2*k+1]] < v_power[heap[k]])
				{
					minnum = 1;
				}
			}
			if (2*k+2 <= num)
			{
				if (minnum == 1)
				{
					if (v_power[heap[2*k+1]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
				else
				{
					if (v_power[heap[k]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
			}
			if (minnum == 1)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+1];
				heap[2*k+1] = ttt;
				k = 2*k+1;
			}
			else if (minnum == 2)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+2];
				heap[2*k+2] = ttt;
				k = 2*k+2;
			}
			else break;
			
		}


		node* ttt = v_graph[local_ptr->y].next;
		sum = 0; //为节点i对应的值;
		count = 0;
		while(ttt != NULL) //计算每个节点的值，并存到最小堆中去;
		{
			count += 2;
			sum += dist(v_array[local_ptr->y].x, v_array[local_ptr->y].y, v_array[local_ptr->y].z, v_array[ttt->x].x, v_array[ttt->x].y, v_array[ttt->x].z);
			sum += dist(v_array[local_ptr->y].x, v_array[local_ptr->y].y, v_array[local_ptr->y].z, v_array[ttt->y].x, v_array[ttt->y].y, v_array[ttt->y].z);
			ttt = ttt->next;
		}
		v_power[local_ptr->y] = sum/count;
		for (k=0; k<num; k++)
		{
			if(heap[k] == local_ptr->y)
				break;
		}
		while( k>0) //上滤;
		{
			if (v_power[heap[k]] < v_power[heap[(k-1)/2]])
			{
				int ttemp_ptr2 = heap[k];
				heap[k] = heap[(k-1)/2];
				heap[(k-1)/2] = ttemp_ptr2;
			}
			k = (k-1)/2;
		}
		while( k < num) //下虑;
		{
			int minnum=0;
			if (2*k+1 <= num)
			{
				if(v_power[heap[2*k+1]] < v_power[heap[k]])
				{
					minnum = 1;
				}
			}
			if (2*k+2 <= num)
			{
				if (minnum == 1)
				{
					if (v_power[heap[2*k+1]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
				else
				{
					if (v_power[heap[k]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
			}
			if (minnum == 1)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+1];
				heap[2*k+1] = ttt;
				k = 2*k+1;
			}
			else if (minnum == 2)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+2];
				heap[2*k+2] = ttt;
				k = 2*k+2;
			}
			else break;

		}

		local_ptr = local_ptr->next;
	}

	if (mode == 1)
	{
	local_ptr = v_graph[spe_ptr].next;

//	rearrage(local_ptr,v_graph,v_array,v_power,num);



	while (local_ptr != NULL) //local_ptr.m, local_ptr.n是需要遍历修改的点;
	{
		
		node* tt = v_graph[local_ptr->x].next;
		float sum = 0; //为节点i对应的值;
		int count = 0;
		while(tt != NULL) 
		{
			count += 2;
			sum += dist(v_array[local_ptr->x].x, v_array[local_ptr->x].y, v_array[local_ptr->x].z, v_array[tt->x].x, v_array[tt->x].y, v_array[tt->x].z);
			sum += dist(v_array[local_ptr->x].x, v_array[local_ptr->x].y, v_array[local_ptr->x].z, v_array[tt->y].x, v_array[tt->y].y, v_array[tt->y].z);
			tt = tt->next;
		}
		v_power[local_ptr->x] = sum/count;
		int k;
		for (k=0; k<num; k++)
		{
			if(heap[k] == local_ptr->x)
				break;
		}
		while( k>0) //上滤;
		{
			if (v_power[heap[k]] < v_power[heap[(k-1)/2]])
			{
				int ttt = heap[k];
				heap[k] = heap[(k-1)/2];
				heap[(k-1)/2] = ttt;
			}
			k = (k-1)/2;
		}
		while( k < num) //下滤;
		{
			int minnum=0;
			if (2*k+1 <= num)
			{
				if(v_power[heap[2*k+1]] < v_power[heap[k]])
				{
					minnum = 1;
				}
			}
			if (2*k+2 <= num)
			{
				if (minnum == 1)
				{
					if (v_power[heap[2*k+1]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
				else
				{
					if (v_power[heap[k]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
			}
			if (minnum == 1)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+1];
				heap[2*k+1] = ttt;
				k = 2*k+1;
			}
			else if (minnum == 2)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+2];
				heap[2*k+2] = ttt;
				k = 2*k+2;
			}
			else break;
			
		}


		node* ttt = v_graph[local_ptr->y].next;
		sum = 0; 
		count = 0;
		while(ttt != NULL) 
		{
			count += 2;
			sum += dist(v_array[local_ptr->y].x, v_array[local_ptr->y].y, v_array[local_ptr->y].z, v_array[ttt->x].x, v_array[ttt->x].y, v_array[ttt->x].z);
			sum += dist(v_array[local_ptr->y].x, v_array[local_ptr->y].y, v_array[local_ptr->y].z, v_array[ttt->y].x, v_array[ttt->y].y, v_array[ttt->y].z);
			ttt = ttt->next;
		}
		v_power[local_ptr->y] = sum/count;
		for (k=0; k<num; k++)
		{
			if(heap[k] == local_ptr->y)
				break;
		}
		while( k>0) //上滤;
		{
			if (v_power[heap[k]] < v_power[heap[(k-1)/2]])
			{
				int ttemp_ptr2 = heap[k];
				heap[k] = heap[(k-1)/2];
				heap[(k-1)/2] = ttemp_ptr2;
			}
			k = (k-1)/2;
		}
		while( k < num) //下虑;
		{
			int minnum=0;
			if (2*k+1 <= num)
			{
				if(v_power[heap[2*k+1]] < v_power[heap[k]])
				{
					minnum = 1;
				}
			}
			if (2*k+2 <= num)
			{
				if (minnum == 1)
				{
					if (v_power[heap[2*k+1]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
				else
				{
					if (v_power[heap[k]] > v_power[heap[2*k+2]])
					{
						minnum = 2;
					}
				}
			}
			if (minnum == 1)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+1];
				heap[2*k+1] = ttt;
				k = 2*k+1;
			}
			else if (minnum == 2)
			{
				int ttt = heap[k];
				heap[k] = heap[2*k+2];
				heap[2*k+2] = ttt;
				k = 2*k+2;
			}
			else break;

		}

		local_ptr = local_ptr->next;
	}
	}

	} 

	save_obj("out.obj",v_nums, num, heap, v_array, v_graph); //num]
	obj.Destroy();
	cout<<endl<<endl;
	}
	return 0;
}
