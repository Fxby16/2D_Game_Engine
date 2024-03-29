#pragma once

#include <structs.hpp>

struct KeyState{
    KeyState(){
        current=previous=0;
    }

    bool current;
    bool previous;
};

class Input{
public:
    Input()=default;

    void UpdateStates();

    inline KeyState GetKey(int key){
        return m_KeyStates[key];
    }

    inline KeyState GetMouseButton(int button){
        return m_MouseStates[button];
    }

    Vec2 GetMousePosition();
private:
    std::unordered_map<int,KeyState> m_KeyStates;
    std::unordered_map<int,KeyState> m_MouseStates;
};