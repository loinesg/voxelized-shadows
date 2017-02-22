#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "Camera.hpp"
#include "Light.hpp"
#include "MeshInstance.hpp"
#include "Object.hpp"
#include "Texture.hpp"

class Scene
{
public:
    Scene();
    ~Scene();
    
    Camera* mainCamera() { return &cameras_[0]; }
    Light* mainLight() { return &lights_[0]; }
    vector<MeshInstance>* meshInstances() { return &meshInstances_; }
    
    bool loadFromFile(const char* fileName);
    
private:
    vector<Camera> cameras_;
    vector<Light> lights_;
    vector<MeshInstance> meshInstances_;
    map<string, Mesh*> meshes_;
    map<string, Texture*> textures_;
    
    bool loadObject(ifstream &file);
    void loadObjectTransform(ifstream &file, Object* object);
    bool loadCamera(ifstream &file);
    bool loadLight(ifstream &file);
    bool loadMeshInstance(ifstream &file);
    
    Mesh* getMesh(const string &name);
    Texture* getTexture(const string &name);
};
