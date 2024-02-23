#include <pch.hpp>
#include <editor.hpp>
#include <window.hpp>
#include <utilities.hpp>
#include <scene.hpp>
#include <sceneserializer.hpp>
#include <IconsFontAwesome6.h>
#include <scene_buttons.hpp>

#define baseFontSize 24.0f

Editor::Editor(unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height,bool resizable){
    Window::BaseWidth=Window::Width=width;
    Window::BaseHeight=Window::Height=height;
    Window::FullscreenWidth=fullscreen_width;
    Window::FullscreenHeight=fullscreen_height;
    Window::MAX_HEIGHT=Window::MAX_WIDTH/(Window::Width/Window::Height);

    Window::InitGlfwWindow("Editor",resizable);

    m_Camera.InitializeProj();
    m_SceneFramebuffer=new Framebuffer;
    m_Scene=new Scene;
    m_SceneSerializer=new SceneSerializer;
    m_SceneSerializer->SetScene(m_Scene);
    RENDERER->ImGui_Init();

    m_SceneSerializer->Deserialize("test.scene");
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
}

Editor::~Editor(){
    delete m_Scene;
    delete m_SceneFramebuffer;
    RENDERER->ImGui_Close();
}

void Editor::Run(){
    while(!glfwWindowShouldClose(Window::Window)){
        glfwPollEvents();

        RENDERER->StartEditorScene(this);
        OnSceneRender();
        RENDERER->DrawEditorScene(m_SceneFramebuffer);
        
        RENDERER->StartScene();
        RENDERER->Clear({0.0,0.0,0.0,1.0});
        Renderer::ImGui_Start_Frame();
        Renderer::ImGui_Theme();
        
        OnImGuiUpdate();
        OnImGuiRender();
        RENDERER->DrawScene();

        glfwSwapBuffers(Window::Window);
    }
}

void Editor::OnSceneRender(){
    m_Scene->Render();
}

void Editor::OnImGuiUpdate(){
    ImVec2 tempvec;
    float tempf;

    if(ImGui::BeginMainMenuBar()){
        tempvec=ImGui::GetWindowSize();
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("New")){
                delete m_Scene;
                m_Scene=new Scene;
                m_SceneSerializer->SetScene(m_Scene);
            }
            if(ImGui::MenuItem("Open")){
                nfdchar_t *outPath=NULL;
                nfdresult_t result=NFD_OpenDialog("scene",NULL,&outPath);
                    
                if(result==NFD_OKAY){
                    delete m_Scene;
                    m_Scene=new Scene;
                    m_SceneSerializer->SetScene(m_Scene);
                    m_SceneSerializer->Deserialize(outPath);
                }else if(result==NFD_ERROR){
                    printf("Error: %s\n",NFD_GetError());
                }
            }
            if(ImGui::MenuItem("Save")){
                nfdchar_t *outPath=NULL;
                nfdresult_t result=NFD_SaveDialog("scene",NULL,&outPath);
                    
                if(result==NFD_OKAY){
                    m_SceneSerializer->Serialize(outPath);
                }else if(result==NFD_ERROR){
                    printf("Error: %s\n",NFD_GetError());
                }
            }
            if(ImGui::MenuItem("Exit")){
                glfwSetWindowShouldClose(Window::Window, true);
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Edit")){
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    EntitiesMenu({0,tempvec.y});
    ComponentsMenu({0,(Window::Height-tempvec.y)/2.0f+tempvec.y});
    DataMenu({GetWidthPercentageInPx(80),tempvec.y});
    VariablesMenu({GetWidthPercentageInPx(80),(Window::Height-tempvec.y)/2.0f+tempvec.y});

    ImGui::Begin("Viewport",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(ImVec2(GetWidthPercentageInPx(20),tempvec.y));
    tempf=GetWidthPercentageInPx(60);
    ImGui::SetWindowSize(ImVec2(tempf,tempf*9.0f/16.0f));
    ImGui::Image((void*)m_SceneFramebuffer->GetColorbufferID(),ImGui::GetContentRegionAvail(),ImVec2(0,1),ImVec2(1,0));
    
    ImGui::SameLine();
    ImVec2 pos=ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(pos.x-GetWidthPercentageInPx(60)/2.0f,pos.y+baseFontSize/2.0f));
    if(ImGui::Button(ICON_FA_PLAY,ImVec2(baseFontSize*1.5f,baseFontSize*1.5f))){
        SceneButtons::PlayButton();
    }
    
    ImGui::End();
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

        ImGui::PopID();
        k++;
    }

    if(ImGui::BeginPopupContextWindow()){
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
            ImGui::SliderFloat("Width",&texture_component->m_Width,0.0f,30.0f);
            ImGui::SliderFloat("Height",&texture_component->m_Height,0.0f,30.0f);
            ImGui::SliderFloat("Layer",&texture_component->m_Layer,0,100,"%.3f");
            ImGui::TreePop();
        }
        ImGui::OpenPopupOnItemClick("TextureComponentPopup",ImGuiPopupFlags_MouseButtonRight);
        if(ImGui::BeginPopupContextItem("TextureComponentPopup")){
            if(ImGui::MenuItem("Remove Component")){
                m_Scene->RemoveComponent<TextureComponent>(m_SelectedEntity);
            }
            ImGui::EndPopup();
        }
        texture_component->m_Layer=glm::floor(texture_component->m_Layer+0.5f);
    }

    AnimatedTextureComponent *animated_texture_component=m_Scene->GetComponent<AnimatedTextureComponent>(m_SelectedEntity);
    if(animated_texture_component){
        if(StartNode("AnimatedTextureComponent")){
            ImGui::SliderFloat("Width",&animated_texture_component->m_Width,0.0f,30.0f);
            ImGui::SliderFloat("Height",&animated_texture_component->m_Height,0.0f,30.0f);
            ImGui::SliderFloat("Layer",&animated_texture_component->m_Layer,0,100,"%.3f");

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
        animated_texture_component->m_Layer=glm::floor(animated_texture_component->m_Layer+0.5f);
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
    ImGui::End();
}

void Editor::OnImGuiRender(){
    Renderer::ImGui_End_Frame();
}