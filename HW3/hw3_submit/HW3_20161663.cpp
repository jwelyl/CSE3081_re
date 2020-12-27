#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <Windows.h>

#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)
_int64 start, freq, end;
#define NONE	-1

float total_duration = 0;
float make_graph_duration = 0;
float construct_graph_duration = 0;
float component_duration = 0;
float kruskal_duration = 0;	//	make_heap ����
float make_heap_duration = 0;
float delete_duration = 0;

class Set {	//	���� ��ȣ�� ������ disjoint-set�� ���� ���� 
public:
	int vname;		//	���� ��ȣ == set ��ȣ
	int rank;
	Set* parent;	//	�θ� ��带 ����Ű�� ������

	//	Set ������
	Set();
	Set(int _v);
};

class DisjointSet {	//	Union-Find�� ���� disjoint-set
public:
	int numOfSets;	//	���� ���� ���� = ���� ����(�ʱ�)
	Set* disjointSets;

	//	DisjointSet ������, ��� �Լ�, �Ҹ���
	DisjointSet(int _num);
	int find(int v);
	void unionByRank(int x, int y);
	~DisjointSet();
};

class Edge {
public:
	long long name;	//	ename = (v1, v2)	//	name 0 ~ |E| - 1
	int v1;
	int v2;
	long long weight;
	int flag;	//	�ش� edge�� mst�� ���� ��� � component�� mst�� edge���� ǥ��
				//	mst edge�� ���Ե��� ���� ��� NONE(-1)

	//	Edge ������
	Edge();
	Edge(long long _n, int _v1, int _v2, long long _w);
};

class EdgeNode {
public:
	long long edgeName;	//	edge �̸��� ����
	EdgeNode* next;

	//	EdgeNode ������
	EdgeNode(long long _en);
};

class StackNode {	//	DFS�� ���� ������ ���
public:
	int vertexName;	//	vertex �̸��� ����
	StackNode* next;

	//	StackNode ������
	StackNode(int _vn);
};

class Stack {	//	DFS�� ���� Stack
public:
	StackNode* top;

	//	Stack ������, ��� �Լ�, �Ҹ���
	Stack();
	bool empty();
	void push(int vertex);
	int peek();
	int pop();
	~Stack();
};

class Component {
public:
	int Cname;				//	component �̸�(0 ~ component ���� - 1)
	int CVnum;				//	component�� ���Ե� vertex ����
	long long CEnum;		//	component�� MST�� ���Ե� edge ���� (CVnum - 1�� �ɶ�����)
	long long kscanned;		//	MST ã�� �������� ó���� edge ����
	long long mstCost;		//	component�� MST cost

	//	Component ������
	Component();
	Component(int _n);
};

class Graph {
public:
	int Vnum;					//	vertex ����
	int Cnum;					//	component ����
	long long Enum;				//	edge ����
	long long Wmax;				//	�ִ� weight
	EdgeNode** adjList;				//	Adjacent list
	EdgeNode** next;				//	vertex�� edge �� ������ Ȯ���� edge ��ȣ ��� Node ���� �迭, DFS�� �̿�
	Edge* edgeArray;			//	edge ���� �迭
	int* components;			//	�� ������ � component�� ���ϴ� �� ǥ��
	Component* comp;			//	connected-components
	long long* heap;			//	edge ���� heap
	long long heapNum;			//	heap�� ����� ���� ����
	Stack* st;					//	dfs�� ���� ����
	DisjointSet* ds;			//	vertices�� �̷���� disjoint-sets

	//	Graph ������, ��� �Լ�, �Ҹ���
	Graph(int _V, long long _E, long long _W);
	void insertEdge(long long ename, int v1, int v2, long long weight);
	void dfs(int v);
	void findComponents();
	void makeComponents();
	void makeDisjointSets();
	bool heapEmpty();
	void adjustHeap(long long i);
	void makeHeap();
	long long heapDelete();
	void kruskal();
	~Graph();
};

int main(void) {
	std::string cmd_file = "commands.txt";
	std::string input_file, output_file;
	std::ifstream ifs(cmd_file);
	if (ifs.fail()) {
		std::cout << "Command File Open Error!" << std::endl;
		return -1;
	}
	ifs >> input_file >> output_file;
	ifs.close();

	ifs.open(input_file);
	std::ofstream ofs(output_file);
	if (ifs.fail()) {
		std::cout << "Input File Open Error!" << std::endl;
		return -1;
	}

	Graph* graph = NULL;
	int Vnum;
	long long Enum, Wmax;

	ifs >> Vnum >> Enum >> Wmax;
	std::cout << "\n" << input_file << "\n";
	std::cout << "vertex ���� : " << Vnum << "\n" << "edge ���� : " << Enum << "\n" << "max-weight : " << Wmax << "\n\n";

	CHECK_TIME_START;
	graph = new Graph(Vnum, Enum, Wmax);
	CHECK_TIME_END(make_graph_duration);

	CHECK_TIME_START;
	for (long long i = 0; i < graph->Enum; i++) {
		int v1, v2;
		long long weight;
		ifs >> v1 >> v2 >> weight;
		graph->insertEdge(i, v1, v2, weight);
	}
	CHECK_TIME_END(construct_graph_duration);

	CHECK_TIME_START;
	graph->findComponents();
	graph->makeComponents();
	CHECK_TIME_END(component_duration);
	graph->kruskal();
	ofs << graph->Cnum << "\n";
	for (int i = 0; i < graph->Cnum; i++)
		ofs << graph->comp[i].CVnum << " " << graph->comp[i].mstCost << "\n";
	for (int i = 0; i < graph->Cnum; i++) {
		std::cout << "component " << graph->comp[i].Cname << " ����" << "\n";
		std::cout << "Vertex ���� : " << graph->comp[i].CVnum << "\n";
		std::cout << "MST Edge ���� : " << graph->comp[i].CEnum << "\n";
		std::cout << "MST cost : " << graph->comp[i].mstCost << "\n";
		std::cout << "kscanned : " << graph->comp[i].kscanned << "\n\n";
	}

	CHECK_TIME_START;
	delete graph;
	CHECK_TIME_END(delete_duration);

	total_duration = make_graph_duration + construct_graph_duration + component_duration
		+ make_heap_duration + kruskal_duration + delete_duration;
	printf("make-graph duration : %f(s)\n", make_graph_duration);
	printf("construct-graph duration : %f(s)\n", construct_graph_duration);
	printf("component duration : %f(s)\n", component_duration);
	printf("make-heap duration : %f(s)\n", make_heap_duration);
	printf("kruskal_duration : %f(s)\n", kruskal_duration);
	printf("delete-graph duration : %f(s)\n", delete_duration);
	printf("total_duration : %f(s)\n", total_duration);

	ifs.close();
	ofs.close();

	return 0;
}

//	�� Ŭ������ ������, ��� �Լ�, �Ҹ��� ����
//	class Set
Set::Set() {}
Set::Set(int _v) : vname(_v), rank(0), parent(NULL) {}

//	class DisjointSet
DisjointSet::DisjointSet(int _num) : numOfSets(_num) {
	disjointSets = new Set[numOfSets];
	for (int i = 0; i < numOfSets; i++)
		disjointSets[i] = Set(i);
}

int DisjointSet::find(int v) {
	if (!disjointSets[v].parent) return v;
	else return find(disjointSets[v].parent->vname);
}

void DisjointSet::unionByRank(int x, int y) {
	int rootX = find(x);
	int rootY = find(y);

	if (rootX == rootY) return;

	if (disjointSets[rootX].rank < disjointSets[rootY].rank) {
		disjointSets[rootX].parent = &disjointSets[rootY];
	}
	else {
		if (disjointSets[rootX].rank == disjointSets[rootY].rank)
			disjointSets[rootX].rank++;
		disjointSets[rootY].parent = &disjointSets[rootX];
	}
	numOfSets--;	//	���� �ٸ� �� ������ ������. ������ ������ 1�� ����
}

DisjointSet::~DisjointSet() {
	delete[] disjointSets;
	disjointSets = NULL;
}

//	class Edge
Edge::Edge() {}
Edge::Edge(long long _n, int _v1, int _v2, long long _w)
	: name(_n), v1(_v1), v2(_v2), weight(_w), flag(NONE) {}

//	class EdgeNode
EdgeNode::EdgeNode(long long _en) : edgeName(_en), next(NULL) {}

//	class StackNode
StackNode::StackNode(int _vn) : vertexName(_vn), next(NULL) {}

//	class Stack
Stack::Stack() : top(NULL) {}

bool Stack::empty() {
	return !top;
}

void Stack::push(int vertex) {
	StackNode* node = new StackNode(vertex);
	node->next = top;
	top = node;
}

int Stack::peek() {
	if (empty()) return NONE;
	return top->vertexName;
}

int Stack::pop() {
	if (empty()) return NONE;
	StackNode* delNode = top;
	int ret = delNode->vertexName;
	top = delNode->next;
	delNode->next = NULL;
	delete delNode;
	return ret;
}

Stack::~Stack() {
	while (!empty())
		pop();
}

//	class Component
Component::Component() {}
Component::Component(int _n) : Cname(_n), CVnum(0), CEnum(0), kscanned(0), mstCost(0) {}

//	class Graph
Graph::Graph(int _V, long long _E, long long _W)
	: Vnum(_V), Cnum(0), Enum(_E), Wmax(_W), comp(NULL), heapNum(Enum), ds(NULL) {
	adjList = new EdgeNode * [Vnum];
	next = new EdgeNode * [Vnum];
	for (int i = 0; i < Vnum; i++)
		adjList[i] = NULL;
	edgeArray = new Edge[Enum];
	components = new int[Vnum];
	for (int i = 0; i < Vnum; i++)
		components[i] = NONE;
	heap = new long long[heapNum + 1];
	st = new Stack();
}

void Graph::insertEdge(long long ename, int v1, int v2, long long weight) {
	Edge e(ename, v1, v2, weight);
	EdgeNode* node1 = new EdgeNode(ename);
	EdgeNode* node2 = new EdgeNode(ename);

	edgeArray[ename] = e;
	heap[ename + 1] = ename;	//	heap�� �ϴ� ������� ����
	node1->next = adjList[v1];
	node2->next = adjList[v2];
	adjList[v1] = node1;
	adjList[v2] = node2;
}

void Graph::dfs(int v) {	//	connected component�� ã�� ���� DFS
	EdgeNode* w = NULL;
	components[v] = Cnum;	//	v�� �湮
	st->push(v);

	while (!st->empty()) {
		int u = st->peek();
		int prev = u;
		w = next[u];

		for (; ; ) {
			if (!w) {	//	���� vertex�� ���� �� �湮�� ������ �ִ� ������ ���� ���
				st->pop();
				break;
			}

			int vnext = edgeArray[w->edgeName].v1 == prev ?
				edgeArray[w->edgeName].v2 : edgeArray[w->edgeName].v1;	//	���� edge�� �� �ٸ� vertex
			if (components[vnext] == Cnum) {
				w = w->next;
				continue;	//	�̹� dfs�� Ž���� ���, �ٸ� edge�� �Ѿ
			}
			else {
				components[vnext] = Cnum;
				st->push(vnext);
				next[prev] = w->next;
				w = adjList[vnext];
				prev = vnext;
			}
		}
	}
	Cnum++;	//	���� connected component ��ȣ & component ���� 1 ����
}

void Graph::findComponents() {		//	�� vertex�� ���� ������Ʈ�� ������
	for (int i = 0; i < Vnum; i++)
		next[i] = adjList[i];	//	next �迭 �ʱ�ȭ

	for (int i = 0; i < Vnum; i++) {
		if (components[i] == NONE) 	//	���� ��� component�� ���ϴ��� ���� ����
			dfs(i);
	}
}

void Graph::makeComponents() {
	comp = new Component[Cnum];
	for (int i = 0; i < Cnum; i++)
		comp[i] = Component(i);
	for (int i = 0; i < Vnum; i++) {
		comp[components[i]].CVnum++;
	}
}

void Graph::makeDisjointSets() {
	ds = new DisjointSet(Vnum);	//	disjoint sets ��ü ����(�ʱ� ���� ���� = |V|)
	for (int i = 0; i < ds->numOfSets; i++)
		ds->disjointSets[i] = Set(i);
}

bool Graph::heapEmpty() {
	return !(heapNum >= 1);
}

void Graph::adjustHeap(long long i) {
	long long child;
	long long rootKey = edgeArray[heap[i]].weight;
	long long temp = heap[i];
	child = 2 * i;

	while (child <= heapNum) {
		if ((child < heapNum) &&
			edgeArray[heap[child]].weight >= edgeArray[heap[child + 1]].weight)
			child++;
		if (rootKey <= edgeArray[heap[child]].weight) break;
		else {
			heap[child / 2] = heap[child];
			child *= 2;
		}
	}
	heap[child / 2] = temp;
}

void Graph::makeHeap() {	//	min-heap ����
	for (long long i = heapNum / 2; i > 0; i--)
		adjustHeap(i);
}

long long Graph::heapDelete() {
	if (heapEmpty()) {
		return NONE;
	}
	else if (heapNum == 1) {
		heapNum = 0;
		return heap[1];
	}
	else {
		long long ret = heap[1];
		long long temp = heap[heapNum--];
		long long parent = 1, child = 2;

		while (child <= heapNum) {
			if ((child < heapNum) &&
				(edgeArray[heap[child]].weight >= edgeArray[heap[child + 1]].weight))
				child++;
			if (edgeArray[temp].weight <= edgeArray[heap[child]].weight) break;
			else {
				heap[parent] = heap[child];
				parent = child;
				child = child * 2;
			}
		}

		heap[parent] = temp;
		return ret;
	}
}

void Graph::kruskal() {
	CHECK_TIME_START;
	makeHeap();	//	O(|E|)
	CHECK_TIME_END(make_heap_duration);

	CHECK_TIME_START;
	makeDisjointSets();	//	O(|V|)

	int end_flag;
	int vf, vr;
	long long e, w;

	while (true) {
		end_flag = 0;
		for (int i = 0; i < Cnum; i++)
			if (comp[i].CEnum == comp[i].CVnum - (long long)1) end_flag++;	//	� component�� mst�� �ϼ��� ��� end_flag 1 ����
		if (end_flag == Cnum) break;	//	��� component�� mst�� �ϼ��� ���

		e = heapDelete();	//	O(log|E|)
		vf = edgeArray[e].v1;
		vr = edgeArray[e].v2;

		if (components[vf] != components[vr]) {
			return;
		}
		int compNum = components[vf];

		comp[compNum].kscanned++;	//	ó���� edge �� ����
		if (ds->find(vf) != ds->find(vr)) {	//	edge�� �� ������ ���� ���� ���տ� ������ �ʴ� ���
			edgeArray[e].flag = compNum;	//	�ش� component�� mst�� edge��
			comp[compNum].CEnum++;	//	component�� ���� �߰�
			comp[compNum].mstCost += edgeArray[e].weight;	//	mst cost�� �߰�
			ds->unionByRank(vf, vr);	//	�� ������ ��ħ
		}
	}
	CHECK_TIME_END(kruskal_duration);
}

Graph::~Graph() {
	for (int i = 0; i < Vnum; i++) {
		while (adjList[i]) {
			EdgeNode* del = adjList[i];
			adjList[i] = del->next;
			delete del;
		}
	}
	delete[] adjList;
	delete[] next;
	delete[] edgeArray;
	delete[] components;
	delete[] heap;
	delete st;
}