/*
 *  geohash.c
 *  libgeohash
 *
 *  Created by Derek Smith on 10/6/09.
 *  Copyright (c) 2010, SimpleGeo
 *      All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:

 *  Redistributions of source code must retain the above copyright notice, this list
 *  of conditions and the following disclaimer. Redistributions in binary form must 
 *  reproduce the above copyright notice, this list of conditions and the following 
 *  disclaimer in the documentation and/or other materials provided with the distribution.
 *  Neither the name of the SimpleGeo nor the names of its contributors may be used
 *  to endorse or promote products derived from this software without specific prior 
 *  written permission. 
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
 *  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 *  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "geohash.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>



typedef struct IntervalStruct {
    
    double high;
    double low;
    
} Interval;

// cached results from calling geohash_dimensions_for_precision
const GeoBoxDimension dimension_cache[] = {
        {0.0, 0.0}, // 0
        {45.0, 45.0}, // 1
        {5.625, 11.25}, // 2
        {1.40625 , 1.40625 }, // 3
        {0.17578125 , 0.3515625 }, // 4
        {0.0439453125 , 0.0439453125 }, // 5
        {0.0054931640625 , 0.010986328125 }, // 6
        {0.001373291015625 , 0.001373291015625 }, // 7
        {0.000171661376953125 , 0.00034332275390625 }, // 8
        {4.291534423828125e-05 , 4.291534423828125e-05 }, // 9
        {5.3644180297851562e-06 , 1.0728836059570312e-05 }, // 10
        {1.3411045074462891e-06 , 1.3411045074462891e-06 }, // 11
        {1.6763806343078613e-07 , 3.3527612686157227e-07 }, // 12
};


/* Normal 32 characer map used for geohashing */
static char char_map[33] =  "0123456789bcdefghjkmnpqrstuvwxyz";

/*
 *  The follow character maps were created by Dave Troy and used in his Javascript Geohashing
 *  library. http://github.com/davetroy/geohash-js
 */
static char *even_neighbors[] = {"p0r21436x8zb9dcf5h7kjnmqesgutwvy",
                                "bc01fg45238967deuvhjyznpkmstqrwx", 
                                "14365h7k9dcfesgujnmqp0r2twvyx8zb",
                                "238967debc01fg45kmstqrwxuvhjyznp"
                                };

static char *odd_neighbors[] = {"bc01fg45238967deuvhjyznpkmstqrwx", 
                               "p0r21436x8zb9dcf5h7kjnmqesgutwvy",
                                "238967debc01fg45kmstqrwxuvhjyznp",
                               "14365h7k9dcfesgujnmqp0r2twvyx8zb"    
                                };

static char *even_borders[] = {"prxz", "bcfguvyz", "028b", "0145hjnp"};
static char *odd_borders[] = {"bcfguvyz", "prxz", "0145hjnp", "028b"};

int index_for_char(char c, char *string) {
    
    int index = -1;
    if (c && string){
        size_t string_amount = strlen(string);
        int i;
        for(i = 0; (size_t)i < string_amount; i++) {

            if(c == string[i]) {

                index = i;
                break;
            }

        }
    }
    return index;
}

char* get_neighbor(const char *hash, int direction) {
    // SANITY CHECKS
    if (!hash){
        return NULL;
    }
    size_t hash_length = strlen(hash);
    if (hash_length < MIN_PRECISION  || hash_length > MAX_PRECISION){
        return NULL;
    }
    if (direction > WEST || direction < NORTH){
        return NULL;
    }

    // LOOKS SANE, START COMPUTING
	char last_char = hash[hash_length - 1];
    
    size_t is_odd = hash_length % 2;
    char **border = is_odd ? odd_borders : even_borders;
    char **neighbor = is_odd ? odd_neighbors : even_neighbors; 
    
    char *base = (char *)calloc(1, sizeof(char) * hash_length + 1);
    if (!base){return NULL;}

    strncat(base, hash, hash_length - 1);
    
	if(index_for_char(last_char, border[direction]) != -1){
        char * newBase = get_neighbor(base, direction);
        strncpy(base, newBase, hash_length);
        free(newBase);
	}

    
    int neighbor_index = index_for_char(last_char, neighbor[direction]);
	if (neighbor_index != -1){
        last_char = char_map[neighbor_index];
        char *last_hash = (char *)malloc(sizeof(char) * 2);
        if (last_hash){
            last_hash[0] = last_char;
            last_hash[1] = '\0';
            strcat(base, last_hash);
            free(last_hash);
        }
	}
	return base;
}

char* geohash_encode(double lat, double lng, int precision) {
    
    if(precision < MIN_PRECISION || precision > MAX_PRECISION)
        precision = DEF_PRECISION;
    
    char* hash = NULL;
    
    if(lat <= MAX_LAT && lat >= MIN_LAT && lng <= MAX_LONG && lng >= MIN_LONG) {
        
        hash = (char*)calloc(1, sizeof(char) * (precision + 1));
        
        precision *= 5;
        
        Interval lat_interval = {MAX_LAT, MIN_LAT};
        Interval lng_interval = {MAX_LONG, MIN_LONG};

        Interval *interval;
        double coord, mid;
        int is_even = 1;
        unsigned int hashChar = 0;
        int i;
        for(i = 1; i <= precision; i++) {
         
            if(is_even) {
            
                interval = &lng_interval;
                coord = lng;                
                
            } else {
                
                interval = &lat_interval;
                coord = lat;   
            }
            
            mid = (interval->low + interval->high) / 2.0;
            hashChar = hashChar << 1;
            
            if(coord > mid) {
                
                interval->low = mid;
                hashChar |= 0x01;
                
            } else
                interval->high = mid;
            
            if(!(i % 5)) {
                
                hash[(i - 1) / 5] = char_map[hashChar];
                hashChar = 0;

            }
            
            is_even = !is_even;
        }
     
        
    }
    
    return hash;
}

GeoCoord geohash_decode(char *hash) {
    
    GeoCoord coordinate = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    if(hash) {
        
        size_t char_amount = strlen(hash);
        if (char_amount < MIN_PRECISION){
            char_amount = 0;
        } else if (char_amount > MAX_PRECISION){
            char_amount = MAX_PRECISION;
        }
        if(char_amount) {
            
            int char_mapIndex;
            Interval lat_interval = {MAX_LAT, MIN_LAT};
            Interval lng_interval = {MAX_LONG, MIN_LONG};
            Interval *interval;
        
            int is_even = 1;
            double delta;
            int i, j;
            for(i = 0; (size_t)i < char_amount; i++) {
            
                char_mapIndex = index_for_char(hash[i], (char*)char_map);
                
                if(char_mapIndex < 0)
                    break;
            
                // Interpret the last 5 bits of the integer
                for(j = 0; j < 5; j++) {
                
                    interval = is_even ? &lng_interval : &lat_interval;
                
                    delta = (interval->high - interval->low) / 2.0;
                
                    if((char_mapIndex << j) & 0x0010)
                        interval->low += delta;
                    else
                        interval->high -= delta;
                
                    is_even = !is_even;
                }
            
            }
            
            coordinate.latitude = lat_interval.high - ((lat_interval.high - lat_interval.low) / 2.0);
            coordinate.longitude = lng_interval.high - ((lng_interval.high - lng_interval.low) / 2.0);
            
            coordinate.north = lat_interval.high;
            coordinate.east = lng_interval.high;
            coordinate.south = lat_interval.low;
            coordinate.west = lng_interval.low;

            coordinate.dimension = geohash_dimensions_for_precision(i);
        }
    }
    
    return coordinate;
}


char** geohash_neighbors(const char *hash) {

    char** neighbors = NULL;
    
    if(hash) {
        
        // N, NE, E, SE, S, SW, W, NW
        neighbors = (char**)malloc(sizeof(char*) * 8);
        
        neighbors[0] = get_neighbor(hash, NORTH);
        neighbors[1] = get_neighbor(neighbors[0], EAST);
        neighbors[2] = get_neighbor(hash, EAST);
        neighbors[3] = get_neighbor(neighbors[2], SOUTH);
        neighbors[4] = get_neighbor(hash, SOUTH);
        neighbors[5] = get_neighbor(neighbors[4], WEST);                
        neighbors[6] = get_neighbor(hash, WEST);
        neighbors[7] = get_neighbor(neighbors[6], NORTH);        

    }
    
    return neighbors;
}

void geohash_free_neighbors(char ** neighbors){
    int i = 0;
    if (neighbors){
        for (i=0; i<8; i++){
            if (neighbors[i]){
                free(neighbors[i]);
                neighbors[i] = NULL;
            }
        }
        free(neighbors);
    }
}

GeoBoxDimension geohash_dimensions_for_precision(int precision) {
	
	GeoBoxDimension dimensions = {0.0, 0.0};
	
	if(precision > 0) {

	    if (precision <= MAX_PRECISION){
	        dimensions = dimension_cache[precision];
	    } else {
            int lat_times_to_cut = precision * 5 / 2;
            int lng_times_to_cut = precision * 5 / 2 + (precision % 2 ? 1 : 0);

            double width = 360.0;
            double height = 180.0;

            int i;
            for(i = 0; i < lat_times_to_cut; i++)
                height /= 2.0;

            for(i = 0; i < lng_times_to_cut; i++)
                width /= 2.0;

            dimensions.width = width;
            dimensions.height = height;
	    }
	}
	
	return dimensions;
}
