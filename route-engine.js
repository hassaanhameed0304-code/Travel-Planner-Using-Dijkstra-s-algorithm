export const LOCATIONS = [
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
];

// Same 24 undirected sample road links and weights as the C++ DSA program.
export const ROUTES = [
  [0, 1, 2.7], [0, 3, 2.0], [0, 4, 3.4], [0, 10, 5.0],
  [1, 2, 1.5], [1, 3, 2.1], [1, 10, 4.2], [2, 3, 1.8],
  [2, 4, 2.0], [2, 7, 7.0], [3, 4, 2.7], [3, 10, 5.0],
  [4, 6, 7.5], [4, 7, 7.0], [5, 6, 4.0], [5, 7, 11.0],
  [5, 9, 12.5], [6, 7, 10.0], [6, 9, 12.0], [7, 8, 2.3],
  [7, 9, 4.2], [8, 9, 2.0], [8, 10, 11.0], [9, 10, 13.0]
];

const EPSILON = 1e-9;

class MinPriorityQueue {
  constructor() { this.items = []; }
  isEmpty() { return this.items.length === 0; }
  less(a, b) {
    if (a.distance !== b.distance) return a.distance < b.distance;
    if (a.hops !== b.hops) return a.hops < b.hops;
    return a.node < b.node;
  }
  push(value) {
    this.items.push(value);
    let i = this.items.length - 1;
    while (i > 0) {
      const parent = Math.floor((i - 1) / 2);
      if (!this.less(this.items[i], this.items[parent])) break;
      [this.items[i], this.items[parent]] = [this.items[parent], this.items[i]];
      i = parent;
    }
  }
  pop() {
    if (this.items.length === 0) return null;
    const first = this.items[0];
    const last = this.items.pop();
    if (this.items.length > 0) {
      this.items[0] = last;
      let i = 0;
      while (true) {
        const left = 2 * i + 1;
        const right = left + 1;
        let smallest = i;
        if (left < this.items.length && this.less(this.items[left], this.items[smallest])) smallest = left;
        if (right < this.items.length && this.less(this.items[right], this.items[smallest])) smallest = right;
        if (smallest === i) break;
        [this.items[i], this.items[smallest]] = [this.items[smallest], this.items[i]];
        i = smallest;
      }
    }
    return first;
  }
}

const ADJACENCY_LIST = Array.from({ length: LOCATIONS.length }, () => []);
for (const [from, to, distance] of ROUTES) {
  ADJACENCY_LIST[from].push({ destination: to, distance });
  ADJACENCY_LIST[to].push({ destination: from, distance });
}

// Browser preview equivalent of the C++ modified Dijkstra algorithm.
export function findRouteInJavaScript(source, destination) {
  if (!Number.isInteger(source) || !Number.isInteger(destination) ||
      source < 0 || destination < 0 ||
      source >= LOCATIONS.length || destination >= LOCATIONS.length) return null;

  const n = LOCATIONS.length;
  const distance = Array(n).fill(Infinity);
  const hops = Array(n).fill(Number.MAX_SAFE_INTEGER);
  const parent = Array(n).fill(-1);
  const queue = new MinPriorityQueue();

  distance[source] = 0;
  hops[source] = 0;
  parent[source] = source;
  queue.push({ distance: 0, hops: 0, node: source });

  while (!queue.isEmpty()) {
    const current = queue.pop();
    const { distance: currentDistance, hops: currentHops, node: u } = current;

    if (currentDistance > distance[u] + EPSILON ||
        distance[u] > currentDistance + EPSILON ||
        currentHops !== hops[u]) continue;
    if (u === destination) break;

    for (const edge of ADJACENCY_LIST[u]) {
      const v = edge.destination;
      const newDistance = currentDistance + edge.distance;
      const newHops = currentHops + 1;
      const shorterDistance = newDistance < distance[v] - EPSILON;
      const sameDistance = Math.abs(newDistance - distance[v]) <= EPSILON;
      const sameDistanceFewerHops = sameDistance && newHops < hops[v];

      if (shorterDistance || sameDistanceFewerHops) {
        distance[v] = newDistance;
        hops[v] = newHops;
        parent[v] = u;
        queue.push({ distance: newDistance, hops: newHops, node: v });
      }
    }
  }

  if (!Number.isFinite(distance[destination])) return null;
  const path = [];
  let current = destination;
  while (current !== source) {
    if (current < 0 || parent[current] < 0) return null;
    path.push(current);
    current = parent[current];
  }
  path.push(source);
  path.reverse();
  return { path, totalDistance: distance[destination], totalHops: hops[destination] };
}
