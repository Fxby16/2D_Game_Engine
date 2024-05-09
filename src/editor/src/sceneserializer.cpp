#include <pch.hpp>  
#include <sceneserializer.hpp> 
#include <entity.hpp>
#include <global.hpp>
#include <texturesmanager.hpp>

void SceneSerializer::SetScene(Scene *scene){
    m_Scene=scene;
}

std::string BodyTypeToString(RigidbodyComponent::BodyType bodytype){
    switch(bodytype){
        case RigidbodyComponent::BodyType::Static: return "Static";
        case RigidbodyComponent::BodyType::Dynamic: return "Dynamic";
        case RigidbodyComponent::BodyType::Kinematic: return "Kinematic";
        default: return "";
    }
}

RigidbodyComponent::BodyType BodyTypeFromString(std::string bodytype){
    if(bodytype=="Static") return RigidbodyComponent::BodyType::Static;
    if(bodytype=="Dynamic") return RigidbodyComponent::BodyType::Dynamic;
    if(bodytype=="Kinematic") return RigidbodyComponent::BodyType::Kinematic;
    return RigidbodyComponent::BodyType::Static;
}

std::string LightTypeToString(LightType lighttype){
    switch(lighttype){
        case LightType::ALL_LIGHT: return "AllLight";
        case LightType::LIGHT_AROUND_POS: return "LightAroundPos";
        case LightType::LIGHT_AROUND_POS_COLL: return "LightAroundPosColl";
        default: return "";
    }
}

LightType LightTypeFromString(std::string lighttype){
    if(lighttype=="AllLight") return LightType::ALL_LIGHT;
    if(lighttype=="LightAroundPos") return LightType::LIGHT_AROUND_POS;
    if(lighttype=="LightAroundPosColl") return LightType::LIGHT_AROUND_POS_COLL;
    return LightType::ALL_LIGHT;
}

namespace YAML{
    Emitter& operator<<(Emitter &out,const Vec3 &rhs){
        out<<Flow;
        out<<BeginSeq<<rhs.r<<rhs.g<<rhs.b<<EndSeq;
        return out;
    }

    Emitter& operator<<(Emitter &out,const Vec2 &rhs){
        out<<Flow;
        out<<BeginSeq<<rhs.x<<rhs.y<<EndSeq;
        return out;
    }

    template<>
    struct convert<Vec3>{
        static Node encode(const Vec3 &rhs){
            Node node;
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            return node;
        }

        static bool decode(const Node &node,Vec3 &rhs) {
            if(!node.IsSequence() || node.size()!=3){
                return false;
            }

            rhs.r=node[0].as<float>();
            rhs.g=node[1].as<float>();
            rhs.b=node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Vec2>{
        static Node encode(const Vec2 &rhs){
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node &node,Vec2 &rhs) {
            if(!node.IsSequence() || node.size()!=2){
                return false;
            }

            rhs.x=node[0].as<float>();
            rhs.y=node[1].as<float>();
            return true;
        }
    };
}

void SceneSerializer::SerializeEntity(YAML::Emitter &out,Entity &entity,Scene *scene,std::vector<std::pair<std::string,uint32_t>>&script_components){
    try{
    
    out<<YAML::BeginMap; //entity and components
    
    out<<YAML::Key<<"Entity";
    out<<YAML::BeginMap; //entity

    out<<YAML::Key<<"UID"<<YAML::Value<<entity.m_UID;
    out<<YAML::Key<<"Parent"<<YAML::Value<<entity.m_Parent;
    out<<YAML::Key<<"Group"<<YAML::Value<<entity.m_Group;
    out<<YAML::Key<<"X"<<YAML::Value<<entity.m_X;   
    out<<YAML::Key<<"Y"<<YAML::Value<<entity.m_Y;
    out<<YAML::Key<<"PreviousX"<<YAML::Value<<entity.m_PreviousX;
    out<<YAML::Key<<"PreviousY"<<YAML::Value<<entity.m_PreviousY;

    out<<YAML::EndMap;

    if(scene->GetComponent<TagComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"TagComponent";
        out<<YAML::BeginMap; //tag component

        TagComponent *tagcomponent=scene->GetComponent<TagComponent>(entity.m_UID);

        out<<YAML::Key<<"Tag"<<YAML::Value<<tagcomponent->m_Tag.c_str();

        out<<YAML::EndMap; //tag component
    }

    if(scene->GetComponent<TextureComponent>(entity.m_UID)!=nullptr){

        out<<YAML::Key<<"TextureComponent";
        out<<YAML::BeginMap; //texture component

        TextureComponent *texturecomponent=scene->GetComponent<TextureComponent>(entity.m_UID);

        out<<YAML::Key<<"Filepath"<<YAML::Value<<(texturecomponent->m_Texture.get()->m_LoadedFilePath.substr(texturecomponent->m_Texture.get()->m_LoadedFilePath.find_last_of("/")+1).c_str());
        out<<YAML::Key<<"MagFilter"<<YAML::Value<<texturecomponent->m_Texture.get()->m_LoadedMagFilter;
        out<<YAML::Key<<"MinFilter"<<YAML::Value<<texturecomponent->m_Texture.get()->m_LoadedMinFilter;

        out<<YAML::Key<<"Width"<<YAML::Value<<texturecomponent->m_Width;
        out<<YAML::Key<<"Height"<<YAML::Value<<texturecomponent->m_Height;
        out<<YAML::Key<<"Layer"<<YAML::Value<<texturecomponent->m_Layer;
        out<<YAML::Key<<"Visible"<<YAML::Value<<texturecomponent->m_Visible;
        out<<YAML::Key<<"FlipX"<<YAML::Value<<texturecomponent->m_FlipX;
        out<<YAML::Key<<"FlipY"<<YAML::Value<<texturecomponent->m_FlipY;

        out<<YAML::EndMap; //texture component
    }

    if(scene->GetComponent<AnimatedTextureComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"AnimatedTextureComponent";
        out<<YAML::BeginMap; //animated texture component

        AnimatedTextureComponent *animatedtexturecomponent=scene->GetComponent<AnimatedTextureComponent>(entity.m_UID);

        out<<YAML::Key<<"Filepath"<<YAML::Value<<(animatedtexturecomponent->m_AnimatedTexture.get()->m_LoadedFilePath.substr(animatedtexturecomponent->m_AnimatedTexture.get()->m_LoadedFilePath.find_last_of("/")+1).c_str());
        out<<YAML::Key<<"MagFilter"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_LoadedMagFilter;
        out<<YAML::Key<<"MinFilter"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_LoadedMinFilter;
        
        out<<YAML::Key<<"TileWidth"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_TileWidth;
        out<<YAML::Key<<"TileHeight"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_TileHeight;

        out<<YAML::Key<<"PlayAnimation"<<YAML::Value<<animatedtexturecomponent->m_PlayAnimation;
        out<<YAML::Key<<"LoopAnimation"<<YAML::Value<<animatedtexturecomponent->m_LoopAnimation;
        out<<YAML::Key<<"AnimationDelay"<<YAML::Value<<animatedtexturecomponent->m_AnimationDelay;
        out<<YAML::Key<<"AnimationRow"<<YAML::Value<<animatedtexturecomponent->m_AnimationRow;
        out<<YAML::Key<<"AnimationIndex"<<YAML::Value<<animatedtexturecomponent->m_AnimationIndex;

        out<<YAML::Key<<"Width"<<YAML::Value<<animatedtexturecomponent->m_Width;
        out<<YAML::Key<<"Height"<<YAML::Value<<animatedtexturecomponent->m_Height;
        out<<YAML::Key<<"Layer"<<YAML::Value<<animatedtexturecomponent->m_Layer;    
        out<<YAML::Key<<"Visible"<<YAML::Value<<animatedtexturecomponent->m_Visible;
        out<<YAML::Key<<"FlipX"<<YAML::Value<<animatedtexturecomponent->m_FlipX;
        out<<YAML::Key<<"FlipY"<<YAML::Value<<animatedtexturecomponent->m_FlipY;

        out<<YAML::EndMap; //animated texture component
    }

    if(scene->GetComponent<RigidbodyComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"RigidbodyComponent";
        out<<YAML::BeginMap; //rigidbody component

        RigidbodyComponent *rigidbodycomponent=scene->GetComponent<RigidbodyComponent>(entity.m_UID);

        out<<YAML::Key<<"BodyType"<<YAML::Value<<BodyTypeToString(rigidbodycomponent->m_BodyType);
        out<<YAML::Key<<"FixedRotation"<<YAML::Value<<rigidbodycomponent->m_FixedRotation;

        out<<YAML::EndMap; //rigidbody component
    }

    if(scene->GetComponent<BoxColliderComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"BoxColliderComponent";
        out<<YAML::BeginMap; //box collider component

        BoxColliderComponent *boxcollidercomponent=scene->GetComponent<BoxColliderComponent>(entity.m_UID);

        out<<YAML::Key<<"XOffset"<<YAML::Value<<boxcollidercomponent->m_XOffset;
        out<<YAML::Key<<"YOffset"<<YAML::Value<<boxcollidercomponent->m_YOffset;
        out<<YAML::Key<<"Width"<<YAML::Value<<boxcollidercomponent->m_Width;
        out<<YAML::Key<<"Height"<<YAML::Value<<boxcollidercomponent->m_Height;
        out<<YAML::Key<<"Density"<<YAML::Value<<boxcollidercomponent->m_Density;
        out<<YAML::Key<<"Friction"<<YAML::Value<<boxcollidercomponent->m_Friction;
        out<<YAML::Key<<"Restitution"<<YAML::Value<<boxcollidercomponent->m_Restitution;
        out<<YAML::Key<<"RestitutionThreshold"<<YAML::Value<<boxcollidercomponent->m_RestitutionThreshold;
        out<<YAML::Key<<"CategoryBits"<<YAML::Value<<boxcollidercomponent->m_CategoryBits;
        out<<YAML::Key<<"MaskBits"<<YAML::Value<<boxcollidercomponent->m_MaskBits;
        out<<YAML::Key<<"IsSensor"<<YAML::Value<<boxcollidercomponent->m_IsSensor;

        out<<YAML::EndMap; //box collider component
    }

    if(scene->GetComponent<CircleColliderComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"CircleColliderComponent";
        out<<YAML::BeginMap; //circle collider component

        CircleColliderComponent *circlecollidercomponent=scene->GetComponent<CircleColliderComponent>(entity.m_UID);

        out<<YAML::Key<<"XOffset"<<YAML::Value<<circlecollidercomponent->m_XOffset;
        out<<YAML::Key<<"YOffset"<<YAML::Value<<circlecollidercomponent->m_YOffset;
        out<<YAML::Key<<"Radius"<<YAML::Value<<circlecollidercomponent->m_Radius;
        out<<YAML::Key<<"Density"<<YAML::Value<<circlecollidercomponent->m_Density;
        out<<YAML::Key<<"Friction"<<YAML::Value<<circlecollidercomponent->m_Friction;
        out<<YAML::Key<<"Restitution"<<YAML::Value<<circlecollidercomponent->m_Restitution;
        out<<YAML::Key<<"RestitutionThreshold"<<YAML::Value<<circlecollidercomponent->m_RestitutionThreshold;
        out<<YAML::Key<<"CategoryBits"<<YAML::Value<<circlecollidercomponent->m_CategoryBits;
        out<<YAML::Key<<"MaskBits"<<YAML::Value<<circlecollidercomponent->m_MaskBits;
        out<<YAML::Key<<"IsSensor"<<YAML::Value<<circlecollidercomponent->m_IsSensor;

        out<<YAML::EndMap; //circle collider component
    }

    if(scene->GetComponent<LightComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"LightComponent";
        out<<YAML::BeginMap; //light component

        LightComponent *lightcomponent=scene->GetComponent<LightComponent>(entity.m_UID);

        out<<YAML::Key<<"XOffset"<<YAML::Value<<lightcomponent->m_XOffset;
        out<<YAML::Key<<"YOffset"<<YAML::Value<<lightcomponent->m_YOffset;
        out<<YAML::Key<<"Radius"<<YAML::Value<<lightcomponent->m_Radius;
        out<<YAML::Key<<"Blur"<<YAML::Value<<lightcomponent->m_Blur;
        out<<YAML::Key<<"Color"<<YAML::Value<<lightcomponent->m_Color;
        out<<YAML::Key<<"LightType"<<YAML::Value<<LightTypeToString(lightcomponent->m_Type);

        out<<YAML::EndMap; //light component
    }

    if(scene->GetComponent<TextComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"TextComponent";
        out<<YAML::BeginMap; //text component

        TextComponent *textcomponent=scene->GetComponent<TextComponent>(entity.m_UID);

        std::string fontpath=textcomponent->m_TextRenderer->m_LoadedFontPath;
        size_t pos=fontpath.find("fonts/");
        if(pos!=std::string::npos){
            fontpath=fontpath.substr(pos+strlen("fonts/"));
        }

        out<<YAML::Key<<"FontPath"<<YAML::Value<<fontpath.c_str();
        out<<YAML::Key<<"GlyphSize"<<YAML::Value<<textcomponent->m_TextRenderer->m_LoadedGlyphSize*100.0f/Window::Height;
        out<<YAML::Key<<"Fixed"<<YAML::Value<<textcomponent->m_TextRenderer->m_Fixed;
        out<<YAML::Key<<"Text"<<YAML::Value<<textcomponent->m_Text.c_str();
        out<<YAML::Key<<"Offset"<<YAML::Value<<textcomponent->m_Offset;
        out<<YAML::Key<<"Color"<<YAML::Value<<textcomponent->m_Color;
        out<<YAML::Key<<"Scale"<<YAML::Value<<textcomponent->m_Scale;
        out<<YAML::Key<<"Layer"<<YAML::Value<<textcomponent->m_Layer;
        out<<YAML::Key<<"IgnoreLighting"<<YAML::Value<<textcomponent->m_IgnoreLighting;

        out<<YAML::EndMap; //text component
    }

    for(auto &[fn_name,id]:script_components){
        if(id==entity.m_UID){
            if(!fn_name.empty()){
                out<<YAML::Key<<"NativeScriptComponent";
                out<<YAML::BeginMap; //native script component
                out<<YAML::Key<<"FunctionName"<<YAML::Value<<fn_name.c_str();
                out<<YAML::EndMap; //native script component
            }
        }
    }
    out<<YAML::EndMap; //entity and components

    }catch(...){
        printf("Failed to serialize entity\n");
    }
}

void SceneSerializer::Serialize(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components){
    try{
    
    printf("Starting serialization of scene file %s\n",path.c_str());
    YAML::Emitter out;
    out<<YAML::BeginMap;
    out<<YAML::Key<<"Scene"<<YAML::Value<<m_Scene->m_Name.c_str();
    out<<YAML::Key<<"ScalingFactor"<<YAML::Value<<m_Scene->m_ScalingFactor;
    out<<YAML::Key<<"Gravity"<<YAML::Value<<m_Scene->m_Gravity;
    out<<YAML::Key<<"NextUID"<<YAML::Value<<NEXT_UID;
    out<<YAML::Key<<"AmbientLight"<<YAML::Value<<RENDERER->m_AmbientLight;
    out<<YAML::Key<<"ClearColor"<<YAML::Value<<RENDERER->m_ClearColor;
    out<<YAML::Key<<"ToneMap"<<YAML::Value<<TonemapTypeToString(RENDERER->GetTonemapType());
    out<<YAML::Key<<"Gamma"<<YAML::Value<<RENDERER->GetGamma();
    out<<YAML::Key<<"Exposure"<<YAML::Value<<RENDERER->GetExposure();

    out<<YAML::Key<<"Entities"<<YAML::Value<<YAML::BeginSeq;

    std::vector<Entity>& entities=m_Scene->GetEntities();
    for(int i=0;i<entities.size();i++){
        SerializeEntity(out,entities[i],m_Scene,script_components);
    }

    out<<YAML::EndSeq;
    out<<YAML::EndMap;

    FILE *fout=fopen(path.c_str(),"w");
    if(fout==NULL){
        printf("Failed to open scene file %s: %s\n",path.c_str(),strerror(errno));
        return;
    }
    fprintf(fout,"%s",out.c_str());
    fclose(fout);
    printf("Serialization of scene file %s completed\n",path.c_str());

    }catch(...){
        printf("Failed to serialize scene\n");
    }
}

void SceneSerializer::SerializeEncrypted(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components){
    try{
    
    printf("Starting binary serialization of scene file %s\n",path.c_str());

    Serialize(path,script_components);
    FILE *fin=fopen(path.c_str(),"r+");
    if(fin==NULL){
        printf("Failed to open scene file %s: %s\n",path.c_str(),strerror(errno));
        return;
    }

    char ch;
    int i=0;

    srand(time(NULL));
    std::string key;
    for(int i=0;i<rand()%100+1;i++){
        key+=rand()%128;
    }

    FILE *fkey=fopen("key.bin","wb");
    if(fkey==NULL){
        printf("Failed to open key file: %s\n",strerror(errno));
        return;
    }
    for(int i=0;i<key.size();i++){
        for(int j=0;j<8;j++){
            char bit=(key[i]>>j)&1;
            fputc(bit,fkey);
        }
    }
    
    fclose(fkey);

    while((ch=fgetc(fin))!=EOF){
        ch^=key[i];
        i=(i+1)%key.size();
        fseek(fin,-1,SEEK_CUR);
        fputc(ch,fin);
        fseek(fin,0,SEEK_CUR);
    }

    fclose(fin);

    }catch(...){
        printf("Failed to serialize scene\n");
    }
}

#ifdef EDITOR
bool SceneSerializer::Deserialize(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components){
#elif defined(APPLICATION)
bool SceneSerializer::Deserialize(const std::string &path){
#endif
    YAML::Node data;
    try{
        data=YAML::LoadFile(path);
    }catch(...){
        printf("Failed to load scene file %s\n",path.c_str());
        return false;
    }

    int count=0;
    std::string dir;

    for(int i=path.size()-1;i>=0;i--){
        if(path[i]=='/'){
            count++;
        }

        if(count==2){
            dir=path.substr(0,i);
            break;
        }
    }

    #ifdef EDITOR
        return DeserializeNode(dir,data,script_components);
    #elif defined(APPLICATION)
        return DeserializeNode(dir,data);
    #endif
}

#ifdef EDITOR
bool SceneSerializer::DeserializeEncrypted(const std::string &path,std::vector<std::pair<std::string,uint32_t>>&script_components){
#elif defined(APPLICATION)
bool SceneSerializer::DeserializeEncrypted(const std::string &path){
#endif
    std::string file_content;
    std::string key;
    YAML::Node data;

    try{

    FILE *fkey=fopen("key.bin","rb");
    if(fkey==NULL){
        printf("Failed to open key file: %s\n",strerror(errno));
        return false;
    }

    char ch=0;
    int bit_count=0;
    char bit;
    while((bit=fgetc(fkey))!=EOF){
        ch|=(bit<<bit_count);
        bit_count++;
        if(bit_count==8){
            bit_count=0;
            key+=ch;
            ch=0;
        }
    }

    fclose(fkey);

    FILE *fin=fopen(path.c_str(),"r");
    if(fin==NULL){
        printf("Failed to open scene file %s: %s\n",path.c_str(),strerror(errno));
        return false;
    }

    int i=0;
    while((ch=fgetc(fin))!=EOF){
        ch^=key[i];
        i=(i+1)%key.size();
        file_content+=ch;
    }

    fclose(fin);

    data=YAML::Load(file_content);

    }catch(...){
        printf("Failed to load scene file %s\n",path.c_str());
        return false;
    }

    int count=0;
    std::string dir;

    for(int i=path.size()-1;i>=0;i--){
        if(path[i]=='/'){
            count++;
        }

        if(count==2){
            dir=path.substr(0,i);
            break;
        }
    }

    #ifdef EDITOR
        return DeserializeNode(dir,data,script_components);
    #elif defined(APPLICATION)
        return DeserializeNode(dir,data);
    #endif
}

#ifdef EDITOR
bool SceneSerializer::DeserializeNode(const std::string &path,const YAML::Node &data,std::vector<std::pair<std::string,uint32_t>>&script_components){
#elif defined(APPLICATION)
bool SceneSerializer::DeserializeNode(const std::string &path,const YAML::Node &data){
#endif

    try{

    if(!data["Scene"]){
        printf("Scene node is invalid\n");
        return false;
    }

    m_Scene->m_Name=data["Scene"].as<std::string>();
    m_Scene->m_ScalingFactor=data["ScalingFactor"].as<float>();
    m_Scene->SetGravity(data["Gravity"].as<Vec2>());
    NEXT_UID=data["NextUID"].as<uint32_t>();
    RENDERER->m_AmbientLight=data["AmbientLight"].as<Vec3>();
    RENDERER->m_ClearColor=data["ClearColor"].as<Vec3>();
    RENDERER->SetTonemapType(StringToTonemapType(data["ToneMap"].as<std::string>()));
    RENDERER->SetGamma(data["Gamma"].as<float>());
    RENDERER->SetExposure(data["Exposure"].as<float>());

    RENDERER->ReloadShaders();

    auto entities=data["Entities"];
    if(entities){
        for(auto e:entities){
            auto ent=e["Entity"];
            uint32_t uid=ent["UID"].as<uint32_t>();
            uint32_t parent=ent["Parent"].as<uint32_t>();
            uint32_t group=ent["Group"].as<uint32_t>();
            m_Scene->AddEntity(uid,parent,group);
            auto entity=m_Scene->GetEntity(uid);
            entity->m_X=ent["X"].as<float>();
            entity->m_Y=ent["Y"].as<float>();
            entity->m_PreviousX=ent["PreviousX"].as<float>();
            entity->m_PreviousY=ent["PreviousY"].as<float>();

            auto tagcomponent=e["TagComponent"];
            if(tagcomponent){
                std::string tag=tagcomponent["Tag"].as<std::string>();
                m_Scene->AddComponent<TagComponent>(uid,tag);
            }

            auto texturecomponent=e["TextureComponent"];
            if(texturecomponent){
                std::string filepath=path+"/textures/"+texturecomponent["Filepath"].as<std::string>();
                int magfilter=texturecomponent["MagFilter"].as<int>();
                int minfilter=texturecomponent["MinFilter"].as<int>();
                float width=texturecomponent["Width"].as<float>();
                float height=texturecomponent["Height"].as<float>();
                int layer=texturecomponent["Layer"].as<int>();
                bool visible=texturecomponent["Visible"].as<bool>();
                bool flipx=texturecomponent["FlipX"].as<bool>();
                bool flipy=texturecomponent["FlipY"].as<bool>();
                
                auto [id,texture]=TEXTURES_MANAGER->GetTexture(filepath,magfilter,minfilter);
                m_Scene->AddComponent<TextureComponent>(uid,texture,width,height,layer,visible,flipx,flipy);
            }

            auto animatedtexturecomponent=e["AnimatedTextureComponent"];
            if(animatedtexturecomponent){
                std::string filepath=path+"/textures/"+animatedtexturecomponent["Filepath"].as<std::string>();
                int magfilter=animatedtexturecomponent["MagFilter"].as<int>();
                int minfilter=animatedtexturecomponent["MinFilter"].as<int>();
                unsigned int tilewidth=animatedtexturecomponent["TileWidth"].as<unsigned int>();
                unsigned int tileheight=animatedtexturecomponent["TileHeight"].as<unsigned int>();
                bool playanimation=animatedtexturecomponent["PlayAnimation"].as<bool>();
                bool loopanimation=animatedtexturecomponent["LoopAnimation"].as<bool>();
                float animationdelay=animatedtexturecomponent["AnimationDelay"].as<float>();
                int animationrow=animatedtexturecomponent["AnimationRow"].as<int>();
                int animationindex=animatedtexturecomponent["AnimationIndex"].as<int>();
                float width=animatedtexturecomponent["Width"].as<float>();
                float height=animatedtexturecomponent["Height"].as<float>();
                int layer=animatedtexturecomponent["Layer"].as<int>();
                bool visible=animatedtexturecomponent["Visible"].as<bool>();
                bool flipx=animatedtexturecomponent["FlipX"].as<bool>();
                bool flipy=animatedtexturecomponent["FlipY"].as<bool>();
                
                auto [id,texture]=TEXTURES_MANAGER->GetSpriteSheet(filepath,tilewidth,tileheight,magfilter,minfilter);
                m_Scene->AddComponent<AnimatedTextureComponent>(uid,texture,width,height,layer,playanimation,loopanimation,
                    animationdelay,animationrow,animationindex,visible,flipx,flipy);
            }

            auto rigidbodycomponent=e["RigidbodyComponent"];
            if(rigidbodycomponent){
                RigidbodyComponent::BodyType bodytype=BodyTypeFromString(rigidbodycomponent["BodyType"].as<std::string>());
                bool fixedrotation=rigidbodycomponent["FixedRotation"].as<bool>();

                m_Scene->AddComponent<RigidbodyComponent>(uid,bodytype,fixedrotation);
            }

            auto boxcollidercomponent=e["BoxColliderComponent"];
            if(boxcollidercomponent){
                float xoffset=boxcollidercomponent["XOffset"].as<float>();
                float yoffset=boxcollidercomponent["YOffset"].as<float>();
                float width=boxcollidercomponent["Width"].as<float>();
                float height=boxcollidercomponent["Height"].as<float>();
                float density=boxcollidercomponent["Density"].as<float>();
                float friction=boxcollidercomponent["Friction"].as<float>();
                float restitution=boxcollidercomponent["Restitution"].as<float>();
                float restitutionthreshold=boxcollidercomponent["RestitutionThreshold"].as<float>();
                uint16_t categorybits=boxcollidercomponent["CategoryBits"].as<uint16_t>();
                uint16_t maskbits=boxcollidercomponent["MaskBits"].as<uint16_t>();
                bool issensor=boxcollidercomponent["IsSensor"].as<bool>();

                m_Scene->AddComponent<BoxColliderComponent>(uid,xoffset,yoffset,width,height,density,friction,restitution,restitutionthreshold,categorybits,maskbits,issensor);
            }

            auto circlecollidercomponent=e["CircleColliderComponent"];
            if(circlecollidercomponent){
                float xoffset=circlecollidercomponent["XOffset"].as<float>();
                float yoffset=circlecollidercomponent["YOffset"].as<float>();
                float radius=circlecollidercomponent["Radius"].as<float>();
                float density=circlecollidercomponent["Density"].as<float>();
                float friction=circlecollidercomponent["Friction"].as<float>();
                float restitution=circlecollidercomponent["Restitution"].as<float>();
                float restitutionthreshold=circlecollidercomponent["RestitutionThreshold"].as<float>();
                uint16_t categorybits=circlecollidercomponent["CategoryBits"].as<uint16_t>();
                uint16_t maskbits=circlecollidercomponent["MaskBits"].as<uint16_t>();
                bool issensor=circlecollidercomponent["IsSensor"].as<bool>();

                m_Scene->AddComponent<CircleColliderComponent>(uid,xoffset,yoffset,radius,density,friction,restitution,restitutionthreshold,categorybits,maskbits,issensor);
            }

            auto lightcomponent=e["LightComponent"];
            if(lightcomponent){
                float xoffset=lightcomponent["XOffset"].as<float>();
                float yoffset=lightcomponent["YOffset"].as<float>();
                float radius=lightcomponent["Radius"].as<float>();
                float blur=lightcomponent["Blur"].as<float>();
                Vec3 color=lightcomponent["Color"].as<Vec3>();
                LightType lighttype=LightTypeFromString(lightcomponent["LightType"].as<std::string>());

                m_Scene->AddComponent<LightComponent>(uid,xoffset,yoffset,radius,blur,color,lighttype);
            }

            auto textcomponent=e["TextComponent"];
            if(textcomponent){
                std::string fontpath=path+"/fonts/"+textcomponent["FontPath"].as<std::string>();
                float glyphsize=textcomponent["GlyphSize"].as<float>();
                bool fixed=textcomponent["Fixed"].as<bool>();
                std::string text=textcomponent["Text"].as<std::string>();
                Vec2 offset=textcomponent["Offset"].as<Vec2>();
                Vec3 color=textcomponent["Color"].as<Vec3>();
                float scale=textcomponent["Scale"].as<float>();
                int layer=textcomponent["Layer"].as<int>();
                bool ignore_lighting=textcomponent["IgnoreLighting"].as<bool>();

                m_Scene->AddComponent<TextComponent>(uid,fontpath,glyphsize,fixed,text,offset,color,scale,layer,ignore_lighting);
            }


            #ifdef EDITOR
            auto nativescriptcomponent=e["NativeScriptComponent"];
            if(nativescriptcomponent){
                std::string functionname=nativescriptcomponent["FunctionName"].as<std::string>();
                script_components.push_back(std::make_pair(functionname,uid));
            }
            #endif
        }
    }

    }catch(...){
        printf("Failed to deserialize scene\n");
        return false;
    }

    return true;
}