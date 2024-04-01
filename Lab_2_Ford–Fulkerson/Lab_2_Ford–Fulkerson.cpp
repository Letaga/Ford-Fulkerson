#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

//struct Edge
//{
//	int toV;
//	int capacity;
//};

template <class T>
ostream& operator << (ostream& s, const vector<T>& v)
{
	s << "{";
	for (int i = 0; i < v.size(); i++)
	{
		if (i != 0)
			s << ", ";
		s << v[i];
	}
	return s << "}";
}

const int DfsError = 0;

class FordFulkerson
{
public:
	void readEdges(istream& fin)
	{
		initEdges = {{}};
	
		while (true)
		{
			string str;
			if (!getline(fin, str))
				break;

			initEdges.push_back({});
			stringstream ss;
			ss << str;
			
			int toV;
			int capacity;
			while (ss >> toV >> capacity)
				initEdges.back()[toV] += capacity;
		}

		initEdges.push_back({});

		size = initEdges.size() - 1;

		helpBool.resize(size + 1, false);
	}

	void run()
	{
		edges = initEdges;

		backwardEdges.clear();
		backwardEdges.resize(size+1);

		cout << "The given graph\n";
		print();

		int flow = 0;
		while (true)
		{
			int flowBuff = dfs();
			if (flowBuff == DfsError)
				break;

			flow += flowBuff;
		}

		for (int i = 1; i < size + 1; i++)
			helpBool[i] = false;
		findCutS(1);
		for (int i = 1; i < size + 1; i++)
			cout << i << " = " << (helpBool[i] ? "S" : "T") << endl;
		cout << "Cutting capacity " << findCutCapacity() << endl;

		print();
		cout << "flow = " << flow << endl;
	}

	void print()
	{
		for (int v = 1; v <= size; v++)
		{
			cout << v << " -> ";
			for (pair<int, int> edge : edges[v])
			{
				cout << edge.first << " {" << edge.second << ", " << backwardEdges[edge.first][v] << "}  ";
			}
			cout << endl;
		}
	}

private:
	// Находит путь от первой вершины к последней
	// Находит минимальную пропускную способность на пути и, возвращаясь, вычитает её из пропускной способности рёбер
	//Finds the path from the first vertex to the last
	// Finds the minimum bandwidth on the path and, returning, subtracts it from the bandwidth of the edges
	int dfs()
	{
		for (int i = 1; i < size + 1; i++)
			helpBool[i] = false;
		return dfsHelp(1, INT_MAX);
	}
	int dfsHelp(int currV, int currMinCapacity)
	{
		if (currV == size)
			return currMinCapacity;

		if (helpBool[currV])
			return DfsError;
		helpBool[currV] = true;

		for (pair<int, int> edge : edges[currV])
		{
			int toV = edge.first;
			int capacity = edge.second;

			if (capacity == 0)
			{
				// Пробуем пройти через обратное ребро
				for (pair<int, int> edge : backwardEdges[currV])
				{
					if (edge.second != 0)
					{
						int minCapacity = dfsHelp(edge.first, min(currMinCapacity, edge.second));
						if (minCapacity != DfsError)
						{
							edges[edge.first][currV] += minCapacity;
							backwardEdges[currV][edge.first] -= minCapacity;
							return minCapacity;
						}
					}
				}
				continue;
			}

			int minCapacity = dfsHelp(toV, min(currMinCapacity, capacity));
			if (minCapacity != DfsError)
			{
				edges[currV][toV] -= minCapacity;
				backwardEdges[toV][currV] += minCapacity;
				return minCapacity;
			}
		}
		return DfsError;
	}

	// Устанавливает, принадлежит ли каждая вершина к S-части минимального разреза
	// Sets whether each vertex belongs to the S-part of the minimum cut
	void findCutS(int currNode)
	{
		if (helpBool[currNode])
			return;

		helpBool[currNode] = true;
		for (pair <int, int> edge: edges[currNode])
			if (edge.second != 0)
				findCutS(edge.first);
	}

	// Пропускная способность минимального разреза
	// Minimum cut capacity
	int findCutCapacity()
	{
		int cutCapacity = 0;
		for (int v = 1; v <= size; v++)
			for (pair <int, int> edge : edges[v])
			{
				int toV = edge.first;

				bool fromS = helpBool[v];
				bool toS = helpBool[toV];

				if (fromS && !toS)
					cutCapacity += initEdges[v][toV];
				else if (!fromS && toS)
					cutCapacity -= initEdges[v][toV];
			}

		return cutCapacity;
	}

	vector<map<int, int>> initEdges;
	int size = 0;

	vector<map<int, int>> edges;
	vector<map<int, int>> backwardEdges;
	vector<bool> helpBool; // Относится ли к S части и посещена ли вершина, в зависимости от случая
};

int main()
{
	FordFulkerson ff;
	ifstream fin("fin2.txt");
	if (!fin)
	{
		cout << "Can't find the file" << endl;
		return 0;
	}

	ff.readEdges(fin);
	ff.run();
}