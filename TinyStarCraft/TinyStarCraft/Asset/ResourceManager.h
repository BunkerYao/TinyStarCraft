#pragma once

namespace TinyStarCraft
{

class Resource;

/**
    Manages resource creation and destroy.
@remarks
    Base class for specific type resource managers like texture manager, material manager, .etc.
    Each resource in a resource manager has an unique name and a resource can be retrieved by that name. 
 */
class ResourceManager
{
public:
    /**
     	Destructor
    @remarks
        Destroys all resources.
     */
    virtual ~ResourceManager();

    /**
        Retrieve a resource by its name.
    @return
        Returns the resource with the given name. Returns nullptr if the resource is not exist.
     */
    Resource* getResource(const std::string& name) const;

    /**
     	Add a resource
    @param resource
        A resource with a unique resource name.
    @return
        The given resource name must be unique.
     */
    void addResource(Resource* resource);

    /**
      	Destroys a resource with the given name.
    @remarks
        The resource with the given name must be exist.
     */
    void destroyResource(const std::string& name);

    /**
        Destroys a resource with the given name.
    @remarks
        The given resource must be owned by this resource manager.
    */
    void destroyResource(Resource* resource);

    /**	Destroy all resources. */
    void destroyAllResources();

    /** Get all resource names. */
    std::vector<std::string> getResourceNames() const;

    /**	Get the number of resources. */
    size_t getResourcesCount() const { return mResources.size(); }

private:
    std::unordered_map<std::string, Resource*> mResources;
};

};
