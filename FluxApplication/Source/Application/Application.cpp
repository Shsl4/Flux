#include <Application/Application.h>

#include <skia/core/SkDrawable.h>
#include <skia/core/SkPath.h>
#include <skia/core/SkRRect.h>
#include <skia/core/SkFont.h>

#include <Application/DragComponent.h>
#include <Application/Node.h>
#include <Application/Socket.h>

#include <Application/LinkResolver.h>

#define GL_FRAMEBUFFER_BINDING 0x8CA6

namespace Flux {
    
    class FrameInfo : public DragComponent {

    public:
        
        void initialize() override {

            setScale({150, 45});
            setColor(UserInterface::LinearColor::fromHex(0x606060ff));

        }
        
        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            const Float64 currentTime = glfwGetTime();
            ++frameCount;

            if(currentTime - lastTime >= 1.0) {

                lastFrameTime = (1000.0 / static_cast<Float64>(frameCount));
                frameCount = 0;
                lastTime += 1.0;
                
            }
            
            SkPaint paint;
            
            paint.setColor(getColor().toSkColor());

            const SkVector pos = getAbsolutePosition();
            SkRect rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), getScale().y());

            canvas->drawRect(rect, paint);
            
            paint.setColor(UserInterface::LinearColor::fromHex(0x147ff6ff).toSkColor());
            rect = SkRect::MakeXYWH(pos.x(), pos.y(), getScale().x(), 5.0f);
            canvas->drawRect(rect, paint);
            
            paint.setColor(SK_ColorWHITE);

            String text = String::format("Frame time: {}ms", lastFrameTime);
            SkFont font;
            font.setSize(12.0);

            canvas->drawSimpleText(text.toCString(), text.getSize(), SkTextEncoding::kUTF8, pos.fX + 10.0f,
                                   pos.fY + 30.0f, font, paint);

            
        }

    private:

        Float64 lastTime = 0.0;
        UInt frameCount = 0;
        Float64 lastFrameTime = 0.0;
        
    };

    class RotaryKnob : public UserInterface::Component {

    public:
        
        void onDrag(UserInterface::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override {

            rotation = Math::clamp(rotation + deltaY * 0.35, -120.0, 120.0);

            Console::logDebug("{} {}", rotation, deltaY);
            
        }

        void onDoubleClick(UserInterface::MouseButton button, Float64 x, Float64 y) override {

            if(button == UserInterface::MouseButton::Left) {
                rotation = 0.0;
            }
            
        }

        void initialize() override {

            setScale({ 50, 50 });
            setColor(UserInterface::LinearColor::fromHex(0x3b3b3bff));
            
        }
        
        static inline auto inner1 = UserInterface::LinearColor::fromHex(0xadaaa7ff);
        static inline auto inner2 = UserInterface::LinearColor::fromHex(0x797979ff);

        void draw(SkCanvas* canvas, Float64 deltaTime) override {

            SkPaint paint;
            const SkVector pos = getAbsolutePosition();
            const SkVector size = getScale();
            const SkVector mid = getAbsoluteCenteredPosition();
            const Float32 rad = size.x() / 2.0f;
            paint.setColor(getColor().toSkColor());
            canvas->save();
            canvas->rotate(f32(rotation), mid.fX, mid.fY);
            canvas->drawCircle(mid, rad, paint);
            paint.setColor(SK_ColorWHITE);
            canvas->drawCircle(mid, rad * 0.9f, paint);
            paint.setColor(inner1.toSkColor());
            canvas->drawCircle(mid, rad * 0.85f, paint);
            paint.setColor(inner2.toSkColor());
            canvas->drawCircle(mid, rad * 0.8f, paint);
            paint.setColor(getColor().toSkColor());
            canvas->drawCircle(mid, rad * 0.75f, paint);
            paint.setStrokeCap(SkPaint::kRound_Cap);
            paint.setStrokeWidth(rad * 0.05f);
            paint.setColor(SK_ColorWHITE);
            canvas->drawLine(mid.fX, mid.fY, pos.fX + size.fX / 2.0f, pos.fY + size.fY * 0.1f, paint);
            canvas->restore();
            
        }

    private:

        Float64 rotation = 0.0;
        
    };

    template<typename NumberType>
    struct Range{

    public:

        Range(NumberType min, NumberType max) : min(min), max(max) {}

        NODISCARD static Range makeLinearRange() { return { static_cast<NumberType>(0), static_cast<NumberType>(1) }; }

        NODISCARD FORCEINLINE NumberType getMin() const{ return this->min; }

        NODISCARD FORCEINLINE NumberType getMax() const{ return this->max; }

    private:

        NumberType min;
        NumberType max;

    };

    template<typename NumberType>
    NumberType translateValue(NumberType value, Range<NumberType> fromRange, Range<NumberType> toRange){

        NumberType fromValue = fromRange.getMax() - fromRange.getMin();
        NumberType toValue = toRange.getMax() - toRange.getMin();
        return (((value - fromRange.getMin()) * toValue) / fromValue) + toRange.getMin();

    }


    class FilterDrawer : public UserInterface::Component {

    public:

        void initialize() override {
            
            Component::initialize();
            setScale({ 400, 200 });
            setPosition({ 500, 300 });
            setColor(UserInterface::LinearColor::fromHex(0x303030ff));
            filter.prepare(44100, 512);
            filter.setResonance(10);
            
        }

        Float64 freqToRad(Float64 f, Float64 ny){
            return (f * Math::pi<Float64>) / ny;
        }

        Float64 t = 0.0;
        Float64 f = 10.0;
        Float64 dir = 1.0;
        
        void drawGrid(SkCanvas* canvas){
            
            SkPaint paint;
            paint.setStrokeWidth(2.5);
            paint.setColor(UserInterface::LinearColor::fromHex(0x454545ff).toSkColor());

            const Range<Float64> logRange = { log10(10), log10(44100.0 / 2.0) };
            const Range<Float64> linRange = Range<Float64>::makeLinearRange();
            const SkVector position = getAbsolutePosition();
            const SkVector scale = getScale();
            
            canvas->drawLine(position.fX, position.fY + scale.fY / 2.0, position.fX + scale.fX, position.fY + scale.fY / 2.0, paint);
            paint.setStrokeWidth(1.0);

            for (size_t p = 1; p <= 4; ++p) {
                
                Float64 pw = std::pow(10.0, f64(p));
                
                for (size_t d = 1; d <= 9; ++d) {
                    
                    Float64 f = pw * f64(d);
                    
                    const Float64 drawX = translateValue(log10(f), logRange, linRange) * scale.fX + position.fX;
                    
                    if(drawX > position.fX + scale.fX) return;
                    
                    canvas->drawLine(drawX, position.fY, drawX, position.fY + scale.fY, paint);
                    
                }
                
            }
            
        }

        void draw(SkCanvas *canvas, Float64 deltaTime) override {

            UserInterface::Component::draw(canvas, deltaTime);
            drawGrid(canvas);

            SkPaint paint;
            
            paint.setStyle(SkPaint::kStroke_Style);
            paint.setStrokeWidth(2.0f);
            paint.setColor(SK_ColorWHITE);

            SkPath path;

            constexpr Float64 sr = 44100;
            constexpr Float64 nyquist = sr / 2.0;
            constexpr Float64 pi = Math::pi<Float64>;
            constexpr UInt points = nyquist / 2;
            constexpr Float64 interval = pi / f64(points);
            constexpr Float64 mindB = -40.0;
            constexpr Float64 maxdB = 40.0;
            
            f = Math::easeIn(10.0, nyquist, f32(t), 5);

            t += deltaTime / 2.0;

            if(t >= 1.0){
                t = 0.0;
            }

            filter.setCutoffFrequency(f);

            const SkVector pos = getPosition();

            Float64 omega = freqToRad(10.0, nyquist);

            SkVector lastPoint = getPosition();
            lastPoint.fY += getScale().fY;

            // todo: Find a correct distribution algorithm
            for (UInt i = 0; i <= points; ++i) {

                const Float64 mag = filter.getMagnitude(omega);
                const Float64 response = 20.0 * log10(mag);
                const Float64 f = (omega / pi) * nyquist;
                const Float64 finalResponse = Math::clamp(response, mindB, maxdB);
                const Range<Float64> logRange = { log10(10), log10(nyquist) };
                const Range<Float64> linRange = Range<Float64>::makeLinearRange();

                const Float64 normalizedResponse = ((finalResponse - mindB)) / (maxdB - mindB);
                const Float64 normalizedFrequency = translateValue(log10(f), logRange, linRange);

                omega += interval;

                if(!isfinite(normalizedResponse)) continue;

                Float32 drawX = pos.fX + f32(normalizedFrequency * getScale().fX);
                Float32 drawY = pos.fY + getScale().fY - f32(normalizedResponse * getScale().fY);

                SkVector newPoint = { drawX, drawY };

                // todo: Fix this
                if(i == 0){
                    path.moveTo(newPoint);
                }
                else{
                    path.moveTo(lastPoint);
                    if(finalResponse != mindB)
                        path.lineTo(newPoint);
                }

                lastPoint = newPoint;

            }

            canvas->drawPath(path, paint);

        }

    private:

        Audio::LowPassFilter filter;

    };

    void Application::run() {

        fassertf(!instance, "Tried to run multiple Applications at the same time");

        instance = this;

        this->console = UniquePointer<Console>::make();

        console->init();

        setTerminationCallback(&Application::onApplicationTerminate);

        registerCommands();

        glfwSetErrorCallback(&Application::onError);

        fassertf(glfwInit(), "Failed to initialize GLFW");
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_DEPTH_BITS, 0);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        constexpr Int windowWidth = 1280;
        constexpr Int windowHeight = 720;
        
        this->mainWindow = glfwCreateWindow(windowWidth, windowHeight, "Application", nullptr, nullptr);

        fassertf(this->mainWindow, "Failed to create window");

        glfwMakeContextCurrent(mainWindow);

        // Disable VSync
        glfwSwapInterval(1);

        glfwSetKeyCallback(mainWindow, &Application::inputCallback);
        glfwSetMouseButtonCallback(mainWindow, &Application::mouseCallback);
        glfwSetScrollCallback(mainWindow, &Application::scrollCallback);
        glfwSetCursorPosCallback(mainWindow, &Application::cursorCallback);
        
        Int framebufferWidth;
        Int framebufferHeight;

        glfwGetFramebufferSize(mainWindow, &framebufferWidth, &framebufferHeight);

        Int32 framebufferId = 0;
        auto glGetIntegerv = (void(*)(UInt32, Int32*))glfwGetProcAddress("glGetIntegerv");
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebufferId);
        
        this->masterView = UserInterface::MasterView::makeGL(framebufferWidth, framebufferHeight, {framebufferId, 4, 8, f32(framebufferWidth) / f32(windowWidth), f32(framebufferHeight) / f32(windowHeight)});

        initializeAudio();

        this->cursorManager = SharedPointer<UserInterface::CursorManager>::make();
        cursorManager->setCompound(masterView);
        
        masterView->addChild<FrameInfo>();
        
        auto knob = masterView->addChild<RotaryKnob>();
        knob->setPosition({100, 100});
        
        this->shouldRun = true;

        this->masterView->addChild<LinkResolver>()->pipeline = audioDevice->getPipeline();
        this->masterView->addChild<FilterDrawer>();

        Console::logStatus("Created new Flux Application.");

        update();

        Console::logStatus("Exiting Application.");

        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(mainWindow);
        glfwTerminate();

    }

    void Application::stop() { shouldRun = false; }

    void Application::draw() {

        
        const Float64 currentTime = glfwGetTime();
        const Float64 deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        masterView->draw(nullptr, deltaTime);
        glfwSwapBuffers(mainWindow);

    }

    void Application::initializeAudio() {

        this->audioDevice = SharedPointer<MyAudioDevice>::make();

        audioDevice->initialize(44100, 128);

    }

    void Application::update() {
        while (shouldRun && !glfwWindowShouldClose(mainWindow)) {
            
            glfwPollEvents();
            draw();
        }
    }

    void Application::onError(Int errorCode, const char* description) {
        Console::logRuntime("GLFW Error {}: {}", errorCode, description);
    }

    void Application::onApplicationTerminate() { Console::logFatal("Application received termination signal"); }

    void Application::registerCommands() {

        const auto stopNode = CommandNode::make("Application.Stop");

        stopNode->setNodeDescription("Exits the application.");
        stopNode->addExecutable([](auto) { instance->stop(); });

        Console::registerCommand(stopNode);

    }

}
