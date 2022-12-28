#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <thread>
#include <cmath>

struct Point3D {
    float x,y,z;
};

struct Point2D {
    float x,y;
};

Point2D projection(Point3D p,float fov,float aspect,float near,float far,int width,int height) {
    Point2D q;
    float f = 1.0 / tan(fov * 0.5);

    // where
    float centerX = width / 2.0;
    float centerY = height / 2.0;

    // idk what this does but it works so its ok
    q.x = (f * p.x / aspect * (p.z - near)) + centerX;
    q.y = (f * p.y * (p.z - near)) + centerY;
    return q;
}

int main(int argc,char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"2");
    SDL_Window* window = SDL_CreateWindow("Prospettiva lineare",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1920,1080,0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface* surf = SDL_GetWindowSurface(window);
    int width,height;SDL_GetWindowSize(window,&width,&height);
    // faces of the cube
    int faces[6][4] = {
    {0, 1, 2, 3},
    {1, 5, 6, 2},
    {5, 4, 7, 6},
    {4, 0, 3, 7},
    {4, 5, 1, 0},
    {3, 2, 6, 7}
    };
    float angle;
    float x,y,z;
    float protation = 0;
    while(true) {
        Point3D vertices[8] = {
            {-1.0, -1.0, -1.0},
            {1.0, -1.0, -1.0},
            {1.0, 1.0, -1.0},
            {-1.0, 1.0, -1.0},
            {-1.0, -1.0, 1.0},
            {1.0, -1.0, 1.0},
            {1.0, 1.0, 1.0},
            {-1.0, 1.0, 1.0}
        };
        // y axis
        angle = protation * M_PI / 180.0;
        for(int i = 0; i < 8; i++) {
            x = vertices[i].x;
            y = vertices[i].y;
            z = vertices[i].z;
            vertices[i].x = x * cos(angle) + z * sin(angle);
            vertices[i].y = y;
            vertices[i].z = -x * sin(angle) + z * cos(angle);
        }

        // x axis
        angle = -10.0 * M_PI / 180.0;
        for(int i = 0; i < 8; i++) {
            x = vertices[i].x;
            y = vertices[i].y;
            z = vertices[i].z;
            vertices[i].x = x;
            vertices[i].y = y * cos(angle) - z * sin(angle);
            vertices[i].z = y * sin(angle) + z * cos(angle);
        }

        // convert into 2d verticies
        Point2D projectedVertices[8];
        for(int i = 0; i < 8; i++) {
            projectedVertices[i] = projection(vertices[i],M_PI / 8.0,1.0,25.0,10.0,width,height);
        }

        SDL_SetRenderDrawColor(renderer,25,25,25,0.9);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,255,230,0,0.7);
        int poss[4];
        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 4; j++) {
                int a = faces[i][j];
                int b = faces[i][(j + 1) % 4];
                //SDL_Rect square = {projectedVertices[a].x,projectedVertices[a].y,projectedVertices[b].y - projectedVertices[a].y, projectedVertices[b].x - projectedVertices[a].x};
                //std::cout << "a: x " << projectedVertices[a].x << "b: x " << projectedVertices[b].x << " , a: y " << projectedVertices[a].y << " , a: y " << projectedVertices[b].y << std::endl;
                //SDL_RenderDrawRect(renderer,&square);
                SDL_RenderDrawLine(renderer,projectedVertices[a].x,projectedVertices[a].y,projectedVertices[b].x,projectedVertices[b].y);
            }
        }
        SDL_RenderPresent(renderer);
        protation += 0.2;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}