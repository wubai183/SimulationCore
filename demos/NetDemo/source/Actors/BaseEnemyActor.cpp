/*
* Copyright, 2008, Alion Science and Technology Corporation, all rights reserved.
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
#include <prefix/SimCorePrefix.h>

//#ifdef AGEIA_PHYSICS
#include <Actors/BaseEnemyActor.h>
#include <dtPhysics/physicshelper.h>
#include <dtPhysics/physicsobject.h>
//#include <NxAgeiaWorldComponent.h>
//#include <NxAgeiaRaycastReport.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtABC/application.h>
#include <dtAudio/audiomanager.h>
#include <dtAudio/sound.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>
#include <dtCore/keyboard.h>
#include <dtGame/basemessages.h>
#include <SimCore/Messages.h>
#include <SimCore/MessageType.h>
#include <SimCore/Actors/BaseEntity.h>
#include <SimCore/Actors/DRPublishingActComp.h>
#include <SimCore/Components/RenderingSupportComponent.h>
//#include <SimCore/Components/MunitionsComponent.h>
#include <SimCore/CollisionGroupEnum.h>

#include <dtGame/gameactor.h>
#include <dtGame/messagefactory.h>
#include <dtGame/gamemanager.h>

//#include <dtUtil/nodeprintout.h>

#include <AIState.h>
#include <AIEvent.h>
#include <NetDemoUtils.h>
#include <ActorRegistry.h>
#include <Actors/FortActor.h>
#include <Actors/TowerActor.h>


namespace NetDemo
{

   ///////////////////////////////////////////////////////////////////////////////////
   BaseEnemyActor::BaseEnemyActor(SimCore::Actors::BasePhysicsVehicleActorProxy &proxy)
      : SimCore::Actors::BasePhysicsVehicleActor(proxy)
      , mSendScoreMessage(false)
      , mPointValue(1)
      , mTimeSinceBorn(0.0f)
      , mTimeToExistAfterDead(0.5f)
      , mTimeSinceKilled(0.0f)
   {
      /////////////////////////////////////////////////////////////////
      // Set a bunch of initial conditions - Note, some of these may be
      // changed if the actor is loaded from a map or when received as a remote actor
      /////////////////////////////////////////////////////////////////

      // create my unique physics helper.  almost all of the physics is on the helper.
      // The actor just manages properties and key presses mostly.
      dtPhysics::PhysicsHelper *helper = new dtPhysics::PhysicsHelper(proxy);
      //helper->SetBaseInterfaceClass(this);
      SetPhysicsHelper(helper);

      dtCore::RefPtr<dtPhysics::PhysicsObject> physicsObject = new dtPhysics::PhysicsObject("VehicleBody");
      helper->AddPhysicsObject(*physicsObject);
      helper->SetMass(500.0f);
      physicsObject->SetPrimitiveType(dtPhysics::PrimitiveType::CONVEX_HULL);
      physicsObject->SetMass(100.0f);
      //physicsObject->SetExtents(osg::Vec3(1.5f, 1.5f, 1.5f));
      physicsObject->SetMechanicsType(dtPhysics::MechanicsType::STATIC);

      // Preset some Entity values
      SetForceAffiliation(SimCore::Actors::BaseEntityActorProxy::ForceEnum::OPPOSING); // Shows up 'red' in Stealth Viewer
      SetEntityType("Enemy");
      SetMunitionDamageTableName("StandardDamageType");

      // Make a semi-unique name - mostly only useful for Stealth Viewer
      static int uniqueCounter = 0;
      uniqueCounter ++;
      SetName("Enemy " + dtUtil::ToString(uniqueCounter));
   }

   ///////////////////////////////////////////////////////////////////////////////////
   BaseEnemyActor::~BaseEnemyActor(void)
   {
   }


   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::BuildActorComponents()
   {
      BaseClass::BuildActorComponents();

      SimCore::Actors::DRPublishingActComp* drPublishingActComp = GetDRPublishingActComp();
      if (drPublishingActComp == NULL)
      {
         LOG_ERROR("CRITICAL ERROR - No DR Publishing Actor Component.");
         return;
      }
      drPublishingActComp->SetMaxUpdateSendRate(4.0f);
      //drPublishingActComp->SetPublishLinearVelocity(true);
      //drPublishingActComp->SetPublishAngularVelocity(false);
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::OnEnteredWorld()
   {
      dtCore::Transform ourTransform;
      GetTransform(ourTransform);

      // TODO - Maybe use sphere instead???

      dtPhysics::PhysicsObject *physObj = GetPhysicsHelper()->GetMainPhysicsObject();
      physObj->CreateFromProperties(GetNonDamagedFileNode());
      physObj->SetTransform(ourTransform);
      physObj->SetActive(true);

      BaseClass::OnEnteredWorld();

   }


   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::DoExplosion(float)
   {
      //const osg::Vec3& finalVelocity, const osg::Vec3& location, const dtCore::Transformable* target )
      //printf("Sending DETONATION\r\n");

      dtGame::GameManager* gm = GetGameActorProxy().GetGameManager();
      dtCore::Transform ourTransform;
      GetTransform(ourTransform);
      osg::Vec3 trans = ourTransform.GetTranslation();

      // Prepare a detonation message
      dtCore::RefPtr<SimCore::DetonationMessage> msg;
      gm->GetMessageFactory().CreateMessage( SimCore::MessageType::DETONATION, msg );

      // Required Parameters:
      msg->SetEventIdentifier( 1 );
      msg->SetDetonationLocation(trans);
      // --- DetonationResultCode 1 == Entity Impact, 3 == Ground Impact, 5 == Detonation
      msg->SetDetonationResultCode( 5 ); // TO BE DYNAMIC
      //msg->SetMunitionType("Generic Explosive");
      msg->SetMunitionType("Grenade");  // Other example options are "Bullet" and "High Explosive"
      msg->SetFuseType(0);
      msg->SetWarheadType(0);
      msg->SetQuantityFired(dtUtil::RandRange(1, 3));
      msg->SetSendingActorId(GetGameActorProxy().GetId());
      //msg->SetFinalVelocityVector( finalVelocity );
      msg->SetRateOfFire(5);

      gm->SendMessage( *msg );
      gm->SendNetworkMessage( *msg );

      GetGameActorProxy().GetGameManager()->DeleteActor(GetGameActorProxy());
   }

   ///////////////////////////////////////////////////////////////////////////////////
   float BaseEnemyActor::ValidateIncomingDamage(float incomingDamage, const SimCore::DetonationMessage& message, const SimCore::Actors::MunitionTypeActor& munition)
   {
      dtGame::GameActorProxy* gap = GetGameActorProxy().GetGameManager()->FindGameActorById(message.GetSendingActorId());

      //if is enemy true it will multiply incoming damage by a 0
      return 2.0f * incomingDamage * float(!IsEnemyActor(gap));
   }


   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::RespondToHit(const SimCore::DetonationMessage& message,
      const SimCore::Actors::MunitionTypeActor& munition, const osg::Vec3& force, 
      const osg::Vec3& location)
   {
      // This is called after we got hit by something.

      dtGame::GameActorProxy* gap = GetGameActorProxy().GetGameManager()->FindGameActorById(message.GetSendingActorId());

      if(!IsEnemyActor(gap)) 
      {
         // the base class applies an impulse
         BaseClass::RespondToHit(message, munition, force, location);

         // Do nothing for now ... but you could become more aggressive or show a particle effect or 
         // start a count down explosion timer or something.
      }

      // Was a score action triggered?
      if(mSendScoreMessage)
      {
         mSendScoreMessage = false;
         MessageUtils::SendScoreMessage(*this, message.GetSendingActorId(), GetPointValue());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::SetDamageState(SimCore::Actors::BaseEntityActorProxy::DamageStateEnum &damageState)
   {
      // This method is called when property is set or when the DamageHelper thinks we took
      // some damage. We can change our AI here. The fire and such are turned on by the Damage Helper
      // Note, we could do this same logic in RespondToHit() but that would make it difficult to do our
      // little hack in InputComponent to kill enemies by pressing Delete.

      if (damageState != GetDamageState())
      {
         BaseClass::SetDamageState(damageState);

         // Mark the AI as 'dead' so we stop 'steering'
         if(IsMobilityDisabled())
         {
            mAIHelper->GetStateMachine().MakeCurrent(&AIStateType::AI_STATE_DIE);

            // WORKAROUND:
            // Set the flag to send a score message when RespondToHit is called.
            mSendScoreMessage = true;
         }

         // Randomly decide how long to last before exploding. 
         mTimeToExistAfterDead *= dtUtil::RandFloat(0.3f, 0.75f);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////
   float BaseEnemyActor::GetTimeToExistAfterDead()
   {
      return mTimeToExistAfterDead;
   }
   
   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::SetTimeToExistAfterDead(float newTime)
   {
      mTimeToExistAfterDead = newTime;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::SetPointValue(int points)
   {
      mPointValue = points;
   }
   
   ///////////////////////////////////////////////////////////////////////////////////
   int BaseEnemyActor::GetPointValue() const
   {
      return mPointValue;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::UpdateVehicleTorquesAndAngles(float deltaTime)
   {
      mTimeSinceBorn += deltaTime;

      if(!IsMobilityDisabled())
      {
         // Steering, etc is done by the AI.          
      }
      else
      {
         // Self-terminate after we're been dead for a bit - allows time for animations, fire, falling from physics, etc...
         mTimeSinceKilled += deltaTime;
         if (mTimeSinceKilled > mTimeToExistAfterDead)
         {
            // Tell the AI to 'explode'.
            mAIHelper->GetStateMachine().HandleEvent(&AIEvent::AI_EVENT_TOOK_DAMAGE);

            //GetGameActorProxy().GetGameManager()->DeleteActor(GetGameActorProxy());
         }
      }

   }

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::UpdateSoundEffects(float deltaTime)
   {
      BaseClass::UpdateSoundEffects(deltaTime);
   }

  ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::UpdateRotationDOFS(float deltaTime, bool insideVehicle)
   {
   }

   //////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::OnTickLocal( const dtGame::TickMessage& tickMessage )
   {
      BaseClass::OnTickLocal( tickMessage );

      // Set the angular velocity manually because we are abusing the physics engine in all enemies.
      GetDRPublishingActComp()->SetCurrentAngularVelocity(osg::Vec3(0.0f, 0.0f, 0.0f));
   }

   //////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::OnTickRemote( const dtGame::TickMessage& tickMessage )
   {
      BaseClass::OnTickRemote( tickMessage );
   }

   ///////////////////////////////////////////////////////////////////////////////////
   EnemyAIHelper* BaseEnemyActor::GetAIHelper()
   {
      return mAIHelper.get();
   }

   ///////////////////////////////////////////////////////////////////////////////////
   const EnemyAIHelper* BaseEnemyActor::GetAIHelper() const
   {
      return mAIHelper.get();
   }

   //////////////////////////////////////////////////////////////////////
   void BaseEnemyActor::InitAI( const EnemyDescriptionActor* desc )
   {
      mAIHelper->Init(desc);
   }

   //////////////////////////////////////////////////////////////////////
   bool BaseEnemyActor::IsEnemyActor( dtGame::GameActorProxy* gap ) const
   {
      if(gap != NULL)
      {
         const dtDAL::ActorType& atype = gap->GetActorType();
         if( atype == *NetDemoActorRegistry::ENEMY_HELIX_ACTOR_TYPE || 
            atype == * NetDemoActorRegistry::ENEMY_MINE_ACTOR_TYPE ||
            atype == * NetDemoActorRegistry::ENEMY_MOTHERSHIP_ACTOR_TYPE )
         {
            return true;
         }
      }
      return false;
   }

   FortActor* BaseEnemyActor::GetClosestFort()
   {
      //temporarily lets just look for a fort to destroy
      std::vector<dtDAL::ActorProxy*> actors;
      GetGameActorProxy().GetGameManager()->FindActorsByType(*NetDemoActorRegistry::FORT_ACTOR_TYPE, actors);

      osg::Vec3 pos = mAIHelper->mCurrentState.GetPos();

      FortActor* result = NULL;
      float dist = 1000000.0f;

      for (unsigned i = 0; i < actors.size(); ++i)
      {
         FortActor* f = static_cast<FortActor*>(actors[i]->GetActor());

         dtCore::Transform trans;
         f->GetTransform(trans);
         osg::Vec3 fortPos = trans.GetTranslation();
         float newDist = (fortPos - pos).length();
         if(newDist < dist && f->GetDamageState() != SimCore::Actors::BaseEntityActorProxy::DamageStateEnum::DESTROYED)
         {
            dist = newDist;
            result = f;
         }
      }

      return result;
   }

   dtCore::Transformable* BaseEnemyActor::GetClosestTower()
   {
      //temporarily lets just look for a fort to destroy
      std::vector<dtDAL::ActorProxy*> actors;
      GetGameActorProxy().GetGameManager()->FindActorsByType(*NetDemoActorRegistry::TOWER_ACTOR_TYPE, actors);

      osg::Vec3 pos = mAIHelper->mCurrentState.GetPos();

      dtCore::Transformable* result = NULL;
      float dist = 1000000.0f;

      for (unsigned i = 0; i < actors.size(); ++i)
      {
         TowerActor* f = static_cast<TowerActor*>(actors[i]->GetActor());

         dtCore::Transform trans;
         f->GetTransform(trans);
         osg::Vec3 fortPos = trans.GetTranslation();
         float newDist = (fortPos - pos).length();
         if(newDist < dist && f->GetDamageState() != SimCore::Actors::BaseEntityActorProxy::DamageStateEnum::DESTROYED)
         {
            dist = newDist;
            result = f;
         }
      }

      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   // PROXY
   ///////////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString BaseEnemyActorProxy::PROPERTY_POINT_VALUE("Point Value");

   ///////////////////////////////////////////////////////////////////////////////////
   BaseEnemyActorProxy::BaseEnemyActorProxy()
   {
      SetClassName("BaseEnemyActor");
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActorProxy::BuildPropertyMap()
   {
      BaseClass::BuildPropertyMap();

      BaseEnemyActor* actor = NULL;
      GetActor(actor);

      using namespace dtDAL;
      const std::string GROUP = "Enemy Props";

      // INTEGER PROPERTIES
      AddProperty(new IntActorProperty(
         PROPERTY_POINT_VALUE,
         PROPERTY_POINT_VALUE,
         IntActorProperty::SetFuncType(actor, &BaseEnemyActor::SetPointValue),
         IntActorProperty::GetFuncType(actor, &BaseEnemyActor::GetPointValue),
         "Number of point to be awarded to a player or entity who destroys this actor.",
         GROUP));
   }

   ///////////////////////////////////////////////////////////////////////////////////
   BaseEnemyActorProxy::~BaseEnemyActorProxy(){}

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActorProxy::CreateActor()
   {
      SetActor(*new BaseEnemyActor(*this));
   }

   ///////////////////////////////////////////////////////////////////////////////////
   void BaseEnemyActorProxy::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();
   }

} // namespace
//#endif
