#include "dialogManager.h"

DialogManager* dialogManager;

DialogManager::DialogManager(){
    this->dialogContainer = new UIElement({71, 240}, 527, 71, false, dialogBackground);
    UIText t = {
        0,
        "",
        {21, 7},
        RED,
        481
    };
    this->dialogContainer->addText(t);
    this->dialogContainer->setVisible(false);

    this->advanceButton = new UIElement({575, 288}, 23, 23, true, dialogAdvanceButton, dialogAdvanceButtonHL);
    this->advanceButton->onClick = [](int param){
        if(dialogManager != nullptr){dialogManager->advance();}
    };
    this->advanceButton->setVisible(false);
    this->state = State::Inactive;

    this->buttonsContainer = new UIElement({22, 312}, 576, 40, false, container);
    this->buttonsContainer->setVisible(false);
    float x = 30.0f;
    for (int i = 0; i < 4; i++)
    {
        UIElement* optBtn = new UIElement({x, 320.0f}, 132, 24, true, dialogOptionButton, dialogOptionButtonHL, i);
        optBtn->onClick = [](int param){
            if(dialogManager != nullptr){dialogManager->selectOption(param);}
        };
        optBtn->addText({0,"",{4, 4},RED});
        optBtn->setVisible(false);
        this->optionButtons.push_back(optBtn);
        x += 142.0f;
        
    }
    
};

DialogManager::~DialogManager(){};

void DialogManager::set(std::vector<DialogAct> actList){
    this->actList = actList;
};

void DialogManager::show(){
    if(this->actList.size() <= 0) return;
    this->curAct = this->actList[0];
    this->lineIndex = 0;
    this->dialogContainer->setVisible(true);
    this->advanceButton->setVisible(true);
    this->dialogContainer->updateText(0, {0, this->curAct.lines[0].line, {21, 7}, RED, 481});
    this->state = State::Active;

};

void DialogManager::advance(){
    if(this->actList.size() <= 0) return;

    if((this->curAct.lines.size() == 0 || this->lineIndex >= (int)this->curAct.lines.size() - 1) && curAct.options.size() > 0){
        this->advanceButton->setVisible(false);
        this->buttonsContainer->setVisible(true);
        for (int i = 0; i < this->curAct.options.size(); i++)
        {
            this->optionButtons[i]->setVisible(true);
            this->optionButtons[i]->updateText(0, { 0,this->curAct.options[i].title,{4, 4},RED,});
        }
        
    }else if(this->lineIndex == (int)this->curAct.lines.size() - 1){
        this->end();
    }
    else{
        this->lineIndex += 1;
        this->dialogContainer->updateText(0,{0,this->curAct.lines[lineIndex].line,{21, 7},RED,481});
    }

};

void DialogManager::selectOption(int option){
    DialogOption selectedOption = this->curAct.options[option];
    if(selectedOption.goTo < 0 || selectedOption.goTo >= this->actList.size()){this->end();}
    else {
        this->curAct = this->actList[selectedOption.goTo];
        this->lineIndex = 0;
        this->dialogContainer->updateText(0, {0,this->curAct.lines[0].line,{21, 7}, RED,481});
        this->advanceButton->setVisible(true);
        for (int i = 0; i < 4; i++){this->optionButtons[i]->setVisible(false);}
        this->buttonsContainer->setVisible(false);
    }
};

void DialogManager::end(){
    SDL_Log("sair");
    this->dialogContainer->setVisible(false);
    this->advanceButton->setVisible(false);
    for (int i = 0; i < 4; i++) {this->optionButtons[i]->setVisible(false);}
    this->buttonsContainer->setVisible(false);
    this->state = State::Inactive;
};