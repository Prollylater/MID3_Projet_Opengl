// exemple : lecture et affichage d'un mesh, avec un shader de base + gestion des evenements par window.h

#include "vec.h"
#include "mat.h"

#include "window.h"
#include "glcore.h"

#include "mesh_io.h"
#include "buffers.h"
#include "draw.h"


GLuint vao= 0;
unsigned count= 0;

bool init( )
{
    std::vector<Point> positions;
    std::vector<unsigned> indices;
    
    if(!read_indexed_positions("../data/robot.obj", positions, indices))
        return false;   // erreur de lecture
    
    vao= create_buffers(positions, indices);
    count= indices.size();
    
    return true;
}

void quit( ) 
{
    release_buffers(vao);
}


void draw( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Transform model;    // placer le modele
    Transform view= Translation(0, -2, -10);     // camera
    Transform projection = Perspective(45, 1024.0 / 576.0, 0.1, 100);
    
    // super pratique, mais il faut faire traiter les evenements par la fonction events() de window.h
    if(key_state(' '))
        model= Translation(1, 0, 0);
    
    
    draw(vao, GL_TRIANGLES, count, model, view, projection);
}


int main( int argc, char **argv )
{
    Window window= create_window(1024, 576);
    Context context= create_context(window);
    
    // etat openGL de base / par defaut
    glViewport(0, 0, 1024, 576);
    glClearColor(0.2, 0.2, 0.2, 1);
    glClearDepth(1);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    
    if(!init())
        return 1;
    
    while(events(window))
    {
        // dessiner
        draw();
        
        // presenter / montrer le resultat
        SDL_GL_SwapWindow(window);
    }
    
    quit();
    
    release_context(context);
    release_window(window);
    return 0;
}
