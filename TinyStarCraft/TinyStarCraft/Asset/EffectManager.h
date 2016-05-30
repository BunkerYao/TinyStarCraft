#pragma once

#include "ResourceManager.h"

namespace TinyStarCraft 
{

class Effect;

/**
    Manages effect resources creation and destroy.
@remarks
    Effects created by effect manager could be retrieved by its resource name.
    Effects created by one effect manager shares one effect pool.
 */
class EffectManager : public ResourceManager
{
public:
    /** Constructor */
    explicit EffectManager(IDirect3DDevice9* D3DDevice);

    /** Destructor */
    ~EffectManager();

    /**
      	Initialize the effect manager.
    @return
        Returns true if succeeded, otherwise returns false.
     */
    bool initialize();

    /**
      	Create an effect from file.
    @return
        Returns the created effect. Returns nullptr if failed.
     */
    Effect* createEffectFromFile(const std::string& resourceName, const std::string& srcFilename);

    /**
      	Get the effect by resource name.
    @return
        Returns nullptr if the resource is not exist.
     */
    Effect* getEffect(const std::string& resourceName) const;

    /** Handles the device lost event. */
    void onDeviceLost();

    /** Handles the device reset event. */
    void onDeviceReset();

private:
    IDirect3DDevice9* mD3DDevice;
    ID3DXEffectPool* mEffectPool;
};

}

