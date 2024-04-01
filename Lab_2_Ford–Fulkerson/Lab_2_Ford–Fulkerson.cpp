#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

const int DfsError = 0;

class FordFulkerson
{
public:
	void readEdges(istream& fin)
	{
		initEdges = { {} };

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
			{
				if (capacity <= 0)
				{
					cout << "Edge capacity must be positive" << endl;
					exit(1);
				}

				initEdges.back()[toV] += capacity;
			}
		}

		initEdges.push_back({});

		size = initEdges.size() - 1;

		for (int v = 1; v <= size; v++)
		{
			for (pair<int, int> edge : initEdges[v])
			{
				int toV = edge.first;
				int capacity = edge.second;

				if (!(1 <= toV && toV <= size))
				{
					cout << "Couldn't find vertex with number " << toV << endl;
					exit(1);
				}
			}
		}

		helpBool.resize(size + 1, false);
	}

	void run()
	{
		edges = initEdges;

		backwardEdges.clear();
		backwardEdges.resize(size + 1);

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

		findCutS();
		for (int i = 1; i <= size; i++)
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
	// Finds the path from the first vertex to the last
	// Finds the minimum bandwidth on the path and, returning, subtracts it from the bandwidth of the edges
	int dfs()
	{
		resetHelpBool();
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
					int toV = edge.first;
					int capacity = edge.second;

					if (capacity != 0)
					{
						int minCapacity = dfsHelp(toV, min(currMinCapacity, capacity));
						if (minCapacity != DfsError)
						{
							edges[toV][currV] += minCapacity;
							backwardEdges[currV][toV] -= minCapacity;
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

	void findCutS()
	{
		resetHelpBool();
		findCutSHelp(1);
	}

	// Устанавливает, принадлежит ли каждая вершина к S-части минимального разреза, в helpBool
	// Sets helpBool to whether each vertex belongs to the S-part of the minimum cut
	void findCutSHelp(int currNode)
	{
		if (helpBool[currNode])
			return;
		helpBool[currNode] = true;

		for (pair<int, int> edge : edges[currNode])
		{
			int toV = edge.first;
			int capacity = edge.second;

			if (capacity != 0)
				findCutSHelp(toV);
		}
	}

	// Пропускная способность минимального разреза
	// Minimum cut capacity
	int findCutCapacity()
	{
		int cutCapacity = 0;
		for (int v = 1; v <= size; v++)
			for (pair<int, int> edge : edges[v])
			{
				int toV = edge.first;
				if (edges[v][toV] != 0)
					continue;
				int initCapacity = initEdges[v][toV];

				bool fromS = helpBool[v];
				bool toS = helpBool[toV];

				if (fromS && !toS)
					cutCapacity += initCapacity;
				else if (!fromS && toS)
					cutCapacity -= initCapacity;
			}

		return cutCapacity;
	}

	void resetHelpBool()
	{
		for (int i = 1; i <= size; i++)
			helpBool[i] = false;
	}

	vector<map<int, int>> initEdges;
	int size = 0;

	vector<map<int, int>> edges;
	vector<map<int, int>> backwardEdges;
	vector<bool> helpBool;
};

int main()
{
	ifstream fin("fin2.txt");
	if (!fin)
	{
		cout << "Couldn't find the file" << endl;
		return 0;
	}

	FordFulkerson ff;
	ff.readEdges(fin);
	ff.run();
}