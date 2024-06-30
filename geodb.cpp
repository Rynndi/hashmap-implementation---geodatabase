//
//  geodb.cpp
//  Di Project 4
//
//  Created by Catherine Di on 3/8/24.
//

#include "geodb.h"
#include <iostream>
#include "geotools.h"
#include <string>
using namespace std;


//initialize geodatabase object
GeoDatabase::GeoDatabase()
{

}
GeoDatabase::~GeoDatabase()
{

}

bool GeoDatabase::load(const std::string& map_data_file){
    //open the file, read from each line, parse the data, populate the poiLocations, connections and streetNames hashmaps
    std::ifstream inf(map_data_file);
    if (!inf){
        std::cerr << "Failed to open " << map_data_file << std::endl;
        return false;
    }
    std::string line;
    std::string streetName= "";
    GeoPoint* point1  = nullptr;
    GeoPoint* point2 = nullptr;
    int poI;
    
    int lineNumber =0;

    //0 corresponds to street name
    //1 corresponds to Geopoint
    //...
    //when it gets to a line number, reads it in, sees it's 0, will reset the loop
    while(getline(inf, line)) {
        if(lineNumber ==0) {
            //see if you have anything to process before that
            
            streetName = line;
            //goes to the next line
            lineNumber = lineNumber + 1;
            continue; //takes us into next line
        }
        if(lineNumber == 1) {
            int index = 0;
            string latitude1;
            string longitude1;
            string latitude2;
            string longitude2;
            makeGeoPoint(line, index, latitude1, longitude1);
            index = index + 1;
            makeGeoPoint(line, index, latitude2, longitude2);
            
            //dynamically allocate so not destroyed out of scope
             point1 = new GeoPoint(latitude1, longitude1);
             point2 = new GeoPoint(latitude2, longitude2);
            
            string oneToTwo = (*point1).to_string() + (*point2).to_string();
            string twoToOne = (*point2).to_string() + (*point1).to_string();
            streetNames.insert(oneToTwo, streetName);
            streetNames.insert(twoToOne, streetName);
            //bracket value gets you a pointer to the value (the vector)
            connections[(*point1).to_string()].push_back(*point2);
            connections[(*point2).to_string()].push_back(*point1);
            lineNumber = lineNumber + 1;
            continue;
        }
        if(lineNumber == 2) {
            //line is a number. if 0, restart
            poI =std::stoi(line);
            if(poI==0){
                lineNumber = 0;
                if (point1!= nullptr)
                delete point1;
                if(point2!= nullptr)
                delete point2;
                poI = 0;
                continue;
            }
            else {
                lineNumber++;
                continue; //go to next line number
            }
        }
        if(lineNumber >=3) {
            //line number is the same for the ones below
            if(poI > 0){
                //midpoints only happen for points of interest
                int i=0;
                //to be appended
                string poi = "";
                string lat = "";
                string lo = "";
                
                while(i <line.size() && line[i] != '|' ){
                    poi+=line[i]; //appends the poi string
                    i++;
                }
                
                i++;
                
                //automatically appends from longitude to latitude
                makeGeoPoint(line, i, lat, lo);
                mapPOI[poi] = GeoPoint(lat, lo);

                GeoPoint midPoint = midpoint(*point1, *point2);
            
                string oneToMid =(*point1).to_string() + (midPoint).to_string();
                string midToOne =(midPoint).to_string() + (*point1).to_string();
                
                string twoToMid = (*point2).to_string() + (midPoint).to_string();
                string midToTwo = (midPoint).to_string() + (*point2).to_string();
                
                streetNames.insert(oneToMid, streetName);
                streetNames.insert(midToOne, streetName);
                streetNames.insert(twoToMid, streetName);
                streetNames.insert(midToTwo, streetName);
                
                
                connections[(midPoint).to_string()].push_back(*point1);
                connections[(*point1).to_string()].push_back(midPoint);
                connections[(midPoint).to_string()].push_back(*point2);
                connections[(*point2).to_string()].push_back(midPoint);
                
                //insert the paths
                //for the poi
                string poiToMid = GeoPoint(lat, lo).to_string() + (midPoint).to_string();
                string midToPoi = midPoint.to_string() + GeoPoint(lat, lo).to_string();
                streetNames.insert(poiToMid, "a path");
                streetNames.insert(midToPoi, "a path");
                
                //connections from midpoint to poi
                connections[(midPoint).to_string()].push_back(GeoPoint(lat, lo));
                connections[GeoPoint(lat, lo).to_string()].push_back(midPoint);
            
                
                poI--;
               if(poI <= 0){
                    lineNumber = 0;
                    if (point1!= nullptr)
                        delete point1;
                    if(point2!= nullptr)
                        delete point2;
                    poI = 0;
                    continue;
                }
                lineNumber++;
            }
            else{
                lineNumber = 0; //set line number back to 0
            }
        }
    }
    return true;
    
}

bool GeoDatabase::get_poi_location(const std::string& poi, GeoPoint& point) const{
    const GeoPoint* found = mapPOI.find(poi);
    if(found == nullptr) {
        return false;
    }
    else{
        point = *found;
    }
    return true;
   
}

std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const
{
    std::vector<GeoPoint>emptyVec;
    const std::vector<GeoPoint>* segments = connections.find(pt.to_string());
    if (segments == nullptr) {
        return emptyVec;
    }
    return *segments;
    
}
std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const
{
    if(streetNames.find(pt1.to_string() + pt2.to_string()) != nullptr) {
        return *(streetNames.find(pt1.to_string() + pt2.to_string()));
    }
    else if (streetNames.find(pt2.to_string() + pt1.to_string()) != nullptr) {
        return*(streetNames.find(pt2.to_string() + pt1.to_string()));
    }
    else return "";
            
//
    
}
void GeoDatabase::makeGeoPoint(std::string line, int& index, std::string& latitude, std::string& longitude) {
    //index correlates to where you are on the line
  
    int lineNum = index;
    //where you want to start
    int i = index;
    //to be appended
    string lat = "";
    string lo = "";
    while(i <line.size() && line[i] != ' ' ){
        lat+=line[i];
        i++;
    }
    //latitude stored in lat
    i++; //1 after space
    while(i <line.size() && line[i] != ' ' ){
        lo+=line[i]; //store the longitude
        i++;
    }
    latitude = lat;
    longitude = lo;
    index = i;
}

