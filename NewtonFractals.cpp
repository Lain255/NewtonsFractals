#include <SDL2/SDL.h>
#include "mycomplex.h"


const double sqrt3 = 1.73205080757;
int NUM_ZEROS = 5;
const int MAX_NUM_ZEROS = 5;
const double THRESH = 0.0000001;

complex ZEROS[MAX_NUM_ZEROS] = {
    {1.0, 0.0},
    {0.0, 1.0},
    {-1.0, 0.0},
    {0.0, -1.0},
    {0.0, 0.0}
};
unsigned char ZERO_COLOR[(MAX_NUM_ZEROS+1)*3] = {
      0, 128,  64,
     64,   0, 128,
      0,   0, 196,
      0, 196,   0,
    196,   0,   0,
      0,   0,   0

};

complex POLY_COEFF[MAX_NUM_ZEROS+1];

complex choose(complex* list, int numList, int numChoose) {
    if (numChoose <= 0) {
        return toComplex(1, 0);
    }

    complex sum = toComplex(0, 0);
    for (int i = 0; i < numList-(numChoose-1); i++)
    {
        sum = sum + list[i]*choose(list + (i + 1), numList - (i + 1), (numChoose-1));
    }
    return sum;
}
void ZerosToPoly() {

    for (int i = 0; i < NUM_ZEROS+1; i++)
    {
        int sign = i%2 ? -1 : 1;
        POLY_COEFF[NUM_ZEROS - i] = sign*choose(ZEROS, NUM_ZEROS, i);
    }
}

complex NewtonsMethod (complex z) {
    complex top = POLY_COEFF[0];
    complex bot = {0,0};
    complex zprod = {1,0};
    for (int i = 1; i < NUM_ZEROS+1; i++)
    {
        
        bot = bot + i*POLY_COEFF[i]*zprod;
        zprod = zprod * z;
        top = top + POLY_COEFF[i]*zprod;
    }
    

    complex znext = z - top/bot;
    return znext;
    
}

int NewtonsFractal(double a, double b, int n, int* returnNumIter)
{
    complex z = {a,b};
    for (int i = 1; i < n; i++) {
        z = NewtonsMethod(z); 
        for (int j = 0; j < NUM_ZEROS; j++)
        {
            double dist = EuclidDist(z,ZEROS[j]);
            if (dist < THRESH)
            {
                *returnNumIter = i;
                return j;
            }
        } 
    }
    *returnNumIter = n;
    return NUM_ZEROS;
}
    
int main(int argc, char **argv)
{

    SDL_Init(SDL_INIT_VIDEO);

    const int w = 800;
    const int h = 600;

    SDL_Window *window = SDL_CreateWindow(
        "NewtonsFractal", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        w, 
        h,
        0
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, w, h);

    ZerosToPoly();

    bool quit = SDL_FALSE;
    SDL_Event e;
    double Cx = 0;
    double Cy = 0;
    double zoom = 0.005;
    int maxIter = 10;
    unsigned char* pixels;
    int pitch;
    int binaryView = 1;
    int turbo = 1;
    double buttonWidth = 10;
    int mouseSelect = -1;
    int mousex, mousey;
    
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

    while (!quit) 
    {
        SDL_GetMouseState(&mousex, &mousey);

        while (SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = SDL_TRUE;    
                break;
            case SDL_KEYDOWN:       
                if (e.key.keysym.sym == SDLK_t) {
                    binaryView = !binaryView;
                break;
            }
            case SDL_MOUSEBUTTONUP:
                mouseSelect = -1;
                break;
            case SDL_MOUSEBUTTONDOWN:
                for (int i = 0; i < NUM_ZEROS; i++)
                {
                    double x = (ZEROS[i].a - Cx)/(zoom) + w/2.0;
                    double y = (ZEROS[i].b - Cy)/(zoom) + h/2.0;
                    const SDL_Point mousePoint = {(int) mousex, (int) mousey};
                    const SDL_Rect zeroRect = {(int) (x-buttonWidth/2),(int) (y-buttonWidth/2),(int) buttonWidth,(int) buttonWidth};
                    if (SDL_PointInRect(&mousePoint, &zeroRect)) {
                        mouseSelect = i;
                    }
                }
                
                break;
            }
        }

        if (mouseSelect != -1)
        {
            double x = Cx + zoom * (mousex - w/2.0);
            double y = Cy + zoom * (mousey - h/2.0);
            complex newZero = {x,y};
            ZEROS[mouseSelect] = newZero;
            ZerosToPoly();
        }
        
        
        quit = quit || keyboard[SDL_SCANCODE_ESCAPE];
        turbo = 1 + 4*keyboard[SDL_SCANCODE_LSHIFT];
        Cx += 5*turbo*zoom*(keyboard[SDL_SCANCODE_D] - keyboard[SDL_SCANCODE_A]);    
        Cy += 5*turbo*zoom*(keyboard[SDL_SCANCODE_S] - keyboard[SDL_SCANCODE_W]);
        zoom *= 1 - 0.05*turbo*(keyboard[SDL_SCANCODE_EQUALS] - keyboard[SDL_SCANCODE_MINUS]);
        maxIter += turbo*(keyboard[SDL_SCANCODE_E] - keyboard[SDL_SCANCODE_Q]);
        maxIter = maxIter > 0 ? maxIter : 1;

        
        SDL_LockTexture(texture, NULL, (void**) &pixels, &pitch);
        for (int i = 0; i < w*h; i++) {
            
            double x = Cx + zoom * ((i % w) - w/2.0);
            double y = Cy + zoom * ((i / w) - h/2.0);

            //get rgb from xy-coord
            int numIter = 0;
            int zero = NewtonsFractal(x,y,maxIter, &numIter);
            double value = (double) numIter / (double) maxIter;

            unsigned char r = ZERO_COLOR[3*zero + 0];
            unsigned char g = ZERO_COLOR[3*zero + 1];
            unsigned char b = ZERO_COLOR[3*zero + 2];

            pixels[3*i + 0] = r*(1-value);
            pixels[3*i + 1] = g*(1-value);
            pixels[3*i + 2] = b*(1-value);
        }
        SDL_UnlockTexture(texture);

        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        for (int i = 0; i < NUM_ZEROS; i++) {
            double x = (ZEROS[i].a - Cx)/(zoom) + w/2.0;
            double y = (ZEROS[i].b - Cy)/(zoom) + h/2.0;
            const SDL_FRect zeroRect = {(float)(x-buttonWidth/2),(float)(y-buttonWidth/2),(float) buttonWidth,(float) buttonWidth};
            SDL_RenderFillRectF(renderer, &zeroRect);
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);  //closes window
    SDL_Quit();
    return 0;
}