#include <UI/Path.h>
#include <skia/include/core/SkPath.h>

namespace Flux {

    Path::Path(){
        this->path = Shared<SkPath>::make();
    }

    Path &Path::operator=(const Path &other) {

        if (&other == this) return *this;
        this->path = Shared<SkPath>::make(*other.path);
        return *this;

    }

    Path::Path(const Path &other) {
        this->path = Shared<SkPath>::make(*other.path);
    }

    void Path::lineTo(Float32 x, Float32 y) {
        path->lineTo({x, y});
    }

    void Path::moveTo(Float32 x, Float32 y) {
        path->moveTo({x, y});
    }

    void Path::reset() const {
        path->reset();
    }

    Path::~Path() {

    }

}