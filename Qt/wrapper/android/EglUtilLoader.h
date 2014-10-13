// Copyright (c) 2014, Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include "ObjectReference.h"
#include "ConcreetClassLoader.h"
#include "TypeRemap.h"
namespace AMPLoader {

    template<class JNI>
    class EglUtilLoader:  public ConcreetClassLoader<JNI> {
        REMAP_TYPES(JNI);
        typedef ConcreetClassLoader<JNI> base;
    public:
        EglUtilLoader(JNI & jni, _ClassLoader &load, _TypeAdapter &adaptor) 
            : base(jni, load, adaptor, "com/intel/inde/mp/android/graphics/EglUtil", methods){
            typename JNI::_jclass loadedClass = base::loadedClass;
            methods[egetInstance] = load.LoadStaticMethod(loadedClass, "getInstance", "()Lcom/intel/inde/mp/android/graphics/EglUtil;");
            methods[egetCurrentSurfaceResolution] = load.LoadMethod(loadedClass, "getCurrentSurfaceResolution", "()Lcom/intel/inde/mp/domain/Resolution;");
            methods[ecreateProgram] = load.LoadMethod(loadedClass, "createProgram", "(Ljava/lang/String;Ljava/lang/String;)Lcom/intel/inde/mp/domain/graphics/Program;");
            methods[ecreateTexture] = load.LoadMethod(loadedClass, "createTexture", "(I)I");
            methods[edrawFrameStart] = load.LoadMethod(loadedClass, "drawFrameStart", "(Lcom/intel/inde/mp/domain/graphics/Program;Ljava/nio/FloatBuffer;[F[FFLcom/intel/inde/mp/domain/graphics/TextureType;ILcom/intel/inde/mp/domain/Resolution;Z)V");
            methods[edrawFrameFinish] = load.LoadMethod(loadedClass, "drawFrameFinish", "()V");
            methods[edrawFrame] = load.LoadMethod(loadedClass, "drawFrame", "(Lcom/intel/inde/mp/domain/graphics/Program;Ljava/nio/FloatBuffer;[F[FFLcom/intel/inde/mp/domain/graphics/TextureType;ILcom/intel/inde/mp/domain/Resolution;Z)V");
            methods[echeckEglError] = load.LoadMethod(loadedClass, "checkEglError", "(Ljava/lang/String;)V");
            methods[esetIdentityMatrix] = load.LoadMethod(loadedClass, "setIdentityMatrix", "([FI)V");
        }
    public:
        enum {
            egetInstance,
            egetCurrentSurfaceResolution,
            ecreateProgram,
            ecreateTexture,
            edrawFrameStart,
            edrawFrameFinish,
            edrawFrame,
            echeckEglError,
            esetIdentityMatrix,
            eLastFnc
        };
    private:
        _jmethodID methods[eLastFnc];
    };
}
