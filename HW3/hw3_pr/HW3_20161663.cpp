#include <iostream>
#include <fstream>
#include <string>

#define CHECK_TIME_START QueryPerformanceFrequency ((_LARGE_INTEGER*)&freq); QueryPerformanceCounter((_LARGE_INTEGER*)&start)
#define CHECK_TIME_END(a) QueryPerformanceCounter((_LARGE_INTEGER*)&end); a=(float)((float) (end - start)/freq)
_int64 start, freq, end;
#define NONE	-1

class Set {	//	���� ��ȣ�� ������ disjoint-set�� ���� ���� 
public:
	int vname;		//	���� ��ȣ == set ��ȣ
	int rank;
	Set* parent;	//	�θ� ��带 ����Ű�� ������
	Set() {}
	Set(int _v) : vname(_v), rank(0), parent(NULL) {}
};

class DisjointSet {	//	Union-Find�� ���� disjoint-set
public:
	int numOfSets;	//	�κ� ���� ���� = ���� ����
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
	int flag;	//	�ش� edge�� mst�� ���� ��� � component�� mst�� edge���� ǥ��
				//	mst edge�� ���Ե��� ���� ��� NONE(-1)
	
	Edge() {}
	Edge(long long _n, int _v1, int _v2, long long _w) 
		: name(_n), v1(_v1), v2(_v2), weight(_w), flag(NONE) {}
};

class Node {
public:
	long long edgeName;	//	edge �̸��� ����
//	bool flag;			//	DFS �� Ȯ���ϱ� ���� flag
	Node* next;
	Node(long long _en) : edgeName(_en), next(NULL) {}
};

class StackNode {	//	DFS�� ���� ������ ���
public:
	int vertexName;	//	vertex �̸��� ����
	StackNode* next;
	StackNode(int _vn) : vertexName(_vn), next(NULL) {}
};

class Stack {	//	DFS�� ���� Stack
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
	int Cname;				//	component �̸�(0 ~ component ����)
	int CVnum;				//	component�� ���Ե� vertex ����
	long long CEnum;		//	component�� ���Ե� edge ���� (CVnum - 1�� �ɶ�����)
	long long kscanned;		//	MST ã�� �������� ó���� edge ����
	long long mstCost;		//	component�� MST cost
	Component() {}
	Component(int _n) : Cname(_n), CVnum(0), CEnum(0), kscanned(0), mstCost(0) {}
};

class Graph {
public:
	int Vnum;					//	vertex ����
	int Cnum;					//	component ����
	long long Enum;				//	edge ����
	long long Wmax;				//	�ִ� weight
	Node** adjList;				//	Adjacent list
	Node** next;				//	vertex�� edge �� ������ Ȯ���� edge ��ȣ ��� Node ���� �迭, DFS�� �̿�
	Edge* edgeArray;			//	edge ���� �迭
	int* components;			//	�� ������ � component�� ���ϴ� �� ǥ��
	Component* comp;			//	connected-components
	long long* heap;			//	edge ���� heap
	long long heapNum;			//	heap�� ����� ���� ����
	Stack* st;					//	dfs�� ���� ����
	DisjointSet* ds;			//	component�� vertices�� �̷���� disjoint-sets

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
		heap[ename + 1] = ename;	//	heap�� �ϴ� ������� ����
		node1->next = adjList[v1];
		node2->next = adjList[v2];
		adjList[v1] = node1;
		adjList[v2] = node2;
	}

	void dfs(int v) {	//	connected component�� ã�� ���� DFS
		Node* w = NULL;
	//	std::cout << "----------------------------------------------------------" << "\n";
	//	std::cout << "dfs(" << v << ") ����!" << "\n";

		components[v] = Cnum;	//	v�� �湮
	//	std::cout << "���� ���� " << v << "�� �湮(���ÿ� ����)" << "\n";
		st->push(v);

		while (!st->empty()) {
			int u = st->peek();
			int prev = u;
			w = next[u];

		//	if(w) std::cout << "1.������ ���� : e" << w->edgeName << "\n";
			for (; ; ) {
				if (!w) {
					int del;
					//st->pop();
					del = st->pop();
		//			std::cout << "���� " << del << "�� ���ÿ��� ����" << "\n";
					break;
				}

		//		if (w) std::cout << "������ ���� : e" << w->edgeName << "\n";
				int vnext = edgeArray[w->edgeName].v1 == prev ? 
					edgeArray[w->edgeName].v2 : edgeArray[w->edgeName].v1;	//	���� edge�� �� �ٸ� verte
		//		std::cout << "prev : " << prev << "\n";
		//		std::cout << "vnext : " << vnext << "\n";
			//	prev = vnext;
				if (components[vnext] == Cnum) {
		//			std::cout << "���� " << vnext << "�� �̹� �湮�� �����̹Ƿ� skip" << "\n";
					w = w->next;
					
					continue;	//	�̹� dfs�� Ž���� ���, �ٸ� edge�� �Ѿ
				}
				else {
		//			std::cout << "���� " << vnext << "�� �湮(���ÿ� ����)" << "\n";
					components[vnext] = Cnum;
					st->push(vnext);
					next[prev] = w->next;
					w = adjList[vnext];
					prev = vnext;
				}
			}
		}

	//	std::cout << "dfs(" << v << ") ��!" << "\n";
	//	std::cout << "----------------------------------------------------------" << "\n";
		Cnum++;	//	���� connected component ��ȣ & component ���� 1 ����
	}

	void findComponents() {		//	�� vertex�� ���� ������Ʈ�� ������
		for (int i = 0; i < Vnum; i++)
			next[i] = adjList[i];	//	next �迭 �ʱ�ȭ

		for (int i = 0; i < Vnum; i++) {
		//	std::cout << "���� "<<i<<"�� component?" << "\n";
			if (components[i] == NONE) {	//	���� ��� component�� ���ϴ��� ���� ����
		//		std::cout << "���� " << i << " DFS�� component ã��!" << "\n";
				dfs(i);
			}
		//	else {
		//		std::cout << "���� " << i << "�� �̹� component�� " << components[i] << "�� ������!" << "\n";
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
		ds = new DisjointSet(Vnum);	//	disjoint sets ��ü ����
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

	void makeHeap() {	//	min-heap ����
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
				if (comp[i].CEnum == comp[i].CVnum - (long long)1) end_flag++;	//	� component�� mst�� �ϼ��� ��� end_flag 1 ����
			if (end_flag == Cnum) break;	//	��� component�� mst�� �ϼ��� ���

			e = heapDelete();	//	O(log|E|)
			vf = edgeArray[e].v1;
			vr = edgeArray[e].v2;
			std::cout << "���õ� edge e" << edgeArray[e].name << ": (" << vf << ", " << vr << "), weight : " << edgeArray[e].weight << "\n";
//			std::cout << "find(" << vf << ") : " << ds->find(vf) << "\n";
			
			if (components[vf] != components[vr]) {
				std::cout << "���� ������ �� ������ ���� component�� �ٸ� �� ����! �ɰ��� ����" << "\n";
				return;
			}
			int compNum = components[vf];
			
			comp[compNum].kscanned++;	//	ó���� edge �� ����
			if (ds->find(vf) != ds->find(vr)) {	//	edge�� �� ������ ���� ���� ���տ� ������ �ʴ� ���
				edgeArray[e].flag = compNum;	//	�ش� component�� mst�� edge��
				std::cout << "edge e" << edgeArray[e].name << " = (" << vf << ", " << vr << ")�� component"<< comp[compNum].Cname<<"�� �߰�" << "\n";
				comp[compNum].CEnum++;	//	component�� ���� �߰�
				comp[compNum].mstCost += edgeArray[e].weight;	//	mst cost�� �߰�
				ds->unionByRank(vf, vr);	//	�� ������ ��ħ
			}
			else {	//	edge�� �� ������ �̹� ���� ���տ� ���� ��� �߰��ϸ� cycle�� �����ǹǷ� �߰� x
			//	edgeArray[e].flag = NONE;
				std::cout << "�� ���� " << vf << "�� " << vr << "�� �̹� ���� ���տ� ����" << "\n";
				std::cout << "edge e" << edgeArray[e].name << " = (" << vf << ", " << vr << ")�� component" << comp[compNum].Cname << "�� �߰��� �� ����" << "\n";
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
	std::cout << "connected-components ���� : " << graph->Cnum << "\n";
	graph->makeComponent();
	std::cout << "Kruskal Algorithm ��� ��" << "\n";
	for (int i = 0; i < graph->Cnum; i++) {
		std::cout << "component " << graph->comp[i].Cname << " ����" << "\n";
		std::cout << "���� ���� ���� : " << graph->comp[i].CVnum << "\n";
		std::cout << "MST ���� ���� : " << graph->comp[i].CEnum << "\n";
		std::cout << "MST cost : " << graph->comp[i].mstCost << "\n";
		std::cout << "kscanned : " << graph->comp[i].kscanned << "\n";
	}

	graph->kruskal();

	std::cout << "Kruskal Algorithm ��� ��" << "\n";
	for (int i = 0; i < graph->Cnum; i++) {
		std::cout << "component " << graph->comp[i].Cname << " ����" << "\n";
		std::cout << "���� ���� ���� : " << graph->comp[i].CVnum << "\n";
		std::cout << "MST ���� ���� : " << graph->comp[i].CEnum << "\n";
		std::cout << "MST cost : " << graph->comp[i].mstCost << "\n";
		std::cout << "kscanned : " << graph->comp[i].kscanned << "\n";
	}

	return 0;
}