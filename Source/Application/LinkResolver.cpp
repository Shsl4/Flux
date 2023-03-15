#include <Application/LinkResolver.h>

#include <skia/include/effects/SkDashPathEffect.h>
#include <skia/include/effects/SkGradientShader.h>

#include "Application/Socket.h"

namespace Flux{

    SkScalar ps[] = { 10.0f, 10.0f };

    void LinkResolver::draw(SkCanvas *canvas, const Float64 deltaTime) {

        Component::draw(canvas, deltaTime);

        SkPaint paint;
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(2.0f);
        paint.setColor(Color::fromHex(0x454545ff).skColor());
        paint.setPathEffect(SkDashPathEffect::Make(ps, 2, f32(linePhase)));

        for(auto const& element : pipeline->getElements()){

            for(size_t channel = 0; channel < element->next.size(); ++channel){

                for(auto const& link : element->next[channel]){

                    const auto fromNode = dynamic_cast<Node*>(element->getComponent());
                    const auto toNode = dynamic_cast<Node*>(link.pointer->getComponent());

                    auto pA = fromNode->getOutputSocketPosition(static_cast<UInt>(channel));
                    auto pB = toNode->getInputSocketPosition(link.targetChannel);

                    const SkVector points[2] = { { pA.x, pA.y }, { pB.x, pB.y } };
                    const SkColor colors[2] = { fromNode->getHeaderColor().skColor().toSkColor(), toNode->getHeaderColor().skColor().toSkColor() };

                    const auto shader = SkGradientShader::MakeLinear(points, colors, nullptr, 2, SkTileMode::kClamp);
                    paint.setShader(shader);
                    
                    const Float32 factor = pA.x > pB.x ? -0.5f : 0.5f;
                    SkPath path = Socket::makeSpline({ pA.x, pA.y }, { pB.x, pB.y }, factor);

                    canvas->drawPath(path, paint);

                }

            }

        }

        linePhase -= 20.0 * deltaTime;

    }


}