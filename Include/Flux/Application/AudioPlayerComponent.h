#pragma once

#include <UI/Component.h>
#include <Audio/AudioPlayer.h>

namespace Flux {

    class AudioPlayerComponent : public Component {

    public:

        AudioPlayerComponent(const Point &p, const Point &s, AudioPlayer *player);

        void draw(Graphics const& graphics) override;

        void initialize() override;

        void drag(MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) override;

        void keyDown(const Key &key) override;

    private:

        AudioPlayer* player = nullptr;

    };

}