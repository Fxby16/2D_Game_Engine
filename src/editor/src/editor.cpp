#include <pch.hpp>
#include <editor.hpp>
#include <window.hpp>
#include <utilities.hpp>
#include <scene.hpp>
#include <sceneserializer.hpp>

Editor::Editor(unsigned int width,unsigned int height,float fullscreen_width,float fullscreen_height){
    Window::BaseWidth=Window::Width=width;
    Window::BaseHeight=Window::Height=height;
    Window::FullscreenWidth=fullscreen_width;
    Window::FullscreenHeight=fullscreen_height;
    Window::MAX_HEIGHT=Window::MAX_WIDTH/(Window::Width/Window::Height);

    Window::InitGlfwWindow("Editor");
    m_Camera.InitializeProj();
    m_SceneFramebuffer=new Framebuffer;
    m_Scene=new Scene;
    m_SceneSerializer=new SceneSerializer;
    m_SceneSerializer->SetScene(m_Scene);
    RENDERER->ImGui_Init();

    m_SceneSerializer->Deserialize("test.scene");
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
        RENDERER->DrawEditorScene();
        
        RENDERER->StartScene();
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

    ImGui::Begin("Viewport",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos(ImVec2(GetWidthPercentageInPx(20),tempvec.y));
    ImGui::SetWindowSize(ImVec2(0,0));
    tempf=GetWidthPercentageInPx(60);
    ImGui::Image((void*)m_SceneFramebuffer->GetColorbufferID(),ImVec2(tempf,tempf*9.0f/16.0f),ImVec2(0,1),ImVec2(1,0));
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
    }

    TextureComponent *texture_component=m_Scene->GetComponent<TextureComponent>(m_SelectedEntity);
    if(texture_component){
        if(StartNode("TextureComponent")){
            ImGui::SliderFloat("Width",&texture_component->m_Width,0.0f,30.0f);
            ImGui::SliderFloat("Height",&texture_component->m_Height,0.0f,30.0f);
            ImGui::SliderFloat("Layer",&texture_component->m_Layer,0,100,"%.3f");
            ImGui::TreePop();
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
    }

    LightComponent *light_component=m_Scene->GetComponent<LightComponent>(m_SelectedEntity);
    if(light_component){
        if(StartNode("LightComponent")){
            ImGui::SliderFloat("X Offset",&light_component->m_XOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Y Offset",&light_component->m_YOffset,-30.0f,30.0f);
            ImGui::SliderFloat("Radius",&light_component->m_Radius,0.0f,30.0f);
            ImGui::SliderFloat("Blur",&light_component->m_Blur,0.0f,1.0f);
            ImGui::ColorPicker3("Color",&light_component->m_Color.r);

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

            ImGui::TreePop();
        }
    }
    ImGui::PopItemWidth();

    if(ImGui::BeginPopupContextWindow()){
        if(ImGui::MenuItem("Add Component")){
            ImGui::OpenPopup("AddComponentPopup");
        }
        ImGui::EndPopup(); 
    }

    if(ImGui::BeginPopup("AddComponentPopup")){
        if(ImGui::MenuItem("TagComponent")){
            m_Scene->AddComponent<TagComponent>(m_SelectedEntity,"");
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

void Editor::OnImGuiRender(){
    Renderer::ImGui_End_Frame();
}