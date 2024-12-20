#pragma once
#include "image.h"
#include "image_io.h"
#include "stb_image_write.h"
#include "frustum.h"
#include <math.h>

// Map generation from a Grayscale map
int genWorldFromHmap(std::vector<vec4> &translation, const char *map_name, std::vector<Region> &regions, int divide = 1)
{
    const Image height_map = read_image(map_name);
    regions.clear();
    regions.reserve(divide * divide);
    const float height_scale = 18.0;

    // Part of the map may be lost
    int regions_height = round(height_map.height() / divide);
    int regions_width = round(height_map.width() / divide);

    int map_reper_height = height_map.height() / 4;
    int map_reper_width = height_map.width() / 4;
    for (int i = 0; i < divide; i++)
    {
        for (int j = 0; j < divide; j++)
        {
            int curr_regions_end_x = regions_width * (i + 1);
            int curr_regions_start_x = regions_width * (i);
            int curr_regions_end_y = regions_height * (j + 1);
            int curr_regions_start_y = regions_height * (j);
            regions.emplace_back(Point(curr_regions_end_x - map_reper_width, 0, curr_regions_end_y - map_reper_height),
                                 Point(curr_regions_start_x - map_reper_width, 0, curr_regions_start_y - map_reper_height));

            for (int y = curr_regions_start_y; y < curr_regions_end_y; y++)
            {
                for (int x = curr_regions_start_x; x < curr_regions_end_x; x++)
                {

                    vec4 tmp = 2 * Vector(x - map_reper_width, (height_map(y, x)).r * height_scale, y - map_reper_height);
                    if (tmp.y <= 3)
                    {
                        tmp.w = 3;
                    }
                    else if (tmp.y <= 10)
                    {
                        tmp.w = 1;
                    }
                    else if (tmp.y <= 20)
                    {
                        tmp.w = 2;
                    }
                    else if (tmp.y <= 30)
                    {
                        tmp.w = 5;
                    }
                    else
                    {
                        tmp.w = 9;
                    }
                    std::cout << tmp.y << std::endl;
                    tmp.y -= 50;
                    // Just here for output
                    const float x_offset = 2 * x;
                    const float y_offset = (height_map(y, x)).r; // Normalize
                    const float z_offset = 2 * y;
                    translation.push_back(tmp);
                }
            }
        }
    }
    divide = 2;
    if (divide == 1)
    {
        regions.emplace_back(Point(height_map.height(), 0, height_map.width()), Point(0, 0, 0));
        return translation.size();
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
    if (pixel.r == 0)
        return 10; // Light
    return 0;      // Default Plain
}

int generateTerrain(std::vector<vec4> &translation, const char *map_name, const char *height_map_name, std::vector<Region> &regions, int divide = 1)
{

    const Image height_map = read_imageI(height_map_name);
    const Image map_map = read_imageI(map_name);
    std::cout << "Starting Terrain generation" << std::endl;
    const float height_scale = 4.0;
    translation.clear();
    // Only made for cube

    regions.clear();
    regions.reserve(divide * divide);

    // Regions individiual size
    // Part of the map may be lost
    int regions_height = round(height_map.height() / divide);
    int regions_width = round(height_map.width() / divide);

    // Simply used to move around the map
    int map_reper_height = height_map.height() / 4;
    int map_reper_width = height_map.width() / 4;
    for (int i = 0; i < divide; i++)
    {
        for (int j = 0; j < divide; j++)
        {
            int curr_regions_end_x = regions_width * (i + 1);
            int curr_regions_start_x = regions_width * (i);
            int curr_regions_end_y = regions_height * (j + 1);
            int curr_regions_start_y = regions_height * (j);
            regions.emplace_back(Point(curr_regions_end_x - map_reper_width, 0, curr_regions_end_y - map_reper_height),
                                 Point(curr_regions_start_x - map_reper_width, 0, curr_regions_start_y - map_reper_height));

            /*          std::cout << "Region " << (i * divide + j) << " Bounding Box:" << std::endl;
                        std::cout << "  Min Point: (" << curr_regions_start_x << ", 0, " << curr_regions_start_y << ")" << std::endl;
                        std::cout << "  Max Point: (" << curr_regions_end_x << ", 0, " << curr_regions_end_y << ")" << std::endl;
            */
            for (int y = curr_regions_start_y; y < curr_regions_end_y; y++)
            {
                for (int x = curr_regions_start_x; x < curr_regions_end_x; x++)
                {

                    // Switch for block
                    vec4 tmp = 2 * Vector(x - map_reper_width, round(((height_map(y, x)).r) / height_scale) - 50, y - map_reper_height); // Scale down the elevation to 10 level
                    tmp.w = getTerrainType(map_map(y, x));
                    // Just herev for output
                    translation.push_back(tmp);
                }
            }
        }
    }

    // Paramétrise

    std::cout << "Terrain Generation Done" << std::endl;

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

void fillWithTexturesNames(std::vector<const char *> &files_names)
{
    files_names.clear();
    files_names.push_back("./data/cube_world/Textures/grassy.png"); // Plain
    files_names.push_back("./data/cube_world/Textures/grass2.png"); // Forest
    files_names.push_back("./data/cube_world/Textures/grass.png");  // OtherPlain
    files_names.push_back("./data/cube_world/Textures/water.png");  // Water
    files_names.push_back("./data/cube_world/Textures/weird.png");  // Hole

    files_names.push_back("./data/cube_world/Textures/granite.png"); // Rocky Rock
    files_names.push_back("./data/cube_world/Textures/stone1.png");  // Gray Rock
    files_names.push_back("./data/cube_world/Textures/pearm.png");   // Beach

    files_names.push_back("./data/cube_world/Textures/bground.png");       // Rock Top
    files_names.push_back("./data/cube_world/Textures/snow2.png");         // Snow
    files_names.push_back("./data/cube_world/Textures/chorus_flower.png"); // Light;

    files_names.push_back("./data/cube_world/Textures/red.png");
    files_names.push_back("./data/cube_world/Textures/weird2.png");
}

// Temporary position

struct shadowMapFBO
{
    const Transform biasMatrix = Transform(
        0.5, 0.0, 0.0, 0.5,
        0.0, 0.5, 0.0, 0.5,
        0.0, 0.0, 0.5, 0.5,
        0.0, 0.0, 0.0, 1.0);
    shadowMapFBO() = default;

    ~shadowMapFBO() =
        default;

    void init(const float &width, const float &height, const char *program)
    {
        // Create the framebuffer
        glGenFramebuffers(1, &depthMapshadowFB);

        // SHadow map texture
        depthMapshadow = make_depth_texture(0, width, height);
        glBindTexture(GL_TEXTURE_2D, depthMapshadowFB);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapshadowFB);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapshadow, 0);

        // Fbo is incomplete without color attachment thus:
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        shadowprog = read_program(program);
        return;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    };

    void BindForWriting()
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthMapshadowFB);
        glUseProgram(shadowprog);
        glClear(GL_DEPTH_BUFFER_BIT);
    };

    void BindForReading(int unit, const char *uniform, GLuint program)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, depthMapshadowFB);
        program_use_texture(program, uniform, unit, depthMapshadow, shadowSampler);
        /*   */
    };

    const Transform &returnBiasMat() const
    {
        return biasMatrix;
    }

    GLuint shadowprog;
    GLuint depthMapshadowFB;
    GLuint depthMapshadow;
    GLuint shadowSampler;
};
