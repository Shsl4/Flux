#pragma once

#include <skia/include/core/SkFont.h>

namespace Flux {

    class Fonts {
        
    public:

        static inline SkFont varelaRound = SkFont(SkTypeface::MakeFromFile("../Resources/Fonts/VarelaRound-Regular.ttf"));
        static inline SkFont notoSansMath = SkFont(SkTypeface::MakeFromFile("../Resources/Fonts/NotoSansMath-Regular.ttf"));
    
    };
    
}
