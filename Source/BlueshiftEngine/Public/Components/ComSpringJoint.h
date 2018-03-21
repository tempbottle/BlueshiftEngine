// Copyright(c) 2017 POLYGONTEK
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ComJoint.h"

BE_NAMESPACE_BEGIN

class LuaVM;

class ComSpringJoint : public ComJoint {
    friend class LuaVM;

public:
    OBJECT_PROTOTYPE(ComSpringJoint);

    ComSpringJoint();
    virtual ~ComSpringJoint();

                            /// Initializes this component. Called after deserialization.
    virtual void            Init() override;

                            /// Called once when game started.
                            /// When game already started, called immediately after spawned
    virtual void            Start() override;

                            /// Visualize the component in editor
    virtual void            DrawGizmos(const SceneView::Parms &sceneView, bool selected) override;

    const Vec3 &            GetAnchor() const;
    void                    SetAnchor(const Vec3 &anchor);

    Angles                  GetAngles() const;
    void                    SetAngles(const Angles &angles);

    bool                    GetEnableLimitDistances() const;
    void                    SetEnableLimitDistances(bool enable);

    float                   GetMinimumDistance() const;
    void                    SetMinimumDistance(float minDist);
    float                   GetMaximumDistance() const;
    void                    SetMaximumDistance(float maxDist);

    float                   GetStiffness() const;
    void                    SetStiffness(float stiffness);

    float                   GetDamping() const;
    void                    SetDamping(float damping);

protected:
    Mat3                    localAxis;
    Vec3                    localAnchor;
    bool                    enableLimitDistances;
    float                   minDist;
    float                   maxDist;
    float                   stiffness;
    float                   damping;
};

BE_NAMESPACE_END
