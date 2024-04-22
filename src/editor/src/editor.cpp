#include <pch.hpp>
#include <editor.hpp>
#include <window.hpp>
#include <utilities.hpp>
#include <scene.hpp>
#include <sceneserializer.hpp>
#include <IconsFontAwesome6.h>
#include <scene_buttons.hpp>
#include <buttons.hpp>
#include <global.hpp>
#include <Instrumentor.h>
#include <gizmo.hpp>

#define baseFontSize 24.0f
#define STRLEN 100

std::string WINDOW_NAME;
unsigned int WINDOW_WIDTH;
unsigned int WINDOW_HEIGHT;
unsigned int FULLSCREEN_WIDTH;
unsigned int FULLSCREEN_HEIGHT;
std::string SCENE_PATH;
bool RESIZABLE;

Editor::Editor(unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height,bool resizable){
    Window::BaseWidth=Window::Width=width;
    Window::BaseHeight=Window::Height=height;
    Window::FullscreenWidth=fullscreen_width;
    Window::FullscreenHeight=fullscreen_height;
    Window::MAX_HEIGHT=Window::MAX_WIDTH/(Window::Width/Window::Height);

    Window::InitGlfwWindow("Editor",resizable);
    Window::ProjUpdate=true;

    m_SceneFramebuffer=new Framebuffer;
    m_Scene=new Scene;
    m_SceneSerializer=new SceneSerializer;
    m_SceneSerializer->SetScene(m_Scene);
    RENDERER->ImGui_Init();

    //m_SceneSerializer->Deserialize("test.scene",m_ScriptComponents);
    //DeserializeProject();
    //m_SceneSerializer->SerializeEncrypted("test_encrypted.scene");
    //m_SceneSerializer->DeserializeEncrypted("test_encrypted.scene");

    ImGuiIO &io=ImGui::GetIO();
    io.Fonts->AddFontDefault();
    float iconFontSize=baseFontSize*2.0f/3.0f;

    static const ImWchar icons_ranges[]={ICON_MIN_FA,ICON_MAX_FA,0};
    ImFontConfig icons_config;
    icons_config.MergeMode=true;
    icons_config.PixelSnapH=true;
    icons_config.GlyphMinAdvanceX=iconFontSize;
    if(io.Fonts->AddFontFromFileTTF("vendor/FontAwesome/fa-solid-900.ttf",iconFontSize,&icons_config,icons_ranges)==NULL){
        printf("Failed to load FontAwesome font\n");
    }

    m_UpdateFiles=false;
    m_HdrOpen=false;

    m_SelectedScene=-1;
}

Editor::~Editor(){
    delete m_Scene;
    delete m_SceneFramebuffer;
    RENDERER->ImGui_Close();
}

void Editor::Run(){
    while(!glfwWindowShouldClose(Window::Window)){
        glfwPollEvents();

        if(Window::ProjUpdate){
            m_Camera.InitializeProj();
        }

        INPUT->UpdateStates();
        HandleInputs();
        #ifdef ENABLE_PROFILING
            KeyState p=INPUT->GetKey(KEY_P);
            if(p.current && !p.previous){
                Instrumentor::Get().BeginSession("Editor","Editor.json");
            }
        #endif


        RENDERER->StartEditorScene(this);
        m_Camera.ResetSceneProj();
        OnSceneRender();
        m_Camera.DrawSceneProj();
        RENDERER->DrawEditorScene(m_SceneFramebuffer);
        HighlightEntity(m_SelectedEntity);

        Entity *e=m_Scene->GetEntity(m_SelectedEntity);

        if(e){
            glm::mat4 matrix=glm::translate(glm::mat4(1.0f),glm::vec3(e->m_X,e->m_Y,0.0f));
            m_Gizmo.Manipulate(matrix,m_Camera.GetViewMatrix(),m_Camera.GetProjMatrix());
            e->m_X=matrix[3][0];
            e->m_Y=matrix[3][1];
        }
        
        RENDERER->StartScene();
        RENDERER->Clear({0.0,0.0,0.0,1.0});
        Renderer::ImGui_Start_Frame();
        Renderer::ImGui_Theme();
        
        OnImGuiUpdate();
        OnImGuiRender();
        RENDERER->DrawScene();

        glfwSwapBuffers(Window::Window);

        #ifdef ENABLE_PROFILING
            if(p.current && !p.previous){
                Instrumentor::Get().EndSession();
            }
        #endif
    }
}

void Editor::OnSceneRender(){
    m_Scene->Render();
}

void Editor::HighlightEntity(uint32_t uid){
    auto e=m_Scene->GetEntity(uid);

    if(!e)
        return;

    RENDERER->SetLineWidth(0.05f);

    auto tc=m_Scene->GetComponent<TextureComponent>(uid);
    if(tc){
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)},{Interpolate(e->m_X,e->m_PreviousX)+tc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)},{0,0,1,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+tc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)},{Interpolate(e->m_X,e->m_PreviousX)+tc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)+tc->m_Height},{0,0,1,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+tc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)+tc->m_Height},{Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)+tc->m_Height},{0,0,1,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)+tc->m_Height},{Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)},{0,0,1,1},5);
    
        Window::VertexCount-=8;
    }

    auto atc=m_Scene->GetComponent<AnimatedTextureComponent>(uid);
    if(atc){
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)},{Interpolate(e->m_X,e->m_PreviousX)+atc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)},{1,0.5f,0,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+atc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)},{Interpolate(e->m_X,e->m_PreviousX)+atc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)+atc->m_Height},{1,0.5f,0,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+atc->m_Width,Interpolate(e->m_Y,e->m_PreviousY)+atc->m_Height},{Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)+atc->m_Height},{1,0.5f,0,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)+atc->m_Height},{Interpolate(e->m_X,e->m_PreviousX),Interpolate(e->m_Y,e->m_PreviousY)},{1,0.5f,0,1},5);
    
        Window::VertexCount-=8;
    }

    auto bc=m_Scene->GetComponent<BoxColliderComponent>(uid);
    if(bc){
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_YOffset},{Interpolate(e->m_X,e->m_PreviousX)+bc->m_Width+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_YOffset},{0,1,0,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+bc->m_Width+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_YOffset},{Interpolate(e->m_X,e->m_PreviousX)+bc->m_Width+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_Height+bc->m_YOffset},{0,1,0,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+bc->m_Width+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_Height+bc->m_YOffset},{Interpolate(e->m_X,e->m_PreviousX)+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_Height+bc->m_YOffset},{0,1,0,1},5);
        RENDERER->DrawLine({Interpolate(e->m_X,e->m_PreviousX)+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_Height+bc->m_YOffset},{Interpolate(e->m_X,e->m_PreviousX)+bc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+bc->m_YOffset},{0,1,0,1},5);
    
        Window::VertexCount-=8;
    }
    auto cc=m_Scene->GetComponent<CircleColliderComponent>(uid);
    if(cc){
        float current_point_size=RENDERER->GetPointSize();
        RENDERER->SetPointSize(cc->m_Radius*2);
        RENDERER->DrawCircle({Interpolate(e->m_X,e->m_PreviousX)+cc->m_XOffset,Interpolate(e->m_Y,e->m_PreviousY)+cc->m_YOffset},{1,0,0,1.0f},0.02f,5);
        RENDERER->Render();
        RENDERER->SetPointSize(current_point_size);

        Window::VertexCount-=1;
    }

    RENDERER->Render();
}

void Editor::OnImGuiUpdate(){
    ImVec2 tempvec;
    float tempf;

    if(ImGui::BeginMainMenuBar()){
        tempvec=ImGui::GetWindowSize();
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("New")){
                m_SelectedScene=-1;
                m_SelectedEntity=std::numeric_limits<uint32_t>::max();
                m_ScenesPaths.clear();
                delete m_Scene;
                m_Scene=new Scene;
                m_ProjectPath="";
                m_CurrentPath="";
                m_ScriptComponents.clear();

                WINDOW_NAME="";
                WINDOW_NAME.resize(STRLEN);
                WINDOW_WIDTH=0;
                WINDOW_HEIGHT=0;
                FULLSCREEN_WIDTH=0;
                FULLSCREEN_HEIGHT=0;
                SCENE_PATH="";
                SCENE_PATH.resize(STRLEN);
                RESIZABLE=false;

                RENDERER->SetTonemapType(TonemapType::None);
                RENDERER->SetGamma(0);
                RENDERER->SetExposure(0);

                NEXT_UID=0;
                RENDERER->m_AmbientLight=Vec3(1.0f,1.0f,1.0f);
                RENDERER->m_ClearColor=Vec3(0.0f,0.0f,0.0f);
            }
            if(ImGui::MenuItem("Open")){
                nfdchar_t *outPath=NULL;
                nfdresult_t result=NFD_OpenDialog("proj",NULL,&outPath);
                    
                if(result==NFD_OKAY){
                    m_ProjectPath=outPath;
                    m_CurrentPath=std::filesystem::path(outPath).parent_path().string();
                    DeserializeProject();
                    // m_SceneSerializer->SetScene(m_Scene);
                    // m_SceneSerializer->Deserialize((char*)outPath,m_ScriptComponents);
                }else if(result==NFD_ERROR){
                    printf("Error: %s\n",NFD_GetError());
                }
            }
            if(ImGui::MenuItem("Save")){
                if(m_SelectedScene!=-1){
                    m_SceneSerializer->Serialize(m_CurrentPath+"/resources/scenes/"+m_ScenesPaths[m_SelectedScene],m_ScriptComponents);
                }
                SerializeProject();
            }
            if(ImGui::MenuItem("Save As")){
                nfdchar_t *outPath=NULL;
                nfdresult_t result=NFD_SaveDialog("proj",NULL,&outPath);
                    
                if(result==NFD_OKAY){
                    m_ProjectPath=outPath;
                    m_CurrentPath=std::filesystem::path(outPath).parent_path().string();
                    SerializeProject();
                    m_SceneSerializer->Serialize(m_CurrentPath+"/resources/scenes/"+m_ScenesPaths[m_SelectedScene],m_ScriptComponents);
                }else if(result==NFD_ERROR){
                    printf("Error: %s\n",NFD_GetError());
                }
            }
            if(ImGui::MenuItem("Exit")){
                glfwSetWindowShouldClose(Window::Window,true);
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Edit")){
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Reload")){
            if(ImGui::MenuItem("Reload Shaders")){
                RENDERER->ReloadShaders();
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Effects")){
            if(ImGui::MenuItem("HDR")){
                m_HdrOpen=true;
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Scenes")){
            m_ScenesOpen=true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        if(m_HdrOpen){
            HdrWindow(&m_HdrOpen);
        }
        if(m_ScenesOpen){
            ScenesWindow(&m_ScenesOpen);
        }
    }

    EntitiesMenu({0,tempvec.y});
    ComponentsMenu({0,(Window::Height-tempvec.y)/2.0f+tempvec.y});
    DataMenu({GetWidthPercentageInPx(80),tempvec.y});
    VariablesMenu({GetWidthPercentageInPx(80),(Window::Height-tempvec.y)/2.0f+tempvec.y});

    ImGui::Begin("Viewport",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(ImVec2(GetWidthPercentageInPx(20),tempvec.y));
    tempf=GetWidthPercentageInPx(60);
    ImGui::SetWindowSize(ImVec2(tempf,tempf*(Window::Height/Window::Width)));

    m_ScenePos={GetWidthPercentageInPx(20)/Window::Width*Window::MAX_WIDTH,Window::MAX_HEIGHT-(tempvec.y/Window::Height*Window::MAX_HEIGHT)};
    m_SceneSize={tempf/Window::Width*Window::MAX_WIDTH,tempf*(Window::Height/Window::Width)/Window::Height*Window::MAX_HEIGHT};
    
    m_Gizmo.SetViewPortPos(glm::vec2(GetWidthPercentageInPx(20),Window::Height-tempvec.y));
    m_Gizmo.SetViewPortSize(glm::vec2(tempf,tempf*(Window::Height/Window::Width)));
    ImGui::Image((void*)m_SceneFramebuffer->GetColorbufferID(),ImGui::GetContentRegionAvail(),ImVec2(0,1),ImVec2(1,0));
    
    ImGui::SameLine();
    ImVec2 pos=ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(pos.x-GetWidthPercentageInPx(60)/2.0f,pos.y+baseFontSize/2.0f));
    if(ImGui::Button(ICON_FA_PLAY,ImVec2(baseFontSize*1.5f,baseFontSize*1.5f))){
        SceneButtons::PlayButton(m_ScriptComponents);
    }

    Vec2 cameraPos=m_Camera.GetPosition();

    ImGui::SetCursorPos({10,10});
    ImGui::Text("(%.2f, %.2f)",cameraPos.x,cameraPos.y);
    ImGui::SameLine();
    if(ImGui::Button("Reset")){
        m_Camera.SetPosition({0,0});
    }
    
    ImGui::End();

    FileBrowserMenu({GetWidthPercentageInPx(20),tempvec.y+tempf*(Window::Height/Window::Width)});
}

void Editor::EntitiesMenu(ImVec2 pos){
    ImGui::Begin("Entities",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(pos);
    ImGui::SetWindowSize(ImVec2(GetWidthPercentageInPx(20),(Window::Height-pos.y)/2.0f));

    auto entities=m_Scene->GetEntities();
    int k=0;
    for(auto &e:entities){
        std::string name;
        if(m_Scene->GetComponent<TagComponent>(e.m_UID)!=nullptr){
            name=m_Scene->GetComponent<TagComponent>(e.m_UID)->m_Tag;
        }else{
            name="Entity "+std::to_string(k);
        }

        ImGui::PushID(k);

        if(ImGui::Selectable(name.c_str())){
            if(m_SelectedEntity!=e.m_UID){
                m_SelectedEntity=e.m_UID;
            }else{
                m_SelectedEntity=std::numeric_limits<uint32_t>::max();
            }
        }
        ImGui::OpenPopupOnItemClick("EntityPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("EntityPopup")){
            if(ImGui::MenuItem("Remove Entity")){
                m_Scene->RemoveEntity(e.m_UID);
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
        k++;
    }

    if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)){
        ImGui::OpenPopup("CreateEntityPopup");
    }

    if(ImGui::BeginPopup("CreateEntityPopup")){
        if(ImGui::MenuItem("Create Entity")){
            m_Scene->AddEntity();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void Editor::ComponentsMenu(ImVec2 pos){
    ImGui::Begin("Components",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(pos);
    ImGui::SetWindowSize(ImVec2(GetWidthPercentageInPx(20),Window::Height-pos.y));

    Entity *e=m_Scene->GetEntity(m_SelectedEntity);
    if(e){
        ImGui::Text("Entity: %s",(m_Scene->GetComponent<TagComponent>(m_SelectedEntity))?m_Scene->GetComponent<TagComponent>(m_SelectedEntity)->m_Tag.c_str():"");
        ImGui::SliderFloat("X",&e->m_X,-30.0f,30.0f);
        ImGui::SliderFloat("Y",&e->m_Y,-30.0f,30.0f);
        e->m_PreviousX=e->m_X;
        e->m_PreviousY=e->m_Y;
    }

    ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.5f);

    TagComponent *tag_component=m_Scene->GetComponent<TagComponent>(m_SelectedEntity);
    if(tag_component){
        if(StartNode("TagComponent")){
            ImGui::InputText("Tag",&tag_component->m_Tag[0],tag_component->m_Tag.size());
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("TagComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("TagComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<TagComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    TextureComponent *texture_component=m_Scene->GetComponent<TextureComponent>(m_SelectedEntity);
    if(texture_component){
        if(StartNode("TextureComponent")){
            if(ImGui::Button("Reload Texture")){
                Texture *t=texture_component->m_Texture.get();

                texture_component->m_Texture=TEXTURES_MANAGER->UpdateTexture(t->m_TexID,t->m_FilePath,t->m_MagFilter,t->m_MinFilter).second;   
            }

            ImGui::InputText("Path",&texture_component->m_Texture.get()->m_FilePath[0],STRLEN);

            int selectedMagFilter,selectedMinFilter;

            if(texture_component->m_Texture.get()->m_MagFilter==GL_LINEAR){
                selectedMagFilter=0;
            }else{
                selectedMagFilter=1;
            }

            if(texture_component->m_Texture.get()->m_MinFilter==GL_LINEAR){
                selectedMinFilter=0;
            }else{
                selectedMinFilter=1;
            }

            ImGui::Combo("Mag Filter",&selectedMagFilter,"Linear\0Nearest\0");
            ImGui::Combo("Min Filter",&selectedMinFilter,"Linear\0Nearest\0");

            if(selectedMagFilter==0){
                texture_component->m_Texture.get()->m_MagFilter=GL_LINEAR;
            }else{
                texture_component->m_Texture.get()->m_MagFilter=GL_NEAREST;
            }

            if(selectedMinFilter==0){
                texture_component->m_Texture.get()->m_MinFilter=GL_LINEAR;
            }else{
                texture_component->m_Texture.get()->m_MinFilter=GL_NEAREST;
            }

            ImGui::SliderFloat("Width",&texture_component->m_Width,0.0f,30.0f);
            ImGui::SliderFloat("Height",&texture_component->m_Height,0.0f,30.0f);
            ImGui::SliderInt("Layer",&texture_component->m_Layer,-100,100);
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("TextureComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("TextureComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<TextureComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    AnimatedTextureComponent *animated_texture_component=m_Scene->GetComponent<AnimatedTextureComponent>(m_SelectedEntity);
    if(animated_texture_component){
        if(StartNode("AnimatedTextureComponent")){
            if(ImGui::Button("Reload Texture")){
                SpriteSheet *s=animated_texture_component->m_AnimatedTexture.get();

                animated_texture_component->m_AnimatedTexture=TEXTURES_MANAGER->UpdateSpriteSheet(s->m_TexID,s->m_FilePath,s->m_TileWidth,s->m_TileHeight,s->m_MagFilter,s->m_MinFilter).second;   
            }

            ImGui::InputText("Path",&animated_texture_component->m_AnimatedTexture.get()->m_FilePath[0],STRLEN);
            
            int selectedMagFilter,selectedMinFilter;

            if(animated_texture_component->m_AnimatedTexture.get()->m_MagFilter==GL_LINEAR){
                selectedMagFilter=0;
            }else{
                selectedMagFilter=1;
            }

            if(animated_texture_component->m_AnimatedTexture.get()->m_MinFilter==GL_LINEAR){
                selectedMinFilter=0;
            }else{
                selectedMinFilter=1;
            }
            
            ImGui::Combo("Mag Filter",&selectedMagFilter,"Linear\0Nearest\0");
            ImGui::Combo("Min Filter",&selectedMinFilter,"Linear\0Nearest\0");

            if(selectedMagFilter==0){
                animated_texture_component->m_AnimatedTexture.get()->m_MagFilter=GL_LINEAR;
            }else{
                animated_texture_component->m_AnimatedTexture.get()->m_MagFilter=GL_NEAREST;
            }

            if(selectedMinFilter==0){
                animated_texture_component->m_AnimatedTexture.get()->m_MinFilter=GL_LINEAR;
            }else{
                animated_texture_component->m_AnimatedTexture.get()->m_MinFilter=GL_NEAREST;
            }

            ImGui::SliderInt("Tile Width",(int*)&animated_texture_component->m_AnimatedTexture.get()->m_TileWidth,0,500);
            ImGui::SliderInt("Tile Height",(int*)&animated_texture_component->m_AnimatedTexture.get()->m_TileHeight,0,500);

            ImGui::SliderFloat("Width",&animated_texture_component->m_Width,0.0f,30.0f);
            ImGui::SliderFloat("Height",&animated_texture_component->m_Height,0.0f,30.0f);
            ImGui::SliderInt("Layer",&animated_texture_component->m_Layer,-100,100);

            SpriteSheet *s=animated_texture_component->m_AnimatedTexture.get();
            ImGui::Checkbox("Play Animation",&animated_texture_component->m_PlayAnimation);
            ImGui::Checkbox("Loop Animation",&animated_texture_component->m_LoopAnimation);
            ImGui::SliderFloat("Animation Delay",&animated_texture_component->m_AnimationDelay,0.0f,10.0f);
            ImGui::SliderInt("Animation Index",&animated_texture_component->m_AnimationIndex,0,ceil(((float)s->m_Width)/((float)s->m_TileWidth))-1);
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("AnimatedTextureComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("AnimatedTextureComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<AnimatedTextureComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    RigidbodyComponent *rigidbody_component=m_Scene->GetComponent<RigidbodyComponent>(m_SelectedEntity);
    if(rigidbody_component){
        if(StartNode("RigidbodyComponent")){
            const char *bodyTypes[]={"Static","Dynamic","Kinematic"};
            int currentBodyType;
            switch(rigidbody_component->m_BodyType){
                case RigidbodyComponent::BodyType::Static:
                    currentBodyType=0;
                    break;
                case RigidbodyComponent::BodyType::Dynamic:
                    currentBodyType=1;
                    break;
                case RigidbodyComponent::BodyType::Kinematic:
                    currentBodyType=2;
                    break;
            }

            ImGui::Combo("Body Type",&currentBodyType,bodyTypes,3);
            
            switch(currentBodyType){
                case 0:
                    rigidbody_component->m_BodyType=RigidbodyComponent::BodyType::Static;
                    break;
                case 1:
                    rigidbody_component->m_BodyType=RigidbodyComponent::BodyType::Dynamic;
                    break;
                case 2:
                    rigidbody_component->m_BodyType=RigidbodyComponent::BodyType::Kinematic;
                    break;
            }

            //TO IMPLEMENT: 
            //ImGui::Checkbox("Fixed Rotation",&rigidbody_component->m_FixedRotation);
        
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("RigidbodyComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("RigidbodyComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<RigidbodyComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    BoxColliderComponent *box_collider_component=m_Scene->GetComponent<BoxColliderComponent>(m_SelectedEntity);
    if(box_collider_component){
        if(StartNode("BoxColliderComponent")){
            ImGui::SliderFloat("X Offset",&box_collider_component->m_XOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Y Offset",&box_collider_component->m_YOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Width",&box_collider_component->m_Width,0.0f,30.0f);
            ImGui::SliderFloat("Height",&box_collider_component->m_Height,0.0f,30.0f);
            ImGui::SliderFloat("Density",&box_collider_component->m_Density,0.0f,10.0f);
            ImGui::SliderFloat("Friction",&box_collider_component->m_Friction,0.0f,10.0f);
            ImGui::SliderFloat("Restitution",&box_collider_component->m_Restitution,0.0f,10.0f);
            ImGui::SliderFloat("Restitution Threshold",&box_collider_component->m_RestitutionThreshold,0.0f,10.0f);
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("BoxColliderComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("BoxColliderComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<BoxColliderComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    CircleColliderComponent *circle_collider_component=m_Scene->GetComponent<CircleColliderComponent>(m_SelectedEntity);
    if(circle_collider_component){
        if(StartNode("CircleColliderComponent")){
            ImGui::SliderFloat("X Offset",&circle_collider_component->m_XOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Y Offset",&circle_collider_component->m_YOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Radius",&circle_collider_component->m_Radius,0.0f,30.0f);
            ImGui::SliderFloat("Density",&circle_collider_component->m_Density,0.0f,10.0f);
            ImGui::SliderFloat("Friction",&circle_collider_component->m_Friction,0.0f,10.0f);
            ImGui::SliderFloat("Restitution",&circle_collider_component->m_Restitution,0.0f,10.0f);
            ImGui::SliderFloat("Restitution Threshold",&circle_collider_component->m_RestitutionThreshold,0.0f,10.0f);
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("CircleColliderComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("CircleColliderComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<CircleColliderComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    LightComponent *light_component=m_Scene->GetComponent<LightComponent>(m_SelectedEntity);
    if(light_component){
        if(StartNode("LightComponent")){
            ImGui::SliderFloat("X Offset",&light_component->m_XOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Y Offset",&light_component->m_YOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Radius",&light_component->m_Radius,0.0f,30.0f);
            ImGui::SliderFloat("Blur",&light_component->m_Blur,0.0f,1.0f);
            ImGui::ColorEdit3("Color",&light_component->m_Color.r);

            const char *lightTypes[]={"All Light","Spot Light","Spot Light (Collisions on)"};
            int currentLightType;
            switch(light_component->m_Type){
                case LightType::ALL_LIGHT:
                    currentLightType=0;
                    break;
                case LightType::LIGHT_AROUND_POS:
                    currentLightType=1;
                    break;
                case LightType::LIGHT_AROUND_POS_COLL:
                    currentLightType=2;
                    break;
            }

            ImGui::Combo("Light Type",&currentLightType,lightTypes,3);

            switch(currentLightType){
                case 0:
                    light_component->m_Type=LightType::ALL_LIGHT;
                    break;
                case 1:
                    light_component->m_Type=LightType::LIGHT_AROUND_POS;
                    break;
                case 2:
                    light_component->m_Type=LightType::LIGHT_AROUND_POS_COLL;
                    break;
            }

            if(ImGui::Button("Center Light")){
                TextureComponent *texture_component=m_Scene->GetComponent<TextureComponent>(m_SelectedEntity);
                if(texture_component){
                    light_component->SetCentered(texture_component->m_Width,texture_component->m_Height);
                }else{
                    AnimatedTextureComponent *animated_texture_component=m_Scene->GetComponent<AnimatedTextureComponent>(m_SelectedEntity);
                    if(animated_texture_component){
                        light_component->SetCentered(animated_texture_component->m_Width,animated_texture_component->m_Height);
                    }
                }
            }

            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("LightComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("LightComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<LightComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    TextComponent *text_component=m_Scene->GetComponent<TextComponent>(m_SelectedEntity);
    if(text_component){
        if(StartNode("TextComponent")){
            ImGui::InputText("Font Path",&text_component->m_TextRenderer->m_FontPath[0],text_component->m_TextRenderer->m_FontPath.size());
            ImGui::SliderFloat("Font Size",&text_component->m_TextRenderer->m_GlyphSize,0,256);
            if(ImGui::Checkbox("Fixed Position",&text_component->m_TextRenderer->m_Fixed)){
                glm::mat4 mat;
                if(text_component->m_TextRenderer->m_Fixed){
                    mat=m_Camera.GetProjMatrix();
                }else{
                    mat=m_Camera.GetViewProjMatrix();
                }
                text_component->m_TextRenderer->UpdateProjMat(mat);
            }

            if(ImGui::Button("Reload Font")){
                TextRenderer *t=text_component->m_TextRenderer.get();
                text_component->m_TextRenderer=FONT_MANAGER->UpdateFont(t->m_ID,t->m_FontPath,t->m_GlyphSize,t->m_Fixed).second;
            }

            ImGui::InputTextMultiline("Text",&text_component->m_Text[0],text_component->m_Text.size());
            ImGui::SliderFloat("X Offset",&text_component->m_Offset.x,-10.0f,10.0f);
            ImGui::SliderFloat("Y Offset",&text_component->m_Offset.y,-10.0f,10.0f);
            if(ImGui::Button("Center Text")){
                TextureComponent *texture_component=m_Scene->GetComponent<TextureComponent>(m_SelectedEntity);
                if(texture_component){
                    text_component->SetCentered(texture_component->m_Width,texture_component->m_Height);
                }else{
                    AnimatedTextureComponent *animated_texture_component=m_Scene->GetComponent<AnimatedTextureComponent>(m_SelectedEntity);
                    if(animated_texture_component){
                        text_component->SetCentered(animated_texture_component->m_Width,animated_texture_component->m_Height);
                    }
                }
            }
            ImGui::SliderFloat("Scale",&text_component->m_Scale,0.0f,10.0f);
            ImGui::ColorEdit3("Color",&text_component->m_Color.r);
            ImGui::Checkbox("Ignore Lighting",&text_component->m_IgnoreLighting);
            if(!text_component->m_IgnoreLighting){
                ImGui::SliderInt("Layer",&text_component->m_Layer,-100,100);
            }
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("TextComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("TextComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<TextComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
    }

    for(int i=0;i<m_ScriptComponents.size();i++){
        auto &[name,id]=m_ScriptComponents[i];
        if(id==m_SelectedEntity){
            if(StartNode("ScriptComponent")){
                ImGui::InputText("Function Name",&name[0],name.size());
                ImGui::TreePop();
            }
            ImGui::OpenPopupOnItemClick("ScriptComponentPopup",ImGuiPopupFlags_MouseButtonRight);
            if(ImGui::BeginPopupContextItem("ScriptComponentPopup")){
                if(ImGui::MenuItem("Remove Component")){
                    m_ScriptComponents.erase(m_ScriptComponents.begin()+i);
                }
                ImGui::EndPopup();
            }
        }
    }

    ImGui::PopItemWidth();

    if(ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)){
        ImGui::OpenPopup("AddComponentPopup");
    }
    if(ImGui::BeginPopup("AddComponentPopup")){
        if(ImGui::BeginMenu("Add Component")){
            if(ImGui::MenuItem("Tag Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<TagComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<TagComponent>(m_SelectedEntity,"Entity "+std::to_string(m_SelectedEntity));
                }
            }
            if(ImGui::MenuItem("Texture Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<TextureComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<TextureComponent>(m_SelectedEntity);
                }
            }
            if(ImGui::MenuItem("Animated Texture Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<AnimatedTextureComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<AnimatedTextureComponent>(m_SelectedEntity);
                }
            }
            if(ImGui::MenuItem("Rigidbody Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<RigidbodyComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<RigidbodyComponent>(m_SelectedEntity);
                }
            }
            if(ImGui::MenuItem("Box Collider Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<BoxColliderComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<BoxColliderComponent>(m_SelectedEntity);
                }
            }
            if(ImGui::MenuItem("Circle Collider Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<CircleColliderComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<CircleColliderComponent>(m_SelectedEntity);
                }
            }
            if(ImGui::MenuItem("Light Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<LightComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<LightComponent>(m_SelectedEntity);
                }
            }
            if(ImGui::MenuItem("Text Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max() && m_Scene->GetComponent<TextComponent>(m_SelectedEntity)==nullptr){
                    m_Scene->AddComponent<TextComponent>(m_SelectedEntity);
                }
            }
            if(ImGui::MenuItem("Script Component")){
                if(m_SelectedEntity!=std::numeric_limits<uint32_t>::max()){
                    std::string name(100,'\0');
                    m_ScriptComponents.push_back({name,m_SelectedEntity});
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup(); 
    }

    ImGui::End();
}

void Editor::DataMenu(ImVec2 pos){
    ImGui::Begin("Data",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(pos);
    ImGui::SetWindowSize(ImVec2(GetWidthPercentageInPx(20),(Window::Height-pos.y)/2.0f));

    ImGui::Text("Vertices Count: %d",Window::VertexCount);   
    ImGui::Text("Draw Calls: %d",Window::DrawCalls);

    ImGui::End();
}

void Editor::VariablesMenu(ImVec2 pos){
    ImGui::Begin("Variables",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(pos);
    ImGui::SetWindowSize(ImVec2(GetWidthPercentageInPx(20),Window::Height-pos.y));
    ImGui::ColorEdit3("Clear Color",(float*)&RENDERER->m_ClearColor);
    ImGui::ColorEdit3("Ambient Light",(float*)&RENDERER->m_AmbientLight);
    ImGui::Checkbox("Animations Preview",&ANIMATIONS_PREVIEW);

    ImGui::Dummy(ImVec2(0.0f,ImGui::GetTextLineHeight()));

    ImGui::Text("Window variables:");
    ImGui::InputText("Window Name",&WINDOW_NAME[0],WINDOW_NAME.size());
    ImGui::InputInt("Window Width",(int*)&WINDOW_WIDTH);
    ImGui::InputInt("Window Height",(int*)&WINDOW_HEIGHT);
    ImGui::InputInt("Fullscreen Width",(int*)&FULLSCREEN_WIDTH);
    ImGui::InputInt("Fullscreen Height",(int*)&FULLSCREEN_HEIGHT);
    ImGui::InputText("Scene Path",&SCENE_PATH[0],SCENE_PATH.size());
    ImGui::Checkbox("Resizable",&RESIZABLE);

    ImGui::End();
}

void Editor::FileBrowserMenu(ImVec2 pos){
    ImGui::Begin("FileBrowser",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(pos);
    ImGui::SetWindowSize(ImVec2(GetWidthPercentageInPx(60),Window::Height-pos.y));

    if(ImGui::Button(ICON_FA_ARROW_LEFT,ImVec2(baseFontSize*1.5f,baseFontSize*1.5f))){
        for(int i=m_CurrentPath.size()-1;i>=0;i--){
            if(m_CurrentPath[i]=='/'){
                m_CurrentPath=m_CurrentPath.substr(0,i);
                m_CurrentEntries.clear();
                break;
            }
        }
    }

    ImGui::SameLine();
    if(ImGui::Button(ICON_FA_ROTATE,ImVec2(baseFontSize*1.5f,baseFontSize*1.5f))){
        m_UpdateFiles=true;
    }

    if(m_CurrentEntries.empty() || m_UpdateFiles){
        if(m_UpdateFiles){
            m_UpdateFiles=false;
            m_CurrentEntries.clear();
        }

        for(auto &entry:std::filesystem::directory_iterator(m_CurrentPath)){
            m_CurrentEntries.push_back(entry);
        }

        std::sort(m_CurrentEntries.begin(),m_CurrentEntries.end(), 
            [](const std::filesystem::directory_entry &a,const std::filesystem::directory_entry &b){
                if(a.is_directory()&&!b.is_directory()){
                    return true;
                }
                if(!a.is_directory() && b.is_directory()){
                    return false;
                }
                return a.path()<b.path();
            }
        );
    }
    

    ImGui::SameLine();
    ImGui::Text("Current Path: %s",m_CurrentPath.c_str());

    ImGui::PushStyleColor(ImGuiCol_Border,IM_COL32(0,0,0,0));
    ImGui::BeginChild("FileBrowserChild",ImVec2(0,0),true);
        static bool isFileContentOpen=false;
        for(auto &entry:m_CurrentEntries){
            if(entry.is_directory()){ 
                if(ImGui::Selectable((ICON_FA_FOLDER+entry.path().filename().string()).c_str())){
                    m_CurrentPath=entry.path().string();
                    m_CurrentEntries.clear();
                    break;
                }
            }else{
                if(ImGui::Selectable((ICON_FA_FILE+entry.path().filename().string()).c_str())){
                    FILE *file=fopen(entry.path().string().c_str(),"r");

                    if(file){
                        char ch;
                        m_SelectedFileContent.clear();
                        while((ch=fgetc(file))!=EOF){
                            m_SelectedFileContent+=ch;
                        }
                        fclose(file);

                        isFileContentOpen=true;
                    }
                }   
            }
        }

        if(isFileContentOpen && !m_SelectedFileContent.empty()){
            ImGui::Begin("File Content",&isFileContentOpen);
            ImGui::Text("%s",m_SelectedFileContent.c_str());
            ImGui::End();
        }else if(!m_SelectedFileContent.empty()){
            m_SelectedFileContent.clear();
        }
    ImGui::EndChild();

    ImGui::PopStyleColor();

    ImGui::End();
}

void Editor::OnImGuiRender(){
    Renderer::ImGui_End_Frame();
}

void Editor::HdrWindow(bool *open){
    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("HDR",open,ImGuiWindowFlags_NoCollapse);

    TonemapType tonemap=RENDERER->GetTonemapType();
    float gamma=RENDERER->GetGamma();
    float exposure=RENDERER->GetExposure();

    const char *items[]={"Reinhard","Filmic","Uncharted2","ACES","Exponential","Logarithmic","Mantiuk"};
    static int current_item;

    std::string tonemapName=TonemapTypeToString(tonemap);
    for(int i=0;i<IM_ARRAYSIZE(items);i++){
        if(tonemapName==items[i]){
            current_item=i;
            break;
        }
    }

    static int old_item=current_item;

    ImGui::Combo("Tonemap Type",&current_item,items,IM_ARRAYSIZE(items));    

    bool update=false;

    if(current_item!=old_item){
        tonemap=StringToTonemapType(items[current_item]);
        old_item=current_item;
        RENDERER->SetTonemapType(tonemap);
        update=true;
    }

    if(ImGui::SliderFloat("Gamma",&gamma,0.0f,10.0f)){
        RENDERER->SetGamma(gamma);
        update=true;
    }

    if(ImGui::SliderFloat("Exposure",&exposure,0.0f,10.0f)){
        RENDERER->SetExposure(exposure);
        update=true;
    }

    if(update){
        RENDERER->ReloadShaders();
    }

    ImGui::End();
}

void Editor::ScenesWindow(bool *open){
    ImGui::SetNextWindowSize(ImVec2(0,0));
    ImGui::Begin("Scenes",open,ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Select scene:");
    if(ImGui::BeginCombo("Scenes",m_SelectedScene==-1?"None":m_ScenesPaths[m_SelectedScene].c_str())){
        for(int i=0;i<m_ScenesPaths.size();i++){
            bool is_selected=(m_SelectedScene==i);

            if(ImGui::Selectable(m_ScenesPaths[i].c_str(),is_selected)){
                if(m_SelectedScene!=i){
                    if(m_Scene!=nullptr){
                        delete m_Scene;
                    }
                    m_Scene=new Scene(m_ScenesPaths[i]);
                    m_SceneSerializer->SetScene(m_Scene);
                    m_ScriptComponents.clear();
                    m_SceneSerializer->Deserialize(m_CurrentPath+"/resources/scenes/"+m_ScenesPaths[i],m_ScriptComponents);
                }
                m_SelectedScene=i;
            }
            if(is_selected){
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::NewLine();

    static char sceneToAdd[STRLEN];
    static char sceneToRemove[STRLEN];

    ImGui::PushID(98234);
    ImGui::InputText("",&sceneToAdd[0],STRLEN);
    ImGui::SameLine();
    if(ImGui::Button("Add")){
        if(strlen(sceneToAdd)>0){
            m_ScenesPaths.push_back(std::string(sceneToAdd));
            memset(sceneToAdd,'\0',STRLEN);
        }
    }
    ImGui::PopID();

    ImGui::PushID(98235);
    ImGui::InputText("",&sceneToRemove[0],STRLEN);
    ImGui::SameLine();
    if(ImGui::Button("Remove")){
        int i;
        for(i=0;i<m_ScenesPaths.size();i++){
            if(m_ScenesPaths[i]==sceneToRemove){
                m_ScenesPaths.erase(m_ScenesPaths.begin()+i);
                break;
            }
        }
        if(m_SelectedScene==i){
            m_SelectedScene=-1;
        }
    }
    ImGui::PopID();
    ImGui::End();
}

void Editor::SerializeProject(){
    if(m_ProjectPath.empty()){
        return;   
    }

    printf("Starting serialization of project in file %s\n",m_ProjectPath.c_str());

    try{

    YAML::Emitter out;
    out<<YAML::BeginMap;
    out<<YAML::Key<<"WindowName"<<YAML::Value<<WINDOW_NAME;
    out<<YAML::Key<<"WindowWidth"<<YAML::Value<<WINDOW_WIDTH;
    out<<YAML::Key<<"WindowHeight"<<YAML::Value<<WINDOW_HEIGHT;
    out<<YAML::Key<<"FullscreenWidth"<<YAML::Value<<FULLSCREEN_WIDTH;
    out<<YAML::Key<<"FullscreenHeight"<<YAML::Value<<FULLSCREEN_HEIGHT;
    out<<YAML::Key<<"ScenePath"<<YAML::Value<<SCENE_PATH;
    out<<YAML::Key<<"Resizable"<<YAML::Value<<RESIZABLE;

    out<<YAML::Key<<"ToneMap"<<YAML::Value<<TonemapTypeToString(RENDERER->GetTonemapType());
    out<<YAML::Key<<"Gamma"<<YAML::Value<<RENDERER->GetGamma();
    out<<YAML::Key<<"Exposure"<<YAML::Value<<RENDERER->GetExposure();

    out<<YAML::Key<<"ScenesPaths"<<YAML::Value<<YAML::BeginSeq;
    for(const auto &path:m_ScenesPaths){
        out<<path;
    }
    out<<YAML::EndSeq;

    out<<YAML::Key<<"SelectedScene"<<YAML::Value<<m_SelectedScene;
    out<<YAML::EndMap;
    

    FILE *fout=fopen(m_ProjectPath.c_str(),"w");
    if(fout==NULL){
        printf("Failed to open project file %s: %s\n",m_ProjectPath.c_str(),strerror(errno));
        return;
    }
    fprintf(fout,"%s",out.c_str());
    fclose(fout);

    }catch(...){
        printf("Failed to serialize project file %s\n",m_ProjectPath.c_str());
        return;
    }
    
    printf("Serialization of project file %s completed\n",m_ProjectPath.c_str());
}

void Editor::DeserializeProject(){
    try{

    YAML::Node data=YAML::LoadFile(m_ProjectPath);

    WINDOW_NAME=data["WindowName"].as<std::string>();
    WINDOW_NAME.resize(STRLEN);
    WINDOW_WIDTH=data["WindowWidth"].as<unsigned int>();
    WINDOW_HEIGHT=data["WindowHeight"].as<unsigned int>();
    FULLSCREEN_WIDTH=data["FullscreenWidth"].as<unsigned int>();
    FULLSCREEN_HEIGHT=data["FullscreenHeight"].as<unsigned int>();
    SCENE_PATH=data["ScenePath"].as<std::string>();
    SCENE_PATH.resize(STRLEN);
    RESIZABLE=data["Resizable"].as<bool>();

    RENDERER->SetTonemapType(StringToTonemapType(data["ToneMap"].as<std::string>()));
    RENDERER->SetGamma(data["Gamma"].as<float>());
    RENDERER->SetExposure(data["Exposure"].as<float>());

    m_ScenesPaths.clear();
    for(const auto &path:data["ScenesPaths"]){
        m_ScenesPaths.push_back(path.as<std::string>());
    }

    m_SelectedScene=data["SelectedScene"].as<int>();
    if(m_Scene!=nullptr){
        delete m_Scene;
    }
    m_Scene=new Scene;
    m_SceneSerializer->SetScene(m_Scene);
    m_ScriptComponents.clear();
    m_SceneSerializer->Deserialize(m_CurrentPath+"/resources/scenes/"+m_ScenesPaths[m_SelectedScene],m_ScriptComponents);

    RENDERER->ReloadShaders();

    }catch(...){
        printf("Failed to deserialize project file %s\n",m_ProjectPath.c_str());
        return;
    }
}

void Editor::HandleInputs(){
    static Vec2 lastMousePos;
    static bool wheelPressed=false;

    KeyState wheel=INPUT->GetMouseButton(MOUSE_BUTTON_MIDDLE);
    if(wheel.current && !wheel.previous){
        lastMousePos=INPUT->GetMousePosition();
        if(lastMousePos.x>=m_ScenePos.x && lastMousePos.x<=m_ScenePos.x+m_SceneSize.x && lastMousePos.y<=m_ScenePos.y && lastMousePos.y>=m_ScenePos.y-m_SceneSize.y){
            wheelPressed=true;
        }
    }

    if(wheel.current && wheelPressed){
        Vec2 currentMousePos=INPUT->GetMousePosition();
        m_Camera.Move(lastMousePos.x-currentMousePos.x,lastMousePos.y-currentMousePos.y);
        lastMousePos=currentMousePos;
    }

    if(!wheel.current && wheel.previous){
        wheelPressed=false;
    }
}