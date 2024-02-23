#include <pch.hpp>  
#include <sceneserializer.hpp> 
#include <entity.hpp>
#include <global.hpp>

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
void SerializeEntity(YAML::Emitter &out,Entity &entity,Scene *scene){
    out<<YAML::BeginMap; //entity and components
    
    out<<YAML::Key<<"Entity";
    out<<YAML::BeginMap; //entity

    out<<YAML::Key<<"UID"<<YAML::Value<<entity.m_UID;
    out<<YAML::Key<<"X"<<YAML::Value<<entity.m_X;   
    out<<YAML::Key<<"Y"<<YAML::Value<<entity.m_Y;
    out<<YAML::Key<<"PreviousX"<<YAML::Value<<entity.m_PreviousX;
    out<<YAML::Key<<"PreviousY"<<YAML::Value<<entity.m_PreviousY;

    out<<YAML::EndMap;

    if(scene->GetComponent<TagComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"TagComponent";
        out<<YAML::BeginMap; //tag component

        TagComponent *tagcomponent=scene->GetComponent<TagComponent>(entity.m_UID);

        std::string tag=tagcomponent->m_Tag;
        tag.erase(std::remove(tag.begin(),tag.end(),'\0'),tag.end());

        out<<YAML::Key<<"Tag"<<YAML::Value<<tag;

        out<<YAML::EndMap; //tag component
    }

    if(scene->GetComponent<TextureComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"TextureComponent";
        out<<YAML::BeginMap; //texture component

        TextureComponent *texturecomponent=scene->GetComponent<TextureComponent>(entity.m_UID);
        
        out<<YAML::Key<<"Filepath"<<YAML::Value<<texturecomponent->m_Texture.get()->m_FilePath;
        out<<YAML::Key<<"MagFilter"<<YAML::Value<<texturecomponent->m_Texture.get()->m_MagFilter;
        out<<YAML::Key<<"MinFilter"<<YAML::Value<<texturecomponent->m_Texture.get()->m_MinFilter;

        out<<YAML::Key<<"Width"<<YAML::Value<<texturecomponent->m_Width;
        out<<YAML::Key<<"Height"<<YAML::Value<<texturecomponent->m_Height;
        out<<YAML::Key<<"Layer"<<YAML::Value<<(int)texturecomponent->m_Layer;

        out<<YAML::EndMap; //texture component
    }

    if(scene->GetComponent<AnimatedTextureComponent>(entity.m_UID)!=nullptr){
        out<<YAML::Key<<"AnimatedTextureComponent";
        out<<YAML::BeginMap; //animated texture component

        AnimatedTextureComponent *animatedtexturecomponent=scene->GetComponent<AnimatedTextureComponent>(entity.m_UID);

        out<<YAML::Key<<"Filepath"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_FilePath;
        out<<YAML::Key<<"MagFilter"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_MagFilter;
        out<<YAML::Key<<"MinFilter"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_MinFilter;
        
        out<<YAML::Key<<"TileWidth"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_TileWidth;
        out<<YAML::Key<<"TileHeight"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_TileHeight;

        out<<YAML::Key<<"PlayAnimation"<<YAML::Value<<animatedtexturecomponent->m_PlayAnimation;
        out<<YAML::Key<<"LoopAnimation"<<YAML::Value<<animatedtexturecomponent->m_LoopAnimation;
        out<<YAML::Key<<"AnimationDelay"<<YAML::Value<<animatedtexturecomponent->m_AnimationDelay;
        out<<YAML::Key<<"AnimationIndex"<<YAML::Value<<animatedtexturecomponent->m_AnimationIndex;

        out<<YAML::Key<<"Width"<<YAML::Value<<animatedtexturecomponent->m_Width;
        out<<YAML::Key<<"Height"<<YAML::Value<<animatedtexturecomponent->m_Height;
        out<<YAML::Key<<"Layer"<<YAML::Value<<(int)animatedtexturecomponent->m_Layer;

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

    out<<YAML::EndMap; //entity and components
}

void SceneSerializer::Serialize(const std::string &path){
    printf("Starting serialization of scene file %s\n",path.c_str());
    YAML::Emitter out;
    out<<YAML::BeginMap;
    out<<YAML::Key<<"Scene"<<YAML::Value<<m_Scene->m_Name;
    out<<YAML::Key<<"ScalingFactor"<<YAML::Value<<m_Scene->m_ScalingFactor;
    out<<YAML::Key<<"Gravity"<<YAML::Value<<m_Scene->m_Gravity;
    out<<YAML::Key<<"NextUID"<<YAML::Value<<NEXT_UID;
    out<<YAML::Key<<"AmbientLight"<<YAML::Value<<RENDERER->m_AmbientLight;
    out<<YAML::Key<<"ClearColor"<<YAML::Value<<RENDERER->m_ClearColor;
    out<<YAML::Key<<"Entities"<<YAML::Value<<YAML::BeginSeq;

    std::vector<Entity>& entities=m_Scene->GetEntities();
    for(int i=0;i<entities.size();i++){
        SerializeEntity(out,entities[i],m_Scene);
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
}

void SceneSerializer::SerializeEncrypted(const std::string &path){
    printf("Starting binary serialization of scene file %s\n",path.c_str());

    Serialize(path);
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
}

std::string EncodeTexture(bool animated,const std::string &path,unsigned int *tile_width,unsigned int *tile_height,int mag_filter,int min_filter){
    std::string encoded;
    encoded+=std::to_string(animated)+";";
    encoded+=path+";";
    encoded+=std::to_string(mag_filter)+";";
    encoded+=std::to_string(min_filter)+";";
    if(animated){
        #ifdef DEBUG
            assert(tile_width!=nullptr);
            assert(tile_height!=nullptr);
        #endif
        encoded+=std::to_string(*tile_width)+";";
        encoded+=std::to_string(*tile_height)+";";
    }
    return encoded;
}

bool GetTextureType(const std::string &encoded){
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenstream(encoded);
    std::getline(tokenstream,token,';');
    return std::stoi(token);
}

void DecodeTexture(const std::string &encoded,std::string &path,unsigned int &tile_width,unsigned int &tile_height,int &mag_filter,int &min_filter){
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenstream(encoded);
    while(std::getline(tokenstream,token,';')){
        tokens.push_back(token);
    }
    if(tokens.size()<6){
        printf("Invalid encoded animated texture\n");
        printf("%s\n",encoded.c_str());
        return;
    }
    path=tokens[1];
    mag_filter=std::stoi(tokens[2]);
    min_filter=std::stoi(tokens[3]);
    tile_width=std::stoi(tokens[4]);
    tile_height=std::stoi(tokens[5]);
}

void DecodeTexture(const std::string &encoded,std::string &path,int &mag_filter,int &min_filter){
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenstream(encoded);
    while(std::getline(tokenstream,token,';')){
        tokens.push_back(token);
    }
    if(tokens.size()<4){
        printf("Invalid encoded texture\n");
        printf("%s\n",encoded.c_str());
        return;
    }
    path=tokens[1];
    mag_filter=std::stoi(tokens[2]);
    min_filter=std::stoi(tokens[3]);
}

bool SceneSerializer::Deserialize(const std::string &path){
    YAML::Node data=YAML::LoadFile(path);

    //missing error handling
    
    return DeserializeNode(data);
}

bool SceneSerializer::DeserializeEncrypted(const std::string &path){
    std::string file_content;
    std::string key;

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

    YAML::Node data=YAML::Load(file_content);

    //missing error handling

    return DeserializeNode(data);
}

bool SceneSerializer::DeserializeNode(const YAML::Node &data){

    std::map<std::string,std::vector<TextureData>> textures;

    if(!data["Scene"]){
        printf("Scene node is invalid\n");
        return false;
    }

    m_Scene->m_Name=data["Scene"].as<std::string>();
    m_Scene->m_ScalingFactor=data["ScalingFactor"].as<float>();
    m_Scene->m_Gravity=data["Gravity"].as<Vec2>();
    NEXT_UID=data["NextUID"].as<uint32_t>();
    RENDERER->m_AmbientLight=data["AmbientLight"].as<Vec3>();
    RENDERER->m_ClearColor=data["ClearColor"].as<Vec3>();

    auto entities=data["Entities"];
    if(entities){
        for(auto e:entities){
            auto ent=e["Entity"];
            uint32_t uid=ent["UID"].as<uint32_t>();
            m_Scene->AddEntity(uid);
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
                std::string filepath=texturecomponent["Filepath"].as<std::string>();
                int magfilter=texturecomponent["MagFilter"].as<int>();
                int minfilter=texturecomponent["MinFilter"].as<int>();
                int width=texturecomponent["Width"].as<int>();
                int height=texturecomponent["Height"].as<int>();
                int layer=texturecomponent["Layer"].as<int>();

                textures[EncodeTexture(false,filepath,nullptr,nullptr,magfilter,minfilter)].push_back(TextureData(uid,width,height,layer));
                //m_Scene->AddComponent<TextureComponent>(uid,filepath,magfilter,minfilter,width,height,layer);
            }

            auto animatedtexturecomponent=e["AnimatedTextureComponent"];
            if(animatedtexturecomponent){
                std::string filepath=animatedtexturecomponent["Filepath"].as<std::string>();
                int magfilter=animatedtexturecomponent["MagFilter"].as<int>();
                int minfilter=animatedtexturecomponent["MinFilter"].as<int>();
                unsigned int tilewidth=animatedtexturecomponent["TileWidth"].as<unsigned int>();
                unsigned int tileheight=animatedtexturecomponent["TileHeight"].as<unsigned int>();
                bool playanimation=animatedtexturecomponent["PlayAnimation"].as<bool>();
                bool loopanimation=animatedtexturecomponent["LoopAnimation"].as<bool>();
                float animationdelay=animatedtexturecomponent["AnimationDelay"].as<float>();
                int animationindex=animatedtexturecomponent["AnimationIndex"].as<int>();
                int width=animatedtexturecomponent["Width"].as<float>();
                int height=animatedtexturecomponent["Height"].as<float>();
                int layer=animatedtexturecomponent["Layer"].as<float>();

                textures[EncodeTexture(true,filepath,&tilewidth,&tileheight,magfilter,minfilter)].push_back(TextureData(uid,width,height,layer,playanimation,loopanimation,animationdelay,animationindex));
                //m_Scene->AddComponent<AnimatedTextureComponent>(uid,filepath,tilewidth,tileheight,magfilter,minfilter,width,height,layer,playanimation,loopanimation,animationdelay);
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

                m_Scene->AddComponent<BoxColliderComponent>(uid,xoffset,yoffset,width,height,density,friction,restitution,restitutionthreshold);
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

                m_Scene->AddComponent<CircleColliderComponent>(uid,xoffset,yoffset,radius,density,friction,restitution,restitutionthreshold);
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
        }
    }

    for(auto &[key,val]:textures){
        //create components
        if(GetTextureType(key)){
            std::string path;
            unsigned int tilewidth,tileheight;
            int magfilter,minfilter;
            bool playanimation,loopanimation;
            float animationdelay;
            DecodeTexture(key,path,tilewidth,tileheight,magfilter,minfilter);
            std::shared_ptr<SpriteSheet>texture=std::make_shared<SpriteSheet>(path,tilewidth,tileheight,magfilter,minfilter);
            for(auto &data:val){
                m_Scene->AddComponent<AnimatedTextureComponent>(data.uid,texture,data.width,data.height,data.layer,data.playanimation,data.loopanimation,data.animationdelay,data.animationindex);
            }
        }else{
            std::string path;
            int magfilter,minfilter;
            DecodeTexture(key,path,magfilter,minfilter);
            std::shared_ptr<Texture>texture=std::make_shared<Texture>(path,magfilter,minfilter);
            for(auto &data:val){
                m_Scene->AddComponent<TextureComponent>(data.uid,texture,data.width,data.height,data.layer);
            }
        }
    }
    return true;
}