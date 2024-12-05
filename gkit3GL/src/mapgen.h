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

    divide = 2;
    if (divide == 1)
    {
        regions.emplace_back(Point(height_map.height(), 0, height_map.width()), Point(0, 0, 0));
        return translation.size();
    }
    int regions_height = height_map.height() / divide;
    int regions_width = height_map.width() / divide;

    for (int j = 0; j < divide; j++)
    {
        for (int i = 0; i < divide; i++)
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

int getTerrainType(const Color &pixel)
{
    if (pixel.g == 255)
        return 0; // Plain
    if (pixel.g == 100)
        return 1; // Forest
    if (pixel.g == 150)
        return 2; // Other Plain
    if (pixel.b == 255)
        return 3; // Water
    if (pixel.b == 100)
        return 4; // Hole
    if (pixel.r == 50)
        return 5; // Rocky Rock
    if (pixel.r == 100)
        return 6; // Gray Rock
    if (pixel.r == 150)
        return 7; // Beach
    if (pixel.r == 200)
        return 8; // Rock Top
    if (pixel.r == 255)
        return 9; // Snow
    if (pixel.r == 1)
        return 10; // Light
    return 0;      // Default Plain
}

int generateTerrain(std::vector<vec4> &translation, const char *map_name, const char *height_map_name, std::vector<Region> &regions, int raise = 1, int divide = 1)
{

    const Image height_map = read_imageI(height_map_name);
    const Image map_map = read_imageI(map_name);
    std::cout << "Starting Terrain generation" << std::endl;
    const float height_scale = 10.0;
    translation.clear();
    // Only made for cube

    regions.clear();
    regions.reserve(divide * divide);

    // Regions individiual size
    // Part of the map may be lost
    int regions_height = round(height_map.height() / divide);
    int regions_width = round(height_map.width() / divide);

    for (int i = 0; i < divide; i++)
    {
        for (int j = 0; j < divide; j++)
        {
            int curr_regions_end_x = regions_width * (i + 1);
            int curr_regions_start_x = regions_width * (i);
            int curr_regions_end_y = regions_height * (j + 1);
            int curr_regions_start_y = regions_height * (j);
            regions.emplace_back(Point(curr_regions_end_x, 0, curr_regions_end_y),
                                 Point(curr_regions_start_x, 0, curr_regions_start_y));
            std::cout << "Region " << (i * divide + j) << " Bounding Box:" << std::endl;
            std::cout << "  Min Point: (" << curr_regions_start_x << ", 0, " << curr_regions_start_y << ")" << std::endl;
            std::cout << "  Max Point: (" << curr_regions_end_x << ", 0, " << curr_regions_end_y << ")" << std::endl;

            for (int y = curr_regions_start_y; y < curr_regions_end_y; y++)
            {
                for (int x = curr_regions_start_x; x < curr_regions_end_x; x++)
                {

                    // Switch for block

                    vec4 tmp = 2 * Vector(x, round(((height_map(y, x)).r) / height_scale) - 20, y); // Scale down the elevation to 10 level
                    tmp.w = getTerrainType(map_map(y, x));
                    // Just herev for output
                    translation.push_back(tmp);

                    //
                    /*
                    Add translations for critter inside
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
                    }*/
                }
            }
        }
    }

    // Paramétrise

    std::cout << "Terrain Generation Done" << std::endl;

    /*
    for (int i = 0; i < divide; i++)
    {
        for (int j = 0; j < divide; j++)
        {
            regions.emplace_back(Point(regions_width * (i + 1), 0, regions_height * (j + 1)),
                                 Point(regions_width * i, 0, regions_height * j));
        }
    }*/
    // Return individual regions sizes
    std::cout << "Bounding Box Done" << std::endl;

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

// Temporary position

struct ShadowMapFBO
{
    ShadowMapFBO() {};

    ~ShadowMapFBO() =
        default;

    void init(const float& width, const float& height)
    {
        // Create the framebuffer
        glGenFramebuffers(1, &depthMapshadowFB);

        // SHadow map texture
        //  depthMapshadow = make_flat_texture(2, 1024.0, 576.0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
        depthMapshadow = make_depth_texture(0,width, height);


        // fixe les parametres de filtrage par defaut
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapshadowFB);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapshadow, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        shadowprog = read_program("projets/tp4shad.glsl");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };

    void BindForWriting()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthMapshadowFB);
        glBindTexture(GL_TEXTURE_2D, depthMapshadow);
        glClear(GL_DEPTH_BUFFER_BIT);
    };

    void BindForReading()
    {
        // glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_2D, depthMapshadow);
    };

    GLuint shadowprog = 0;
    GLuint depthMapshadowFB = 0;
    GLuint depthMapshadow = 0;
};
