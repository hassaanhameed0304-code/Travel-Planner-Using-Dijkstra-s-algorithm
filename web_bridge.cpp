#define TRAVEL_PLANNER_NO_MAIN
#include "travel_planner.cpp"

// C-compatible functions exported to the browser through WebAssembly.
extern "C" {

int tp_location_count() {
    return TravelPlanner::getInstance().locationCount();
}

const char* tp_location_name(int index) {
    TravelPlanner& planner = TravelPlanner::getInstance();
    if (index < 0 || index >= planner.locationCount()) return "";
    return planner.locationName(index).c_str();
}

double tp_segment_distance(int from, int to) {
    return TravelPlanner::getInstance().segmentDistance(from, to);
}

int tp_find_route(int source, int destination, int* pathBuffer, int maxPath,
                  double* totalDistance, int* totalHops) {
    return TravelPlanner::getInstance().exportShortestRoute(
        source, destination, pathBuffer, maxPath, totalDistance, totalHops);
}

}
