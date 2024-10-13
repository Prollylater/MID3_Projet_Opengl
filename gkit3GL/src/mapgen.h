#pragma once
#include "image.h"
#include "image_io.h"
#include "stb_image_write.h"
#include "frustum.h"
#include <math.h>

// Map generation from a Grayscale map
int genWorldFromHmap(std::vector<Vector> &translation, const char *map_name, std::vector<Region> &regions, int divide = 1)
{
    const Image height_map = read_image(map_name);
    regions.clear();
    regions.reserve(divide * divide);

    for (float y = 0; y < height_map.height(); y++)
    {
        for (float x = 0; x < height_map.width(); x++)
        {

            Vector tmp = 2 * Vector(x, (height_map(y, x)).r, y);
            // Just here for output
            const float x_offset = 2 * x;
            const float y_offset = (height_map(y, x)).r; // Normalize
            const float z_offset = 2 * y;
            translation.push_back(tmp);
        }
    }

    if (divide == 1)
    {
        regions.emplace_back(Point(height_map.height(), 0, height_map.width()), Point(0, 0, 0));
        return translation.size();
    }
    int regions_height = height_map.height() / divide;
    int regions_width = height_map.width() / divide;

    for (int i = 0; i < divide; i++)
    {
        for (int j = 0; j < divide; j++)
        {
            regions.emplace_back(Point(regions_height * (j + 1), 0, regions_width * (i + 1)),
                                 Point(regions_height * j, 0, regions_width * i));
        }
    }
    // Return individual regions sizes
    return regions_height * regions_width;
}

// Map generation from a Grayscale image as heightmap and a RGB map
// Very specific formula that won't work with any setup
int genWorldFromThatMapmap(std::vector<std::vector<Vector>> &translation, std::vector<unsigned> &text_index, const char *map_name, const char *height_map_name, std::vector<Region> &regions, int divide = 1)
{

    const Image height_map = read_imageI(height_map_name);
    const Image map_map = read_imageI(map_name);

    regions.clear();
    regions.reserve(divide * divide);

    // Paramétrise

    for (float y = 0; y < height_map.height(); y++)
    {
        for (float x = 0; x < height_map.width(); x++)
        {

            // Switch for block

            Vector tmp = 2 * Vector(x, ((height_map(y, x)).r) / 25, y); // Scale down the elevation to 10 level
            // Just herev for output
            const float x_offset = 2 * x;
            const float y_offset = (height_map(y, x)).r; // Normalize
            const float z_offset = 2 * y;
            translation[0].push_back(tmp); // Push block position

            // TODO TO ANOTHER FUNCTION
            if (map_map(y, x).g == 255)
            {
                text_index.push_back(0);
            } //"Plain"
            else if (map_map(y, x).g == 100)
            {
                text_index.push_back(1);
                // text_index.push_back(1); // Add tree here with translation[0]
            } //  "Forest (random sampling for trees)";
            else if (map_map(y, x).g == 150)
            {
                text_index.push_back(2);
            } //"Plain (random sampling other)"; Add sutff ehre
            else if (map_map(y, x).b == 255)
            {
                text_index.push_back(3);
            } // "Water";
            else if (map_map(y, x).b == 100)
            {
                text_index.push_back(4);
            } // Hole";
            else if (map_map(y, x).r == 50)
            {
                text_index.push_back(5);
            } //"Rocky Rock";
            else if (map_map(y, x).r == 100)
            {
                text_index.push_back(6);
            } // "Gray Rock";
            else if (map_map(y, x).r == 150)
            {
                text_index.push_back(7);
            } //"Beach";
            else if (map_map(y, x).r == 200)
            {
                text_index.push_back(8);
            } // "Rock Top";
            else if (map_map(y, x).r == 255)
            {
                text_index.push_back(9);
            } // "Snow";
            else if (map_map(y, x).r == 1)
            {
                text_index.push_back(10);
            } // "Light";
            else
            {
                text_index.push_back(0);
            }
        }
    }

    if (divide == 1)
    {
        regions.emplace_back(Point(height_map.height(), 0, height_map.width()), Point(0, 0, 0));
        return translation.size();
    }
    int regions_height = height_map.height() / divide;
    int regions_width = height_map.width() / divide;

    for (int i = 0; i < divide; i++)
    {
        for (int j = 0; j < divide; j++)
        {
            regions.emplace_back(Point(regions_height * (j + 1), 0, regions_width * (i + 1)),
                                 Point(regions_height * j, 0, regions_width * i));
        }
    }
    // Return individual regions sizes
    return regions_height * regions_width;
}

// Simple flat map
void genMap(std::vector<Vector> &translation, int amount)
{
    for (float y = 0; y < amount; y++)
    {
        for (float x = 0; x < amount; x++)
        {
            float z = 0;
            if (x == y)
            {
                z = 1.0;
            }
            Vector tmp = 2 * Vector(x, z, y);
            translation.push_back(tmp);
        }
    }
}
