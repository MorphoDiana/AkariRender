// MIT License
//
// Copyright (c) 2019 椎名深雪
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef AKARIRENDER_SCENEGRAPH_H
#define AKARIRENDER_SCENEGRAPH_H

#include <Akari/Core/Component.h>
#include <Akari/Render/Accelerator.h>
#include <Akari/Render/Camera.h>
#include <Akari/Render/Integrator.h>
#include <Akari/Render/Light.h>
#include <Akari/Render/Material.h>
#include <Akari/Render/Mesh.h>
#include <Akari/Render/Sampler.h>
#include <fstream>

namespace Akari {
    struct RenderSetting {
        std::shared_ptr<Camera> camera;

        std::shared_ptr<Sampler> sampler;
        std::shared_ptr<Integrator> integrator;
        fs::path output;
        AKR_SER(camera, sampler, integrator, output)
    };
    struct SceneSetting {
        std::vector<MeshWrapper> meshes;
        std::shared_ptr<Accelerator> accelerator;
        AKR_SER(meshes, accelerator)
    };
    struct AKR_EXPORT SceneGraph {
        std::vector<RenderSetting> render;
        SceneSetting scene;
        AKR_SER(render, scene)
        std::shared_ptr<RenderTask> CreateRenderTask(int settingId);
        void Commit();

      private:
        void CommitSetting(RenderSetting &setting);
        std::shared_ptr<Scene> pScene;
    };
} // namespace Akari

#endif // AKARIRENDER_SCENEGRAPH_H
