# Karachi Route Finder — Smart Travel Planner

A responsive website for the Karachi version of the C++ DSA Travel Planner. It follows the core idea of the Bhubaneswar reference project: select a start and destination, calculate the shortest path through a weighted graph, and visualize the chosen route.

## Features

- 11 Karachi landmarks and 24 undirected sample road links.
- Choose a starting location and destination, or swap them.
- Shortest route calculated with modified Dijkstra's algorithm.
- Equal-distance routes prefer fewer road segments.
- Clear distance, intermediate-stop, and road-segment summaries.
- Step-by-step route with the distance of each selected road segment.
- Interactive SVG graph; the recommended path is highlighted.
- Responsive layout for desktop, tablet, and mobile.
- Light and dark themes with a navigation-bar toggle; the selected theme is saved in the browser.
- C++ routing engine can be compiled to WebAssembly and run in the browser.
- JavaScript preview engine is included, so the website remains usable before the WebAssembly build is created.

## Change the website theme

Use the **Dark mode / Light mode** button in the top navigation bar. The selected theme is saved in your browser, so it remains selected when you revisit the site on the same browser.

## Update the website

### Update it on your computer

1. Open the `Karachi_Travel_Planner_Web` folder in VS Code.
2. Edit the file you want to change (`index.html` for page content, `styles.css` for design and colors, or `app.js` / `route-engine.js` for interactions and route data).
3. Press `Ctrl + S` to save.
4. If Live Server is running, the browser refreshes automatically. If it does not, refresh the browser with `Ctrl + R`.

### Update the online GitHub Pages website

1. Open the project folder in VS Code and make your changes.
2. Save the files.
3. Open the Source Control panel in VS Code, stage the changed files, enter a message such as `Add dark and light themes`, and commit.
4. Push/sync the commit to the GitHub repository's `main` branch.
5. Open the repository's **Actions** tab and wait for the deployment workflow to finish. The published website will then show the update. If the old version remains visible, hard-refresh with `Ctrl + Shift + R`.

## Important distance note

The Karachi attraction names are real landmarks, but the 24 graph weights are illustrative sample values used for the DSA demonstration. They are not verified road measurements, live navigation directions, traffic-aware travel times, or guaranteed real-world shortest routes. The graph visualization is schematic, not a geographic street map.

## Project structure

```text
Karachi_Travel_Planner_Web/
├── index.html
├── styles.css
├── app.js
├── route-engine.js
├── favicon.svg
├── build.sh
├── cpp/
│   ├── travel_planner.cpp
│   └── web_bridge.cpp
├── .github/
│   └── workflows/
│       └── deploy.yml
└── README.md
```

## Run the website locally

You can preview the website without installing Python or Emscripten by using VS Code Live Server:

1. Open this project folder in VS Code.
2. Install the **Live Server** extension by Ritwick Dey if it is not already installed.
3. Open `index.html`, right-click inside the editor, and select **Open with Live Server**.
4. Your browser opens the local website, usually at `http://127.0.0.1:5500`.

Alternatively, if Python is installed, run this from the project folder:

```bash
python -m http.server 8000
```

Then open `http://localhost:8000`.

The website will automatically use the JavaScript preview engine if the WebAssembly build files are not present.

## Run the original C++ console project

The console version uses only the five requested headers:

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
```

Compile it from this folder with:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic cpp/travel_planner.cpp -o karachi_travel_planner
```

Run on Windows:

```text
karachi_travel_planner.exe
```

Run on Linux/macOS:

```bash
./karachi_travel_planner
```

## Build the C++ engine for the website (WebAssembly)

The browser bridge calls the same `TravelPlanner` C++ class and the same modified Dijkstra algorithm as the console application. To compile it to WebAssembly:

1. Install the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).
2. Activate the SDK environment in your terminal.
3. From this project folder, run:

```bash
./build.sh
```

This creates:

```text
travel_planner.js
travel_planner.wasm
```

Reload the website through the local server. The status indicator will show **C++ WebAssembly engine** when the C++ module loads successfully. If those files are absent, the site uses the matching JavaScript preview implementation.

## Publish with GitHub Pages

The included GitHub Actions workflow builds the C++ WebAssembly module and deploys the website automatically whenever you push to the `main` branch.

1. Create a new public GitHub repository, for example `karachi-travel-planner`.
2. Upload the contents of this project folder to the repository root.
3. Push the files to the `main` branch.
4. Open **Settings → Pages** and set the source to **GitHub Actions** if GitHub has not selected it automatically.
5. Open the **Actions** tab and wait for “Build and deploy Karachi Travel Planner” to finish.
6. Your site will be published at a URL similar to:

```text
https://YOUR-USERNAME.github.io/karachi-travel-planner/
```

## DSA concepts demonstrated

- **Graph:** each landmark is a vertex; each road link is a weighted edge.
- **Adjacency List:** stores each location's connected neighbours and distances.
- **Priority Queue / Min-Heap:** chooses the next state with the smallest distance.
- **Modified Dijkstra:** minimizes total distance, then number of road segments.
- **Parent Array:** reconstructs the final route in travel order.
- **Singleton Pattern:** the C++ planner uses one shared `TravelPlanner` instance.

Time complexity of the modified Dijkstra implementation with a binary heap is approximately `O((V + E) log V)`; route reconstruction takes `O(V)`.

## Reference

The project is inspired by the structure and algorithmic idea described in [Ritwika101's Bhubaneswar Travel Planner](https://github.com/Ritwika101/travel-planner-bbsr) and its [published project page](https://ritwika101.github.io/travel-planner-bbsr/). The Karachi location data and website interface have been adapted for this project.
