#include <iostream>
#include <fstream>
#include <string>

#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)
_int64 start, freq, end;
#define NONE	-1

class Set {	//	정점 번호를 포함한 disjoint-set의 개별 집합 
public:
	int vname;		//	정점 번호 == set 번호
	int rank;
	Set* parent;	//	부모 노드를 가리키는 포인터
	Set() {}
	Set(int _v) : vname(_v), rank(0), parent(NULL) {}
};

class DisjointSet {	//	Union-Find를 위한 disjoint-set
public:
	int numOfSets;	//	부분 집합 개수 = 정점 개수
	Set* disjointSets;
	
	DisjointSet(int _num) : numOfSets(_num) {
		disjointSets = new Set[numOfSets];
		for (int i = 0; i < numOfSets; i++) {
			disjointSets[i] = Set(i);
		//	disjointSets[i].parent = &disjointSets[i];
		}
	}

	int find(int v) {
	//	std::cout << "find(" << v << ")" << "\n";
	//	if (disjointSets[v].parent == &disjointSets[v])
	//		return v;
		if (!disjointSets[v].parent) return v;
		else return find(disjointSets[v].parent->vname);
	}

	int findPathCompression(int v) {
		if (disjointSets[v].parent != &disjointSets[v])
			disjointSets[v].parent =
			&disjointSets[findPathCompression(disjointSets[v].parent->vname)];
		return disjointSets[v].parent->vname;
	}

	void unionByRank(int x, int y) {
	//	std::cout << "Union(" << x << ", " << y << ")" << "\n";
		
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
	}

	~DisjointSet() {
		delete[] disjointSets;
		disjointSets = NULL;
	}
};

class Edge {
public:
	long long name;	//	ename = (v1, v2)	//	name 0 ~ |E| - 1
	int v1;		
	int v2;
	long long weight;
	int flag;	//	해당 edge가 mst에 속할 경우 어떤 component의 mst의 edge인지 표시
				//	mst edge에 포함되지 않을 경우 NONE(-1)
	
	Edge() {}
	Edge(long long _n, int _v1, int _v2, long long _w) 
		: name(_n), v1(_v1), v2(_v2), weight(_w), flag(NONE) {}
};

class Node {
public:
	long long edgeName;	//	edge 이름을 저장
//	bool flag;			//	DFS 시 확인하기 위한 flag
	Node* next;
	Node(long long _en) : edgeName(_en), next(NULL) {}
};

class StackNode {	//	DFS를 위한 스택의 노드
public:
	int vertexName;	//	vertex 이름을 저장
	StackNode* next;
	StackNode(int _vn) : vertexName(_vn), next(NULL) {}
};

class Stack {	//	DFS를 위한 Stack
public:
	StackNode* top;
	Stack() : top(NULL) {}
	bool empty() {
		return !top;
	}
	void push(int vertex) {
		StackNode* node = new StackNode(vertex);
		node->next = top;
		top = node;
	}
	int peek() {
		if (empty()) return NONE;
		return top->vertexName;
	}
	int pop() {
		if (empty()) return NONE;
		StackNode* delNode = top;
		int ret = delNode->vertexName;
		top = delNode->next;
		delNode->next = NULL;
		delete delNode;
		return ret;
	}
	~Stack() {
		while (!empty()) {
			pop();
		}
	}
};

class Component {
public:
	int Cname;				//	component 이름(0 ~ component 개수)
	int CVnum;				//	component에 포함된 vertex 개수
	long long CEnum;		//	component에 포함된 edge 개수 (CVnum - 1이 될때까지)
	long long kscanned;		//	MST 찾는 과정에서 처리한 edge 개수
	long long mstCost;		//	component의 MST cost
	Component() {}
	Component(int _n) : Cname(_n), CVnum(0), CEnum(0), kscanned(0), mstCost(0) {}
};

class Graph {
public:
	int Vnum;					//	vertex 개수
	int Cnum;					//	component 개수
	long long Enum;				//	edge 개수
	long long Wmax;				//	최대 weight
	Node** adjList;				//	Adjacent list
	Node** next;				//	vertex의 edge 중 다음에 확인할 edge 번호 담긴 Node 저장 배열, DFS에 이용
	Edge* edgeArray;			//	edge 저장 배열
	int* components;			//	각 정점이 어떤 component에 속하는 지 표시
	Component* comp;			//	connected-components
	long long* heap;			//	edge 저장 heap
	long long heapNum;			//	heap에 저장된 원소 개수
	Stack* st;					//	dfs를 위한 스택
	DisjointSet* ds;			//	component의 vertices로 이루어진 disjoint-sets

	Graph(int _V, long long _E, long long _W) 
		: Vnum(_V), Cnum(0), Enum(_E), Wmax(_W), comp(NULL), heapNum(Enum), ds(NULL) {
		adjList = new Node * [Vnum];
		next = new Node * [Vnum];
		for (int i = 0; i < Vnum; i++) 
			adjList[i] = NULL;
		edgeArray = new Edge[Enum];
		components = new int[Vnum];
		for (int i = 0; i < Vnum; i++)
			components[i] = NONE;
		heap = new long long[heapNum + 1];
		st = new Stack();
	}

	void insertEdge(long long ename, int v1, int v2, long long weight) {
		Edge e(ename, v1, v2, weight);
		Node* node1 = new Node(ename);
		Node* node2 = new Node(ename);

		edgeArray[ename] = e;
		heap[ename + 1] = ename;	//	heap에 일단 순서대로 저장
		node1->next = adjList[v1];
		node2->next = adjList[v2];
		adjList[v1] = node1;
		adjList[v2] = node2;
	}

	void dfs(int v) {	//	connected component를 찾기 위한 DFS
		Node* w = NULL;
	//	std::cout << "----------------------------------------------------------" << "\n";
	//	std::cout << "dfs(" << v << ") 시작!" << "\n";

		components[v] = Cnum;	//	v를 방문
	//	std::cout << "시작 정점 " << v << "를 방문(스택에 삽입)" << "\n";
		st->push(v);

		while (!st->empty()) {
			int u = st->peek();
			int prev = u;
			w = next[u];

		//	if(w) std::cout << "1.조사할 간선 : e" << w->edgeName << "\n";
			for (; ; ) {
				if (!w) {
					int del;
					//st->pop();
					del = st->pop();
		//			std::cout << "정점 " << del << "를 스택에서 제거" << "\n";
					break;
				}

		//		if (w) std::cout << "조사할 간선 : e" << w->edgeName << "\n";
				int vnext = edgeArray[w->edgeName].v1 == prev ? 
					edgeArray[w->edgeName].v2 : edgeArray[w->edgeName].v1;	//	현재 edge의 또 다른 verte
		//		std::cout << "prev : " << prev << "\n";
		//		std::cout << "vnext : " << vnext << "\n";
			//	prev = vnext;
				if (components[vnext] == Cnum) {
		//			std::cout << "정점 " << vnext << "는 이미 방문한 정점이므로 skip" << "\n";
					w = w->next;
					
					continue;	//	이미 dfs로 탐색한 경우, 다른 edge로 넘어감
				}
				else {
		//			std::cout << "정점 " << vnext << "를 방문(스택에 삽입)" << "\n";
					components[vnext] = Cnum;
					st->push(vnext);
					next[prev] = w->next;
					w = adjList[vnext];
					prev = vnext;
				}
			}
		}

	//	std::cout << "dfs(" << v << ") 끝!" << "\n";
	//	std::cout << "----------------------------------------------------------" << "\n";
		Cnum++;	//	다음 connected component 번호 & component 개수 1 증가
	}

	void findComponents() {		//	각 vertex가 속한 컴포넌트를 결정함
		for (int i = 0; i < Vnum; i++)
			next[i] = adjList[i];	//	next 배열 초기화

		for (int i = 0; i < Vnum; i++) {
		//	std::cout << "정점 "<<i<<"의 component?" << "\n";
			if (components[i] == NONE) {	//	아직 어느 component에 속하는지 알지 못함
		//		std::cout << "정점 " << i << " DFS로 component 찾기!" << "\n";
				dfs(i);
			}
		//	else {
		//		std::cout << "정점 " << i << "는 이미 component가 " << components[i] << "로 결정됨!" << "\n";
		//	}
		}
	}

	void makeComponent() {
		comp = new Component[Cnum];
		for (int i = 0; i < Cnum; i++)
			comp[i] = Component(i);
		for (int i = 0; i < Vnum; i++) {
			comp[components[i]].CVnum++;
		}
	}

	void makeDisjointSets() {
		ds = new DisjointSet(Vnum);	//	disjoint sets 객체 만듬
		for (int i = 0; i < ds->numOfSets; i++)
			ds->disjointSets[i] = Set(i);
	}

	bool heapEmpty() {
		return !(heapNum >= 1);
	}

	void adjustHeap(long long i) {
		long long child;
		long long rootKey = edgeArray[heap[i]].weight;
		long long temp = heap[i];
		child = 2 * i;

		while (child <= heapNum) {
			if ((child < heapNum) && 
				edgeArray[heap[child]].weight > edgeArray[heap[child + 1]].weight)
				child++;
			if (rootKey <= edgeArray[heap[child]].weight) break;
			else {
				heap[child / 2] = heap[child];
				child *= 2;
			}
		}
		heap[child / 2] = temp;
	}

	void makeHeap() {	//	min-heap 만듬
		for (long long i = heapNum / 2; i > 0; i--)
			adjustHeap(i);
	}

	long long heapDelete() {
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
					(edgeArray[heap[child]].weight > edgeArray[heap[child + 1]].weight))
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

	void kruskal() {
	/*
		std::cout << "Before make heap" << "\n";
		for (int i = 1; i <= heapNum; i++)
			std::cout << heap[i] << " ";
		std::cout << "\n";
		for (int i = 1; i <= heapNum; i++)
			std::cout << edgeArray[heap[i]].weight << " ";
		std::cout << "\n";
	*/
		makeHeap();	//	O(|E|)
	/*
		std::cout << "After make heap" << "\n";
		for (int i = 1; i <= heapNum; i++)
			std::cout << heap[i] << " ";
		std::cout << "\n";
		for (int i = 1; i <= heapNum; i++)
			std::cout << edgeArray[heap[i]].weight << " ";
		std::cout << "\n";
	*/
		makeDisjointSets();	//	O(|V|)
		int end_flag;
		int vf, vr;
		long long e, w;

		std::cout << "Disjoint sets" << "\n";
		for (int i = 0; i < ds->numOfSets; i++) 
			std::cout << ds->disjointSets[i].vname << " ";
		std::cout << "\n";
		for (int i = 0; i < ds->numOfSets; i++) {
			if (!ds->disjointSets[i].parent) std::cout << NONE << " ";
			else std::cout << ds->disjointSets[i].parent->vname << " ";
		}
		std::cout << "\n";

		
		int it = 1;
		while(true) {
			std::cout << "Kruskal iteration " << it++<<"\n";
			end_flag = 0;
			for (int i = 0; i < Cnum; i++) 
				if (comp[i].CEnum == comp[i].CVnum - (long long)1) end_flag++;	//	어떤 component의 mst가 완성될 경우 end_flag 1 증가
			if (end_flag == Cnum) break;	//	모든 component의 mst가 완성될 경우

			e = heapDelete();	//	O(log|E|)
			vf = edgeArray[e].v1;
			vr = edgeArray[e].v2;
			std::cout << "선택된 edge e" << edgeArray[e].name << ": (" << vf << ", " << vr << "), weight : " << edgeArray[e].weight << "\n";
//			std::cout << "find(" << vf << ") : " << ds->find(vf) << "\n";
			
			if (components[vf] != components[vr]) {
				std::cout << "같은 엣지의 두 정점이 속한 component가 다를 수 없음! 심각한 에러" << "\n";
				return;
			}
			int compNum = components[vf];
			
			comp[compNum].kscanned++;	//	처리한 edge 수 증가
			if (ds->find(vf) != ds->find(vr)) {	//	edge의 두 정점이 서로 같은 집합에 속하지 않는 경우
				edgeArray[e].flag = compNum;	//	해당 component의 mst의 edge임
				std::cout << "edge e" << edgeArray[e].name << " = (" << vf << ", " << vr << ")를 component"<< comp[compNum].Cname<<"에 추가" << "\n";
				comp[compNum].CEnum++;	//	component에 간선 추가
				comp[compNum].mstCost += edgeArray[e].weight;	//	mst cost에 추가
				ds->unionByRank(vf, vr);	//	두 집합을 합침
			}
			else {	//	edge의 두 정점이 이미 같은 집합에 속할 경우 추가하면 cycle이 생성되므로 추가 x
			//	edgeArray[e].flag = NONE;
				std::cout << "두 정점 " << vf << "와 " << vr << "이 이미 같은 집합에 있음" << "\n";
				std::cout << "edge e" << edgeArray[e].name << " = (" << vf << ", " << vr << ")를 component" << comp[compNum].Cname << "에 추가할 수 없음" << "\n";
			}
		}
		
	}

	~Graph() {

	}
};

int main(void) {
	std::string cmd_file = "command.txt";
	std::string input_file, output_file;
	std::ifstream cmd_ifs(cmd_file);
	if (cmd_ifs.fail()) {
		std::cout << "File Open Error!" << std::endl;
		return -1;
	}
	cmd_ifs >> input_file >> output_file;
//	std::cout << input_file << "\n" << output_file << "\n";
	std::ifstream ifs(input_file);
	std::ofstream ofs(output_file);

	Graph* graph = NULL;
	int Vnum;
	long long Enum, Wmax;

	ifs >> Vnum >> Enum >> Wmax;
	graph = new Graph(Vnum, Enum, Wmax);
	//	std::cout << Vnum << " " << Enum << " " << Wmax << "\n";
/*
	ofs << Vnum << " " << Enum << " " << Wmax << "\n";
	for (int i = 0; i < Enum; i++) {
		std::cout << i << std::endl;
		ifs >> v1 >> v2 >> weight;
		ofs << "edge " << i << " (" << v1 << ", " << v2 << ") cost : " << weight << "\n";
	}
*/
	for (long long i = 0; i < graph->Enum; i++) {
		int v1, v2;
		long long weight;
		ifs >> v1 >> v2 >> weight;
		graph->insertEdge(i, v1, v2, weight);
	}
/*
	for (int i = 0; i < graph->Vnum; i++) {
		Node* w = graph->adjList[i];
		ofs << "V" << i;
		if (w) ofs << " -> ";
		else ofs << " -> 0";
		for (; w; w = w->next) {
			ofs << "e" << w->edgeName;
			if (w->next) ofs << " -> ";
		}
		ofs << "\n";
	}
*/
/*
	for (long long i = 0; i < graph->Enum; i++) {
		std::cout << "edge" << graph->edgeArray[i].name << " : (" << graph->edgeArray[i].v1 << ", " << graph->edgeArray[i].v2 << "), " << "weight : " << graph->edgeArray[i].weight << "\n";
	}
*/
	graph->findComponents();
//	std::cout << "connected-components" << "\n";
	for (int i = 0; i < graph->Vnum; i++) {
		std::cout << graph->components[i] << " ";
	}
	std::cout << "\n";
	std::cout << "connected-components 개수 : " << graph->Cnum << "\n";
	graph->makeComponent();
	std::cout << "Kruskal Algorithm 사용 전" << "\n";
	for (int i = 0; i < graph->Cnum; i++) {
		std::cout << "component " << graph->comp[i].Cname << " 정보" << "\n";
		std::cout << "포함 정점 개수 : " << graph->comp[i].CVnum << "\n";
		std::cout << "MST 간선 개수 : " << graph->comp[i].CEnum << "\n";
		std::cout << "MST cost : " << graph->comp[i].mstCost << "\n";
		std::cout << "kscanned : " << graph->comp[i].kscanned << "\n";
	}

	graph->kruskal();

	std::cout << "Kruskal Algorithm 사용 후" << "\n";
	for (int i = 0; i < graph->Cnum; i++) {
		std::cout << "component " << graph->comp[i].Cname << " 정보" << "\n";
		std::cout << "포함 정점 개수 : " << graph->comp[i].CVnum << "\n";
		std::cout << "MST 간선 개수 : " << graph->comp[i].CEnum << "\n";
		std::cout << "MST cost : " << graph->comp[i].mstCost << "\n";
		std::cout << "kscanned : " << graph->comp[i].kscanned << "\n";
	}

	return 0;
}