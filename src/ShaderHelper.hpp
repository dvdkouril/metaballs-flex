//
//  ShaderHelper.hpp
//  MetaballsFlex
//
//  Created by David Kouril on 17.06.18.
//

#ifndef ShaderHelper_hpp
#define ShaderHelper_hpp

#include <stdio.h>
#include "cinder/gl/GlslProg.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ShaderHelper
{
    static gl::GlslProgRef loadShaders(std::string vsFilename, std::string fsFilename);
};

#endif /* ShaderHelper_hpp */
