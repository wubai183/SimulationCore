/* -*-c++-*-
* Driver Demo
* Copyright (C) 2008, Alion Science and Technology Corporation
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author Curtiss Murphy
*/
#ifndef _HOVER_VEHICLE_PHYSICS_HELPER_
#define _HOVER_VEHICLE_PHYSICS_HELPER_

#include <DriverExport.h>
//#include <Vehicles/NxWheelDesc.h>
#include <NxAgeiaPhysicsHelper.h>
#include <NxAgeiaWorldComponent.h>
#include <osgSim/DOFTransform>

namespace DriverDemo
{
   //#define NUM_BRAKE_LEVELS_FOR_TABLE 10
   class DRIVER_DEMO_EXPORT HoverVehiclePhysicsHelper : public dtAgeiaPhysX::NxAgeiaPhysicsHelper
   {
      public:
         HoverVehiclePhysicsHelper(dtGame::GameActorProxy &proxy);

      protected:
         virtual ~HoverVehiclePhysicsHelper();

         /**
          * A convenience method to get the physics component instead of looking it up each time.
          * If the component doesn't exist, the app will likely crash.  
          */
         virtual dtAgeiaPhysX::NxAgeiaWorldComponent *GetPhysicsComponent();

      private:

         //////////////////////////////////
         // Runtime information
         dtCore::RefPtr<dtAgeiaPhysX::NxAgeiaWorldComponent> mWorldComponent;

         //////////////////////////////////
         // Properties
         float  mVehicleMaxForwardMPH;     /// The max mph you want your vehicle to have
         float  mVehicleMaxStrafeMPH;      /// The max reverse mph you can go 
         float  mVehicleBaseWeight;        /// How much is just the chassis / wheels without people in it
         float  mSphereRadius;             /// The radius of the hover sphere. Used to calculate lots of things...
         float  mGroundClearance;          /// How far above the ground we should be.

      public:

         float GetCurentMPH();

         /// Reset to starting position, good for when u crash ;p
         virtual void ResetToStarting();

         /// Turns it up and moves up
         void RepositionVehicle(float deltaTime);

         //void ApplySteerLeft(float deltaTime, float currentMPH);
         //void ApplySteerRight(float deltaTime, float currentMPH);
         //void ApplyReverse(float deltaTime, float currentMPH);
         //void ApplyAccel(float deltaTime, float currentMPH);

         /**
         * /brief Purpose : Create Wheels onto the main NxActor from the base class
         */
         //NxWheelShape* AddWheelToActor(NxActor* actor, const NxVec3& position, bool BackTires);

         /**
          * Call this method each frame from your actor after you have determined which direction to accelerate. 
          * Note, the method may not correct you if you say you accelerated forward and back, left and right 
          * at the same time - if it does check, left wins over right & forward wins over reverse.
          */
         void UpdateVehicle(float deltaTime, float currentMPH, 
            bool accelForward, bool accelReverse, bool accelLeft, bool accelRight);

         /**
          * /brief Purpose : To create the hover vehicle
          */ 
         bool CreateVehicle(const dtCore::Transform& transformForRot, osgSim::DOFTransform* bodyNode); 

         float GetClosestIntersectionUsingDirection(const osg::Vec3& location, 
            const osg::Vec3& direction , osg::Vec3& outPoint, int groupFlags);

         float ComputeEstimatedForceCorrection(const osg::Vec3 &location, 
            const osg::Vec3 &direction, float &distanceToHit);

         void DoJump(float deltaTime);

         //////////////////////////////////////////////////////////////////////////////////////
         //                                    Properties                                    //
         //////////////////////////////////////////////////////////////////////////////////////

         //////////////////////////////////////////////////////////////////
         // Build the property list for the actor
         virtual void BuildPropertyMap(std::vector<dtCore::RefPtr<dtDAL::ActorProperty> >& toFillIn);

         float GetVehicleMaxForwardMPH() {return mVehicleMaxForwardMPH;}            
         void SetVehicleMaxForwardMPH(float value) {mVehicleMaxForwardMPH = value;}            

         float GetVehicleMaxStrafeMPH() {return mVehicleMaxStrafeMPH;}     
         void SetVehicleMaxStrafeMPH(float value)  {mVehicleMaxStrafeMPH = value;}     

         float GetVehicleBaseWeight() {return mVehicleBaseWeight;}        
         void SetVehicleBaseWeight(float value)  {mVehicleBaseWeight = value;}        

         float GetSphereRadius() {return mSphereRadius;}        
         void SetSphereRadius(float value)  {mSphereRadius = value;}        

         float GetGroundClearance() {return mGroundClearance;}        
         void SetGroundClearance(float value)  {mGroundClearance = value;}        
   };
}

#endif
