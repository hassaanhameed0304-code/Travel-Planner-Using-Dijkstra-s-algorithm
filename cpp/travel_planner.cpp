#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <queue>

using namespace std;

class TravelPlanner {
private:

    struct State {
        double distance;
        int hops;
        int node;
    };

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
        for (const Route& route : routes) {
            if (route.from < 0 || route.to < 0 ||
                route.from >= static_cast<int>(locations.size()) ||
                route.to >= static_cast<int>(locations.size()) ||
                route.from == route.to || !(route.distance >= 0.0) ||
                route.distance >= 1e18) {
                continue;
            }

            adjacencyList[route.from].push_back({route.to, route.distance});
            adjacencyList[route.to].push_back({route.from, route.distance});
        }
    }

    bool parseInteger(const string& input, int& value) const {
        size_t position = 0;

        while (position < input.size() &&
               (input[position] == ' ' || input[position] == '\t' ||
                input[position] == '\r')) {
            ++position;
        }

        bool negative = false;
        if (position < input.size() &&
            (input[position] == '-' || input[position] == '+')) {
            negative = input[position] == '-';
            ++position;
        }

        if (position >= input.size() ||
            input[position] < '0' || input[position] > '9') {
            return false;
        }

        long long number = 0;
        long long limit = negative ? 2147483648LL : 2147483647LL;

        while (position < input.size() &&
               input[position] >= '0' && input[position] <= '9') {
            int digit = input[position] - '0';
            if (number > (limit - digit) / 10) {
                return false;
            }
            number = number * 10 + digit;
            ++position;
        }

        while (position < input.size() &&
               (input[position] == ' ' || input[position] == '\t' ||
                input[position] == '\r')) {
            ++position;
        }

        if (position != input.size()) {
            return false;
        }

        value = static_cast<int>(negative ? -number : number);
        return true;
    }

    bool readIntegerInput(const string& prompt, int& value) const {
        string input;

        while (true) {
            cout << prompt;
            if (!getline(cin, input)) {
                return false;
            }

            if (parseInteger(input, value)) {
                return true;
            }

            cout << "Invalid input. Please enter a whole number.\n";
        }
    }

    TravelPlanner(const TravelPlanner&) = delete;
    TravelPlanner& operator=(const TravelPlanner&) = delete;

public:
    static TravelPlanner& getInstance() {
        static TravelPlanner instance;
        return instance;
    }

    int locationCount() const {
        return static_cast<int>(locations.size());
    }

    const string& locationName(int index) const {
        static const string emptyName = "";
        if (index < 0 || index >= locationCount()) {
            return emptyName;
        }
        return locations[index];
    }

    double segmentDistance(int from, int to) const {
        return getRoadDistance(from, to);
    }

    int exportShortestRoute(int source, int destination, int* pathBuffer,
                            int maxPath, double* totalDistance,
                            int* totalHops) const {
        if (totalDistance != nullptr) *totalDistance = 0.0;
        if (totalHops != nullptr) *totalHops = 0;

        if (source < 0 || destination < 0 ||
            source >= locationCount() || destination >= locationCount() ||
            pathBuffer == nullptr || maxPath < 1 ||
            totalDistance == nullptr || totalHops == nullptr) {
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

    void printOneDecimal(double value) const {
        if (value < 0.0) {
            cout << "N/A";
            return;
        }

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

    bool readLocationChoice(const string& prompt, int& selectedIndex) const {
        int choice;

        while (true) {
            if (!readIntegerInput(prompt, choice)) {
                return false;
            }

            if (choice >= 1 && choice <= static_cast<int>(locations.size())) {
                selectedIndex = choice - 1;
                return true;
            }

            cout << "Invalid choice. Enter a number from 1 to "
                 << locations.size() << ".\n";
        }
    }

    double getRoadDistance(int from, int to) const {
        if (from < 0 || to < 0 ||
            from >= static_cast<int>(adjacencyList.size()) ||
            to >= static_cast<int>(adjacencyList.size())) {
            return -1.0;
        }

        if (from == to) {
            return 0.0;
        }

        for (const Edge& neighbour : adjacencyList[from]) {
            if (neighbour.destination == to) {
                return neighbour.distance;
            }
        }
        return -1.0;
    }

    PathResult shortestPathWithMinimumStops(int source, int destination) const {
        PathResult result;
        const int n = static_cast<int>(locations.size());

        if (source < 0 || destination < 0 || source >= n || destination >= n) {
            return result;
        }

        if (source == destination) {
            result.found = true;
            result.path.push_back(source);
            return result;
        }

        const double INF = 1e18;
        const double EPS = 1e-9;

        vector<double> distance(n, INF);
        vector<int> hops(n, 1000000000);
        vector<int> parent(n, -1);

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

        if (distance[destination] >= INF / 2) {
            return result;
        }

        result.found = true;
        result.totalDistance = distance[destination];
        result.totalHops = hops[destination];

        int current = destination;
        bool reachedSource = false;

        for (int steps = 0; steps < n; ++steps) {
            if (current < 0 || current >= n || parent[current] == -1) {
                result.found = false;
                result.path.clear();
                return result;
            }

            result.path.push_back(current);
            if (current == source) {
                reachedSource = true;
                break;
            }
            current = parent[current];
        }

        if (!reachedSource) {
            result.found = false;
            result.path.clear();
            return result;
        }

        reverse(result.path.begin(), result.path.end());
        return result;
    }

    bool planJourney() const {
        cout << "\n========== WHERE WOULD YOU LIKE TO START FROM? ==========\n";
        displayLocations();

        int source;
        if (!readLocationChoice(
                "\nSelect your starting location (enter its number): ", source)) {
            return false;
        }

        cout << "\n================ WHERE WOULD YOU LIKE TO GO? ================\n";
        displayLocations();

        int destination;
        if (!readLocationChoice(
                "\nSelect your destination (enter its number): ", destination)) {
            return false;
        }

        PathResult result = shortestPathWithMinimumStops(source, destination);

        cout << "\n================ SHORTEST ROUTE RESULT ================\n";
        if (!result.found) {
            cout << "No route is available between these locations.\n";
            return true;
        }

        cout << "Shortest route found!\n\n";
        cout << "START       : " << locations[source] << '\n';
        cout << "DESTINATION : " << locations[destination] << '\n';

        if (source == destination) {
            cout << "You are already at your selected destination. No travel is required.\n";
        }

        cout << "TOTAL SHORTEST DISTANCE: ";
        printOneDecimal(result.totalDistance);
        cout << " km\n";
        cout << "INTERMEDIATE STOPS     : "
             << max(0, result.totalHops - 1) << '\n';
        cout << "ROAD SEGMENTS          : " << result.totalHops << '\n';

        cout << "\nRECOMMENDED SHORTEST ROUTE (in travel order):\n";
        if (result.path.size() == 1) {
            cout << "1. " << locations[result.path[0]]
                 << "  [START / DESTINATION]\n";
        } else {
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
        }

        cout << "\n\nSTEP-BY-STEP DISTANCE BREAKDOWN:\n";
        if (result.totalHops == 0) {
            cout << "No road segments required.\n";
        } else {
            for (size_t i = 0; i + 1 < result.path.size(); ++i) {
                int from = result.path[i];
                int to = result.path[i + 1];
                double legDistance = getRoadDistance(from, to);
                cout << (i + 1) << ". " << locations[from]
                     << " -> " << locations[to] << " = ";
                printOneDecimal(legDistance);
                cout << " km\n";
            }
        }

        cout << "-----------------------------------------------\n";
        cout << "TOTAL DISTANCE: ";
        printOneDecimal(result.totalDistance);
        cout << " km\n";
        cout << "Note: The planner finds the shortest route in its sample road graph.\n";
        cout << "Road distances are illustrative values, not live navigation data.\n";
        return true;
    }

    void run() const {
        while (true) {
            cout << "\n\n**************************************************\n";
            cout << "       KARACHI SMART TRAVEL PLANNER\n";
            cout << "**************************************************\n";
            cout << "1. View all tourist locations\n";
            cout << "2. View all direct routes\n";
            cout << "3. Find the shortest route between two locations\n";
            cout << "4. Exit\n";

            int choice;
            if (!readIntegerInput("Enter your choice: ", choice)) {
                cout << "\nInput ended. Exiting the Travel Planner.\n";
                return;
            }

            switch (choice) {
                case 1:
                    displayLocations();
                    break;
                case 2:
                    displayDirectRoutes();
                    break;
                case 3:
                    if (!planJourney()) {
                        cout << "\nInput ended. Exiting the Travel Planner.\n";
                        return;
                    }
                    break;
                case 4:
                    cout << "Thank you for using the Travel Planner!\n";
                    return;
                default:
                    cout << "Invalid option. Please select 1, 2, 3, or 4.\n";
            }
        }
    }
};

#ifndef TRAVEL_PLANNER_NO_MAIN
int main() {
    TravelPlanner& planner = TravelPlanner::getInstance();
    planner.run();
    return 0;
}
#endif
