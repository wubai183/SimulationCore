/*
* Copyright, 2009, Alion Science and Technology Corporation, all rights reserved.
*
* See the .h file for complete licensing information.
*
* Alion Science and Technology Corporation
* 5365 Robin Hood Road
* Norfolk, VA 23513
* (757) 857-5670, www.alionscience.com
*
* @author Curtiss Murphy
*/
#include <prefix/SimCorePrefix-src.h>
#include <ActorRegistry.h>
#include <SimCore/Actors/EntityActorRegistry.h>

#include <HoverVehicleActor.h>
#include <PlayerStatusActor.h>

#include <dtCore/shadermanager.h>
#include <dtCore/scene.h>

using dtCore::RefPtr;

namespace NetDemo
{

   RefPtr<dtDAL::ActorType> NetDemoActorRegistry::HOVER_VEHICLE_ACTOR_TYPE(
      new dtDAL::ActorType("HoverActor", "NetDemo", "A floaty drivable vehicle for Driver Demo", 
      SimCore::Actors::EntityActorRegistry::PLATFORM_ACTOR_TYPE.get()));

   RefPtr<dtDAL::ActorType> NetDemoActorRegistry::PLAYER_STATUS_ACTOR_TYPE(
      new dtDAL::ActorType("PlayerStatusActor", "NetDemo", "Status of each real player such as game state, team, and vehicle for NetDemo",
      SimCore::Actors::EntityActorRegistry::PLAYER_ACTOR_TYPE.get()));

   ///////////////////////////////////////////////////////////////////////////
   extern "C" NETDEMO_EXPORT dtDAL::ActorPluginRegistry* CreatePluginRegistry()
   {
       return new NetDemoActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////
   extern "C" NETDEMO_EXPORT void DestroyPluginRegistry(dtDAL::ActorPluginRegistry *registry)
   {
       delete registry;
   }

   ///////////////////////////////////////////////////////////////////////////
   NetDemoActorRegistry::NetDemoActorRegistry() :
      dtDAL::ActorPluginRegistry("This library holds actors from the NetDemo")
   {
   }

   ///////////////////////////////////////////////////////////////////////////
   void NetDemoActorRegistry::RegisterActorTypes()
   {
      mActorFactory->RegisterType<HoverVehicleActorProxy>(HOVER_VEHICLE_ACTOR_TYPE.get());
      mActorFactory->RegisterType<PlayerStatusActorProxy>(PLAYER_STATUS_ACTOR_TYPE.get());
   }
}