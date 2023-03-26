#pragma once

#include <Application/BodePlot.h>

namespace Flux {

    class NyquistPlot : public Component {
        
    public:
        
        NyquistPlot(Point const& p, Point const& s);

        void draw(SkCanvas* canvas, Float64 deltaTime) override;

        void drawCircle(SkCanvas* canvas, Float64 deltaTime) const;

        void initialize() override;
        
        void setFilter(Audio::Filter* fil);

        void recalculateResponse();

    private:

        Text* reText = nullptr;
        Text* imText = nullptr;
        SkPath path;
        Float32 textSize = 12.0f;
        Audio::Filter* filter = nullptr;
        ColorScheme scheme = ColorScheme::coral;
        
    };
    
}