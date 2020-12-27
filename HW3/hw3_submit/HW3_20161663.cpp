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
float kruskal_duration = 0;	//	make_heap 제외
float make_heap_duration = 0;
float delete_duration = 0;

class Set {	//	정점 번호를 포함한 disjoint-set의 개별 집합 
public:
	int vname;		//	정점 번호 == set 번호
	int rank;
	Set* parent;	//	부모 노드를 가리키는 포인터

	//	Set 생성자
	Set();
	Set(int _v);
};

class DisjointSet {	//	Union-Find를 위한 disjoint-set
public:
	int numOfSets;	//	개별 집합 개수 = 정점 개수(초기)
	Set* disjointSets;

	//	DisjointSet 생성자, 멤버 함수, 소멸자
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
	int flag;	//	해당 edge가 mst에 속할 경우 어떤 component의 mst의 edge인지 표시
				//	mst edge에 포함되지 않을 경우 NONE(-1)

	//	Edge 생성자
	Edge();
	Edge(long long _n, int _v1, int _v2, long long _w);
};

class EdgeNode {
public:
	long long edgeName;	//	edge 이름을 저장
	EdgeNode* next;

	//	EdgeNode 생성자
	EdgeNode(long long _en);
};

class StackNode {	//	DFS를 위한 스택의 노드
public:
	int vertexName;	//	vertex 이름을 저장
	StackNode* next;

	//	StackNode 생성자
	StackNode(int _vn);
};

class Stack {	//	DFS를 위한 Stack
public:
	StackNode* top;

	//	Stack 생성자, 멤버 함수, 소멸자
	Stack();
	bool empty();
	void push(int vertex);
	int peek();
	int pop();
	~Stack();
};

class Component {
public:
	int Cname;				//	component 이름(0 ~ component 개수 - 1)
	int CVnum;				//	component에 포함된 vertex 개수
	long long CEnum;		//	component의 MST에 포함된 edge 개수 (CVnum - 1이 될때까지)
	long long kscanned;		//	MST 찾는 과정에서 처리한 edge 개수
	long long mstCost;		//	component의 MST cost

	//	Component 생성자
	Component();
	Component(int _n);
};

class Graph {
public:
	int Vnum;					//	vertex 개수
	int Cnum;					//	component 개수
	long long Enum;				//	edge 개수
	long long Wmax;				//	최대 weight
	EdgeNode** adjList;				//	Adjacent list
	EdgeNode** next;				//	vertex의 edge 중 다음에 확인할 edge 번호 담긴 Node 저장 배열, DFS에 이용
	Edge* edgeArray;			//	edge 저장 배열
	int* components;			//	각 정점이 어떤 component에 속하는 지 표시
	Component* comp;			//	connected-components
	long long* heap;			//	edge 저장 heap
	long long heapNum;			//	heap에 저장된 원소 개수
	Stack* st;					//	dfs를 위한 스택
	DisjointSet* ds;			//	vertices로 이루어진 disjoint-sets

	//	Graph 생성자, 멤버 함수, 소멸자
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
	std::cout << "vertex 개수 : " << Vnum << "\n" << "edge 개수 : " << Enum << "\n" << "max-weight : " << Wmax << "\n\n";

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
		std::cout << "component " << graph->comp[i].Cname << " 정보" << "\n";
		std::cout << "Vertex 개수 : " << graph->comp[i].CVnum << "\n";
		std::cout << "MST Edge 개수 : " << graph->comp[i].CEnum << "\n";
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

//	각 클래스의 생성자, 멤버 함수, 소멸자 정의
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
	numOfSets--;	//	서로 다른 두 집합이 합쳐짐. 집합의 개수는 1개 감소
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
	heap[ename + 1] = ename;	//	heap에 일단 순서대로 저장
	node1->next = adjList[v1];
	node2->next = adjList[v2];
	adjList[v1] = node1;
	adjList[v2] = node2;
}

void Graph::dfs(int v) {	//	connected component를 찾기 위한 DFS
	EdgeNode* w = NULL;
	components[v] = Cnum;	//	v를 방문
	st->push(v);

	while (!st->empty()) {
		int u = st->peek();
		int prev = u;
		w = next[u];

		for (; ; ) {
			if (!w) {	//	현재 vertex의 간선 중 방문할 정점이 있는 간선이 없을 경우
				st->pop();
				break;
			}

			int vnext = edgeArray[w->edgeName].v1 == prev ?
				edgeArray[w->edgeName].v2 : edgeArray[w->edgeName].v1;	//	현재 edge의 또 다른 vertex
			if (components[vnext] == Cnum) {
				w = w->next;
				continue;	//	이미 dfs로 탐색한 경우, 다른 edge로 넘어감
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
	Cnum++;	//	다음 connected component 번호 & component 개수 1 증가
}

void Graph::findComponents() {		//	각 vertex가 속한 컴포넌트를 결정함
	for (int i = 0; i < Vnum; i++)
		next[i] = adjList[i];	//	next 배열 초기화

	for (int i = 0; i < Vnum; i++) {
		if (components[i] == NONE) 	//	아직 어느 component에 속하는지 알지 못함
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
	ds = new DisjointSet(Vnum);	//	disjoint sets 객체 만듬(초기 집합 개수 = |V|)
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

void Graph::makeHeap() {	//	min-heap 만듬
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
			if (comp[i].CEnum == comp[i].CVnum - (long long)1) end_flag++;	//	어떤 component의 mst가 완성될 경우 end_flag 1 증가
		if (end_flag == Cnum) break;	//	모든 component의 mst가 완성될 경우

		e = heapDelete();	//	O(log|E|)
		vf = edgeArray[e].v1;
		vr = edgeArray[e].v2;

		if (components[vf] != components[vr]) {
			return;
		}
		int compNum = components[vf];

		comp[compNum].kscanned++;	//	처리한 edge 수 증가
		if (ds->find(vf) != ds->find(vr)) {	//	edge의 두 정점이 서로 같은 집합에 속하지 않는 경우
			edgeArray[e].flag = compNum;	//	해당 component의 mst의 edge임
			comp[compNum].CEnum++;	//	component에 간선 추가
			comp[compNum].mstCost += edgeArray[e].weight;	//	mst cost에 추가
			ds->unionByRank(vf, vr);	//	두 집합을 합침
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