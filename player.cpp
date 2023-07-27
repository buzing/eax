#include "includes.h"

void Hooks::DoExtraBoneProcessing( int a2, int a3, int a4, int a5, int a6, int a7 ) {
	return;
}

void Hooks::CalcView( vec3_t& eye_origin, vec3_t& eye_angles, float& z_near, float& z_far, float& fov )
{
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	const auto use_new_anim_state = *( int* )( ( uintptr_t )player + 0x39E1 );

	*( bool* )( ( uintptr_t )player + 0x39E1 ) = false;

	g_hooks.m_CalcView( this, eye_origin, eye_angles, z_near, z_far, fov );

	*( bool* )( ( uintptr_t )player + 0x39E1 ) = use_new_anim_state;
}

void Hooks::BuildTransformations( int a2, int a3, int a4, int a5, int a6, int a7 ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	if( !player || !player->IsPlayer( ) || !player->alive( ) )
		return g_hooks.m_BuildTransformations( this, a2, a3, a4, a5, a6, a7 );;

	const model_t* model = player->GetModel( );

	if( !model )
		return g_hooks.m_BuildTransformations( this, a2, a3, a4, a5, a6, a7 );

	studiohdr_t* hdr = g_csgo.m_model_info->GetStudioModel( model );

	if( !hdr )
		return g_hooks.m_BuildTransformations( this, a2, a3, a4, a5, a6, a7 );

	mstudiobone_t* bone{ nullptr };

	for( int i{ }; i < hdr->m_num_bones; ++i ) {
		// get bone.
		bone = hdr->GetBone( i );

		if( !bone )
			continue;

		bone->m_flags &= ~0x04;
	}

	// call og.
	g_hooks.m_BuildTransformations( this, a2, a3, a4, a5, a6, a7 );
}

void Hooks::UpdateClientSideAnimation( ) {
	if( !g_csgo.m_engine->IsInGame( ) || !g_csgo.m_engine->IsConnected( ) )
		return g_hooks.m_UpdateClientSideAnimation( this );

	if( !g_cl.m_update_anims )
		return;

	g_hooks.m_UpdateClientSideAnimation( this );
}

Weapon* Hooks::GetActiveWeapon( ) {
	Stack stack;

	static Address ret_1 = pattern::find( g_csgo.m_client_dll, XOR( "85 C0 74 1D 8B 88 ? ? ? ? 85 C9" ) );

	// note - dex; stop call to CIronSightController::RenderScopeEffect inside CViewRender::RenderView.
	if( g_menu.main.visuals.removals.get( 5 ) ) {
		if( stack.ReturnAddress( ) == ret_1 )
			return nullptr;
	}

	return g_hooks.m_GetActiveWeapon( this );
}

void Hooks::StandardBlendingRules( CStudioHdr* hdr, int a3, int a4, int a5, int mask ) {
	// cast thisptr to player ptr.
	Player* player = ( Player* )this;

	if( !player 
        || !player->IsPlayer( )
        || !player->alive( ) )
        return g_hooks.m_StandardBlendingRules( this, hdr, a3, a4, a5, mask );


    if( player->index( ) == g_cl.m_local->index( ) )
        mask |= BONE_USED_BY_BONE_MERGE;
	else {
		// fix arms.
		mask = BONE_USED_BY_SERVER;
	}

    player->AddEffect( EF_NOINTERP );
	g_hooks.m_StandardBlendingRules( player, hdr, a3, a4, a5, mask );
    player->RemoveEffect( EF_NOINTERP );
}	

void CustomEntityListener::OnEntityCreated( Entity* ent ) {
	if( ent ) {
		// player created.
		if( ent->IsPlayer( ) ) {
			Player* player = ent->as< Player* >( );

			// access out player data stucture and reset player data.
			AimPlayer* data = &g_aimbot.m_players[player->index( ) - 1];
			if( data )	
				data->reset( );

			// get ptr to vmt instance and reset tables.
			VMT* vmt = &g_hooks.m_player[player->index( ) - 1];
			if( vmt ) {
				// init vtable with new ptr.
				vmt->reset( );
				vmt->init( player );

				// hook this on every player.
				g_hooks.m_DoExtraBoneProcessing = vmt->add< Hooks::DoExtraBoneProcessing_t >( Player::DOEXTRABONEPROCESSING, util::force_cast( &Hooks::DoExtraBoneProcessing ) );
				g_hooks.m_StandardBlendingRules = vmt->add< Hooks::StandardBlendingRules_t >( Player::STANDARDBLENDINGRULES, util::force_cast( &Hooks::StandardBlendingRules ) );

				// local gets special treatment.
				if( player->index( ) == g_csgo.m_engine->GetLocalPlayer( ) ) {
					g_hooks.m_UpdateClientSideAnimation = vmt->add< Hooks::UpdateClientSideAnimation_t >( Player::UPDATECLIENTSIDEANIMATION, util::force_cast( &Hooks::UpdateClientSideAnimation ) );
					g_hooks.m_CalcView = vmt->add< Hooks::CalcView_t >( 270, util::force_cast( &Hooks::CalcView ) );
					g_hooks.m_GetActiveWeapon = vmt->add< Hooks::GetActiveWeapon_t >( Player::GETACTIVEWEAPON, util::force_cast( &Hooks::GetActiveWeapon ) );
					g_hooks.m_BuildTransformations = vmt->add< Hooks::BuildTransformations_t >( Player::BUILDTRANSFORMATIONS, util::force_cast( &Hooks::BuildTransformations ) );
				}
			}
		}
	}
}

void CustomEntityListener::OnEntityDeleted( Entity* ent ) {
	// note; IsPlayer doesn't work here because the ent class is CBaseEntity.
	if( ent && ent->index( ) >= 1 && ent->index( ) <= 64 ) {
		Player* player = ent->as< Player* >( );

		// access out player data stucture and reset player data.
		AimPlayer* data = &g_aimbot.m_players[player->index( ) - 1];
		if( data )
			data->reset( );

		// get ptr to vmt instance and reset tables.
		VMT* vmt = &g_hooks.m_player[player->index( ) - 1];
		if( vmt )
			vmt->reset( );
	}
}