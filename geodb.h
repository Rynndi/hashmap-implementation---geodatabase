//
//  geodb.h
//  Di Project 4
//
//  Created by Catherine Di on 3/8/24.
//

#ifndef geodb_h
#define geodb_h


#endif /* geodb_h */
#include "base_classes.h"
#include <iostream>
#include "HashMap.h"
//#include "scaffoldHashMap.h"
#include "geopoint.h"
#include <functional>
#include <vector>
#include <queue>

#include <string>
#include <sstream> 
#include <fstream>


class GeoDatabase: public GeoDatabaseBase {
public:
GeoDatabase();
virtual ~GeoDatabase();
virtual bool load(const std::string& map_data_file);
virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;

virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
virtual std::string get_street_name(const GeoPoint& pt1,
const GeoPoint& pt2) const;
    
private:
    HashMap<GeoPoint> mapPOI;
    
    HashMap<std::string> streetNames; 
  
    HashMap<std::vector<GeoPoint>> connections;

    void makeGeoPoint(std::string line, int& index, std::string& latitude, std::string& longitude);
};
