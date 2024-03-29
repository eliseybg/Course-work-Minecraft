#ifndef MenuApi_hpp
#define MenuApi_hpp

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include <functional>

namespace ma
{
    class MenuHolder;
    class TextButton;
    class MenuElement;
    class Menu;
    class ButtonAccesseble;

    struct Point
    {
        Point() = default;
        Point(int x, int y) :x(x), y(y) {};

        int x;
        int y;
    };

    enum type
    {
        none = 0,
        menuHolder,
        textButton,
        function,
        iconButton,
        buttonGroup,
        onOffButton,
        plainText,
        plainSprite,
        buttonChoiceGroup,
    };

    class ButtonAccesseble
    {
    public:
        ButtonAccesseble() {};

        virtual int getType() = 0;
        virtual void execute() = 0;
    };

    class MenuElement
    {
    public:
        MenuElement() {};

        virtual void draw(sf::RenderWindow *window) = 0;
        virtual int checkInput(sf::RenderWindow *window, bool mouseReleased);
        ///this function below will be called on click by the function above ^
        virtual void additionalFunctonality() {}
        virtual int getType() = 0;
        virtual Point getSize() = 0;
        virtual void setPositionX(int x) = 0;
        virtual void setPositionY(int y) = 0;
        virtual int getPositionX() = 0;
        virtual int getPositionY() = 0;
        //Menu *context;
        ButtonAccesseble *actionType = 0;
    };

    ///this class is just a button only made out of some text
    class PlainText : public MenuElement
    {
    protected:
        sf::Font font;
    public:
        PlainText() {};
        PlainText(sf::Font f, ButtonAccesseble* action, const char* text = nullptr, int textSize = 30, sf::Color color = sf::Color::Black)
        {
            actionType = action;

            font = f;
            textContent.setCharacterSize(textSize);
            textContent.setFillColor(color);
            textContent.setFont(font);
            if (text)
            {
                textContent.setString(text);
            }
        }

        sf::Text textContent;

        virtual void draw(sf::RenderWindow *window) override;
        virtual int getType() override { return type::plainText; }
        virtual Point getSize() override;
        virtual void setPositionX(int x) override;
        virtual void setPositionY(int y) override;
        virtual int getPositionX() override;
        virtual int getPositionY() override;
    };

    ///this class is just a button only made out of a sf::sprite. You have tu supply a texture to it
    class PlainSprite : public MenuElement
    {
    protected:
        sf::Font font;
    public:
        PlainSprite() {};
        PlainSprite(sf::Texture *t, ButtonAccesseble* action)
        {
            actionType = action;

            if(t != nullptr)
            {
                s.setTexture(*t);
            }
        
        }

        sf::Sprite s;

        virtual void draw(sf::RenderWindow *window) override;
        virtual int getType() override { return type::plainSprite; }
        virtual Point getSize() override;
        virtual void setPositionX(int x) override;
        virtual void setPositionY(int y) override;
        virtual int getPositionX() override;
        virtual int getPositionY() override;
    };


    ///bref this class holds some buttons and the user cand have only one of them
    ///chosen at a time.
    class ButtonChoiceGroup : public MenuElement
    {
    protected:

        //the second element from the pair is just an offset from the first one
        std::vector<std::pair<MenuElement*, int>> buttons;
    public:

        ///this is -1 if nothing is chosen.
        int *index = nullptr;

        ButtonChoiceGroup() {};
        ButtonChoiceGroup(Menu *menu):menu(menu) {};
        
        ///this two should have the same size but it still works
        sf::Sprite chosenBackground;
        sf::Sprite notChosenBackground;

        void appendElement(MenuElement *element);
        void updateElementsPosition();

        Menu *menu;

        virtual void draw(sf::RenderWindow *window) override;
        virtual int getType() override { return type::buttonChoiceGroup; }
        virtual Point getSize() override;
        virtual void setPositionX(int x) override;
        virtual void setPositionY(int y) override;
        virtual int getPositionX() override;
        virtual int getPositionY() override;
        virtual int checkInput(sf::RenderWindow *window, bool mouseReleased) override;
    
        friend Menu;
    };


    ///bref this is the main menu class in where you will be storing your menu group.
    ///!!!It needs to have a pointer to a MenuHolder in where you will put your buttons.!!!
    class Menu
    {
    public:
        Menu() {}
        Menu(sf::RenderWindow *window) :window(window){}

        //the first element of the pair is the index in menu, the second is the index in the subGroup
        std::vector<std::pair<int, int>> stack;
        MenuHolder *mainMenu = 0;
        MenuElement *backButton = 0;
        
        ///I couldn't figure out how to set this programatically so i leave this up to your personal choice.
        ///this variables set the padding of the back button starting from the backgrouns position's left upper corner.
        int backButtonPaddingx = 160;
        int backButtonPaddingy = 80;


        sf::RenderWindow *window = 0;
        sf::Sprite background;

        ///i don't recommand changing this values by hand but if you want to do so, go for it
        int backgroundPositionx = 0;
        int backgroundPositiony = 0;

        ///this variable will determin if the api should or should not
        ///take into consideration the resize of the screen and resize it's elements per frame.
        ///if you do something like window.setView(sf::View({ 0.f, 0.f, (float)size.x, (float)size.y }));
        ///you might want to set this to true for convenience.
        ///If you don't want to do so for performance reasons, you have to update it manually when the window resizes
        ///see updateBackgrounsPosition
        bool checkForResize = true;

        void updateElementsPosition(MenuHolder *h = nullptr);
        void updateBackgrounsPosition();

        int update(bool mouseReleased, bool escapeReleased = 0);
        void resetStack() { stack.clear(); }
    };



    ///bref this class can hold other menu elements inside it.
    ///!!!It MUST have a refference to it's Menu since it needs to know things like the size of
    ///the window.!!!
    class MenuHolder : public  ButtonAccesseble
    {
    protected:
        std::vector<MenuElement*> elements;
    public:
        MenuHolder(Menu *menu = nullptr):menu(menu) {};
        
        Menu *menu;

        void appendElement(MenuElement *e);
        void updateElementsPosition();

        int getType() override { return type::menuHolder; }
        void execute() override {};

        friend Menu;
    };

    ///bref this is a function that is accessed by a clicked button.
    ///all it does is to wrap the std::function type
    class Function : public  ButtonAccesseble
    {
    public:
        Function() {};
        Function(std::function<void()> functionPointer) :functionPointer(functionPointer) {};

        std::function<void()> functionPointer;

        virtual int getType() override{ return type::function; };
        virtual void execute() override;
    };

    ///bref this is a button with some text in it and a sprite with a texture
    class TextButton : public virtual MenuElement
    {
    protected:
        sf::Font font;
    public:
        TextButton() {};
        TextButton(sf::Texture *t, sf::Font f, ButtonAccesseble* action, const char* text = nullptr, int textSize = 30, sf::Color color = sf::Color::Black)
        {
            actionType = action;
            if( t!= nullptr)
            {
                s.setTexture(*t);
            }

            font = f;
            textContent.setCharacterSize(textSize);
            textContent.setFillColor(color);
            textContent.setFont(font);
            if(text)
            {
                textContent.setString(text);
            }
        }

        sf::Sprite s;
        sf::Text textContent;


        virtual void draw(sf::RenderWindow *window) override;
        virtual int getType() override { return type::textButton; }
        virtual Point getSize() override;
        virtual void setPositionX(int x) override;
        virtual void setPositionY(int y) override;
        virtual int getPositionX() override;
        virtual int getPositionY() override;
    };

    ///bref This is a button with a sprite in it and a background sprite,
    ///hence have tu supply 2 textures to it. It won't error out if you don't.
    class IconButton : public virtual MenuElement
    {
    public:
        IconButton() {};
        IconButton(sf::Texture *background, sf::Texture *foreground, ButtonAccesseble* action)
        {
            actionType = action;
            
            if (background != nullptr)
            {
                backgroundSprite.setTexture(*background);
            }

            if(foreground != nullptr)
            {
                foregroundSprite.setTexture(*foreground);
            }
        }

        sf::Sprite backgroundSprite;
        sf::Sprite foregroundSprite;


        virtual void draw(sf::RenderWindow *window) override;
        virtual int getType() override { return type::iconButton; }
        virtual Point getSize() override;
        virtual void setPositionX(int x) override;
        virtual void setPositionY(int y) override;
        virtual int getPositionX() override;
        virtual int getPositionY() override;
    
    };

    ///bref the OnOffButton can be on or off and writes to a boll variable that you supply the adress of
    class OnOffButton : public virtual MenuElement
    {
    public:
        OnOffButton() {};
        OnOffButton(sf::Texture *background, sf::Texture *onState, sf::Texture *offState, bool *data ,ButtonAccesseble* action = nullptr)
        {
            actionType = action;
            this->data = data;
            if (background != nullptr)
            {
                backgroundSprite.setTexture(*background);
            }

            if (onState != nullptr)
            {
                onStateSprite.setTexture(*onState);
            }

            if(offState != nullptr)
            {
                offStateSprite.setTexture(*offState);
            }
        }

        sf::Sprite backgroundSprite;
        sf::Sprite onStateSprite;
        sf::Sprite offStateSprite;

        bool *data = nullptr;

        virtual void draw(sf::RenderWindow *window) override;
        virtual int getType() override { return type::onOffButton; }
        virtual Point getSize() override;
        virtual void setPositionX(int x) override;
        virtual void setPositionY(int y) override;
        virtual int getPositionX() override;
        virtual int getPositionY() override;
        virtual void additionalFunctonality() override;

    };

    ///bref This is an horizontal group of buttons.
    ///You need something like this because the buttons are placed vertical by default in the menu.
    ///I reccomand using it with small buttons.
    class ButtonGroup : public MenuElement
    {
    protected:

        //the second element from the pair is just an offset from the first one
        std::vector<std::pair<MenuElement*, int>> buttons;
    public:
        ButtonGroup() {};
        ButtonGroup(Menu *menu):menu(menu) {};
        
        void appendElement(MenuElement *element);
        void updateElementsPosition();

        Menu *menu = nullptr;

        virtual void draw(sf::RenderWindow *window) override;
        virtual int getType() override { return type::buttonGroup; }
        virtual Point getSize() override;
        virtual void setPositionX(int x) override;
        virtual void setPositionY(int y) override;
        virtual int getPositionX() override;
        virtual int getPositionY() override;
        virtual int checkInput(sf::RenderWindow *window, bool mouseReleased) override;
    
        friend Menu;
    };



}

#endif /* MenuApi_hpp */
