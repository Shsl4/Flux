#include <Application/AudioPlayerComponent.h>

namespace Flux {

    AudioPlayerComponent::AudioPlayerComponent(const Point &p) : Component(p, { 400, 300 }) {

        constexpr Float32 knobSize = 50;

        this->renderer = Factory::createComponent<WaveFormRenderer>(Point::zero, Point(400, 150), this);

        transposeKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        transposeKnob->setRange({-24, 24});
        transposeKnob->setDefaultValue(0);
        transposeKnob->setLabelText("Transpose");
        transposeKnob->setLabelPrecision(0);
        transposeKnob->addListener(this);
        transposeKnob->setScheme(scheme);

        startKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        startKnob->setDefaultValue(0);
        startKnob->setLabelText("Start");
        startKnob->setLabelPrecision(2);
        startKnob->addListener(this);
        startKnob->setScheme(scheme);

        endKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        endKnob->setDefaultValue(0);
        endKnob->setLabelText("End");
        endKnob->setLabelPrecision(2);
        endKnob->addListener(this);
        endKnob->setScheme(scheme);

        gainKnob = Factory::createComponent<RotaryKnob>(Point::zero, knobSize);
        gainKnob->setRange({0.0, Audio::toAmplitude(2.0)});
        gainKnob->setDefaultValue(Audio::toAmplitude(-6.0));
        gainKnob->setLabelText("Gain");
        gainKnob->setLabelExtension("dB");
        gainKnob->setLabelPrecision(1);
        gainKnob->addListener(this);
        gainKnob->setScheme(scheme);
        gainKnob->useCustomFormatFunction([](Float64 v, String const& extension) {
            if(Math::deq(0.0, v)) return String::format("-Inf {}", extension);
            return String::format("{.2} {}", Audio::toDecibels(v), extension);
        });

    }

    void AudioPlayerComponent::draw(Graphics const& graphics) {

        const Transform t = globalTransform();

        graphics.setColor(scheme.darkest);
        graphics.drawRect(t);

        for(auto const& child : children()){
            child->draw(graphics);
        }

    }

    void AudioPlayerComponent::initialize() {

        Component::initialize();

        auto stack = Factory::createComponent<VStack>(Point::zero, size(), VAlignment::center, HAlignment::center);
        auto topStack = Factory::createComponent<HStack>(Point::zero, Point(size().x, 150), VAlignment::center, HAlignment::center);
        auto bottomStack = Factory::createComponent<HStack>(Point::zero, Point(size().x, 150), VAlignment::center, HAlignment::center);

        bottomStack->setSpacing(25.0);

        stack->addChild(topStack);
        stack->addChild(bottomStack);

        topStack->addChild(renderer);
        bottomStack->addChild(startKnob);
        bottomStack->addChild(endKnob);
        bottomStack->addChild(transposeKnob);
        bottomStack->addChild(gainKnob);

        addChild(stack);

    }

    void AudioPlayerComponent::linkAudioPlayer(AudioPlayer *audioPlayer) {
        this->player = audioPlayer;
        this->player->addListener(this);
        this->gainKnob->setValue(player->amplitude(), false);
    }

    void AudioPlayerComponent::valueChanged(Slider *knob, Float64 newValue) {

        if (knob == transposeKnob){
            player->transpose(i32(newValue));
        }

        if (knob == startKnob){
            player->setStartTime(newValue);
            endKnob->setRange({newValue, player->duration()});
        }

        if (knob == endKnob){
            player->setEndTime(newValue);
            startKnob->setRange({0.0, newValue});
        }

        if (knob == gainKnob){
            player->setAmplitude(newValue);
        }

    }

    void AudioPlayerComponent::fileLoaded(WaveFile *newFile) {

        renderer->refreshWaveform();

        this->startKnob->setRange({ 0.0, player->duration()});
        this->startKnob->setDefaultValue(0.0);
        this->startKnob->setValue(0.0, false);

        this->endKnob->setRange({ 0.0, player->duration()});
        this->endKnob->setDefaultValue(player->duration());
        this->endKnob->setValue(player->duration(), false);

    }

    void AudioPlayerComponent::WaveFormRenderer::draw(const Graphics &graphics) {

        const Transform t = globalTransform();

        graphics.setColor(playerComponent->scheme.lightest);
        graphics.setStrokeWidth(1.0);
        graphics.setAntiAliasing(true);
        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);

        graphics.drawPath(waveform);

        static const Color blackTransparent = Color::fromHex(0x00000088);

        const Float64 sTime = player()->startTime();
        const Float64 eTime = player()->endTime();

        const Float32 xsOff = t.size.x * f32(sTime) / f32(player()->duration());
        const Float32 xeOff = t.size.x * f32(eTime) / f32(player()->duration());

        graphics.setColor(blackTransparent);

        graphics.drawRect(t.position, { xsOff, t.size.y});

        graphics.setColor(blackTransparent);

        graphics.drawRect(t.position + Point(xeOff, 0.0), { t.size.x - xeOff, t.size.y});

        Float64 progress = player()->progress();
        Float32 drawX = t.position.x + t.size.x * f32(progress);

        graphics.setStrokeWidth(1.0);
        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);
        graphics.setColor(Color::white);
        graphics.drawLine({ drawX, t.position.y }, {drawX, t.position.y + t.size.y });

        graphics.setColor(playerComponent->scheme.base);
        graphics.setStrokeWidth(1.0);
        graphics.setStrokeStyle(Graphics::StrokeStyle::stroke);

        graphics.drawRect(t);

    }

    void AudioPlayerComponent::WaveFormRenderer::buttonDown(MouseButton button, Float64 x, Float64 y) {

        if(button == MouseButton::Left){
            updatePlayTime(f32(x));
        }

    }

    void AudioPlayerComponent::WaveFormRenderer::drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX,
                                                      Float64 deltaY) {
        if(button == MouseButton::Left){
            updatePlayTime(f32(x));
        }

    }

    void AudioPlayerComponent::WaveFormRenderer::refreshWaveform() {

        if(!player()) return;

        WaveFile* file = player()->file();

        if(!file) return;

        waveform.reset();

        const auto step = file->samplesPerChannel / waveformResolution;

        const Point pos = globalTransform().position;

        for(size_t i = 0; i < waveformResolution; i++) {

            const Float64 value = (Math::clamp(file->buffers[0][i * step] * 0.5, -1.0, 1.0) + 1.0) / 2.0;
            const Float32 drawX = pos.x + size().x * (f32(i) / f32(waveformResolution));
            const Float32 drawY = pos.y + size().y * f32(value);

            const Point newPoint = { drawX, drawY };

            if(i == 0) {
                waveform.moveTo(newPoint.x, newPoint.y);
            }
            else{
                waveform.lineTo(newPoint.x, newPoint.y);
            }

        }


    }

    void AudioPlayerComponent::WaveFormRenderer::updatePlayTime(Float32 mouseX) {

        AudioPlayer* audioPlayer = player();

        const Transform t = globalTransform();

        Float32 linValue = Math::clamp((mouseX - t.position.x) / t.size.x, 0.0f, 1.0f);

        audioPlayer->setPlayHead(linValue * audioPlayer->duration());

    }

    AudioPlayerComponent::WaveFormRenderer::WaveFormRenderer(const Point &p, const Point &s, AudioPlayerComponent* component)
        : Component(p, s), playerComponent(component) {

    }

    void AudioPlayerComponent::WaveFormRenderer::keyDown(const Key &key) {

        if (key.key == GLFW_KEY_SPACE){
            if (player()->isPlaying()){
                player()->pause();
            }
            else{
                player()->play();
            }
        }

    }

    void AudioPlayerComponent::WaveFormRenderer::scroll(Float64 deltaX, Float64 deltaY) {

        player()->setPlayHead(Math::clampAround(player()->currentTime() - 0.1 * deltaY, player()->startTime(), player()->endTime()));

    }

    void AudioPlayerComponent::WaveFormRenderer::modified() {
        refreshWaveform();
    }
}
