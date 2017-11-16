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

#include "Precompiled.h"
#include "Core/Serializable.h"
#include "Core/Object.h"
#include "Math/Math.h"

BE_NAMESPACE_BEGIN

const SignalDef Serializable::SIG_PropertyChanged("Serializable::PropertyChanged", "si");
const SignalDef Serializable::SIG_PropertyArrayCountChanged("Serializable::PropertyArrayCountChanged", "s");
const SignalDef Serializable::SIG_PropertyInfoUpdated("Serializable::PropertyInfoUpdated");

bool Serializable::GetPropertyInfo(int index, PropertyInfo &propertyInfo) const {
    Array<PropertyInfo> propertyInfoList;
    GetPropertyInfoList(propertyInfoList);

    if (index < 0 || index > propertyInfoList.Count() - 1) {
        return false;
    }

    propertyInfo = propertyInfoList[index];
    return true;
}

bool Serializable::GetPropertyInfo(const char *name, PropertyInfo &propertyInfo) const {
    Array<PropertyInfo> propertyInfoList;
    GetPropertyInfoList(propertyInfoList);

    for (int i = 0; i < propertyInfoList.Count(); i++) {
        if (!Str::Cmp(propertyInfoList[i].GetName(), name)) {
            propertyInfo = propertyInfoList[i];
            return true;
        }
    }

    return false;
}

void Serializable::Serialize(Json::Value &out) const {
    Array<PropertyInfo> propertyInfoList;

    GetPropertyInfoList(propertyInfoList);

    for (int propertyIndex = 0; propertyIndex < propertyInfoList.Count(); propertyIndex++) {
        const PropertyInfo &propertyInfo = propertyInfoList[propertyIndex];

        if (propertyInfo.GetFlags() & PropertyInfo::SkipSerializationFlag) {
            continue;
        }

        const char *name = propertyInfo.name.c_str();

        if (propertyInfo.GetFlags() & PropertyInfo::ArrayFlag) {
            out[name] = Json::arrayValue;

            for (int elementIndex = 0; elementIndex < GetPropertyArrayCount(propertyIndex); elementIndex++) {
                Variant value;
                GetArrayProperty(propertyInfo, elementIndex, value);

                out[name][elementIndex] = value.ToJsonValue();
            }
        } else {
            Variant value;
            GetProperty(propertyInfo, value);

            out[name] = value.ToJsonValue();
        }
    }
}

void Serializable::Deserialize(const Json::Value &node) {
    Array<PropertyInfo> propertyInfoList;

    GetPropertyInfoList(propertyInfoList);

    for (int propertyIndex = 0; propertyIndex < propertyInfoList.Count(); propertyIndex++) {
        const PropertyInfo &propertyInfo = propertyInfoList[propertyIndex];

        if (propertyInfo.GetFlags() & PropertyInfo::ReadOnlyFlag) {
            continue;
        }

        const char *name = propertyInfo.name.c_str();
        const Variant::Type type = propertyInfo.GetType();
        const Variant defaultValue = propertyInfo.GetDefaultValue();

        if (propertyInfo.GetFlags() & PropertyInfo::ArrayFlag) {
            const Json::Value subNode = node.get(name, Json::Value());

            SetPropertyArrayCount(propertyIndex, subNode.size());

            for (int elementIndex = 0; elementIndex < subNode.size(); elementIndex++) {
                switch (type) {
                case Variant::IntType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<int>());
                    SetArrayProperty(propertyIndex, elementIndex, value.asInt());
                    break;
                }
                case Variant::Int64Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<int64_t>());
                    SetArrayProperty(propertyIndex, elementIndex, value.asInt64());
                    break;
                }
                case Variant::BoolType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<bool>());
                    SetArrayProperty(propertyIndex, elementIndex, value.asBool());
                    break; 
                }
                case Variant::FloatType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<float>());
                    SetArrayProperty(propertyIndex, elementIndex, value.asFloat());
                    break; 
                }
                case Variant::Vec2Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Vec2>().ToString());
                    const char *s = value.asCString();
                    Vec2 v;
                    sscanf(s, "%f %f", &v.x, &v.y);
                    SetArrayProperty(propertyIndex, elementIndex, v);
                    break; 
                }
                case Variant::Vec3Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Vec3>().ToString());
                    const char *s = value.asCString();
                    Vec3 v;
                    sscanf(s, "%f %f %f", &v.x, &v.y, &v.z);
                    SetArrayProperty(propertyIndex, elementIndex, v);
                    break; 
                }
                case Variant::Vec4Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Vec4>().ToString());
                    const char *s = value.asCString();
                    Vec4 v;
                    sscanf(s, "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);
                    SetArrayProperty(propertyIndex, elementIndex, v);
                    break; 
                }
                case Variant::Color3Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Color3>().ToString());
                    const char *s = value.asCString();
                    Color3 v;
                    sscanf(s, "%f %f %f", &v.r, &v.g, &v.b);
                    SetArrayProperty(propertyIndex, elementIndex, v);
                    break; 
                }
                case Variant::Color4Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Color4>().ToString());
                    const char *s = value.asCString();
                    Color4 v;
                    sscanf(s, "%f %f %f %f", &v.r, &v.g, &v.b, &v.a);
                    SetArrayProperty(propertyIndex, elementIndex, v);
                    break; 
                }
                case Variant::Mat2Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Mat2>().ToString());
                    const char *s = value.asCString();
                    Mat2 m;
                    sscanf(s, "%f %f %f %f", &m[0].x, &m[0].y, &m[1].x, &m[1].y);
                    SetArrayProperty(propertyIndex, elementIndex, m);
                    break;
                }
                case Variant::Mat3Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Mat3>().ToString());
                    const char *s = value.asCString();
                    Mat3 m;
                    sscanf(s, "%f %f %f %f %f %f %f %f %f", &m[0].x, &m[0].y, &m[0].z, &m[1].x, &m[1].y, &m[1].z, &m[2].x, &m[2].y, &m[2].z);
                    SetArrayProperty(propertyIndex, elementIndex, m);
                    break;
                }
                case Variant::Mat3x4Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Mat3x4>().ToString());
                    const char *s = value.asCString();
                    Mat3x4 m;
                    sscanf(s, "%f %f %f %f %f %f %f %f %f %f %f %f", &m[0].x, &m[0].y, &m[0].z, &m[0].w, &m[1].x, &m[1].y, &m[1].z, &m[1].w, &m[2].x, &m[2].y, &m[2].z, &m[2].w);
                    SetArrayProperty(propertyIndex, elementIndex, m);
                    break;
                }
                case Variant::Mat4Type: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Mat4>().ToString());
                    const char *s = value.asCString();
                    Mat4 m;
                    sscanf(s, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &m[0].x, &m[0].y, &m[0].z, &m[0].w, &m[1].x, &m[1].y, &m[1].z, &m[1].w, &m[2].x, &m[2].y, &m[2].z, &m[2].w, &m[3].x, &m[3].y, &m[3].z, &m[3].w);
                    SetArrayProperty(propertyIndex, elementIndex, m);
                    break;
                }
                case Variant::AnglesType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Angles>().ToString());
                    const char *s = value.asCString();
                    Angles a;
                    sscanf(s, "%f %f %f", &a.yaw, &a.pitch, &a.roll);
                    SetArrayProperty(propertyIndex, elementIndex, a);
                    break; 
                }
                case Variant::QuatType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Quat>().ToString());
                    const char *s = value.asCString();
                    Quat q;
                    sscanf(s, "%f %f %f %f", &q.x, &q.y, &q.z, &q.w);
                    SetArrayProperty(propertyIndex, elementIndex, q);
                    break;
                }
                case Variant::PointType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Point>().ToString());
                    const char *s = value.asCString();
                    Point p;
                    sscanf(s, "%i %i", &p.x, &p.y);
                    SetArrayProperty(propertyIndex, elementIndex, p);
                    break;
                }
                case Variant::RectType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Rect>().ToString());
                    const char *s = value.asCString();
                    Rect r;
                    sscanf(s, "%i %i %i %i", &r.x, &r.y, &r.w, &r.h);
                    SetArrayProperty(propertyIndex, elementIndex, r);
                    break;
                }
                case Variant::GuidType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Guid>().ToString());
                    Guid guid = Guid::FromString(value.asCString());
                    SetArrayProperty(propertyIndex, elementIndex, guid);
                    break;
                }
                case Variant::StrType: {
                    const Json::Value value = subNode.get(elementIndex, defaultValue.As<Str>().c_str());
                    SetArrayProperty(propertyIndex, elementIndex, Str(value.asCString()));
                    break;
                }
                default:
                    assert(0);
                    break;
                }
            }
        } else {
            const Json::Value value = node.get(name, defaultValue.ToJsonValue());

            switch (type) {
            case Variant::IntType:
                SetProperty(propertyIndex, value.asInt());
                break;
            case Variant::Int64Type:
                SetProperty(propertyIndex, value.asInt64());
                break;
            case Variant::BoolType:
                SetProperty(propertyIndex, value.asBool());
                break;
            case Variant::FloatType:
                SetProperty(propertyIndex, value.asFloat());
                break;
            case Variant::Vec2Type: {
                const char *s = value.asCString();
                Vec2 v;
                sscanf(s, "%f %f", &v.x, &v.y);
                SetProperty(propertyIndex, v);
                break;
            }
            case Variant::Vec3Type: {
                const char *s = value.asCString();
                Vec3 v;
                sscanf(s, "%f %f %f", &v.x, &v.y, &v.z);
                SetProperty(propertyIndex, v);
                break;
            }
            case Variant::Vec4Type: {
                const char *s = value.asCString();
                Vec4 v;
                sscanf(s, "%f %f %f %f", &v.x, &v.y, &v.z, &v.w);
                SetProperty(propertyIndex, v);
                break;
            }
            case Variant::Color3Type: {
                const char *s = value.asCString();
                Color3 v;
                sscanf(s, "%f %f %f", &v.r, &v.g, &v.b);
                SetProperty(propertyIndex, v);
                break;
            }
            case Variant::Color4Type: {
                const char *s = value.asCString();
                Color4 v;
                sscanf(s, "%f %f %f %f", &v.r, &v.g, &v.b, &v.a);
                SetProperty(propertyIndex, v);
                break;
            }
            case Variant::Mat2Type: {
                const char *s = value.asCString();
                Mat2 m;
                sscanf(s, "%f %f %f %f", &m[0].x, &m[0].y, &m[1].x, &m[1].y);
                SetProperty(propertyIndex, m);
                break;
            }
            case Variant::Mat3Type: {
                const char *s = value.asCString();
                Mat3 m;
                sscanf(s, "%f %f %f %f %f %f %f %f %f", &m[0].x, &m[0].y, &m[0].z, &m[1].x, &m[1].y, &m[1].z, &m[2].x, &m[2].y, &m[2].z);
                SetProperty(propertyIndex, m);
                break;
            }
            case Variant::Mat3x4Type: {
                const char *s = value.asCString();
                Mat3x4 m;
                sscanf(s, "%f %f %f %f %f %f %f %f %f %f %f %f", &m[0].x, &m[0].y, &m[0].z, &m[0].w, &m[1].x, &m[1].y, &m[1].z, &m[1].w, &m[2].x, &m[2].y, &m[2].z, &m[2].w);
                SetProperty(propertyIndex, m);
                break;
            }
            case Variant::Mat4Type: {
                const char *s = value.asCString();
                Mat4 m;
                sscanf(s, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &m[0].x, &m[0].y, &m[0].z, &m[0].w, &m[1].x, &m[1].y, &m[1].z, &m[1].w, &m[2].x, &m[2].y, &m[2].z, &m[2].w, &m[3].x, &m[3].y, &m[3].z, &m[3].w);
                SetProperty(propertyIndex, m);
                break;
            }
            case Variant::AnglesType: {
                const char *s = value.asCString();
                Angles a;
                sscanf(s, "%f %f %f", &a.yaw, &a.pitch, &a.roll);
                SetProperty(propertyIndex, a);
                break;
            }
            case Variant::QuatType: {
                const char *s = value.asCString();
                Quat q;
                sscanf(s, "%f %f %f %f", &q.x, &q.y, &q.z, &q.w);
                SetProperty(propertyIndex, q);
                break;
            }
            case Variant::PointType: {
                const char *s = value.asCString();
                Point p;
                sscanf(s, "%i %i", &p.x, &p.y);
                SetProperty(propertyIndex, p);
                break;
            }
            case Variant::RectType: {
                const char *s = value.asCString();
                Rect r;
                sscanf(s, "%i %i %i %i", &r.x, &r.y, &r.w, &r.h);
                SetProperty(propertyIndex, r);
                break;
            }
            case Variant::GuidType: {
                Guid guid = Guid::FromString(value.asCString());
                SetProperty(propertyIndex, guid);
                break;
            }
            case Variant::StrType:
                SetProperty(propertyIndex, Str(value.asCString()));
                break;
            default:
                assert(0);
                break;
            }
        }
    }
}

Variant Serializable::GetPropertyDefault(const char *name) const {
    PropertyInfo propertyInfo;
    Variant out;

    if (!GetPropertyInfo(name, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetPropertyDefault: invalid property name '%hs'\n", name);
        return out;
    }

    out = propertyInfo.GetDefaultValue();
    return out;
}

Variant Serializable::GetPropertyDefault(int index) const {
    PropertyInfo propertyInfo;
    Variant out;

    if (!GetPropertyInfo(index, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetPropertyDefault: invalid property index %i\n", index);
        return out;
    }

    out = propertyInfo.GetDefaultValue();
    return out;
}

Variant Serializable::GetProperty(int index) const {
    PropertyInfo propertyInfo;
    Variant out;

    if (!GetPropertyInfo(index, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetProperty: invalid property index %i\n", index);
        return out;
    }

    GetProperty(propertyInfo, out);
    return out;
}

Variant Serializable::GetProperty(const char *name) const {
    PropertyInfo propertyInfo;
    Variant out;

    if (!GetPropertyInfo(name, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetProperty: invalid property name '%hs'\n", name);
        return out;
    }

    GetProperty(propertyInfo, out);
    return out;
}

void Serializable::GetProperty(const PropertyInfo &propertyInfo, Variant &out) const {
    if (propertyInfo.accessor) {
        propertyInfo.accessor->Get(this, out);
        return;
    }

    const void *src = reinterpret_cast<const byte *>(this) + propertyInfo.offset;

    switch (propertyInfo.GetType()) {
    case Variant::IntType:
        out = *(reinterpret_cast<const int *>(src));
        break;
    case Variant::Int64Type:
        out = *(reinterpret_cast<const int64_t *>(src));
        break;
    case Variant::BoolType:
        out = *(reinterpret_cast<const bool *>(src));
        break;
    case Variant::FloatType:
        out = *(reinterpret_cast<const float *>(src));
        break;
    case Variant::Vec2Type:
        out = *(reinterpret_cast<const Vec2 *>(src));
        break;
    case Variant::Vec3Type:
        out = *(reinterpret_cast<const Vec3 *>(src));
        break;
    case Variant::Vec4Type:
        out = *(reinterpret_cast<const Vec4 *>(src));
        break;
    case Variant::Color3Type:
        out = *(reinterpret_cast<const Color3 *>(src));
        break;
    case Variant::Color4Type:
        out = *(reinterpret_cast<const Color4 *>(src));
        break;
    case Variant::AnglesType:
        out = *(reinterpret_cast<const Angles *>(src));
        break;
    case Variant::Mat2Type:
        out = *(reinterpret_cast<const Mat2 *>(src));
        break;
    case Variant::Mat3Type:
        out = *(reinterpret_cast<const Mat3 *>(src));
        break;
    case Variant::Mat3x4Type:
        out = *(reinterpret_cast<const Mat3x4 *>(src));
        break;
    case Variant::Mat4Type:
        out = *(reinterpret_cast<const Mat4 *>(src));
        break;
    case Variant::PointType:
        out = *(reinterpret_cast<const Point *>(src));
        break;
    case Variant::RectType:
        out = *(reinterpret_cast<const Rect *>(src));
        break;
    case Variant::GuidType:
        out = *(reinterpret_cast<const Guid *>(src));
        break;
    case Variant::StrType:
        out = *(reinterpret_cast<const Str *>(src));
        break;
    case Variant::MinMaxCurveType:
        out = *(reinterpret_cast<const MinMaxCurve *>(src));
        break;
    }

    return;
}

Variant Serializable::GetArrayProperty(int index, int elementIndex) const {
    PropertyInfo propertyInfo;
    Variant out;

    if (!GetPropertyInfo(index, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetArrayProperty: invalid property index %i\n", index);
        return out;
    }

    GetArrayProperty(propertyInfo, elementIndex, out);
    return out;
}

Variant Serializable::GetArrayProperty(const char *name, int elementIndex) const {
    PropertyInfo propertyInfo;
    Variant out;

    if (!GetPropertyInfo(name, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetArrayProperty: invalid property name '%hs'\n", name);
        return out;
    }

    GetArrayProperty(propertyInfo, elementIndex, out);
    return out;
}

void Serializable::GetArrayProperty(const PropertyInfo &propertyInfo, int elementIndex, Variant &out) const {
    if (!(propertyInfo.GetFlags() & PropertyInfo::ArrayFlag)) {
        BE_WARNLOG(L"Serializable::GetArrayProperty: property '%hs' is not array\n", propertyInfo.name.c_str());
        return;
    }

    if (propertyInfo.accessor) {
        propertyInfo.accessor->Get(this, elementIndex, out);
        return;
    }

    const void *src = reinterpret_cast<const byte *>(this) + propertyInfo.offset;

    switch (propertyInfo.GetType()) {
    case Variant::IntType:
        out = (*reinterpret_cast<const Array<int> *>(src))[elementIndex];
        break;
    case Variant::Int64Type:
        out = (*reinterpret_cast<const Array<int64_t> *>(src))[elementIndex];
        break;
    case Variant::BoolType:
        out = (*reinterpret_cast<const Array<bool> *>(src))[elementIndex];
        break;
    case Variant::FloatType:
        out = (*reinterpret_cast<const Array<float> *>(src))[elementIndex];
        break;
    case Variant::Vec2Type:
        out = (*reinterpret_cast<const Array<Vec2> *>(src))[elementIndex];
        break;
    case Variant::Vec3Type:
        out = (*reinterpret_cast<const Array<Vec3> *>(src))[elementIndex];
        break;
    case Variant::Vec4Type:
        out = (*reinterpret_cast<const Array<Vec4> *>(src))[elementIndex];
        break;
    case Variant::Color3Type:
        out = (*reinterpret_cast<const Array<Color3> *>(src))[elementIndex];
        break;
    case Variant::Color4Type:
        out = (*reinterpret_cast<const Array<Color4> *>(src))[elementIndex];
        break;
    case Variant::AnglesType:
        out = (*reinterpret_cast<const Array<Angles> *>(src))[elementIndex];
        break;
    case Variant::Mat2Type:
        out = (*reinterpret_cast<const Array<Mat2> *>(src))[elementIndex];
        break;
    case Variant::Mat3Type:
        out = (*reinterpret_cast<const Array<Mat3> *>(src))[elementIndex];
        break;
    case Variant::Mat3x4Type:
        out = (*reinterpret_cast<const Array<Mat3x4> *>(src))[elementIndex];
        break;
    case Variant::Mat4Type:
        out = (*reinterpret_cast<const Array<Mat4> *>(src))[elementIndex];
        break;
    case Variant::PointType:
        out = (*reinterpret_cast<const Array<Point> *>(src))[elementIndex];
        break;
    case Variant::RectType:
        out = (*reinterpret_cast<const Array<Rect> *>(src))[elementIndex];
        break;
    case Variant::GuidType:
        out = (*reinterpret_cast<const Array<Guid> *>(src))[elementIndex];
        break;
    case Variant::StrType:
        out = (*reinterpret_cast<const Array<Str> *>(src))[elementIndex];
        break;
    case Variant::MinMaxCurveType:
        out = (*reinterpret_cast<const Array<MinMaxCurve> *>(src))[elementIndex];
        break;
    }
}

bool Serializable::SetProperty(const char *name, const Variant &value, bool forceWrite) {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(name, propertyInfo)) {
        BE_WARNLOG(L"Serializable::SetProperty: invalid property name '%hs'\n", name);
        return false;
    }

    return SetProperty(propertyInfo, value, forceWrite);
}

bool Serializable::SetProperty(int index, const Variant &value, bool forceWrite) {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(index, propertyInfo)) {
        BE_WARNLOG(L"Serializable::SetProperty: invalid property index %i\n", index);
        return false;
    }

    return SetProperty(propertyInfo, value, forceWrite);
}

bool Serializable::SetProperty(const PropertyInfo &propertyInfo, const Variant &value, bool forceWrite) {
    // You can force to write a value even though a property has read only flag.
    if (!forceWrite && (propertyInfo.GetFlags() & PropertyInfo::ReadOnlyFlag)) {
        BE_WARNLOG(L"Serializable::SetProperty: property '%hs' is readonly\n", propertyInfo.name.c_str());
        return false;
    }

    Variant newValue;
    float minValue;
    float maxValue;

    bool ranged = propertyInfo.IsRanged();

    if (ranged) {
        minValue = propertyInfo.GetMinValue();
        maxValue = propertyInfo.GetMaxValue();
    }

    switch (propertyInfo.GetType()) {
    case Variant::IntType: {
        int i = value.As<int>();
        if (ranged) {
            Clamp(i, (int)minValue, (int)maxValue);
        }
        newValue = i;
        break; 
    }
    case Variant::Int64Type: {
        int64_t i = value.As<int64_t>();
        if (ranged) {
            Clamp(i, (int64_t)minValue, (int64_t)maxValue);
        }
        newValue = i;
        break;
    }
    case Variant::BoolType:
        newValue = value.As<bool>();
        break;
    case Variant::FloatType: {
        float f = value.As<float>();
        if (ranged) {
            Clamp(f, minValue, maxValue);
        }
        newValue = f;
        break;
    }
    case Variant::Vec2Type: {
        Vec2 vec2 = value.As<Vec2>();
        if (ranged) {
            Clamp(vec2.x, minValue, maxValue);
            Clamp(vec2.y, minValue, maxValue);
        }
        newValue = vec2;
        break; 
    }
    case Variant::Vec3Type: {
        Vec3 vec3 = value.As<Vec3>();
        if (ranged) {
            Clamp(vec3.x, minValue, maxValue);
            Clamp(vec3.y, minValue, maxValue);
            Clamp(vec3.z, minValue, maxValue);
        }
        newValue = vec3;
        break; 
    }
    case Variant::Vec4Type: {
        Vec4 vec4 = value.As<Vec4>();
        if (ranged) {
            Clamp(vec4.x, minValue, maxValue);
            Clamp(vec4.y, minValue, maxValue);
            Clamp(vec4.z, minValue, maxValue);
            Clamp(vec4.w, minValue, maxValue);
        }
        newValue = vec4;
        break; 
    }
    case Variant::Color3Type: {
        Color3 color3 = value.As<Color3>();
        if (ranged) {
            Clamp(color3.r, minValue, maxValue);
            Clamp(color3.g, minValue, maxValue);
            Clamp(color3.b, minValue, maxValue);
        }
        newValue = color3;
        break; 
    }
    case Variant::Color4Type: {
        Color4 color4 = value.As<Color4>();
        if (ranged) {
            Clamp(color4.r, minValue, maxValue);
            Clamp(color4.g, minValue, maxValue);
            Clamp(color4.b, minValue, maxValue);
            Clamp(color4.a, minValue, maxValue);
        }
        newValue = color4;
        break; 
    }
    case Variant::Mat2Type:
        newValue = value.As<Mat2>();
        break;
    case Variant::Mat3Type:
        newValue = value.As<Mat3>();
        break;
    case Variant::Mat3x4Type:
        newValue = value.As<Mat3x4>();
        break;
    case Variant::Mat4Type:
        newValue = value.As<Mat4>();
        break;
    case Variant::AnglesType: {
        Angles angles = value.As<Angles>();
        if (ranged) {
            Clamp(angles[0], minValue, maxValue);
            Clamp(angles[1], minValue, maxValue);
            Clamp(angles[2], minValue, maxValue);
        }
        newValue = angles;
        break;
    }
    case Variant::PointType: {
        Point pt = value.As<Point>();
        if (ranged) {
            Clamp(pt.x, (int)minValue, (int)maxValue);
            Clamp(pt.y, (int)minValue, (int)maxValue);
        }
        newValue = pt;
        break;
    }
    case Variant::RectType: {
        Rect rect = value.As<Rect>();
        if (ranged) {
            Clamp(rect.x, (int)minValue, (int)maxValue);
            Clamp(rect.y, (int)minValue, (int)maxValue);
            Clamp(rect.w, (int)minValue, (int)maxValue);
            Clamp(rect.h, (int)minValue, (int)maxValue);
        }
        newValue = rect;
        break;
    }
    case Variant::GuidType:
        newValue = value.As<Guid>();
        break;
    case Variant::StrType:
        newValue = value.As<Str>();
        break;
    case Variant::MinMaxCurveType:
        newValue = value.As<MinMaxCurve>();
        break;
    default:
        assert(0);
        break;
    }

    if (propertyInfo.accessor) {
        propertyInfo.accessor->Set(this, newValue);
    } else {
        void *dest = reinterpret_cast<byte *>(this) + propertyInfo.offset;

        switch (propertyInfo.GetType()) {
        case Variant::IntType:
            *(reinterpret_cast<int *>(dest)) = newValue.As<int>();
            break;
        case Variant::Int64Type:
            *(reinterpret_cast<int64_t *>(dest)) = newValue.As<int64_t>();
            break;
        case Variant::BoolType: 
            *(reinterpret_cast<bool *>(dest)) = newValue.As<bool>();
            break;
        case Variant::FloatType:
            *(reinterpret_cast<float *>(dest)) = newValue.As<float>();
            break;
        case Variant::Vec2Type:
            *(reinterpret_cast<Vec2 *>(dest)) = newValue.As<Vec2>();
            break;
        case Variant::Vec3Type:
            *(reinterpret_cast<Vec3 *>(dest)) = newValue.As<Vec3>();
            break;
        case Variant::Vec4Type:
            *(reinterpret_cast<Vec4 *>(dest)) = newValue.As<Vec4>();
            break;
        case Variant::Color3Type:
            *(reinterpret_cast<Color3 *>(dest)) = newValue.As<Color3>();
            break;
        case Variant::Color4Type:
            *(reinterpret_cast<Color4 *>(dest)) = newValue.As<Color4>();
            break;
        case Variant::Mat2Type: 
            *(reinterpret_cast<Mat2 *>(dest)) = newValue.As<Mat2>();
            break;
        case Variant::Mat3Type:
            *(reinterpret_cast<Mat3 *>(dest)) = newValue.As<Mat3>();
            break;
        case Variant::Mat3x4Type:
            *(reinterpret_cast<Mat3x4 *>(dest)) = newValue.As<Mat3x4>();
            break;
        case Variant::Mat4Type:
            *(reinterpret_cast<Mat4 *>(dest)) = newValue.As<Mat4>();
            break;
        case Variant::AnglesType:
            *(reinterpret_cast<Angles *>(dest)) = newValue.As<Angles>();
            break;
        case Variant::QuatType: 
            *(reinterpret_cast<Quat *>(dest)) = newValue.As<Quat>();
            break;
        case Variant::PointType:
            *(reinterpret_cast<Point *>(dest)) = newValue.As<Point>();
            break;
        case Variant::RectType:
            *(reinterpret_cast<Rect *>(dest)) = newValue.As<Rect>();
            break;
        case Variant::GuidType:
            *(reinterpret_cast<Guid *>(dest)) = newValue.As<Guid>();
            break;
        case Variant::StrType:
            *(reinterpret_cast<Str *>(dest)) = newValue.As<Str>();
            break;
        case Variant::MinMaxCurveType:
            *(reinterpret_cast<MinMaxCurve *>(dest)) = newValue.As<MinMaxCurve>();
            break;
        }
    }

    EmitSignal(&Serializable::SIG_PropertyChanged, propertyInfo.name.c_str(), -1);
    
    return true;
}

bool Serializable::SetArrayProperty(const char *name, int elementIndex, const Variant &value, bool forceWrite) {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(name, propertyInfo)) {
        BE_WARNLOG(L"Serializable::SetArrayProperty: invalid property name '%hs'\n", name);
        return false;
    }

    return SetArrayProperty(propertyInfo, elementIndex, value, forceWrite);
}

bool Serializable::SetArrayProperty(int index, int elementIndex, const Variant &value, bool forceWrite) {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(index, propertyInfo)) {
        BE_WARNLOG(L"Serializable::SetArrayProperty: invalid property index %i\n", index);
        return false;
    }

    return SetArrayProperty(propertyInfo, elementIndex, value, forceWrite);
}

bool Serializable::SetArrayProperty(const PropertyInfo &propertyInfo, int elementIndex, const Variant &value, bool forceWrite) {
    if (!(propertyInfo.GetFlags() & PropertyInfo::ArrayFlag)) {
        BE_WARNLOG(L"Serializable::SetArrayProperty: property '%hs' is not array\n", propertyInfo.name.c_str());
        return false;
    }

    if (!forceWrite && (propertyInfo.GetFlags() & PropertyInfo::ReadOnlyFlag)) {
        BE_WARNLOG(L"Serializable::SetArrayProperty: property '%hs' is readonly\n", propertyInfo.name.c_str());
        return false;
    }

    Variant newValue;
    float minValue;
    float maxValue;

    bool ranged = propertyInfo.IsRanged();

    if (ranged) {
        minValue = propertyInfo.GetMinValue();
        maxValue = propertyInfo.GetMaxValue();
    }

    switch (propertyInfo.GetType()) {
    case Variant::IntType: {
        int i = value.As<int>();
        if (ranged) {
            Clamp(i, (int)minValue, (int)maxValue);
        }
        newValue = i;
        break; 
    }
    case Variant::Int64Type: {
        int64_t i = value.As<int64_t>();
        if (ranged) {
            Clamp(i, (int64_t)minValue, (int64_t)maxValue);
        }
        newValue = i;
        break;
    }
    case Variant::BoolType:
        newValue = value.As<bool>();
        break;
    case Variant::FloatType: {
        float f = value.As<float>();
        if (ranged) {
            Clamp(f, minValue, maxValue);
        }
        newValue = f;
        break;
    }
    case Variant::Vec2Type: {
        Vec2 vec2 = value.As<Vec2>();
        if (ranged) {
            Clamp(vec2.x, minValue, maxValue);
            Clamp(vec2.y, minValue, maxValue);
        }
        newValue = vec2;
        break; 
    }
    case Variant::Vec3Type: {
        Vec3 vec3 = value.As<Vec3>();
        if (ranged) {
            Clamp(vec3.x, minValue, maxValue);
            Clamp(vec3.y, minValue, maxValue);
            Clamp(vec3.z, minValue, maxValue);
        }
        newValue = vec3;
        break; 
    }
    case Variant::Vec4Type: {
        Vec4 vec4 = value.As<Vec4>();
        if (ranged) {
            Clamp(vec4.x, minValue, maxValue);
            Clamp(vec4.y, minValue, maxValue);
            Clamp(vec4.z, minValue, maxValue);
            Clamp(vec4.w, minValue, maxValue);
        }
        newValue = vec4;
        break; 
    }
    case Variant::Color3Type: {
        Color3 color3 = value.As<Color3>();
        if (ranged) {
            Clamp(color3.r, minValue, maxValue);
            Clamp(color3.g, minValue, maxValue);
            Clamp(color3.b, minValue, maxValue);
        }
        newValue = color3;
        break; 
    }
    case Variant::Color4Type: {
        Color4 color4 = value.As<Color4>();
        if (ranged) {
            Clamp(color4.r, minValue, maxValue);
            Clamp(color4.g, minValue, maxValue);
            Clamp(color4.b, minValue, maxValue);
            Clamp(color4.a, minValue, maxValue);
        }
        newValue = color4;
        break; 
    }
    case Variant::Mat2Type:
        newValue = value.As<Mat2>();
        break;
    case Variant::Mat3Type:
        newValue = value.As<Mat3>();
        break;
    case Variant::Mat3x4Type:
        newValue = value.As<Mat3x4>();
        break;
    case Variant::Mat4Type:
        newValue = value.As<Mat4>();
        break;
    case Variant::AnglesType: {
        Angles angles = value.As<Angles>();
        if (ranged) {
            Clamp(angles[0], minValue, maxValue);
            Clamp(angles[1], minValue, maxValue);
            Clamp(angles[2], minValue, maxValue);
        }
        newValue = angles;
        break;
    }
    case Variant::PointType: {
        Point pt = value.As<Point>();
        if (ranged) {
            Clamp(pt.x, (int)minValue, (int)maxValue);
            Clamp(pt.y, (int)minValue, (int)maxValue);
        }
        newValue = pt;
        break;
    }
    case Variant::RectType: {
        Rect rect = value.As<Rect>();
        if (ranged) {
            Clamp(rect.x, (int)minValue, (int)maxValue);
            Clamp(rect.y, (int)minValue, (int)maxValue);
            Clamp(rect.w, (int)minValue, (int)maxValue);
            Clamp(rect.h, (int)minValue, (int)maxValue);
        }
        newValue = rect;
        break;
    }
    case Variant::GuidType:
        newValue = value.As<Guid>();
        break;
    case Variant::StrType:
        newValue = value.As<Str>();
        break;
    case Variant::MinMaxCurveType:
        newValue = value.As<MinMaxCurve>();
        break;
    default:
        assert(0);
        break;
    }

    if (propertyInfo.accessor) {
        propertyInfo.accessor->Set(this, elementIndex, newValue);
    } else {
        void *dest = reinterpret_cast<byte *>(this) + propertyInfo.offset;

        switch (propertyInfo.GetType()) {
        case Variant::IntType:
            (*reinterpret_cast<Array<int> *>(dest))[elementIndex] = newValue.As<int>();
            break;
        case Variant::Int64Type:
            (*reinterpret_cast<Array<int64_t> *>(dest))[elementIndex] = newValue.As<int64_t>();
            break;
        case Variant::BoolType:
            (*reinterpret_cast<Array<bool> *>(dest))[elementIndex] = newValue.As<bool>();
            break;
        case Variant::FloatType:
            (*reinterpret_cast<Array<float> *>(dest))[elementIndex] = newValue.As<float>();
            break;
        case Variant::Vec2Type:
            (*reinterpret_cast<Array<Vec2> *>(dest))[elementIndex] = newValue.As<Vec2>();
            break;
        case Variant::Vec3Type:
            (*reinterpret_cast<Array<Vec3> *>(dest))[elementIndex] = newValue.As<Vec3>();
            break;
        case Variant::Vec4Type:
            (*reinterpret_cast<Array<Vec4> *>(dest))[elementIndex] = newValue.As<Vec4>();
            break;
        case Variant::Color3Type:
            (*reinterpret_cast<Array<Color3> *>(dest))[elementIndex] = newValue.As<Color3>();
            break;
        case Variant::Color4Type:
            (*reinterpret_cast<Array<Color4> *>(dest))[elementIndex] = newValue.As<Color4>();
            break;
        case Variant::Mat2Type: 
            (*reinterpret_cast<Array<Mat2> *>(dest))[elementIndex] = newValue.As<Mat2>();
            break;
        case Variant::Mat3Type:
            (*reinterpret_cast<Array<Mat3> *>(dest))[elementIndex] = newValue.As<Mat3>();
            break;
        case Variant::Mat3x4Type:
            (*reinterpret_cast<Array<Mat3x4> *>(dest))[elementIndex] = newValue.As<Mat3x4>();
            break;
        case Variant::Mat4Type:
            (*reinterpret_cast<Array<Mat4> *>(dest))[elementIndex] = newValue.As<Mat4>();
            break;
        case Variant::AnglesType:
            (*reinterpret_cast<Array<Angles> *>(dest))[elementIndex] = newValue.As<Angles>();
            break;
        case Variant::QuatType: 
            (*reinterpret_cast<Array<Quat> *>(dest))[elementIndex] = newValue.As<Quat>();
            break;
        case Variant::PointType:
            (*reinterpret_cast<Array<Point> *>(dest))[elementIndex] = newValue.As<Point>();
            break;
        case Variant::RectType:
            (*reinterpret_cast<Array<Rect> *>(dest))[elementIndex] = newValue.As<Rect>();
            break;
        case Variant::GuidType:
            (*reinterpret_cast<Array<Guid> *>(dest))[elementIndex] = newValue.As<Guid>();
            break;
        case Variant::StrType:
            (*reinterpret_cast<Array<Str> *>(dest))[elementIndex] = newValue.As<Str>();
            break;
        case Variant::MinMaxCurveType:
            (*reinterpret_cast<Array<MinMaxCurve> *>(dest))[elementIndex] = newValue.As<MinMaxCurve>();
            break;
        }
    }

    EmitSignal(&Serializable::SIG_PropertyChanged, propertyInfo.name.c_str(), elementIndex);

    return true;
}

int Serializable::GetPropertyArrayCount(const char *name) const {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(name, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetPropertyArrayCount: invalid property name '%hs'\n", name);
        return 0;
    }

    return GetPropertyArrayCount(propertyInfo);
}

int Serializable::GetPropertyArrayCount(int index) const {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(index, propertyInfo)) {
        BE_WARNLOG(L"Serializable::GetPropertyArrayCount: invalid property index %i\n", index);
        return 0;
    }

    return GetPropertyArrayCount(propertyInfo);
}

int Serializable::GetPropertyArrayCount(const PropertyInfo &propertyInfo) const {
    if (!(propertyInfo.GetFlags() & PropertyInfo::ArrayFlag)) {
        BE_WARNLOG(L"Serializable::GetPropertyArrayCount: property '%hs' is not array\n", propertyInfo.name.c_str());
        return 0;
    }

    if (propertyInfo.accessor) {
        return propertyInfo.accessor->GetCount(this);
    }

    const void *src = reinterpret_cast<const byte *>(this) + propertyInfo.offset;
    return reinterpret_cast<const Array<int> *>(src)->Count();
}

void Serializable::SetPropertyArrayCount(const char *name, int count) {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(name, propertyInfo)) {
        BE_WARNLOG(L"Serializable::SetPropertyArrayCount: invalid property name '%hs'\n", name);
        return;
    }

    SetPropertyArrayCount(propertyInfo, count);
}

void Serializable::SetPropertyArrayCount(int index, int count) {
    PropertyInfo propertyInfo;

    if (!GetPropertyInfo(index, propertyInfo)) {
        BE_WARNLOG(L"Serializable::SetPropertyArrayCount: invalid property index %i'\n", index);
        return;
    }

    SetPropertyArrayCount(propertyInfo, count);
}

void Serializable::SetPropertyArrayCount(const PropertyInfo &propertyInfo, int count) {
    if (!(propertyInfo.GetFlags() & PropertyInfo::ArrayFlag)) {
        BE_WARNLOG(L"Serializable::SetPropertyArrayCount: property '%hs' is not array\n", propertyInfo.name.c_str());
        return;
    }

    if (propertyInfo.GetFlags() & PropertyInfo::ReadOnlyFlag) {
        BE_WARNLOG(L"Serializable::SetPropertyArrayCount: property '%hs' is readonly\n", propertyInfo.name.c_str());
        return;
    }

    int oldCount = 0;

    if (propertyInfo.accessor) {
        oldCount = propertyInfo.accessor->GetCount(this);
        if (oldCount == count) {
            return;
        }

        propertyInfo.accessor->SetCount(this, count);
    } else {
        void *src = reinterpret_cast<byte *>(this) + propertyInfo.offset;

        switch (propertyInfo.GetType()) {
        case Variant::IntType:
            oldCount = reinterpret_cast<Array<int> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<int> *>(src)->SetCount(count);
            break;
        case Variant::Int64Type:
            oldCount = reinterpret_cast<Array<int64_t> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<int64_t> *>(src)->SetCount(count);
            break;
        case Variant::BoolType:
            oldCount = reinterpret_cast<Array<bool> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<bool> *>(src)->SetCount(count);
            break;
        case Variant::FloatType:
            oldCount = reinterpret_cast<Array<float> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<float> *>(src)->SetCount(count);
            break;
        case Variant::Vec2Type:
            oldCount = reinterpret_cast<Array<Vec2> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Vec2> *>(src)->SetCount(count);
            break;
        case Variant::Vec3Type:
            oldCount = reinterpret_cast<Array<Vec3> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Vec3> *>(src)->SetCount(count);
            break;
        case Variant::Vec4Type:
            oldCount = reinterpret_cast<Array<Vec4> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Vec4> *>(src)->SetCount(count);
            break;
        case Variant::Color3Type:
            oldCount = reinterpret_cast<Array<Color3> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Color3> *>(src)->SetCount(count);
            break;
        case Variant::Color4Type:
            oldCount = reinterpret_cast<Array<Color4> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Color4> *>(src)->SetCount(count);
            break;
        case Variant::AnglesType:
            oldCount = reinterpret_cast<Array<Angles> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Angles> *>(src)->SetCount(count);
            break;
        case Variant::Mat2Type:
            oldCount = reinterpret_cast<Array<Mat2> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Mat2> *>(src)->SetCount(count);
            break;
        case Variant::Mat3Type:
            oldCount = reinterpret_cast<Array<Mat3> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Mat3> *>(src)->SetCount(count);
            break;
        case Variant::Mat3x4Type:
            oldCount = reinterpret_cast<Array<Mat3x4> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Mat3x4> *>(src)->SetCount(count);
            break;
        case Variant::Mat4Type:
            oldCount = reinterpret_cast<Array<Mat4> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Mat4> *>(src)->SetCount(count);
            break;
        case Variant::PointType:
            oldCount = reinterpret_cast<Array<Point> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Point> *>(src)->SetCount(count);
            break;
        case Variant::RectType:
            oldCount = reinterpret_cast<Array<Rect> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Rect> *>(src)->SetCount(count);
            break;
        case Variant::GuidType:
            oldCount = reinterpret_cast<Array<Guid> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Guid> *>(src)->SetCount(count);
            break;
        case Variant::StrType:
            oldCount = reinterpret_cast<Array<Str> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<Str> *>(src)->SetCount(count);
            break;
        case Variant::MinMaxCurveType:
            oldCount = reinterpret_cast<Array<MinMaxCurve> *>(src)->Count();
            if (oldCount == count) {
                return;
            }
            reinterpret_cast<Array<MinMaxCurve> *>(src)->SetCount(count);
            break;
        }

        BlockSignals(true);

        if (count > oldCount) {
            for (int elementIndex = oldCount; elementIndex < count; elementIndex++) {
                SetArrayProperty(propertyInfo, elementIndex, propertyInfo.defaultValue, true);
            }
        }

        BlockSignals(false);
    }

    EmitSignal(&SIG_PropertyArrayCountChanged, propertyInfo.name.c_str());
}

BE_NAMESPACE_END
