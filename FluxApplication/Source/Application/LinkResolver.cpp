#include "Application/LinkResolver.h"

#include <skia/effects/SkDashPathEffect.h>
#include <skia/effects/SkGradientShader.h>

namespace Flux{

    void LinkResolver::initialize() {
        Component::initialize();
    }

    SkScalar ps[] = { 10.0f, 10.0f };

    void LinkResolver::draw(SkCanvas *canvas, Float64 deltaTime) {

        Component::draw(canvas, deltaTime);

        SkPaint paint;
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2.0f);
        paint.setColor(UserInterface::LinearColor::fromHex(0x454545ff).toSkColor());
        paint.setPathEffect(SkDashPathEffect::Make(ps, 2, f32(linePhase)));

        for(auto const& element : pipeline->elements){

            for(size_t channel = 0; channel < element->next.getSize(); ++channel){

                for(auto const& link : element->next[channel]){

                    auto fromNode = element->getComponent()->cast<Node>();
                    auto toNode = link.pointer->getComponent().raw()->cast<Node>();

                    auto pA = fromNode->getOutputSocketPosition(channel);
                    auto pB = toNode->getInputSocketPosition(link.targetChannel);

                    SkVector points[2] = { pA, pB };
                    SkColor colors[2] = { fromNode->getHeaderColor().toSkColor().toSkColor(), toNode->getHeaderColor().toSkColor().toSkColor() };

                    auto shader = SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkTileMode::kClamp);
                    paint.setShader(shader);
                    
                    const Float32 factor = pA.x() > pB.x() ? -0.5f : 0.5f;
                    SkPath path = Socket::makeSpline(pA, pB, factor);

                    canvas->drawPath(path, paint);

                }

            }

        }

        linePhase -= 20.0 * deltaTime;

    }


}
