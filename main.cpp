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

// returns the current distance of the vertex a
int getDist(vertex a) {
  return dist[a.first][a.second];
}

// sets the distance of the vertex a to newDist
void setDist(vertex a, int newDist) {
  dist[a.first][a.second] = newDist;
}

// gets the current parent vertex of the vertex a
vertex getPar(vertex a) {
  return par[a.first][a.second];
}

// sets the parent vertex of the vertex a to newPar
void setPar(vertex a, vertex newPar) {
  par[a.first][a.second] = newPar;
}

// runs a dijkstra algorithm over all vertices (all town & coin configuration),
// determining both the distances of shortest paths from the startVertex to other vertices
// and the parent vertex of each vertex in such shortest path configuration
// parents are set in the par array in the form par[townNo][coinConfiguration]
// distances are ser in the dist array in the form dist[townNo][coinConfiguration]
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
        // there are enough coins to pass the thieves on this road
        if (checkCoins(currVertex.second, currEdge.second.second)) {
          vertex nextVertex = make_pair(currEdge.first, combineCoins(currVertex.second, coinsInTown[currEdge.first]));
          int newDist = getDist(currVertex) + currEdge.second.first;
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
// (checks each coin configuration for that town)
vertex getSolution(short endTown) {
  // set the coin configuration to 0 initally.
  short minSolutionCoins = 0;
  // start from coin cofiguration (i) = 1, since 0 is the initial solution.
  for (short i = 1; i < (1 << maxThieves); ++i) {
    // currDist is the current solution
    int currDist = getDist(make_pair(endTown, minSolutionCoins));
    // nextDist is the next one
    int nextDist = getDist(make_pair(endTown, i));
    // dist = -1 means it is not reachable
    if (currDist == -1 || (nextDist != -1 && nextDist < currDist)) {
      // if currDist is -1, current solution is not a real solution, set it to i
      // if neither of them are -1 and nextDist is smaller than currDist, i is a betterSolution than the current one
      // so, set minSolutionCoins (current solution) to i
      minSolutionCoins = i;
    }
  }
  // return the vertex with the given town and the coin configuration for the best solution.
  return make_pair(endTown, minSolutionCoins);
}

// main method, takes the file names as args, (input & output)
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

  // for all jewelers
  while (k--) {
    short townId;
    int q;
    // q: number of coins of the current jeweler
    inFile >> townId >> q;
    // for each coin
    while (q--) {
      short coinType;
      inFile >> coinType;
      // add the coin to the coins in that town
      coinsInTown[townId] = combineCoins (coinsInTown[townId], (short)1 << (coinType - 1));
    }
  }

  // for each road
  while (m--) {
    short begTown, endTown, thievesBitmask = 0;
    int dist, numberOfThieves;
    inFile >> begTown >> endTown >> dist >> numberOfThieves;
    // for each thief in town
    while (numberOfThieves--) {
      short thieveType;
      inFile >> thieveType;
      // add the type of thief to the thievesBitmask for that road
      // that is the bitmask of the coins required to pass this road
      thievesBitmask = combineCoins(thievesBitmask, (short)1 << (thieveType - 1));
    }
    // since the towns create an undirected graph, push two edges in opposing directions.
    edges[begTown].push_back(make_pair(endTown, make_pair(dist, thievesBitmask)));
    edges[endTown].push_back(make_pair(begTown, make_pair(dist, thievesBitmask)));
  }

  // starting vertex is the first town, and all the coins in the first town.
  vertex startVertex = make_pair(1, coinsInTown[1]);

  // use the dijkstra algorithm and decide distances and parent vertices of each vertex.
  dijkstra(startVertex);

  // get the result vertex (the one with the minimal distance) in the n'th town (last)
  vertex resultVertex = getSolution(n);

  // if the resultVertex has a distance -1, that means, the last town is not reachable -> print -1 and quit
  // else print the path to the resultVertex
  if (getDist(resultVertex) == -1) {
    // no possible paths
    outFile << "-1" << endl;
  } else {
    // print the best path
    // a container to hold the vertices of the path in reversed order.
    vector<vertex> reversedPath;

    while (resultVertex != startVertex) {
      // push the current vertex starting from the resultVertex until startVertex is reached
      reversedPath.push_back(resultVertex);
      resultVertex = getPar(resultVertex);
    }

    // push the startVertex also
    reversedPath.push_back(resultVertex);

    // print the town id of each vertex in the path
    // print in reverse direction because the container holds the vertices in reverse direction.
    for (int i = reversedPath.size() - 1; i >= 0; --i) {
      // print only the town id (.first) of each vertex
      outFile << reversedPath[i].first << " ";
    }
    outFile << endl;
  }
  // output is completed

  // close filestreams
  inFile.close();
  outFile.close();
}