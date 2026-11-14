import { LOCATIONS, ROUTES, findRouteInJavaScript } from "./route-engine.js";

const NODE_POSITIONS = [
  [350, 76], [160, 170], [82, 316], [345, 238], [555, 180],
  [95, 435], [292, 435], [610, 318], [684, 430], [485, 444], [560, 72]
];
const NODE_LABELS = [
  "Mazar-e-Quaid", "Empress Market", "Frere Hall", "National Museum",
  "Quaid House Museum", "Maritime Museum", "PAF Museum", "Mohatta Palace",
  "Abdullah Shah Ghazi", "Clifton Sea View", "Port Grand"
];
const $ = (selector) => document.querySelector(selector);

let locations = LOCATIONS.slice();
let routeEngine = null;
let lastRoute = null;

const THEME_STORAGE_KEY = "karachi-route-theme";

function applyTheme(theme) {
  const isDark = theme === "dark";
  document.documentElement.dataset.theme = isDark ? "dark" : "light";

  const toggle = $("#theme-toggle");
  if (toggle) {
    toggle.setAttribute("aria-pressed", String(isDark));
    toggle.setAttribute("aria-label", isDark ? "Switch to light mode" : "Switch to dark mode");
    toggle.innerHTML = `<span class="theme-toggle-icon" aria-hidden="true">${isDark ? "☀" : "☾"}</span><span class="theme-toggle-label">${isDark ? "Light mode" : "Dark mode"}</span>`;
  }

  const themeColor = document.querySelector('meta[name="theme-color"]');
  if (themeColor) themeColor.content = isDark ? "#101a19" : "#f4f3ee";
}

function initializeTheme() {
  let savedTheme = document.documentElement.dataset.theme || "light";
  try {
    savedTheme = localStorage.getItem(THEME_STORAGE_KEY) || savedTheme;
  } catch (error) {
    // The theme still works for this page even if browser storage is unavailable.
  }
  applyTheme(savedTheme);

  $("#theme-toggle").addEventListener("click", () => {
    const nextTheme = document.documentElement.dataset.theme === "dark" ? "light" : "dark";
    applyTheme(nextTheme);
    try {
      localStorage.setItem(THEME_STORAGE_KEY, nextTheme);
    } catch (error) {
      // Ignore storage restrictions; the selected theme remains active until the page closes.
    }
  });
}

async function loadRouteEngine() {
  try {
    const moduleUrl = new URL("./travel_planner.js", import.meta.url);
    const moduleFactory = (await import(moduleUrl.href)).default;
    const wasm = await moduleFactory();
    const count = wasm._tp_location_count();
    if (count !== LOCATIONS.length) throw new Error("C++ location data does not match the website.");
    const wasmNames = [];
    const getName = wasm.cwrap("tp_location_name", "string", ["number"]);
    for (let i = 0; i < count; i++) wasmNames.push(getName(i));
    locations = wasmNames;

    return {
      kind: "wasm",
      findRoute(source, destination) {
        const pathBytes = count * 4;
        const pathPtr = wasm._malloc(pathBytes);
        const distancePtr = wasm._malloc(8);
        const hopsPtr = wasm._malloc(4);
        try {
          const length = wasm._tp_find_route(source, destination, pathPtr, count, distancePtr, hopsPtr);
          if (length <= 0) return null;
          const path = [];
          for (let i = 0; i < length; i++) path.push(wasm.HEAP32[(pathPtr >> 2) + i]);
          return {
            path,
            totalDistance: wasm.HEAPF64[distancePtr >> 3],
            totalHops: wasm.HEAP32[hopsPtr >> 2]
          };
        } finally {
          wasm._free(pathPtr);
          wasm._free(distancePtr);
          wasm._free(hopsPtr);
        }
      }
    };
  } catch (error) {
    console.info("C++ WebAssembly build not found; using the matching JavaScript preview engine.", error);
    return { kind: "javascript", findRoute: findRouteInJavaScript };
  }
}

function fillSelect(select, selectedIndex) {
  select.innerHTML = "";
  locations.forEach((name, index) => {
    const option = document.createElement("option");
    option.value = String(index);
    option.textContent = `${String(index + 1).padStart(2, "0")}  ·  ${name}`;
    select.appendChild(option);
  });
  select.value = String(selectedIndex);
}

function svgElement(tag, attributes = {}) {
  const element = document.createElementNS("http://www.w3.org/2000/svg", tag);
  for (const [key, value] of Object.entries(attributes)) element.setAttribute(key, String(value));
  return element;
}

function edgeKey(a, b) { return a < b ? `${a}-${b}` : `${b}-${a}`; }

function drawNetwork(route = null) {
  const edgeLayer = $("#graph-edges");
  const nodeLayer = $("#graph-nodes");
  edgeLayer.replaceChildren();
  nodeLayer.replaceChildren();
  const activeEdges = new Set();
  if (route) {
    for (let i = 0; i + 1 < route.path.length; i++) activeEdges.add(edgeKey(route.path[i], route.path[i + 1]));
  }

  ROUTES.forEach(([from, to, distance]) => {
    const [x1, y1] = NODE_POSITIONS[from];
    const [x2, y2] = NODE_POSITIONS[to];
    const active = activeEdges.has(edgeKey(from, to));
    const line = svgElement("line", { x1, y1, x2, y2, class: active ? "graph-edge active" : "graph-edge" });
    const title = svgElement("title");
    title.textContent = `${locations[from]} ↔ ${locations[to]}: ${distance.toFixed(1)} km (sample distance)`;
    line.appendChild(title);
    edgeLayer.appendChild(line);
    if (active) {
      const label = svgElement("text", {
        x: (x1 + x2) / 2,
        y: (y1 + y2) / 2 - 7,
        class: "edge-weight active",
        "text-anchor": "middle"
      });
      label.textContent = `${distance.toFixed(1)} km`;
      edgeLayer.appendChild(label);
    }
  });

  const source = route ? route.path[0] : Number($("#start-select").value || 0);
  const destination = route ? route.path[route.path.length - 1] : Number($("#destination-select").value || 9);
  NODE_POSITIONS.forEach(([x, y], index) => {
    const group = svgElement("g", { class: `graph-node${index === source ? " selected" : ""}${index === destination ? " destination" : ""}` });
    const title = svgElement("title");
    title.textContent = `${index + 1}. ${locations[index]}`;
    group.appendChild(title);
    group.appendChild(svgElement("circle", { cx: x, cy: y, r: 21, class: "node-halo" }));
    group.appendChild(svgElement("circle", { cx: x, cy: y, r: 15, class: "node-core" }));
    const number = svgElement("text", { x, y: y + .5, class: "node-number" });
    number.textContent = String(index + 1);
    group.appendChild(number);
    const label = svgElement("text", { x, y: y + 34, class: "node-label" });
    label.textContent = NODE_LABELS[index];
    group.appendChild(label);
    nodeLayer.appendChild(group);
  });
}

function renderLandmarkChips() {
  const strip = $("#landmark-strip");
  strip.replaceChildren();
  locations.forEach((name, index) => {
    const chip = document.createElement("button");
    chip.type = "button";
    chip.className = "landmark-chip";
    chip.title = `Set ${name} as your destination`;
    chip.innerHTML = `<b>${String(index + 1).padStart(2, "0")}</b>`;
    const label = document.createElement("span");
    label.textContent = name;
    chip.appendChild(label);
    chip.addEventListener("click", () => {
      $("#destination-select").value = String(index);
      updateActiveChip(index);
      calculateAndRender();
    });
    strip.appendChild(chip);
  });
}

function updateActiveChip(index) {
  document.querySelectorAll(".landmark-chip").forEach((chip, chipIndex) => {
    chip.classList.toggle("active", chipIndex === index);
  });
}

function renderRoute(result, source, destination) {
  if (!result) {
    $("#empty-result").hidden = false;
    $("#route-result").hidden = true;
    $("#empty-result h3").textContent = "No route found";
    $("#empty-result p").textContent = "There is no connected path between the selected locations in this graph.";
    lastRoute = null;
    drawNetwork(null);
    return;
  }

  lastRoute = result;
  $("#empty-result").hidden = true;
  $("#route-result").hidden = false;
  $("#result-tag").textContent = result.totalHops === 0 ? "SAME LOCATION" : "SHORTEST PATH";
  $("#total-distance").textContent = `${result.totalDistance.toFixed(1)}`;
  $("#total-stops").textContent = String(Math.max(0, result.totalHops - 1));
  $("#total-segments").textContent = String(result.totalHops);
  $("#route-count").textContent = `${result.path.length} ${result.path.length === 1 ? "location" : "locations"}`;

  const list = $("#route-steps");
  list.replaceChildren();
  result.path.forEach((node, index) => {
    const item = document.createElement("li");
    const isStart = index === 0;
    const isDestination = index === result.path.length - 1;
    item.className = `route-step${isDestination ? " destination" : ""}`;

    const marker = document.createElement("span");
    marker.className = "step-marker";
    marker.textContent = isStart ? "A" : (isDestination ? "B" : String(index + 1));

    const nameWrap = document.createElement("span");
    const name = document.createElement("span");
    name.className = "step-name";
    name.textContent = locations[node];
    const role = document.createElement("span");
    role.className = "step-role";
    role.textContent = isStart ? "STARTING POINT" : (isDestination ? "DESTINATION" : "INTERMEDIATE STOP");
    nameWrap.append(name, role);

    const distance = document.createElement("span");
    distance.className = "step-distance";
    if (!isDestination) {
      const nextNode = result.path[index + 1];
      const route = ROUTES.find(([a, b]) => edgeKey(a, b) === edgeKey(node, nextNode));
      distance.textContent = route ? `${route[2].toFixed(1)} km` : "—";
    } else {
      distance.textContent = "END";
    }

    item.append(marker, nameWrap, distance);
    list.appendChild(item);
  });
  drawNetwork(result);
  updateActiveChip(destination);
}

function calculateAndRender() {
  const source = Number($("#start-select").value);
  const destination = Number($("#destination-select").value);
  const result = routeEngine.findRoute(source, destination);
  renderRoute(result, source, destination);
}

function bindEvents() {
  $("#find-route").addEventListener("click", calculateAndRender);
  const clearPendingRoute = () => {
    lastRoute = null;
    $("#empty-result").hidden = false;
    $("#route-result").hidden = true;
    $("#empty-result h3").textContent = "Ready for a new route";
    $("#empty-result p").textContent = "Your updated shortest route will appear after you press the button.";
    drawNetwork(null);
  };
  $("#start-select").addEventListener("change", clearPendingRoute);
  $("#destination-select").addEventListener("change", () => {
    updateActiveChip(Number($("#destination-select").value));
    clearPendingRoute();
  });
  $("#swap-button").addEventListener("click", () => {
    const start = $("#start-select").value;
    $("#start-select").value = $("#destination-select").value;
    $("#destination-select").value = start;
    calculateAndRender();
  });
}

async function startApp() {
  initializeTheme();
  routeEngine = await loadRouteEngine();
  fillSelect($("#start-select"), 0);
  fillSelect($("#destination-select"), 9);
  renderLandmarkChips();
  bindEvents();
  const status = $("#engine-status");
  const dot = document.querySelector(".status-dot");
  if (routeEngine.kind === "wasm") {
    status.textContent = "C++ WebAssembly engine";
    dot.classList.add("ready");
  } else {
    status.textContent = "Browser preview engine";
    dot.classList.add("ready");
  }
  calculateAndRender();
}

startApp();
