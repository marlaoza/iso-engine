#include "dialogManager.h"

DialogManager* dialogManager;

DialogManager::DialogManager(){
    this->state = State::Inactive;

    this->dialogContainer = new UIElement({71, 240}, 527, 71, false, dialogBackground);
    UIText t = {
        0,
        "",
        {21, 7},
        RED,
        481
    };
    this->dialogContainer->setText(t);

    UIElement* advanceButton = new UIElement({575, 288}, 23, 23, true, dialogAdvanceButton, dialogAdvanceButtonHL);
    advanceButton->onClick = [](int param){
        if(dialogManager != nullptr){dialogManager->advance();}
    };
    this->dialogContainer->addChildren(advanceButton);

    this->buttonsContainer = new UIElement({22, 312}, 576, 40, false, container);
    
    float x = 30.0f;
    for (int i = 0; i < 4; i++)
    {
        UIElement* optBtn = new UIElement({x, 320.0f}, 132, 24, true, dialogOptionButton, dialogOptionButtonHL, i);
        optBtn->onClick = [](int param){
            if(dialogManager != nullptr){dialogManager->selectOption(param);}
        };
        optBtn->setText({0,"",{4, 4},RED});
        optBtn->setVisible(false);
        this->buttonsContainer->addChildren(optBtn);
        x += 142.0f;
        
    }

    registerElement(this->buttonsContainer);
    registerElement(this->dialogContainer);
    this->buttonsContainer->setVisible(false);
    this->dialogContainer->setVisible(false);

    
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
    this->dialogContainer->setTextContent(this->curAct.lines[0].line);
    this->state = State::Active;

};

void DialogManager::advance(){
    if(this->actList.size() <= 0) return;

    if((this->curAct.lines.size() == 0 || this->lineIndex >= (int)this->curAct.lines.size() - 1) && curAct.options.size() > 0){
        this->dialogContainer->getChild(0)->setVisible(false);
        this->buttonsContainer->setVisible(true);
        for (int i = 0; i < this->curAct.options.size(); i++)
        {
            this->buttonsContainer->getChild(i)->setVisible(true);
            this->buttonsContainer->getChild(i)->setTextContent(this->curAct.options[i].title);
        }
        
    }else if(this->lineIndex == (int)this->curAct.lines.size() - 1){
        this->end();
    }
    else{
        this->lineIndex += 1;
        this->dialogContainer->setTextContent(this->curAct.lines[lineIndex].line);
    }

};

void DialogManager::selectOption(int option){
    DialogOption selectedOption = this->curAct.options[option];
    if(selectedOption.goTo < 0 || selectedOption.goTo >= this->actList.size()){this->end();}
    else {
        this->curAct = this->actList[selectedOption.goTo];
        this->lineIndex = 0;
        this->dialogContainer->setTextContent(this->curAct.lines[0].line);
        this->dialogContainer->getChild(0)->setVisible(true);
        this->buttonsContainer->setVisible(false);
    }
};

void DialogManager::end(){
    SDL_Log("sair");
    this->dialogContainer->setVisible(false);
    this->buttonsContainer->setVisible(false);
    this->state = State::Inactive;
};