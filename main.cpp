//
// Created by corupta on 06.12.2017.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string.h>

using namespace std;

#define maxThieves 13
// 2^maxThieves = 2^13 =~ 8 thousands => fits the variable type short, so that type is used for all the bitmasking.
// 2 * maxN * maxDistPerEdge = 2 * 2000 * 500 =~ 30 millions. So, any distance fits the int type.
#define maxN 2001 // not 2000, because town ids start from 1

// returns the bitmask of the combined coins, (returns the bitwise or of the two bitmasks)
short combineCoins(short coinA, short coinB) {
  return coinA | coinB;
}

// returns true if coins are enough for the thieves. (both coins and thieves are bitmasks)
bool checkCoins(short coins, short thieves) {
  // bitwise and of coins bitmask and thieves bitmask return a bitmask of the owned coins for the thieves.
  // if that bitmask is equal to the thieves, that means there are sufficient coins for all thieves. So, returns true.
  return ((coins & thieves) == thieves);
}

typedef pair <short, short> vertex; // a vertex is in form<short, short> = <townId, coins>

int dist[maxN][1 << maxThieves]; // all distances for each town-coin(vertex) configuration 2000 * 2^13 ~= 16 million => (int = 4 bytes) => 64 MB
vertex par[maxN][1 << maxThieves]; // for each town-coin(vertex) the parent (preceeding) vertex in the shortest path ** (vertex = 2 shorts = 4 bytes) => 64 MB
vector < pair<short, pair<int, short> > > edges[maxN]; // all incident edges of i'th town is edges[i] = <nextTown, <distance, thieves> >
short coinsInTown[maxN]; // bitmask of the coins sold by jewelers in each townId

int getDist(vertex a) {
  return dist[a.first][a.second];
}

void setDist(vertex a, int newDist) {
  dist[a.first][a.second] = newDist;
}

vertex getPar(vertex a) {
  return par[a.first][a.second];
}

void setPar(vertex a, vertex newPar) {
  par[a.first][a.second] = newPar;
}

void dijkstra(vertex startVertex) {
  // below is a min heap whose every element is in the form <distance, vertex>
  // so that in every step the vertex with the minimum distance can be gotten. (ordered by distances)
  priority_queue <pair <int, vertex>, vector <pair <int, vertex> >, std::greater<pair <int, vertex> > > vertexQueue;
  memset(dist,-1,sizeof(dist)); // set -1 to every element of dist - not reached vertex

  setDist(startVertex, 0);
  vertexQueue.push(make_pair(0, startVertex));
  while (!vertexQueue.empty()) {
    vertex currVertex = vertexQueue.top().second;
    int currDist = vertexQueue.top().first;
    vertexQueue.pop();
    // do the following only if this is the first time that vertex is being taken from the queue
    // for example, if there are more than one ways to get to vertex a, after the shorter one processed, the others are ignored
    // there can't be duplicate length, vertex pairs pushed in the queue, because a vertex is pushed only if its distance is shortened by an edge.
    if (currDist == getDist(currVertex)) {
      for (int i = 0; i < edges[currVertex.first].size(); ++i) {
        // current edge in the form <nextTown, <distance, thieves> >
        pair<short, pair<int, short> > currEdge = edges[currVertex.first][i];
        // currVertex.second = current coins,
        // currEdge.second.second, is the thieves on the current edge
        if (checkCoins(currVertex.second, currEdge.second.second)) {
          vertex nextVertex = make_pair(currEdge.first, combineCoins(currVertex.second, coinsInTown[currEdge.first]));
          int newDist = getDist(currVertex) + currEdge.first;
          // if nextVertex is not reached or it can be reached with a shorter distance
          if (getDist(nextVertex) == -1 || getDist(nextVertex) > newDist) {
            // set the preceeding vertex of nextVertex, currVertex
            setPar(nextVertex, currVertex);
            // set the distance of the nextVertex
            setDist(nextVertex, newDist);
            // push the nextVertex to the queue.
            vertexQueue.push(make_pair(newDist, nextVertex));
          }
        }
      }
    }
  }
}

// returns a reachable vertex in the given town whose distance is minimized
vertex getSolution(short endTown) {
  short minSolutionCoins = 0;
  for (short i = 0; i < (1 << maxThieves); ++i) {
    int currDist = getDist(make_pair(endTown, minSolutionCoins));
    int nextDist = getDist(make_pair(endTown, i));
    // dist = -1 means it is not reachable
    if (currDist == -1 || (nextDist != -1 && nextDist < currDist)) {
      minSolutionCoins = i;
    }
  }
  return make_pair(endTown, minSolutionCoins);
}

int main(int argc, char *argv[]) {
  // to read & write faster
  ios_base::sync_with_stdio(false);

  // check argc & argv
  if (argc != 3) {
    cerr << "Run the code with the following command: ./project3 [input_file] [output_file]" << endl;
    return 1;
  }

  // open filestreams
  ifstream inFile(argv[1]);
  ofstream outFile(argv[2]);

  short n, p;
  int m, k;
  // n: number of towns, m: number of roads, p: number of thief kinds, k: number of jewelers
  inFile >> n >> m >> p >> k;
  // cout << "read: " << n << " " << m << " " << k << endl;
  while (k--) {
    short townId;
    int q;
    inFile >> townId >> q;
    while (q--) {
      short coinType;
      inFile >> coinType;
      coinsInTown[townId] = combineCoins (coinsInTown[townId], (short)1 << (coinType - 1));
    }
  }

  while (m--) {
    short begTown, endTown, thievesBitmask = 0;
    int dist, numberOfThieves;
    inFile >> begTown >> endTown >> dist >> numberOfThieves;
    while (numberOfThieves--) {
      short thieveType;
      inFile >> thieveType;
      thievesBitmask = combineCoins(thievesBitmask, (short)1 << (thieveType - 1));
    }
    edges[begTown].push_back(make_pair(endTown, make_pair(dist, thievesBitmask)));
    edges[endTown].push_back(make_pair(begTown, make_pair(dist, thievesBitmask)));
  }

  vertex startVertex = make_pair(1, coinsInTown[1]);
  dijkstra(startVertex);
  vertex resultVertex = getSolution(n);

  /*
  cout << "dist: " << getDist(resultVertex) << endl;

  for (int i = 1; i <= n; ++i) {
    for (int j = 0; j < (1<<p); ++j) {
      cout << getDist(make_pair(i,j)) << " ";
    }
    cout << endl;
  }
   */

  if (getDist(resultVertex) == -1) {
    // no possible paths
    outFile << "-1" << endl;
  } else {
    vector<vertex> reversedPath;

    while (resultVertex != startVertex) {
      reversedPath.push_back(resultVertex);
      resultVertex = getPar(resultVertex);
    }
    reversedPath.push_back(resultVertex);

    for (int i = reversedPath.size() - 1; i >= 0; --i) {
      outFile << reversedPath[i].first << " ";
    }
    outFile << endl;
  }

  // close filestreams
  inFile.close();
  outFile.close();
}

/* SAMPLE INPUT

 6 7 4 2
 2 1 2
 3 2 1 3
 1 2 2 0
 2 3 9 0
 1 4 2 1 2
 2 5 3 0
 4 5 5 2 2 3
 4 6 18 0
 5 6 3 2 1 2

 OUTPUT

 1 2 1 4 6

*/