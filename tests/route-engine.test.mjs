import assert from "node:assert/strict";
import { LOCATIONS, ROUTES, findRouteInJavaScript } from "../route-engine.js";

assert.equal(LOCATIONS.length, 11, "The planner should contain 11 Karachi landmarks.");
assert.equal(ROUTES.length, 24, "The planner should contain 24 sample road links.");

for (let source = 0; source < LOCATIONS.length; source++) {
  for (let destination = 0; destination < LOCATIONS.length; destination++) {
    const result = findRouteInJavaScript(source, destination);
    assert.ok(result, `Expected a route from ${source} to ${destination}.`);
    assert.equal(result.path[0], source);
    assert.equal(result.path[result.path.length - 1], destination);
    assert.equal(result.totalHops, result.path.length - 1);
    assert.ok(result.totalDistance >= 0);

    let summedDistance = 0;
    for (let i = 0; i + 1 < result.path.length; i++) {
      const a = result.path[i];
      const b = result.path[i + 1];
      const edge = ROUTES.find(([from, to]) =>
        (from === a && to === b) || (from === b && to === a));
      assert.ok(edge, `Path contains a missing road edge ${a}-${b}.`);
      summedDistance += edge[2];
    }
    assert.ok(Math.abs(summedDistance - result.totalDistance) < 1e-8);
  }
}

const sameLocation = findRouteInJavaScript(4, 4);
assert.deepEqual(sameLocation.path, [4]);
assert.equal(sameLocation.totalDistance, 0);
assert.equal(sameLocation.totalHops, 0);

console.log("PASS: 121 source/destination combinations and same-location case.");
