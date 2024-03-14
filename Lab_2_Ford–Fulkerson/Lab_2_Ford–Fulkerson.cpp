#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct Edge
{
	int toV;
	int capacity;
};

const int DfsError = 0;

class FordFulkerson
{
public:
	FordFulkerson()
	{}

	void read()
	{
		size = 9;

		initEdges.resize(size + 1);
		initEdges[1] = { Edge{2, 67}, Edge{3, 29}, Edge{4, 37} };
		initEdges[2] = { Edge{5, 11}, Edge{3, 23} };
		initEdges[3] = { Edge{5, 15}, Edge{6, 9}, Edge{7, 13} };
		initEdges[4] = { Edge{5, 19}, Edge{3, 28}, Edge{7, 21} };
		initEdges[5] = { Edge{8, 26}, Edge{6, 35} };
		initEdges[6] = { Edge{8, 27}, Edge{7, 31} };
		initEdges[7] = { Edge{8, 39}, Edge{9, 15} };
		initEdges[8] = { Edge{9, 51} };

		cutS.resize(size + 1, false);
	}

	void run()
	{
		edges = initEdges;

		int flow = 0;
		while (true)
		{
			int flowBuff = dfs(1, INT_MAX);
			if (flowBuff == DfsError)
				break;

			flow += flowBuff;
			cout << "*** dfs = " << flowBuff << endl;
		}

		findCutS(1);
		cout << "*****" << endl;
		for (int i = 1; i < size + 1; i++)
			cout << i << " = " << (cutS[i] ? "S" : "T") << endl;
		cout << "*****" << endl;

		print();
		cout << "flow = " << flow << endl;
	}

	void print()
	{
		for (int i = 1; i <= size; i++)
		{
			cout << i << " -> ";
			for (int j = 0; j < edges[i].size(); j++)
			{
				if (j != 0)
					cout << ", ";
				cout << edges[i][j].toV << " {" << edges[i][j].capacity << "}";
			}
			cout << endl;
		}
	}

private:
	// Находит путь от первой вершины к последней
	// Находит минимальную пропускную способность на пути и, возвращаясь, вычитает её из пропускной способности рёбер
	int dfs(int currNode, int currMinCapacity)
	{
		if (currNode == size)
			return currMinCapacity;

		for (Edge& edge : edges[currNode])
		{
			if (edge.capacity == 0)
				continue;

			int minCapacity = dfs(edge.toV, min(currMinCapacity, edge.capacity));
			if (minCapacity != DfsError)
			{
				edge.capacity -= minCapacity;
				return minCapacity;
			}
		}
		return DfsError;
	}

	// Устанавливает, принадлежит ли каждая вершина к S-части минимального разреза
	void findCutS(int currNode)
	{
		if (cutS[currNode])
			return;

		cutS[currNode] = true;
		for (Edge edge : edges[currNode])
			if (edge.capacity != 0)
				findCutS(edge.toV);
	}

	// Пропускная способность минимального разреза
	int findCutCapacity()
	{
		int cutCapacity = 0;
		for (int node = 1; node <= size; node++)
			for (int i = 0; i < edges[node].size(); i++)
				if (cutS[node] != cutS[edges[node][i].toV])
					cutCapacity += initEdges[node][i].capacity;
		return cutCapacity;
	}

	vector<vector<Edge>> initEdges;
	int size;

	vector<vector<Edge>> edges;
	vector<bool> cutS;
};

int main()
{
	FordFulkerson ff;
	ff.read();
	ff.run();
}