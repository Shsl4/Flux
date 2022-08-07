#include <Interfaces/Identifiable.h>

using namespace Flux;

#ifdef __GNUG__

#include <cxxabi.h>

String demangle(const char* name) {

    int status = -1;

    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);

    if(status == 0){
        String className = demangled;
        std::free(demangled);
        return className;
    }
    
    return name;
    
}

#endif

String Identifiable::getClassName() const {

    const char* className = typeid(*this).name();
    
#ifdef __GNUG__
    return demangle(className);
#else
    return String(className).split(' ')[1];
#endif

}

String Identifiable::getDescription() const {

    return getClassName();

}