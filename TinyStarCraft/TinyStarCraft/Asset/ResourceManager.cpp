#include "Precompiled.h"
#include "ResourceManager.h"

namespace TinyStarCraft
{

ResourceManager::~ResourceManager()
{
    destroyAllResources();
}

Resource* ResourceManager::getResource(const std::string& name) const
{
    auto& it = mResources.find(name);
    if (it == mResources.end())
        return nullptr;
    else
        return it->second;
}

bool ResourceManager::addResource(Resource* resource)
{
    if (mResources.count(resource->getName()) == 1) 
        return false;

    mResources.insert(std::make_pair(resource->getName(), resource));

    return true;
}

void ResourceManager::destroyResource(const std::string& name)
{
    auto& it = mResources.find(name);
    if (it != mResources.end()) {
        delete it->second;
        mResources.erase(it);
    }
}

void ResourceManager::destroyAllResources()
{
    if (!mResources.empty()) {
        for (auto& it : mResources) {
            delete it.second;
        }

        mResources.clear();
    }
}

}