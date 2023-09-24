double deltaTime=0.0;
double lastFrame=0.0;
bool v_sync=true;

unsigned int SCREEN_WIDTH=1280;
unsigned int SCREEN_HEIGHT=720;

unsigned int NUM_QUADS=(SCREEN_WIDTH/10)*(SCREEN_HEIGHT/10);
unsigned int MAX_QUADS=10000;
unsigned int MAX_VERTICES=4*MAX_QUADS;
unsigned int MAX_INDICES=6*MAX_QUADS;