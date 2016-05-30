#pragma once

#include "Resource.h"

namespace TinyStarCraft
{

class Effect : public Resource
{
public:
    /** Constructor */
    Effect(ResourceManager* owner, const std::string& name);

    /** Destructor */
    ~Effect();

    /**
        Create an effect from an ASCII or binary effect description.
    @param D3DDevice
        Pointer to the device that will create the effect.
    @param srcFilename
        File name of the effect source.
    @param flags
        Compilation flags.
    @param pool
        Which pool to use.
    @param [out] compilationErrors
        Compilation output messages.
     */
    bool createFromFile(IDirect3DDevice9* D3DDevice, const std::string& srcFilename, DWORD flags, ID3DXEffectPool* pool, 
        ID3DXBuffer** compilationErrors);

    /** Get the pointer to the D3DX effect. */
    ID3DXEffect* getPointer() const { return mEffect; }

private:
    ID3DXEffect* mEffect;
};

};
