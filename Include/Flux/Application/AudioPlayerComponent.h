#pragma once

#include <Factory.h>
#include <Audio/AudioPlayer.h>
#include <UI/Stack.h>
#include <Application/RotaryKnob.h>

namespace Flux {

class AudioPlayerComponent : public Component, public Slider::Listener, public AudioPlayer::Listener {

    public:

        class WaveFormRenderer : public Component {


        public:

            WaveFormRenderer(Point const& p, Point const& s, AudioPlayerComponent* component);

            void draw(const Graphics &graphics) override;

            void buttonDown(MouseButton button, Float64 x, Float64 y) override;

            void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

            void refreshWaveform();

        protected:

            void modified() override;

        public:

            void keyDown(const Key &key) override;

            void scroll(Float64 deltaX, Float64 deltaY) override;

            NODISCARD FORCEINLINE AudioPlayer* player() const { return playerComponent->player; }

        private:

            const AudioPlayerComponent* playerComponent;
            void updatePlayTime(Float32 mouseX);

            Path waveform;
            size_t waveformResolution = 4096;

        };

        AudioPlayerComponent(const Point &p);

        void draw(Graphics const& graphics) override;

        void initialize() override;

        void linkAudioPlayer(AudioPlayer* audioPlayer);

        void valueChanged(Slider *knob, Float64 newValue) override;

        void fileLoaded(WaveFile *newFile) override;

private:

        ColorScheme scheme = ColorScheme::darkScheme(Color::tintOrange);

        RotaryKnob* transposeKnob = nullptr;
        RotaryKnob* startKnob = nullptr;
        RotaryKnob* endKnob = nullptr;
        RotaryKnob* gainKnob = nullptr;

        WaveFormRenderer* renderer = nullptr;
        AudioPlayer* player = nullptr;

    };

}