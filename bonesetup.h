#pragma once

class Bones {
public:
    bool m_running;

    enum BoneSetupFlags {
        None = 0,
        UseInterpolatedOrigin = ( 1 << 0 ),
        UseCustomOutput = ( 1 << 1 ),
        ForceInvalidateBoneCache = ( 1 << 2 ),
        AttachmentHelper = ( 1 << 3 ),
    };

public:
    bool SetupBones( Player* entity, BoneArray* pBoneMatrix, int nBoneCount, int boneMask, float time, int flags );
    bool SetupBonesRebuild( Player* entity, BoneArray* pBoneMatrix, int nBoneCount, int boneMask, float time, int flags );
};

extern Bones g_bones;