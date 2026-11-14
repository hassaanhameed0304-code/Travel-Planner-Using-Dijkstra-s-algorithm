# Karachi Smart Travel Planner — Edge-Case Update

This update preserves the existing C++17 travel planner and its five-header requirement.

## Improvements
- Selecting the same starting point and destination returns a valid zero-distance route.
- The route is labelled `[START / DESTINATION]` and reports zero stops and zero road segments.
- Invalid location indexes are checked before accessing vectors.
- Invalid menu and location input is handled line-by-line, including letters, decimals, extra characters, and out-of-range values.
- End-of-file/input cancellation exits cleanly instead of looping forever.
- The WebAssembly export validates pointers and buffer capacity and initializes output values safely.
- Route reconstruction has a step limit to guard against a broken parent chain.
- Invalid graph edges are ignored when building the adjacency list.
- The source code remains comment-free and uses only the five requested headers.

## Compile

```bash
g++ -std=c++17 -Wall -Wextra -pedantic travel_planner.cpp -o karachi_travel_planner
```

## Run

```bash
./karachi_travel_planner
```

On Windows with MinGW:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic travel_planner.cpp -o karachi_travel_planner.exe
.\karachi_travel_planner.exe
```

Keep `travel_planner.cpp` and `web_bridge.cpp` in the same folder because the bridge includes the main source file.

**Note:** The graph's road distances remain illustrative sample values, not verified navigation distances.
