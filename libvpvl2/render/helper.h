/**

 Copyright (c) 2010-2013  hkrn

 All rights reserved.

 Redistribution and use in source and binary forms, with or
 without modification, are permitted provided that the following
 conditions are met:

 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials provided
   with the distribution.
 - Neither the name of the MMDAI project team nor the names of
   its contributors may be used to endorse or promote products
   derived from this software without specific prior written
   permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

*/

#include <vpvl2/vpvl2.h>
#include <vpvl2/extensions/Archive.h>
#include <vpvl2/extensions/BaseRenderContext.h>
#include <vpvl2/extensions/World.h>
#include <vpvl2/extensions/icu4c/Encoding.h>
#include <vpvl2/extensions/icu4c/StringMap.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace vpvl2;
using namespace vpvl2::extensions;

namespace ui {

static void drawScreen(const Scene &scene, size_t width, size_t height)
{
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    Array<IRenderEngine *> enginesForPreProcess, enginesForStandard, enginesForPostProcess;
    Hash<HashPtr, IEffect *> nextPostEffects;
    scene.getRenderEnginesByRenderOrder(enginesForPreProcess,
                                        enginesForStandard,
                                        enginesForPostProcess,
                                        nextPostEffects);
    for (int i = enginesForPostProcess.count() - 1; i >= 0; i--) {
        IRenderEngine *engine = enginesForPostProcess[i];
        engine->preparePostProcess();
    }
    for (int i = 0, nengines = enginesForPreProcess.count(); i < nengines; i++) {
        IRenderEngine *engine = enginesForPreProcess[i];
        engine->performPreProcess();
    }
    for (int i = 0, nengines = enginesForStandard.count(); i < nengines; i++) {
        IRenderEngine *engine = enginesForStandard[i];
        engine->renderModel();
        engine->renderEdge();
        if (!scene.shadowMapRef()) {
            engine->renderShadow();
        }
    }
    for (int i = 0, nengines = enginesForPostProcess.count(); i < nengines; i++) {
        IRenderEngine *engine = enginesForPostProcess[i];
        IEffect *const *nextPostEffect = nextPostEffects[engine];
        engine->performPostProcess(*nextPostEffect);
    }
}

static void loadSettings(const std::string &path, icu4c::StringMap &settings)
{
    std::ifstream stream(path.c_str());
    std::string line;
    UnicodeString k, v;
    while (stream && std::getline(stream, line)) {
        if (line.empty() || line.find_first_of("#;") != std::string::npos) {
            continue;
        }
        std::istringstream ss(line);
        std::string key, value;
        std::getline(ss, key, '=');
        std::getline(ss, value);
        k.setTo(UnicodeString::fromUTF8(key));
        v.setTo(UnicodeString::fromUTF8(value));
        settings[k.trim()] = v.trim();
    }
}

static void initializeDictionary(const icu4c::StringMap &settings, icu4c::Encoding::Dictionary &dictionary)
{
    dictionary.insert(IEncoding::kArm, new icu4c::String(settings.value("encoding.constant.arm", UnicodeString())));
    dictionary.insert(IEncoding::kAsterisk, new icu4c::String(settings.value("encoding.constant.asterisk", UnicodeString("*"))));
    dictionary.insert(IEncoding::kCenter, new icu4c::String(settings.value("encoding.constant.center", UnicodeString())));
    dictionary.insert(IEncoding::kElbow, new icu4c::String(settings.value("encoding.constant.elbow", UnicodeString())));
    dictionary.insert(IEncoding::kFinger, new icu4c::String(settings.value("encoding.constant.finger", UnicodeString())));
    dictionary.insert(IEncoding::kLeft, new icu4c::String(settings.value("encoding.constant.left", UnicodeString())));
    dictionary.insert(IEncoding::kOpacityMorphAsset, new icu4c::String(settings.value("encoding.constant.opacityMorphAsset", UnicodeString())));
    dictionary.insert(IEncoding::kRight, new icu4c::String(settings.value("encoding.constant.right", UnicodeString())));
    dictionary.insert(IEncoding::kRootBone, new icu4c::String(settings.value("encoding.constant.rootBoneAsset", UnicodeString())));
    dictionary.insert(IEncoding::kScaleBoneAsset, new icu4c::String(settings.value("encoding.constant.scaleBoneAsset", UnicodeString())));
    dictionary.insert(IEncoding::kSPAExtension, new icu4c::String(settings.value("encoding.constant.spa", UnicodeString(".spa"))));
    dictionary.insert(IEncoding::kSPHExtension, new icu4c::String(settings.value("encoding.constant.sph", UnicodeString(".sph"))));
    dictionary.insert(IEncoding::kWrist, new icu4c::String(settings.value("encoding.constant.wrist", UnicodeString())));
}

}
