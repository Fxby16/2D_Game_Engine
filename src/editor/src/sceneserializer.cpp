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
        out<<YAML::Key<<"Layer"<<YAML::Value<<texturecomponent->m_Layer;

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

        out<<YAML::Key<<"PlayAnimation"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_PlayAnimation;
        out<<YAML::Key<<"LoopAnimation"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_LoopAnimation;
        out<<YAML::Key<<"AnimationDelay"<<YAML::Value<<animatedtexturecomponent->m_AnimatedTexture.get()->m_AnimationDelay;

        out<<YAML::Key<<"Width"<<YAML::Value<<animatedtexturecomponent->m_Width;
        out<<YAML::Key<<"Height"<<YAML::Value<<animatedtexturecomponent->m_Height;
        out<<YAML::Key<<"Layer"<<YAML::Value<<animatedtexturecomponent->m_Layer;

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
        out<<YAML::Key<<"Color"<<YAML::Value<<YAML::Flow<<YAML::BeginSeq<<lightcomponent->m_Color.r<<lightcomponent->m_Color.g<<lightcomponent->m_Color.b<<YAML::EndSeq;
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
    out<<YAML::Key<<"Gravity"<<YAML::Value<<YAML::Flow<<YAML::BeginSeq<<m_Scene->m_Gravity.x<<m_Scene->m_Gravity.y<<YAML::EndSeq;
    out<<YAML::Key<<"NextUID"<<YAML::Value<<NEXT_UID;
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

bool SceneSerializer::Deserialize(const std::string &path){
    printf("Starting deserialization of scene file %s\n",path.c_str());
    YAML::Node data;
    try{
        data=YAML::LoadFile(path);
    }catch(YAML::ParserException e){
        printf("Failed to load scene file %s\n",path.c_str());
        return false;
    }

    if(!data["Scene"]){
        printf("Scene file %s is invalid\n",path.c_str());
        return false;
    }

    m_Scene->m_Name=data["Scene"].as<std::string>();
    m_Scene->m_ScalingFactor=data["ScalingFactor"].as<float>();
    m_Scene->m_Gravity=data["Gravity"].as<Vec2>();
    NEXT_UID=data["NextUID"].as<uint32_t>();

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

                m_Scene->AddComponent<TextureComponent>(uid,filepath,magfilter,minfilter,width,height,layer);
            }

            auto animatedtexturecomponent=e["AnimatedTextureComponent"];
            if(animatedtexturecomponent){
                std::string filepath=animatedtexturecomponent["Filepath"].as<std::string>();
                int magfilter=animatedtexturecomponent["MagFilter"].as<int>();
                int minfilter=animatedtexturecomponent["MinFilter"].as<int>();
                int tilewidth=animatedtexturecomponent["TileWidth"].as<unsigned int>();
                int tileheight=animatedtexturecomponent["TileHeight"].as<unsigned int>();
                bool playanimation=animatedtexturecomponent["PlayAnimation"].as<bool>();
                bool loopanimation=animatedtexturecomponent["LoopAnimation"].as<bool>();
                float animationdelay=animatedtexturecomponent["AnimationDelay"].as<float>();
                int width=animatedtexturecomponent["Width"].as<float>();
                int height=animatedtexturecomponent["Height"].as<float>();
                int layer=animatedtexturecomponent["Layer"].as<float>();

                m_Scene->AddComponent<AnimatedTextureComponent>(uid,filepath,tilewidth,tileheight,magfilter,minfilter,width,height,layer,playanimation,loopanimation,animationdelay);
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
    printf("Deserialization of scene file %s completed\n",path.c_str());
    return true;
}