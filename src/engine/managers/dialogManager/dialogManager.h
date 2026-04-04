#pragma once
#include "ui/ui.h"

struct Actor{
char* name;
  //imagem do falante -> id dela ou algo do genero
};

struct DialogLine{
    Actor actor;
    char* line;
};

// struct DialogOptionCost{
//     int amount;
    
// };
struct DialogOption{
    char* title;
    char* line;
    // DialogOptionCost cost;
    int goTo;
};

struct DialogAct{
    std::vector<DialogLine> lines;
    std::vector<DialogOption> options;
};

enum class State{
    Active,
    Inactive
};

class DialogManager{
    public:
        State state;
        void set(std::vector<DialogAct> actList);
        void show();
        void advance();

        DialogManager();
        ~DialogManager();

    private:
        std::vector<DialogAct> actList;
        DialogAct curAct;
        UIElement* dialogContainer;
        UIElement* buttonsContainer;
        int lineIndex;
        void selectOption(int option);
        void end();

};

extern DialogManager* dialogManager;