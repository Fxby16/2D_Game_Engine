#include <editor.hpp>

int main(int argc,char **argv){
    Editor *editor=new Editor(1600,900,1920,1080,true);
    editor->Run();
    delete editor;
    
    return 0;
}