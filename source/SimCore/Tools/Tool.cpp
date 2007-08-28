/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Eddie Johnson
 */
#include <prefix/dvteprefix-src.h>
#include <SimCore/Tools/Tool.h>
#include <SimCore/Actors/StealthActor.h>
#include <sstream>

#ifdef None
#undef None
#endif

#include <CEGUI/CEGUI.h>

namespace SimCore
{
   namespace Tools
   {
      Tool::Tool(CEGUI::Window *window) : 
         mMainWindow(window),
         mIsEnabled(false),
         mStealthActor(NULL)
      {

      }

      Tool::~Tool()
      {

      }

      std::string Tool::PadNumber(int num)
      {
         std::ostringstream paddedNum;
         int absNum = abs(num);

         if(absNum == 0)
            paddedNum << "0000";
         else if(absNum < 10)
            paddedNum << "000" << absNum;
         else if(absNum < 100)
            paddedNum << "00" << absNum;
         else if(absNum < 1000)
            paddedNum << "0" << absNum;
         else
            paddedNum << absNum;

         if(num < 0)
            paddedNum.str("-" + paddedNum.str());

         return paddedNum.str();
      }

      float Tool::CalculateMils(const float distance, const float elevation)
      {
         if(distance == 0.0f)
         {
            LOG_ERROR("Cannot calculate mils with a distance of 0.");
            return 0.0f;
         }
         float radians = asinf(elevation / distance);
         return radians * (6400.0f / (2.0f * osg::PI));
      }
   }
}
