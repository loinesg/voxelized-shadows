#include "Scene.hpp"

#include <cstdio>

#include "Platform.hpp"

Scene::Scene()
    : cameras_(),
    lights_(),
    meshInstances_(),
    meshes_(),
    textures_()
{
    
}

Scene::~Scene()
{
    
}

bool Scene::loadFromFile(const string &fileName)
{
    string fullPath = SCENES_DIRECTORY + fileName;
    
    ifstream file(fullPath.c_str());
    while(file.is_open() && !file.fail() && !file.eof())
    {
        if(!loadObject(file))
        {
            printf("Failed to load object from scene %s \n", fileName.c_str());
            return false;
        }
    }
    
    if(file.fail())
    {
        printf("Failed to read scene file %s \n", fileName.c_str());
        return false;
    }
    
    printf("Loaded scene %s successfully \n", fileName.c_str());
    return true;
}

bool Scene::loadObject(ifstream &file)
{
    string objectType;
    file >> objectType;
    
    if(objectType == "camera")
    {
        return loadCamera(file);
    }
    else if(objectType == "light")
    {
        return loadLight(file);
    }
    else if(objectType == "mesh")
    {
        return loadMeshInstance(file);
    }
    
    printf("Object type %s not known. \n", objectType.c_str());
    return false;
}

void Scene::loadObjectTransform(ifstream &file, Object* object)
{
    // Transform params are stored sequentially
    Vector3 position, eulerAngles, scale;
    file >> position >> eulerAngles >> scale;
    
    // Convert the rotation to a quaternion
    Quaternion rotation = Quaternion::euler(eulerAngles);
    
    // Set the object transform params
    object->setPosition(position);
    object->setRotation(rotation);
    object->setScale(scale);
}

bool Scene::loadCamera(ifstream &file)
{
    // Fov, nearplane and farplane are stored sequentially
    float fov, nearPlane, farPlane;
    file >> fov >> nearPlane >> farPlane;
    
    // Create a new camera
    Camera camera;
    camera.setType(CameraType::Perspective);
    camera.setFov(fov);
    camera.setNearPlane(nearPlane);
    camera.setFarPlane(farPlane);
    loadObjectTransform(file, &camera);
    
    // Add to the cameras list
    cameras_.push_back(camera);
    
    return true;
}

bool Scene::loadLight(ifstream &file)
{
    // Color and ambient color are stored sequentially
    Vector3 color, ambient;
    file >> color >> ambient;
    
    // Create a light
    Light light(color, ambient);
    loadObjectTransform(file, &light);
    
    // Add to the lights list
    lights_.push_back(light);
    
    return true;
}

bool Scene::loadMeshInstance(ifstream &file)
{
    // Mesh name, shader features and textures are stored sequentially.
    string meshName, textureName, normalMapName;
    ShaderFeatureList shaderFeatures;
    file >> meshName >> shaderFeatures >> textureName >> normalMapName;
    
    // Get or create the mesh and texture
    Mesh* mesh = getMesh(meshName);
    Texture* texture = getTexture(textureName);
    Texture* normalMap = getTexture(normalMapName);
    
    // Check for errors
    if(mesh == NULL || texture == NULL)
    {
        printf("Error loading mesh %s or texture %s \n", meshName.c_str(), textureName.c_str());
        return false;
    }
    
    // Create the instance
    MeshInstance instance(mesh, shaderFeatures, texture, normalMap);
    loadObjectTransform(file, &instance);
    meshInstances_.push_back(instance);
    return true;
}

Mesh* Scene::getMesh(const string &name)
{
    // Use a cached mesh if possible.
    auto existing = meshes_.find(name);
    if(existing != meshes_.end())
    {
        return existing->second;
    }
    
    // Load the mesh.
    string fullPath = MESHES_DIRECTORY + name;
    Mesh* mesh = Mesh::load(fullPath.c_str());
    meshes_.insert(pair<string, Mesh*>(name, mesh));
    return mesh;
}

Texture* Scene::getTexture(const string &name)
{
    // Use a cached texture if possible.
    auto existing = textures_.find(name);
    if(existing != textures_.end())
    {
        return existing->second;
    }
    
    string fullPath = TEXTURES_DIRECTORY + name;
    Texture* texture = Texture::load(fullPath.c_str());
    textures_.insert(pair<string, Texture*>(name, texture));
    return texture;
}
