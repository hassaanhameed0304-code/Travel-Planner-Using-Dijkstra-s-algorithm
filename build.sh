#!/usr/bin/env bash
set -euo pipefail

if ! command -v emcc >/dev/null 2>&1; then
  echo "Emscripten (emcc) was not found. Activate your Emscripten SDK first."
  echo "The website can still run using its built-in JavaScript preview engine."
  exit 1
fi

emcc cpp/web_bridge.cpp \
  -std=c++17 -O2 \
  --no-entry \
  -s MODULARIZE=1 \
  -s EXPORT_ES6=1 \
  -s EXPORT_NAME=createTravelPlannerModule \
  -s EXPORTED_FUNCTIONS='["_tp_location_count","_tp_location_name","_tp_segment_distance","_tp_find_route","_malloc","_free"]' \
  -s EXPORTED_RUNTIME_METHODS='["cwrap","UTF8ToString"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s ENVIRONMENT=web \
  -o travel_planner.js

echo "Built travel_planner.js and travel_planner.wasm."
echo "The website will now use the C++ Dijkstra engine through WebAssembly."
