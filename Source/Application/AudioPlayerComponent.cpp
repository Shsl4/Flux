#include <Application/AudioPlayerComponent.h>

namespace Flux {

    AudioPlayerComponent::AudioPlayerComponent(const Point &p, const Point &s,
                                               AudioPlayer *player) : Component(p, s), player(player) {

        nthrowif(!player, "Bad AudioPlayer!");

    }

    void AudioPlayerComponent::draw(SkCanvas *canvas, Float64 deltaTime) {
        Component::draw(canvas, deltaTime);
    }

    void AudioPlayerComponent::initialize() {
        Component::initialize();
    }

    void AudioPlayerComponent::drag(Flux::MouseButton button, Float64 x, Float64 y, Float64 deltaX, Float64 deltaY) {
        Reactive::drag(button, x, y, deltaX, deltaY);
    }

    void AudioPlayerComponent::keyDown(const Flux::Key &key) {
        Reactive::keyDown(key);
    }


}
