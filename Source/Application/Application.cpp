#define _CRT_SECURE_NO_WARNINGS
#include <Application/Application.h>

#include <UI/DropdownMenu.h>
#include <UI/Text.h>
#include <UI/Stack.h>
#include <UI/FrameInfo.h>
#include <UI/Button.h>
#include <Examples/ColorsExample.h>
#include <Examples/HelloWorld.h>
#include <Examples/StacksExample.h>
#include <type_traits>
#include <Application/BodePlot.h>
#include <rtaudio/RtAudio.h>
#include <UI/SceneComponent.h>

#include <Utility/Timer.h>

using namespace Flux;

namespace Flux {

    class PopoutMenu : public Component {
    
    public:
        
        PopoutMenu(Point const& p, Float32 width, MutableArray<String> const& choices) : Component(p, { width, choices.size() * 25.0f }) {

            setColor(Colors::transparent);
            
            for (auto const& choice : choices) {
                
                auto* button = Factory::createComponent<Button>(Point::zero, Point(width, 25.0f), choice);
                button->setAction([this] (Button* b){ if(callback) callback(b); });
                button->label()->setAlignment(VAlignment::center, HAlignment::left);
                choiceButtons += button;
                
            }
            
        }

        void setCallback(Function<void(Button*)> const& f) {
            this->callback = f;
        }

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            SkPaint paint;
            paint.setAntiAlias(true);
            const auto t = globalTransform();
            const SkRect rect = SkRect::MakeXYWH(t.position.x, t.position.y, t.size.x, t.size.y);

            paint.setColor(style.scheme.base.skColor());
            canvas->drawRoundRect(rect, 0, 0, paint);
            paint.setColor(style.borderColor.skColor());
            paint.setStyle(SkPaint::Style::kStroke_Style);
            paint.setStrokeWidth(0.5);
            canvas->drawRoundRect(rect, 0, 0, paint);

            for(const auto& child : children()) {
                child->draw(canvas, deltaTime);
            }
            
        }
        
        void initialize() override {

            auto* stack = Factory::createComponent<VStack>(Point::zero, size(), VAlignment::center, HAlignment::center);
            
            for (auto const& button : choiceButtons) {
                stack->addChild(button);
            }

            addChild(stack);
            
        }

        void setStyle(Button::Style const& value) {
            this->style = value;

            Button::Style st = value;
            st.borderColor = Colors::transparent;
            
            for (auto& button : choiceButtons) {
                button->setStyle(st);
            }
        }

    protected:
        
        void parentLinked() override {
            window()->cursorManager()->focusReactive(this);
        }
    
    private:

        Button::Style style;
        Function<void(Button*)> callback = nullptr;
        MutableArray<Button*> choiceButtons = {};

    };

    
    class Dropdown : public Component {
            
    public:

        using DropdownCallback = Function<void(size_t, String const&)>;
            
        Dropdown(Point const& p, Point const& s) : Component(p, s) {

            this->button = Factory::createComponent<Button>(Point::zero, size(), "None");
                
        }

        void buttonPressed() {

            if(Factory::valid(popout)) {

                popout->dispose();
                return;
                
            }

            auto pos = button->globalTransform().position;
            pos.y += button->size().y;
                
            this->popout = Factory::createComponent<PopoutMenu>(pos, button->size().x, choices);

            popout->setStyle(this->button->style());
            
            popout->setCallback([this](const Button* choice) {

                for (size_t i = 0; i < choices.size(); ++i) {
                    if(choice->label()->text() == choices[i]) {
                        setSelectedIndex(i);
                        break;
                    }
                }

                popout->dispose();
                
            });

            button->window()->mainComponent()->addChild(popout);
                
        }

        void setCallback(DropdownCallback const& value) { this->callback = value; }
        
        void initialize() override {
                
            button->setAction([this](Button*) { buttonPressed(); });
            addChild(button);
                
        }

        void setStyle(Button::Style const& value) {
            this->button->setStyle(value);
        }

        void setChoices(MutableArray<String> const& value) {

            MutableArray<String> checked = {};
            
            for (auto const& elem : value) {
                if(!checked.contains(elem)) {
                    checked += elem;
                }
            }
            
            this->choices = checked;
            setSelectedIndex(0, false);
            
        }

        bool setSelectedIndex(size_t index, bool notify = true) {

            if(choices.size() == 0) {
                button->label()->setText("None");
                this->selectedChoice = 0;
                return false;
            }

            
            if (index > choices.size()) { return false; }
            this->selectedChoice = index;
            button->label()->setText(choices[selectedChoice]);

            
            if(notify) {
                callback(selectedChoice, choices[selectedChoice]);
            }
            
            return true;
            
        }

        bool setSelected(String const& value) {

            if(choices.size() == 0) {
                button->label()->setText("None");
                this->selectedChoice = 0;
                return false;
            }

            for (size_t i = 0; i < choices.size(); ++i) {
                if(value == choices[i]) {
                    this->selectedChoice = i;
                    button->label()->setText(choices[i]);
                    return true;
                }
            }
            
            return false;
            
        }
        
    private:
        
        size_t selectedChoice = 0;
        MutableArray<String> choices = {};
        Button* button = nullptr;
        PopoutMenu* popout = nullptr;
        DropdownCallback callback;
            
    };
    
    class AudioSettings : public Component {
        
    public:
        
        AudioSettings(Point const& p, Point const& s, AudioEngine* engine) : Component(p, s), engine(engine) {

            setColor(Colors::transparent);
            
            constexpr Float32 textMaxWidth = 150;
            constexpr Float32 textHeight = 35;
            constexpr Float32 buttonMaxWidth = 250;
            constexpr Float32 buttonHeight = 25;
            constexpr Float32 fontHeight = 12.0f;
            constexpr Float32 elemWidth = textMaxWidth + buttonMaxWidth;
            const ColorScheme buttonScheme = ColorScheme::fromHex(0x202020ff);

            Button::Style style(fontHeight, buttonScheme, Colors::white, HAlignment::left, VAlignment::center);

            this->driverDropdown = Factory::createComponent<Dropdown>(Point::zero, Point(buttonMaxWidth, buttonHeight));
            this->sampleRateDropdown = Factory::createComponent<Dropdown>(Point::zero, Point(buttonMaxWidth, buttonHeight));
            this->bufferSizeDropdown = Factory::createComponent<Dropdown>(Point::zero, Point(buttonMaxWidth, buttonHeight));
            this->inputDropdown = Factory::createComponent<Dropdown>(Point::zero, Point(buttonMaxWidth, buttonHeight));
            this->outputDropdown = Factory::createComponent<Dropdown>(Point::zero, Point(buttonMaxWidth, buttonHeight));

            driverDropdown->setStyle(style);
            sampleRateDropdown->setStyle(style);
            bufferSizeDropdown->setStyle(style);
            inputDropdown->setStyle(style);
            outputDropdown->setStyle(style);
            
            engine->addOpenCallback([this] {
                refresh(false);
            });

        }

        void refresh(bool notify = true) {

            std::vector<RtAudio::Api> apis = {};
            RtAudio::getCompiledApi(apis);

            auto apiNames = MutableArray<String>(apis.size());
            
            for(auto const& api : apis) { apiNames += apiName(api); }
            
            driverDropdown->setChoices(apiNames);
            driverDropdown->setSelected(apiName(engine->api()));
            driverDropdown->setCallback([this](size_t, String const& value) {

                engine->setApi(nameToApi(value));
                
            });

            MutableArray<String> sampleRates = {};

            for (auto const& sr : engine->supportedSampleRates()) {
                sampleRates += String::fromFloatingPoint(sr);
            }

            sampleRateDropdown->setChoices(sampleRates);
            sampleRateDropdown->setSelected(String::fromFloatingPoint(engine->sampleRate()));
            sampleRateDropdown->setCallback([this](size_t, String const& value) {

                engine->setSampleRate(value.toFloat64());
                
            });
            
            MutableArray<String> bufferSizes = {};

            for (auto const& size : engine->supportedBufferSizes()) {
                bufferSizes += String::fromInteger(size);
            }

            bufferSizeDropdown->setChoices(bufferSizes);
            bufferSizeDropdown->setSelected(String::fromInteger(engine->bufferSize()));
            bufferSizeDropdown->setCallback([this](size_t, String const& value) {

                engine->setBufferSize(i32(value.toInteger()));
                
            });
            
            MutableArray<String> inputDevices = { "None" };

            for (auto const& dev : engine->enumerateInputDevices()) {
                inputDevices += dev.name();
            }

            inputDropdown->setChoices(inputDevices);
            inputDropdown->setSelected(engine->inputDevice().valid()? engine->inputDevice().name() : "None");
            inputDropdown->setCallback([this](size_t, String const& value) {
                
                for (auto const& dev : engine->enumerateInputDevices()) {

                    if(dev.name() == value) {
                        
                        engine->setInputDevice(dev);
                        return;
                        
                    }
                    
                }

                engine->setInputDevice(AudioDevice());
                
            });
            
            MutableArray<String> outputDevices = { "None" };

            for (auto const& dev : engine->enumerateOutputDevices()) {
                outputDevices += dev.name();
            }

            outputDropdown->setChoices(outputDevices);
            outputDropdown->setSelected(engine->outputDevice().valid()? engine->outputDevice().name() : "None");
            outputDropdown->setCallback([this](size_t, String const& value) {
                
                for (auto const& dev : engine->enumerateOutputDevices()) {

                    if(dev.name() == value) {
                        
                        engine->setOutputDevice(dev);
                        return;
                        
                    }
                    
                }

                engine->setOutputDevice(AudioDevice());
                
            });

        }
        
        void initialize() override {
                        
            constexpr Float32 textMaxWidth = 150;
            constexpr Float32 textHeight = 35;
            constexpr Float32 buttonMaxWidth = 250;
            constexpr Float32 fontHeight = 12.0f;
            constexpr Float32 buttonHeight = 25;

            constexpr Float32 elemWidth = textMaxWidth + buttonMaxWidth;

            auto* stack = Factory::createComponent<VStack>(Point::zero, size(), VAlignment::center, HAlignment::center);

            auto* stack0 = Factory::createComponent<HStack>(Point::zero, Point::zero, VAlignment::center, HAlignment::center);
            auto* comp = Factory::createComponent<Component>(Point::zero, Point(buttonMaxWidth, buttonHeight));
            comp->setVisible(false);

            stack0->setSpacing(size().x - elemWidth - elemWidth * 0.05f);
            stack0->addChild(Factory::createComponent<Text>(Point::zero, Point(textMaxWidth, textHeight), "Audio Settings", 18.0f));
            stack0->addChild(comp);

            auto* stack1 = Factory::createComponent<HStack>(Point::zero, Point::zero, VAlignment::center, HAlignment::center);
            stack1->addChild(Factory::createComponent<Text>(Point::zero, Point(textMaxWidth, textHeight), "Driver Type: ", fontHeight));
            stack1->setSpacing(size().x - elemWidth - elemWidth * 0.05f);
            stack1->addChild(driverDropdown);

            auto* stack2 = Factory::createComponent<HStack>(Point::zero, Point::zero, VAlignment::center, HAlignment::center);
            stack2->addChild(Factory::createComponent<Text>(Point::zero, Point(textMaxWidth, textHeight), "Sample Rate: ", fontHeight));
            stack2->setSpacing(size().x - elemWidth - elemWidth * 0.05f);
            stack2->addChild(sampleRateDropdown);

            auto* stack3 = Factory::createComponent<HStack>(Point::zero, Point::zero, VAlignment::center, HAlignment::center);
            stack3->addChild(Factory::createComponent<Text>(Point::zero, Point(textMaxWidth, textHeight), "Buffer Size: ", fontHeight));
            stack3->setSpacing(size().x - elemWidth - elemWidth * 0.05f);
            stack3->addChild(bufferSizeDropdown);

            auto* stack4 = Factory::createComponent<HStack>(Point::zero, Point::zero, VAlignment::center, HAlignment::center);
            stack4->addChild(Factory::createComponent<Text>(Point::zero, Point(textMaxWidth, textHeight), "Input device: ", fontHeight));
            stack4->setSpacing(size().x - elemWidth - elemWidth * 0.05f);
            stack4->addChild(inputDropdown);

            auto* stack5 = Factory::createComponent<HStack>(Point::zero, Point::zero, VAlignment::center, HAlignment::center);
            stack5->addChild(Factory::createComponent<Text>(Point::zero, Point(textMaxWidth, textHeight), "Output device: ", fontHeight));
            stack5->setSpacing(size().x - elemWidth - elemWidth * 0.05f);
            stack5->addChild(outputDropdown);

            stack->addChild(stack0);
            stack->addChild(stack1);
            stack->addChild(stack2);
            stack->addChild(stack3);
            stack->addChild(stack4);
            stack->addChild(stack5);
            this->addChild(stack);

            refresh();
            
        }

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            SkPaint paint;
            paint.setAntiAlias(true);
            const auto t = globalTransform();
            const SkRect rect = SkRect::MakeXYWH(t.position.x, t.position.y, t.size.x, t.size.y);

            paint.setColor(Color::fromHex(0x202020ff).skColor());
            canvas->drawRoundRect(rect, 0, 0, paint);
            paint.setColor(SkColors::kWhite);
            paint.setStyle(SkPaint::Style::kStroke_Style);
            paint.setStrokeWidth(0.5);
            canvas->drawRoundRect(rect, 0, 0, paint);

            for(const auto& child : children()) {
                child->draw(canvas, deltaTime);
            }
            
        }

    private:

        Dropdown* driverDropdown = nullptr;
        Dropdown* sampleRateDropdown = nullptr;
        Dropdown* bufferSizeDropdown = nullptr;
        Dropdown* inputDropdown = nullptr;
        Dropdown* outputDropdown = nullptr;
        AudioEngine* engine = nullptr;

    };

    void Application::run() {
        
        if(application) throw Exceptions::Exception( "Tried to run multiple Applications at the same time");

        application = this;

        this->console = Unique<Console>::make();
        this->factory = Unique<Factory>::make();

        glfwSetErrorCallback(&Application::onError);

        if(!glfwInit()){
            throw Exceptions::Exception("Failed to initialize GLFW");
        }

        constexpr Int windowWidth = 1280;
        constexpr Int windowHeight = 720;

        
        auto* scene = Factory::createComponent<SceneComponent>(Point::zero, Point(windowWidth, windowHeight)); 
        this->mainWindow = Factory::createWindow(scene, RenderBackend::Best, "Application", windowWidth, windowHeight);
        
        constexpr Float32 fw = 100.0f;
        constexpr Float32 fh = 100.0f;

        initializeAudio();
        
        scene->addChild(Factory::createComponent<AudioSettings>(Point::zero, Point(500, 250), this->audioDevice.pointer()));
        
        Point s = { fw, fh };

        //factory->windows()[0]->mainComponent()->addChild(Factory::createComponent<FrameInfo>(Point::zero, s));
        
        this->shouldRun = true;

        Console::log("Created new Flux Application.\n");

        update();

        Console::log("Exiting Application.\n");

        glfwMakeContextCurrent(nullptr);
        
        this->audioDevice->close();

        this->factory = nullptr;

        glfwTerminate();

    }

    void Application::stop() { shouldRun = false; }

    void Application::draw()  {

        const Float64 currentTime = glfwGetTime();
        const Float64 deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        for(const auto& window : Factory::windows()){

            window->draw(deltaTime);

        }

    }

    void Application::initializeAudio() {

        this->audioDevice = Shared<Engine>::make();

        audioDevice->initialize(44100, 128);

    }

    void Application::update() {

        while (shouldRun) {

            this->draw();
            glfwPollEvents();

        }

    }

    void Application::onError(Int errorCode, const char* description) {
        Console::error("GLFW Error {}: {}\n", errorCode, description);
    }

    void Application::onApplicationTerminate() { Console::log("Application received termination signal\n"); }

    void Application::registerCommands() {

    }

}
