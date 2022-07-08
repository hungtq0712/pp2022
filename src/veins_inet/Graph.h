//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef VEINS_GRAPH_H_
#define VEINS_GRAPH_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

class ItineraryRecord {// Ban ghi hanh trinh cua xe
public:
    std::string laneId, prevLane = "";
    int prevVertex = -1;
    std::string station = "";
    int localWait;
};

class AGV {
public:
    std::string id;
    ItineraryRecord *itinerary;
    int reRouteAt = -1;
    bool passedStation = false;
    double atStation = 0;
};

#endif /* VEINS_GRAPH_H_ */
