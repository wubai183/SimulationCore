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

#ifndef _DRIVER_GAME_APP_COMPONENT_
#define _DRIVER_GAME_APP_COMPONENT_

// project includes needed
#include <SimCore/Components/BaseGameAppComponent.h>
#include <SimCore/Actors/BasePhysicsVehicleActor.h>
#include <dtCore/refptr.h>
#include <osg/ArgumentParser>

namespace dtGame
{
   class GameManager;
   class TickMessage;
   class ActorPublishedMessage;
   class ActorDeletedMessage;
   class ActorUpdateMessage;
}

namespace SimCore
{
   namespace Actors
   {
      //class Platform;
      class StealthActor;
   }
}


namespace DriverDemo
{
   class DriverInputComponent;

   ///////////////////////////////////////////////////////
   //    The Component
   ///////////////////////////////////////////////////////
   class GameAppComponent : public SimCore::Components::BaseGameAppComponent
   {
      public:
         static const std::string DEFAULT_NAME;
         static const std::string APPLICATION_NAME;
         static const std::string SIM_ROLE_NAME_DRIVER;
         static const std::string SIM_ROLE_NAME_COMMANDER;
         static const std::string SIM_ROLE_NAME_GUNNER;
         static const std::string SIM_ROLE_NAME_SOLDIER;

         static const std::string CMD_LINE_STARTING_POSITION;
         static const std::string CMD_LINE_HAS_NIGHTVISION;
         static const std::string CMD_LINE_MACHINE_ID;
         static const std::string CMD_LINE_VEHICLE_CALLSIGN;
         static const std::string CMD_LINE_WEAPON;
         static const std::string CMD_LINE_START_HEADING;
         //static const std::string CMD_LINE_START_LAT;
         //static const std::string CMD_LINE_START_LON;
         //static const std::string CMD_LINE_START_MGRS;
         
         static const int  LOG_TIME_AMOUNT = 20; // for logging

         /*enum SimMode
         {
            SIM_MODE_NONE = -1,
            SIM_MODE_DRIVER,
            SIM_MODE_GUNNER,
            SIM_MODE_COMMANDER,
            SIM_MODE_SOLDIER,
            SIM_MODE_MAX
         };*/

         /*enum PlayerStartCordType
         {  
            PLAYER_START_NONE = -1,
               PLAYER_START_MGRS,
               PLAYER_START_XYZ,
               PLAYER_START_LAT_LON,
            PLAYER_START_MAX_CORD_TYPE
         };*/

         /// Constructor
         GameAppComponent(const std::string &name = DEFAULT_NAME);

         /**
         * Processes messages sent from the Game Manager
         * @param The message to process
         * @see dtGame::GameManager
         */
         virtual void ProcessMessage(const dtGame::Message &msg);
         
         /// initializes command line to parser and sets the options in the command
         /// line component.
         virtual void InitializeCommandLineOptionsAndRead(osg::ArgumentParser* parser);

         /// inits tools from command line param
         void InitializeTools();

         /// inits the player for the application
         void InitializePlayer();

         /// inits the vhiecle for app
         SimCore::Actors::BasePhysicsVehicleActor *CreateNewVehicle(const std::string &vehicleName);

         /// loads the terrain to the application, specific to CDMTS command line option.
         //void InitializeTerrain();

      protected:
         /// Destructor
         virtual ~GameAppComponent(void);

         /// updates the player start position from what the cmd line args were sent in
         void UpdatePlayerStartingPosition();

      private:
         dtCore::RefPtr<SimCore::Actors::StealthActor> mStealth;

         // players start lat through command line
         double mLatitudeStart;

         // players start lon through command line
         double mLongitudeStart;

         // players start mgrs through command line
         std::string mMGRSStart;

         // starting position for xyz in the world.
         osg::Vec3 mStartingPosition;

         // for cordinates
         //PlayerStartCordType mStartingCordType;

         // if this is set to true, we are waiting for a vehicle to join the network
         // with the name of the group sent in.
         bool mWaitForVehicle;

         // True if the user sent in and X and Y starting position
         bool mStartingCoordSet;
   };
} // namespace
#endif