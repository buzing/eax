#include "includes.h"

Bones g_bones{};;

__forceinline void m_AttachmentHelper(Entity* entity, CStudioHdr* hdr) {

    using AttachmentHelperFn = void(__thiscall*)(Entity*, CStudioHdr*);
    g_csgo.m_AttachmentHelper.as< AttachmentHelperFn  >()(entity, hdr);
}

bool Bones::SetupBones(Player* entity, BoneArray* pBoneMatrix, int nBoneCount, int boneMask, float time, int flags) {
    entity->m_bIsJiggleBonesEnabled() = false;

    float m_flRealtime = g_csgo.m_globals->m_realtime;
    float m_flCurtime = g_csgo.m_globals->m_curtime;
    float m_flFrametime = g_csgo.m_globals->m_frametime;
    float m_flAbsFrametime = g_csgo.m_globals->m_abs_frametime;
    int m_iFramecount = g_csgo.m_globals->m_frame;
    int m_iTickcount = g_csgo.m_globals->m_tick_count;

    // fixes for networked players
    float m_flTime = entity->m_flSimulationTime();
    g_csgo.m_globals->m_realtime = m_flTime;
    g_csgo.m_globals->m_curtime = m_flTime;
    g_csgo.m_globals->m_frametime = g_csgo.m_globals->m_interval;
    g_csgo.m_globals->m_abs_frametime = g_csgo.m_globals->m_interval;
    g_csgo.m_globals->m_frame = game::TIME_TO_TICKS(m_flTime);
    g_csgo.m_globals->m_tick_count = game::TIME_TO_TICKS(m_flTime);

    bool result = SetupBonesRebuild(entity, pBoneMatrix, nBoneCount, boneMask, time, flags);

    // restore globals
    g_csgo.m_globals->m_realtime = m_flRealtime;
    g_csgo.m_globals->m_curtime = m_flCurtime;
    g_csgo.m_globals->m_frametime = m_flFrametime;
    g_csgo.m_globals->m_abs_frametime = m_flAbsFrametime;
    g_csgo.m_globals->m_frame = m_iFramecount;
    g_csgo.m_globals->m_tick_count = m_iTickcount;

    return result;
}

bool Bones::SetupBonesRebuild(Player* entity, BoneArray* pBoneMatrix, int nBoneCount, int boneMask, float time, int flags) {

    if (entity->m_nSequence() == -1)
        return false;

    if (boneMask == -1)
        boneMask = entity->m_iPrevBoneMask();

    boneMask = boneMask | 0x80000;

    int nLOD = 0;
    int nMask = BONE_USED_BY_VERTEX_LOD0;

    for (; nLOD < 8; ++nLOD, nMask <<= 1) {
        if (boneMask & nMask)
            break;
    }

    for (; nLOD < 8; ++nLOD, nMask <<= 1) {
        boneMask |= nMask;
    }

    auto model_bone_counter = g_csgo.InvalidateBoneCache.add(0x000A).to< size_t >();

    CBoneAccessor backup_bone_accessor = entity->m_BoneAccessor();
    CBoneAccessor* bone_accessor = &entity->m_BoneAccessor();

    if (!bone_accessor)
        return false;

    if (entity->m_iMostRecentModelBoneCounter() != model_bone_counter || (flags & BoneSetupFlags::ForceInvalidateBoneCache)) {
        if (FLT_MAX >= entity->m_flLastBoneSetupTime() || time < entity->m_flLastBoneSetupTime()) {
            bone_accessor->m_ReadableBones = 0;
            bone_accessor->m_WritableBones = 0;
            entity->m_flLastBoneSetupTime() = (time);
        }

        entity->m_iPrevBoneMask() = entity->m_iAccumulatedBoneMask();
        entity->m_iAccumulatedBoneMask() = 0;

        auto hdr = entity->GetModelPtr();
        if (hdr) { // profiler stuff
            ((CStudioHdrEx*)hdr)->m_nPerfAnimatedBones = 0;
            ((CStudioHdrEx*)hdr)->m_nPerfUsedBones = 0;
            ((CStudioHdrEx*)hdr)->m_nPerfAnimationLayers = 0;
        }
    }

    entity->m_iAccumulatedBoneMask() |= boneMask;
    entity->m_iOcclusionFramecount() = 0;
    entity->m_iOcclusionFlags() = 0;
    entity->m_iMostRecentModelBoneCounter() = model_bone_counter;

    bool bReturnCustomMatrix = (flags & BoneSetupFlags::UseCustomOutput) && pBoneMatrix;

    CStudioHdr* hdr = entity->GetModelPtr();

    if (!hdr)
        return false;

    vec3_t origin = (flags & BoneSetupFlags::UseInterpolatedOrigin) ? entity->GetAbsOrigin() : entity->m_vecOrigin();
    ang_t angles = entity->GetAbsAngles();

    alignas(16) matrix3x4_t parentTransform;
    math::AngleMatrix(angles, origin, parentTransform);

    boneMask |= entity->m_iPrevBoneMask();

    if (bReturnCustomMatrix)
        bone_accessor->m_pBones = pBoneMatrix;

    int oldReadableBones = bone_accessor->m_ReadableBones;
    int oldWritableBones = bone_accessor->m_WritableBones;

    int newWritableBones = oldReadableBones | boneMask;

    bone_accessor->m_WritableBones = newWritableBones;
    bone_accessor->m_ReadableBones = newWritableBones;

    if (!(hdr->m_pStudioHdr->m_flags & 0x00000010)) {
        entity->AddEffect(EF_NOINTERP);
        entity->m_iEFlags() |= EFL_SETTING_UP_BONES;

        entity->m_pIK() = 0;
        entity->m_ClientEntEffects() |= 2;

        alignas(16) vec3_t pos[128];
        alignas(16) quaternion_t q[128];

        entity->StandardBlendingRules(hdr, pos, q, time, boneMask);

        uint8_t computed[0x100];
        std::memset(computed, 0, 0x100);

        entity->BuildTransformations(hdr, pos, q, parentTransform, boneMask, computed);

        entity->m_iEFlags() &= ~EFL_SETTING_UP_BONES;
        entity->RemoveEffect(EF_NOINTERP);
    }
    else
        parentTransform = bone_accessor->m_pBones[0];

    if (flags & BoneSetupFlags::AttachmentHelper)
        m_AttachmentHelper(entity, hdr);

    if (bReturnCustomMatrix) {
        *bone_accessor = backup_bone_accessor;
        return true;
    }

    return true;
}