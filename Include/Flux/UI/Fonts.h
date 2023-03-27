#pragma once

#include <skia/include/core/SkFont.h>

namespace Flux {

    class Fonts {
        
    public:

        static inline SkFont varelaRound = SkFont(SkTypeface::MakeFromFile(FLUX_RESOURCES"/Fonts/VarelaRound-Regular.ttf"));
        static inline SkFont notoSansMath = SkFont(SkTypeface::MakeFromFile(FLUX_RESOURCES"/Fonts/NotoSansMath-Regular.ttf"));
    
    };
    
}
