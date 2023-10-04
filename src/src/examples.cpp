#include <examples.hpp>
#include <global.hpp>

void Examples::BatchRendering(Renderer &renderer,Texture &texture){
static unsigned int IMAGE_WIDTH=10,IMAGE_HEIGHT=10;
static unsigned int NUM_QUADS=(SCREEN_WIDTH/IMAGE_WIDTH)*(SCREEN_HEIGHT/IMAGE_HEIGHT);
    float x=0.0f,y=0.0f; //creating data for the vertex buffer
    for(int i=0;i<NUM_QUADS;i++){
        renderer.Render(x,y,(float)IMAGE_WIDTH,(float)IMAGE_HEIGHT,1.0f,(float)texture.GetTexID());
        x+=IMAGE_WIDTH;
        if(x>=SCREEN_WIDTH){
            x=0;
            y+=IMAGE_HEIGHT;
        }
        if(y>=SCREEN_HEIGHT){
            y=0;
            x=0;
        }
    }
    renderer.Draw();

    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("Batch Rendering",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::SliderInt("NUM_QUADS",(int *)&NUM_QUADS,0,100000);
    ImGui::SliderInt("MAX_QUADS",(int *)&MAX_QUADS,1,10000);
    MAX_VERTICES=4*MAX_QUADS;
    MAX_INDICES=6*MAX_QUADS;

    ImGui::Text("DRAW CALLS: %d",DRAW_CALLS);
    ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
    ImGui::End();
}

void Examples::Spritesheet(Renderer &renderer,SpriteSheet &spritesheet){
static unsigned int row=0,col=0;
static float scale=1.0f;
auto [a,b,c,d]=spritesheet.CreateQuadSpriteSheet(100,100,row,col,scale,(float)spritesheet.GetTexID());
    renderer.Render(a,b,c,d);
    renderer.Draw();

    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("Spritesheet",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::SliderInt("Row",(int *)&row,0,10);
    ImGui::SliderInt("Col",(int *)&col,0,10);
    ImGui::SliderFloat("Scale",&scale,0.0f,10.0f);
    ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
    ImGui::End();
}

void Examples::BatchingMultipleTextures(Renderer &renderer,std::vector<Texture*>&t){
static unsigned int IMAGE_WIDTH=10,IMAGE_HEIGHT=10;
static unsigned int NUM_QUADS=(SCREEN_WIDTH/IMAGE_WIDTH)*(SCREEN_HEIGHT/IMAGE_HEIGHT);
    float x=0.0f,y=0.0f; //creating data for the vertex buffer
    for(int i=0;i<NUM_QUADS;i++){
        renderer.Render(x,y,(float)IMAGE_WIDTH,(float)IMAGE_HEIGHT,1.0f,(float)t[i%40]->GetTexID());
        x+=IMAGE_WIDTH;
        if(x>=SCREEN_WIDTH){
            x=0;
            y+=IMAGE_HEIGHT;
        }
        if(y>=SCREEN_HEIGHT){
            y=0;
            x=0;
        }
    }
    renderer.Draw();

    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("Batching Multiple Textures",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("DRAW CALLS: %d",DRAW_CALLS);
    ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
    ImGui::End();
}