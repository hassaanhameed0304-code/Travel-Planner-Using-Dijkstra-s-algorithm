#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <queue>

using namespace std;

class TravelPlanner {
private:
    // Replaces tuple-based priority-queue entries.
    struct State {
        double distance;
        int hops;
        int node;
    };

    // Priority: smaller distance first, then fewer road segments, then node ID.
    struct StateCompare {
        bool operator()(const State& a, const State& b) const {
            if (a.distance != b.distance) return a.distance > b.distance;
            if (a.hops != b.hops) return a.hops > b.hops;
            return a.node > b.node;
        }
    };

    struct Route {
        int from;
        int to;
        double distance;
    };

    // Stores one neighbouring location and the road distance to it.
    // A custom edge record stores both values, so no extra header is needed.
    struct Edge {
        int destination;
        double distance;
    };

    struct PathResult {
        bool found = false;
        double totalDistance = 0.0;
        int totalHops = 0;
        vector<int> path;
    };

    vector<string> locations;
    vector<Route> routes;
    vector<vector<Edge>> adjacencyList;

    // Singleton: only one TravelPlanner object is used by the program.
    TravelPlanner() {
        locations = {
            "Mazar-e-Quaid",
            "Empress Market",
            "Frere Hall",
            "National Museum of Pakistan",
            "Quaid-e-Azam House Museum",
            "Pakistan Maritime Museum",
            "PAF Museum",
            "Mohatta Palace Museum",
            "Abdullah Shah Ghazi Shrine",
            "Clifton Beach (Sea View)",
            "Port Grand"
        };

        // Illustrative sample road-link distances for the Karachi DSA graph.
        // These weights are for algorithm demonstration, not verified live
        // navigation distances. Replace them with checked road distances if
        // the project is later used for real-world trip planning.
        routes = {
            {0, 1, 2.7},
            {0, 3, 2.0},
            {0, 4, 3.4},
            {0, 10, 5.0},
            {1, 2, 1.5},
            {1, 3, 2.1},
            {1, 10, 4.2},
            {2, 3, 1.8},
            {2, 4, 2.0},
            {2, 7, 7.0},
            {3, 4, 2.7},
            {3, 10, 5.0},
            {4, 6, 7.5},
            {4, 7, 7.0},
            {5, 6, 4.0},
            {5, 7, 11.0},
            {5, 9, 12.5},
            {6, 7, 10.0},
            {6, 9, 12.0},
            {7, 8, 2.3},
            {7, 9, 4.2},
            {8, 9, 2.0},
            {8, 10, 11.0},
            {9, 10, 13.0}
        };

        adjacencyList.resize(locations.size());
        buildGraph();
    }

    void buildGraph() {
        // The routes are treated as two-way roads, so each edge is added
        // in both directions in the adjacency list.
        for (const Route& route : routes) {
            adjacencyList[route.from].push_back({route.to, route.distance});
            adjacencyList[route.to].push_back({route.from, route.distance});
        }
    }

    TravelPlanner(const TravelPlanner&) = delete;
    TravelPlanner& operator=(const TravelPlanner&) = delete;

public:
    static TravelPlanner& getInstance() {
        static TravelPlanner instance;
        return instance;
    }

    // Small public interface used by the browser's WebAssembly bridge.
    int locationCount() const {
        return static_cast<int>(locations.size());
    }

    const string& locationName(int index) const {
        return locations[index];
    }

    double segmentDistance(int from, int to) const {
        return getRoadDistance(from, to);
    }

    // Copies the calculated path into a plain integer array for JavaScript.
    // Returns the number of path nodes, 0 if no route exists, or -1 for bad input.
    int exportShortestRoute(int source, int destination, int* pathBuffer,
                            int maxPath, double* totalDistance,
                            int* totalHops) const {
        if (source < 0 || destination < 0 ||
            source >= locationCount() || destination >= locationCount() ||
            pathBuffer == nullptr || totalDistance == nullptr ||
            totalHops == nullptr) {
            return -1;
        }

        PathResult result = shortestPathWithMinimumStops(source, destination);
        if (!result.found) return 0;
        if (static_cast<int>(result.path.size()) > maxPath) return -1;

        for (size_t i = 0; i < result.path.size(); ++i) {
            pathBuffer[i] = result.path[i];
        }
        *totalDistance = result.totalDistance;
        *totalHops = result.totalHops;
        return static_cast<int>(result.path.size());
    }

    // Prints a non-negative distance to one decimal place without <iomanip>.
    void printOneDecimal(double value) const {
        long long tenths = static_cast<long long>(value * 10.0 + 0.5);
        cout << tenths / 10 << "." << tenths % 10;
    }

    void displayLocations() const {
        cout << "\n========== KARACHI TOURIST LOCATIONS ==========\n";
        for (size_t i = 0; i < locations.size(); ++i) {
            cout << (i + 1) << ". " << locations[i] << '\n';
        }
    }

    void displayDirectRoutes() const {
        cout << "\n================ DIRECT ROUTES ===================\n";
        for (const Route& route : routes) {
            cout << locations[route.from] << " <-> "
                 << locations[route.to] << " : ";
            printOneDecimal(route.distance);
            cout << " km\n";
        }
        cout << "\nNote: Distances are illustrative sample values for the DSA demonstration, not verified navigation distances.\n";
    }

    int readLocationChoice(const string& prompt) const {
        int choice;
        cout << prompt;
        cin >> choice;

        while (cin.fail() || choice < 1 || choice > static_cast<int>(locations.size())) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice. Enter a number from 1 to "
                 << locations.size() << ": ";
            cin >> choice;
        }
        return choice - 1;
    }

    double getRoadDistance(int from, int to) const {
        for (const Edge& neighbour : adjacencyList[from]) {
            if (neighbour.destination == to) {
                return neighbour.distance;
            }
        }
        return -1.0; // This should not occur for two consecutive path nodes.
    }

    PathResult shortestPathWithMinimumStops(int source, int destination) const {
        const double INF = 1e18;
        const double EPS = 1e-9;
        const int n = static_cast<int>(locations.size());

        vector<double> distance(n, INF);
        vector<int> hops(n, 1000000000);
        vector<int> parent(n, -1);

        // Priority queue selects shortest distance, then fewer road segments.
        priority_queue<State, vector<State>, StateCompare> pq;

        distance[source] = 0.0;
        hops[source] = 0;
        parent[source] = source;
        pq.push({0.0, 0, source});

        while (!pq.empty()) {
            State current = pq.top();
            pq.pop();

            double currentDistance = current.distance;
            int currentHops = current.hops;
            int u = current.node;

            // Ignore an outdated priority-queue entry (without fabs()).
            if (currentDistance > distance[u] + EPS ||
                distance[u] > currentDistance + EPS ||
                currentHops != hops[u]) {
                continue;
            }

            if (u == destination) {
                break;
            }

            for (const Edge& neighbour : adjacencyList[u]) {
                int v = neighbour.destination;
                double edgeDistance = neighbour.distance;
                double newDistance = currentDistance + edgeDistance;
                int newHops = currentHops + 1;

                bool shorterDistance = newDistance < distance[v] - EPS;
                bool sameDistance =
                    newDistance <= distance[v] + EPS &&
                    distance[v] <= newDistance + EPS;
                bool sameDistanceFewerHops = sameDistance && newHops < hops[v];

                if (shorterDistance || sameDistanceFewerHops) {
                    distance[v] = newDistance;
                    hops[v] = newHops;
                    parent[v] = u;
                    pq.push({newDistance, newHops, v});
                }
            }
        }

        PathResult result;
        if (distance[destination] >= INF / 2) {
            return result;
        }

        result.found = true;
        result.totalDistance = distance[destination];
        result.totalHops = hops[destination];

        // Reconstruct the route by following parent links backwards.
        int current = destination;
        while (current != source) {
            if (current == -1 || parent[current] == -1) {
                result.found = false;
                result.path.clear();
                return result;
            }
            result.path.push_back(current);
            current = parent[current];
        }
        result.path.push_back(source);
        reverse(result.path.begin(), result.path.end());

        return result;
    }

    void planJourney() const {
        cout << "\n========== WHERE WOULD YOU LIKE TO START FROM? ==========" << '\n';
        displayLocations();
        int source = readLocationChoice("\nSelect your starting location (enter its number): ");

        cout << "\n================ WHERE WOULD YOU LIKE TO GO? ================" << '\n';
        displayLocations();
        int destination = readLocationChoice("\nSelect your destination (enter its number): ");

        PathResult result = shortestPathWithMinimumStops(source, destination);

        cout << "\n================ SHORTEST ROUTE RESULT ================\n";
        if (!result.found) {
            cout << "No route is available between these locations.\n";
            return;
        }

        cout << "Shortest route found!\n\n";
        cout << "START       : " << locations[source] << '\n';
        cout << "DESTINATION : " << locations[destination] << '\n';
        cout << "TOTAL SHORTEST DISTANCE: ";
        printOneDecimal(result.totalDistance);
        cout << " km\n";
        cout << "INTERMEDIATE STOPS     : "
             << max(0, result.totalHops - 1) << '\n';
        cout << "ROAD SEGMENTS          : " << result.totalHops << '\n';

        cout << "\nRECOMMENDED SHORTEST ROUTE (in travel order):\n";
        for (size_t i = 0; i < result.path.size(); ++i) {
            cout << (i + 1) << ". " << locations[result.path[i]];
            if (i == 0) {
                cout << "  [START]";
            } else if (i + 1 == result.path.size()) {
                cout << "  [DESTINATION]";
            } else {
                cout << "  [STOP]";
            }
            if (i + 1 < result.path.size()) {
                cout << "\n       |\n       v\n";
            }
        }

        cout << "\n\nSTEP-BY-STEP DISTANCE BREAKDOWN:\n";
        for (size_t i = 0; i + 1 < result.path.size(); ++i) {
            int from = result.path[i];
            int to = result.path[i + 1];
            double legDistance = getRoadDistance(from, to);
            cout << (i + 1) << ". " << locations[from]
                 << " -> " << locations[to] << " = ";
            printOneDecimal(legDistance);
            cout << " km\n";
        }

        cout << "-----------------------------------------------\n";
        cout << "TOTAL DISTANCE: ";
        printOneDecimal(result.totalDistance);
        cout << " km\n";
        cout << "Note: The planner finds the shortest route in its sample road graph.\n";
        cout << "Road distances are illustrative values, not live navigation data.\n";
    }

    void run() const {
        int choice;
        do {
            cout << "\n\n**************************************************\n";
            cout << "       KARACHI SMART TRAVEL PLANNER\n";
            cout << "**************************************************\n";
            cout << "1. View all tourist locations\n";
            cout << "2. View all direct routes\n";
            cout << "3. Find the shortest route between two locations\n";
            cout << "4. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                choice = 0;
            }

            switch (choice) {
                case 1:
                    displayLocations();
                    break;
                case 2:
                    displayDirectRoutes();
                    break;
                case 3:
                    planJourney();
                    break;
                case 4:
                    cout << "Thank you for using the Travel Planner!\n";
                    break;
                default:
                    cout << "Invalid option. Please select 1, 2, 3, or 4.\n";
            }
        } while (choice != 4);
    }
};

#ifndef TRAVEL_PLANNER_NO_MAIN
int main() {
    TravelPlanner& planner = TravelPlanner::getInstance();
    planner.run();
    return 0;
}
#endif
