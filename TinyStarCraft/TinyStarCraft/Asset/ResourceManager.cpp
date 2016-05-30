#include "Precompiled.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "Utilities/Assert.h"

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

void ResourceManager::addResource(Resource* resource)
{
    // Check if resource name is unique.
    TINYSC_ASSERT(mResources.count(resource->getName()) == 0, "Resource name is not unique.");
    mResources.insert(std::make_pair(resource->getName(), resource));
}

void ResourceManager::destroyResource(const std::string& name)
{
    auto& it = mResources.find(name);
    if (it != mResources.end()) 
    {
        // Delete the resource through its pointer and remove the entry
        // from resource map.
        delete it->second;
        mResources.erase(it);
    }
}

void ResourceManager::destroyResource(Resource* resource)
{
    TINYSC_ASSERT(this == resource->getOwner(), "This resource manager doesn't own this resource.");
    destroyResource(resource->getName());
}

void ResourceManager::destroyAllResources()
{
    if (!mResources.empty()) 
    {
        for (auto& it : mResources) 
        {
            delete it.second;
        }
        mResources.clear();
    }
}

std::vector<std::string> ResourceManager::getResourceNames() const
{
    // Gather all resource names to an array.
    std::vector<std::string> resourceNames;
    for (auto& it : mResources)
        resourceNames.push_back(it.first);

    return resourceNames;
}   

}