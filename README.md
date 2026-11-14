# 🧭 Karachi Smart Travel Planner

A C++17 Data Structures and Algorithms (DSA) project that recommends the shortest route between selected tourist destinations in Karachi. The project models locations and road connections as a weighted, undirected graph and uses Dijkstra's shortest-path algorithm with a priority queue.

## 🌐 Live Website

Explore the project website here:

**[Karachi Travel Planner — Live Demo](https://route-khi-karachi-travel-planner.ai.studio)**

The website is a companion demonstration of how the travel-planning idea can be presented through a browser interface. The main DSA semester-project implementation and the members' coding contributions are in the C++ source code described in this README. The website is supplementary and is not counted as a separate member contribution.

> **Project scope:** This is an educational DSA demonstration. The attraction names are real Karachi landmarks, but the road connections and distances in the program are illustrative sample data. They are not verified navigation distances, live traffic information, or turn-by-turn directions.

## ✨ Features

- 📍 Lists 11 popular tourist destinations in Karachi.
- 🛣️ Displays the direct road connections represented in the graph.
- 🎯 Lets the user choose a starting point and destination.
- 🧭 Finds the shortest route using Dijkstra's algorithm.
- 🚦 Uses a priority queue to process the next best candidate.
- ⚖️ If routes have equal total distance, prefers the route with fewer road segments.
- 🗺️ Reconstructs and displays the recommended route in travel order.
- 📏 Shows the total distance, intermediate stops, road segments, and distance for each segment.
- 💻 Uses only five standard C++ headers.

## 📍 Tourist Destinations

The program includes these 11 locations:

1. 🏛️ Mazar-e-Quaid
2. 🛍️ Empress Market
3. 🏛️ Frere Hall
4. 🏺 National Museum of Pakistan
5. 🏠 Quaid-e-Azam House Museum
6. ⚓ Pakistan Maritime Museum
7. ✈️ PAF Museum
8. 🕌 Mohatta Palace Museum
9. 🕌 Abdullah Shah Ghazi Shrine
10. 🌊 Clifton Beach (Sea View)
11. 🌆 Port Grand

## 🧠 DSA Concepts Used

| Concept | How it is used |
|---|---|
| 🕸️ Weighted Graph | Represents tourist locations and road distances. |
| ↔️ Undirected Graph | Each listed road connection can be travelled in both directions. |
| 📋 Adjacency List | Stores the neighbouring locations and the distance to each neighbour. |
| 🧭 Dijkstra's Algorithm | Finds a route with the minimum total distance. |
| 🚦 Priority Queue | Selects the candidate state with the smallest distance first. |
| ⚖️ Custom Comparator | Orders states by distance, then by number of road segments, then by location index. |
| 🌳 Parent Array | Stores the previous location so the final route can be reconstructed. |
| 🔒 Singleton Design Pattern | Provides one shared `TravelPlanner` instance. |

### 🛣️ Shortest-Route Selection

The program first minimizes the **total route distance**. When two candidate routes have effectively equal distances, it prefers the route with **fewer road segments**.

The priority-queue state contains:

- `distance` — total distance travelled so far.
- `hops` — number of road segments used.
- `node` — current location index.

## 🛠️ Technologies and Requirements

- **Language:** C++
- **C++ standard:** C++17
- **Compiler:** GCC / MinGW g++, or another C++17-compatible compiler
- **C++ program interface:** Console-based menu
- **Companion demonstration:** [Live Karachi Travel Planner website](https://route-khi-karachi-travel-planner.ai.studio)

The source code uses only these headers:

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
```

## 🚀 Getting Started

### 1. 📥 Clone the Repository

Replace `YOUR-USERNAME` with your GitHub username:

```bash
git clone https://github.com/YOUR-USERNAME/karachi-smart-travel-planner.git
```

### 2. 📂 Open the Project Folder

```bash
cd karachi-smart-travel-planner
```

### 3. ⚙️ Compile the Program

**Windows (MinGW):**

```bash
g++ -std=c++17 -Wall -Wextra -pedantic travel_planner.cpp -o karachi_travel_planner.exe
```

**Linux / macOS:**

```bash
g++ -std=c++17 -Wall -Wextra -pedantic travel_planner.cpp -o karachi_travel_planner
```

### 4. ▶️ Run the Program

**Windows:**

```bash
.\karachi_travel_planner.exe
```

**Linux / macOS:**

```bash
./karachi_travel_planner
```

## 🎮 How to Use

When the program starts, it displays this menu:

```text
KARACHI SMART TRAVEL PLANNER

1. View all tourist locations
2. View all direct routes
3. Find the shortest route between two locations
4. Exit
```

Choose an option by entering its number.

To find a route:

1. Select option `3`.
2. Choose a starting location from the numbered list.
3. Choose your destination.
4. The program calculates and displays the shortest route.

The result includes:

- Starting location and destination
- Total shortest distance
- Number of intermediate stops
- Number of road segments
- Ordered route with `[START]`, `[STOP]`, and `[DESTINATION]` labels
- Distance for every road segment

## 🖥️ Example Output

The following is an example using the program's sample graph:

```text
START       : Mazar-e-Quaid
DESTINATION : Clifton Beach (Sea View)
TOTAL SHORTEST DISTANCE: 14.6 km
INTERMEDIATE STOPS     : 2
ROAD SEGMENTS          : 3

RECOMMENDED SHORTEST ROUTE (in travel order):
1. 🏛️ Mazar-e-Quaid [START]
2. Quaid-e-Azam House Museum [STOP]
3. Mohatta Palace Museum [STOP]
4. Clifton Beach (Sea View) [DESTINATION]

STEP-BY-STEP DISTANCE BREAKDOWN:
1. 🏛️ Mazar-e-Quaid -> Quaid-e-Azam House Museum = 3.4 km
2. Quaid-e-Azam House Museum -> Mohatta Palace Museum = 7.0 km
3. Mohatta Palace Museum -> Clifton Beach (Sea View) = 4.2 km

TOTAL DISTANCE: 14.6 km
```

*The distances above are illustrative sample values from the project's graph.*

## ⏱️ Algorithm and Complexity

Let:

- `V` = number of vertices (tourist locations)
- `E` = number of edges (road connections)

With an adjacency list and a binary-heap priority queue, the expected time complexity of Dijkstra's algorithm is:

**Time complexity:** `O((V + E) log V)`

**Space complexity:** `O(V + E)` for the graph and algorithm data structures.

Route reconstruction by following the parent links takes `O(V)` time in the worst case.

## 👥 Team Contributions

Update the placeholders below with the actual names and student IDs before submitting the repository.

| Member | Contribution |
|---|---|
| **Member 1 – [Name, Student ID]** | Implementation of Dijkstra's shortest-path algorithm. |
| **Member 2 – [Name, Student ID]** | Priority queue implementation and its custom state comparator for processing candidate routes. |

Both members should describe only the work they actually completed. The website is a demonstration of how the project could be presented; it is not included as an individual coding contribution. Add any other shared or individually completed C++ tasks here if applicable.

## ⚠️ Limitations

- The graph contains a selected set of 11 Karachi attractions, not every location in the city.
- Road distances are sample values for demonstrating graph algorithms.
- The program does not use GPS, live maps, traffic data, or real-time road conditions.
- A mathematically shortest path in this sample graph may not be the shortest real-world driving route.

## 🔮 Possible Future Improvements

- Replace sample distances with verified road-network distances.
- Add more Karachi destinations and road connections.
- Import location and route data from an external file.
- Add a map-based interface as a separate visualization layer.
- Include travel-time estimates when reliable traffic data is available.

## 🔗 Reference and Inspiration

**Live project website:** [Karachi Travel Planner](https://route-khi-karachi-travel-planner.ai.studio)

This project was inspired by the concept of the Bhubaneswar Travel Planner:

- [GitHub repository – Ritwika101/travel-planner-bbsr](https://github.com/Ritwika101/travel-planner-bbsr)
- [Published project website](https://ritwika101.github.io/travel-planner-bbsr/)

The current project adapts the travel-planning concept to selected Karachi destinations and implements the DSA solution in C++.

---

**Karachi Smart Travel Planner — DSA Semester Project**
