#include <examples.hpp>
#include <global.hpp>
#include <iostream>

void Examples::BatchRendering(Renderer &renderer,Texture &texture){
static unsigned int IMAGE_WIDTH=10,IMAGE_HEIGHT=10;
static unsigned int NUM_QUADS=(SCREEN_WIDTH/IMAGE_WIDTH)*(SCREEN_HEIGHT/IMAGE_HEIGHT);
    float x=0.0f,y=0.0f; //creating data for the vertex buffer
    for(int i=0;i<NUM_QUADS;i++){
        renderer.RenderTexture(x,y,(float)IMAGE_WIDTH,(float)IMAGE_HEIGHT,1.0f,1.0f,(float)texture.GetTexID());
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
auto [a,b,c,d]=spritesheet.CreateQuadSpriteSheet(SCREEN_WIDTH/2-(32*scale)/2,SCREEN_HEIGHT/2-(32*scale)/2,row,col,scale,1.0f,(float)spritesheet.GetTexID());
    renderer.RenderQuad(a,b,c,d);
    renderer.Draw();

    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("Spritesheet",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::SliderInt("Row",(int *)&row,0,(672/32)-1);
    ImGui::SliderInt("Col",(int *)&col,0,(704/32)-1);
    ImGui::SliderFloat("Scale",&scale,0.0f,10.0f);
    ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
    ImGui::End();
}

void Examples::BatchingMultipleTextures(Renderer &renderer,std::vector<Texture*>&t){
static unsigned int IMAGE_WIDTH=20,IMAGE_HEIGHT=20;
static unsigned int NUM_QUADS=(SCREEN_WIDTH/IMAGE_WIDTH)*(SCREEN_HEIGHT/IMAGE_HEIGHT);
    float x=0.0f,y=0.0f; //creating data for the vertex buffer
    for(int i=0;i<NUM_QUADS;i++){
        renderer.RenderTexture(x,y,(float)IMAGE_WIDTH,(float)IMAGE_HEIGHT,1.0f,1.0f,(float)t[i%t.size()]->GetTexID());
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

    ImGui::Text("NUM QUADS: %d",NUM_QUADS);
    ImGui::Text("DRAW CALLS: %d",DRAW_CALLS);
    ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
    ImGui::End();
}

void Examples::DepthTest(Renderer &renderer,Texture &t1,Texture &t2){
static unsigned int depth1=1,depth2=0;
    renderer.RenderTexture(SCREEN_WIDTH/2-160,SCREEN_HEIGHT/2-160,32,32,10,depth1,t1.GetTexID());
    renderer.RenderTexture(SCREEN_WIDTH/2-80,SCREEN_HEIGHT/2-80,32,32,10,depth2,t2.GetTexID());
    renderer.Draw();

    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("Batching Multiple Textures",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::SliderInt("Depth 1:",(int *)&depth1,0,1);
    ImGui::SliderInt("Depth 2:",(int *)&depth2,0,1);
    ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
    ImGui::End();
}

void Examples::LinesPoints(Renderer &renderer){
static unsigned int point_x=600,point_y=250;
static float point_colors[4]={255,255,255,1};
static unsigned int line_x1=100,line_y1=100,line_x2=500,line_y2=300;
static float line_colors[4]={255,255,255,1};

    renderer.RenderLine(line_x1,line_y1,line_x2,line_y2,&line_colors[0]);
    renderer.RenderPoint(point_x,point_y,point_colors[0],point_colors[1],point_colors[2],point_colors[3]);
    renderer.Draw();

    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("Point/Lines Rendering",(bool *)__null,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::SliderInt("POINT_X:",(int *)&point_x,0,SCREEN_WIDTH);
    ImGui::SliderInt("POINT_Y:",(int *)&point_y,0,SCREEN_HEIGHT);
    ImGui::SliderInt("LINE_X1:",(int *)&line_x1,0,SCREEN_WIDTH);
    ImGui::SliderInt("LINE_Y1:",(int *)&line_y1,0,SCREEN_HEIGHT);
    ImGui::SliderInt("LINE_X2:",(int *)&line_x2,0,SCREEN_WIDTH);
    ImGui::SliderInt("LINE_Y2:",(int *)&line_y2,0,SCREEN_HEIGHT);
    ImGui::ColorEdit4("POINT COLOR:",&point_colors[0]);
    ImGui::ColorEdit4("LINE COLOR:",&line_colors[0]);
    ImGui::SetWindowPos(ImVec2(SCREEN_WIDTH/2-ImGui::GetWindowWidth()/2,0));
    ImGui::End();
}