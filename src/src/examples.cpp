#include <examples.hpp>
#include <global.hpp>

void Examples::BatchRendering(Renderer &renderer){
    float x=0.0f,y=0.0f; //creating data for the vertex buffer
    for(int i=0;i<NUM_QUADS;i++){
        renderer.Render(x,y,10.0f,10.0f,1.0f,0.0f);
        x+=10;
        if(x>=SCREEN_WIDTH){
            x=0;
            y+=10;
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
auto [a,b,c,d]=spritesheet.CreateQuadSpriteSheet(100,100,0,1,4,1);
    renderer.Render(a,b,c,d);

    renderer.Draw();
}