// exemple : lecture et affichage d'un mesh, avec un shader de base + gestion des evenements par window.h

#include "vec.h"

#include "mat.h"

#include "window.h"

#include "glcore.h"

#include "mesh_io.h"

#include "buffers.h"

#include "program.h"

#include "uniforms.h"

#include "texture.h"


#include <algorithm>


GLuint program = 0;
GLuint groundprog = 0;

GLuint skyboxprog = 0;
GLuint outlineprog = 0;

GLuint vao = 0;
GLuint skyboxvao = 0;

GLuint texture = 0;
GLuint skyboxtexture = 0;

unsigned count = 0;
unsigned countground = 0;
Point pmin;
Point pma;

GLuint mbcgVerticesbsynagfosplanevao, samebutvbo = 0;

void initground() {
    // From someone

}

GLuint shadowsampler;

struct ShadowMapFBO {
    ShadowMapFBO() {};

    ~ShadowMapFBO() =
        default;

    void init() {
        //Create the framebuffer
        glGenFramebuffers(1, & depthMapshadowFB);

        //SHadow map texture
        // depthMapshadow = make_flat_texture(2, 1024.0, 576.0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

        glGenTextures(1, & depthMapshadow);
        glBindTexture(GL_TEXTURE_2D, depthMapshadow);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024.0, 576.0, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        // fixe les parametres de filtrage par defaut
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // glBindFramebuffer(GL_FRAMEBUFFER, depthMapshadowFB);
        //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapshadow, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapshadowFB);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapshadow, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        shadowprog = read_program("projets/tp4shad.glsl");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    };

    void BindForWriting() {
        glViewport(0, 0, 1024, 576);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthMapshadowFB);

        // glBindFramebuffer(GL_FRAMEBUFFER, depthMapshadowFB);
        glBindTexture(GL_TEXTURE_2D, depthMapshadow);
        glClear(GL_DEPTH_BUFFER_BIT);

    };

    void BindForReading() {
        //glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_2D, depthMapshadow);

    };

    GLuint shadowprog = 0;
    GLuint depthMapshadowFB = 0;
    GLuint depthMapshadow = 0;
};

ShadowMapFBO shadowstuf;

unsigned int textureID = 0;

bool init() {
    initground();
    shadowstuf.init();
    glGenSamplers(1, & shadowsampler);
    glSamplerParameteri(shadowsampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(shadowsampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::vector < Point > positions;
    std::vector < Point > textures;
    std::vector < unsigned > indices;
    std::vector < Point > positionssky;
    std::vector < unsigned > indicessky;

    MeshIOData groundcharact;

    std::vector < Point > positionsground;
    std::vector < unsigned > indicesground;

    std::vector < int > material_indices; //Replace by MeshIO struct

    if (!read_indexed_positions("./data/hearthspring.obj", positions, indices, textures, material_indices))
        return false; // erreur de lecture

    if (!read_indexed_positions("./data/skyb.obj", positionssky, indicessky))
        return false; // erreur de lecture

    if (!read_meshio_data("./data/ground.obj", groundcharact))
        return false; // erreur de lecture

    Point pmin = positions[0];
    Point pmax = positions[0];
    for (unsigned i = 1; i < positions.size(); i++) {
        pmin = min(pmin, positions[i]);
        pmax = max(pmax, positions[i]);
    }

    std::cout << positions.size() << std::endl;
    std::cout << textures.size() << std::endl;

    //Calculate normal
    std::vector < Vector > normals(positions.size());
    for (int i = 0; i < indices.size(); i += 3) {
        Point v0 = positions[indices[i]];
        Point v1 = positions[indices[i + 1]];
        Point v2 = positions[indices[i + 2]];
        Vector curr_normal = normalize(cross(v1 - v0, v2 - v0));

        normals[indices[i]] = normals[indices[i]] + curr_normal;
        normals[indices[i + 1]] = normals[indices[i + 1]] + curr_normal;
        normals[indices[i + 2]] = normals[indices[i + 2]] + curr_normal;
    }
    std::for_each(normals.begin(), normals.end(), [](Vector & n) {
        n = normalize(n);
    });

    //Load texture
    texture = read_texture(0, "./data/hearthspring.png");
    skyboxtexture = make_skybox();

    vao = create_buffers(positions, indices, textures, normals);
    mbcgVerticesbsynagfosplanevao = create_buffers(groundcharact.positions, groundcharact.indices,
        groundcharact.texcoords, groundcharact.normals);

    skyboxvao = create_buffers(positionssky, indicessky);

    count = indices.size();
    countground = groundcharact.indices.size();

    // charge et compile les shaders.
    program = read_program("projets/tp4.glsl");

    //outlineprog= read_program("projets/tp4oultine.glsl");
    skyboxprog = read_program("projets/tp4skyb.glsl");

    groundprog = read_program("projets/tp4ground.glsl");

    // affiche les erreurs de maniere lisible.
    program_print_errors(program);
    // affiche les erreurs de maniere lisible.
    program_print_errors(skyboxprog);
    // affiche les erreurs de maniere lisible.
    program_print_errors(groundprog);
    if (!program_ready(program))
        return false; // erreur de compilation / link / chargement des shaders

    return true;
};
//Global cause why not
Transform toWorldTr = Translation(0., 0.5, 3.);
Transform toWorldRt = Identity();
Transform toWorldsc = Identity();
Transform toWorldrotate = Identity();

void input(Transform & translation, Transform & rotation, Transform & rotationworld, Transform & scale) {
    static int v;
    static int i;

    if (key_state('p')) {
        v = (v + 1) % 360;
    }
    if (key_state('o')) {
        v = (v - 1) % 360;
    }

    if (key_state('w')) {
        rotation = RotationY(v);
        rotationworld = RotationY(v);

    }

    if (key_state('x')) {
        rotation = RotationX(v);
    }
    if (key_state('c')) {
        rotation = RotationZ(v);

    }

    //Looking around the skybox without moving the character
    if (key_state('a')) {
        rotationworld = RotationY(v);
    }

    static int fov;

    if (key_state('\t')) {
        translation = Identity() * Translation(0.0, 0.5, 3.);
        rotation = Identity();
        scale = Identity();
    }

    return;

};

void draw(int mode, int modeb) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/ );
    input(toWorldTr, toWorldRt, toWorldrotate, toWorldsc);
    Transform projection = Perspective(200, 1024.0 / 576.0, 0.1, 100);
    Transform model = Identity(); // placer le modele coordonnées des sommets

    //Model data
    Transform view = Inverse(model * toWorldRt * RotationX(mode) * RotationY(modeb) * toWorldTr); // camera
    Transform mvp = projection * view * model;
    Transform mv = view * model;

    glBindVertexArray(vao);

    //SHadow pass
    shadowstuf.BindForWriting();
    //Using the FrameBuffer for Shadow

    // Compute the MVP matrix from the light's point of view,
    Point lightInvDir = Point(1., 10., 1.);

    //Random values till renderdoc output put out a decent shadow
    Transform depthProjectionMatrix = Ortho(-1.5, 1.5, -1.5, 1.5, 0.1, 100.0);
    Transform depthViewMatrix = Lookat(lightInvDir, Point(0, 0, 0), Vector(0, 1, 0));
    Transform depthModelMatrix = Identity();
    Transform depthmvp = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    glUseProgram(shadowstuf.shadowprog);

    program_uniform(shadowstuf.shadowprog, "mvpMatrix", depthmvp);

    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(mbcgVerticesbsynagfosplanevao);
    // Render the ground plane

    program_uniform(shadowstuf.shadowprog, "mvpMatrix", depthmvp);
    glBindTexture(GL_TEXTURE_2D, shadowstuf.depthMapshadow);

    glDrawElements(GL_TRIANGLES, countground, GL_UNSIGNED_INT, nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //////////////////////////////////////////////////
    //RENDERING THE SCENE FR//
    /////////////////////////////////////////////////

    glViewport(0, 0, 1024, 576);

    glBindVertexArray(skyboxvao);
    Transform viewp = Inverse(Identity() * toWorldrotate * RotationX(180) * Identity()); // camera
    Transform mvpb = projection * viewp * Identity();

    //Skybox pass

    glDepthMask(GL_FALSE);
    glUseProgram(skyboxprog);
    glBindVertexArray(skyboxvao);
    program_uniform(skyboxprog, "mvpMatrix", mvpb);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);

    //GRound pass
    viewp = Inverse( Translation(0.0, 1.2, 6.0)); // camera
    mvpb = projection * viewp * Identity();
    glBindVertexArray(mbcgVerticesbsynagfosplanevao);
    // Render the ground plane
    glUseProgram(groundprog);
    program_uniform(groundprog, "mvpMatrix", mvpb);

    program_uniform(groundprog, "lightspaceMatrix", depthmvp);
    glBindTexture(GL_TEXTURE_2D, shadowstuf.depthMapshadow);
    glDrawElements(GL_TRIANGLES, countground, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(vao);

    glClear(GL_DEPTH_BUFFER_BIT);

    //Model pass
    glUseProgram(program);

    glViewport(0, 0, 1024, 576);

    glBindTexture(GL_TEXTURE_2D, texture);
    program_use_texture(program, "shadowsampler", 0, texture);

    //Export toward the glsl shader those damakta
    program_uniform(program, "mMatrix", model);
    program_uniform(program, "vMatrix", view);
    program_uniform(program, "mvpMatrix", mvp);
    program_uniform(program, "lightInvDir", lightInvDir);

    program_uniform(program, "lightspaceMatrix", depthmvp);
    program_use_texture(program, "shadowsampler", 1, shadowstuf.depthMapshadow);

    //glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

}

void quit() {
    release_buffers(vao);
}

int main(int argc, char ** argv) {
    Window window = create_window(1024, 576);
    Context context = create_context(window);

    // etat openGL de base / par defaut
    glClearColor(0.2, 0.2, 0.2, 1);
    glClearDepth(1);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);
    //sfail: action to take if the stencil test fails.
    //dpfail: action to take if the stencil test passes, but the depth test fails.
    //dppass: action to take if both the stencil and the depth test pass.
    //glStencilOp(GL_KEEP,GL_KEEP, GL_REPLACE);

    if (!init())
        return 1;

    int mode = 0;
    int modeb = 0;

    if (argc >= 2) {

        mode = atoi(argv[1]);
    }

    if (argc == 3) {

        modeb = atoi(argv[2]);
    }

    while (events(window)) {
        // dessiner
        draw(mode, modeb);

        // presenter / montrer le resultat
        SDL_GL_SwapWindow(window);
    }

    quit();

    release_context(context);
    release_window(window);
    return 0;
}