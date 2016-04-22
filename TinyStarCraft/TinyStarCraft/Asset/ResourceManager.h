#pragma once

namespace TinyStarCraft
{

class Resource;

/**
 *	Base class for specific type resource manages like spritesheet manager
 *  and audio manager.
 *  
 *  @remarks
 *  Each resource in a resource manager has an unique name and one resource
 *  can be retrieved by that name. 
 */
class ResourceManager
{
public:
    /**
     *	Destructor
     *  Will destroy all resources.
     */
    virtual ~ResourceManager();

    /**
     *	Get a resource by name.
     *  
     *  @return
     *  Returns the resource with the given name. Returns null if the resource is not exist.
     */
    Resource* getResource(const std::string& name) const;

    /**
     *	Add a resource
     *
     *  @param name
     *  A unique resource name.
     *
     *  @return
     *  Returns true if the resource is added to the resource manager. Returns
     *  false if there is a name collision and the resource is not added.
     */
    bool addResource(Resource* resource);

    /**
     *	Destroy a resource in resource manager.
     *
     *  @remarks
     *  Do nothing if resource is not exist.
     */
    void destroyResource(const std::string& name);

    /**
     *	Destroy all resources.
     */
    void destroyAllResources();

    /**
     *	Get the number of added resources.
     */
    size_t getResourcesCount() const
    {
        return mResources.size();
    }

private:
    std::unordered_map<std::string, Resource*> mResources;
};


/**
 *	Base resource class with a unique name. 
 *  Managed by resource manager.
 */
class Resource
{
public:
    explicit Resource(const std::string& name)
        : mName(name)
    {}

    virtual ~Resource() {}

    const std::string& getName() const { return mName; }

private:
    std::string mName;
};

};
