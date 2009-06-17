/* -*-c++-*-
* Using 'The MIT License'
* Copyright (C) 2009, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* @author Bradley Anderegg
*/

#ifndef DELTA_BASEAIHELPER_H
#define DELTA_BASEAIHELPER_H

#include <DemoExport.h>
#include <AISteeringModel.h>
#include <AIPhysicsModel.h>
#include <AIUtility.h>

#include <dtAI/fsm.h>
#include <dtAI/npcstate.h>
#include <dtCore/transform.h>

namespace NetDemo
{

   class AIEvent;
   class AIState;


   class NETDEMO_EXPORT BaseAIHelper: public osg::Referenced
   {
   public:
     BaseAIHelper();

     virtual void Init();
     virtual void Spawn();
     virtual void Update(float dt);

     //there are two variations just for convenience 
     void PreSync(const dtCore::Transform& trans);
     void PreSync(const Kinematic& ko);

     //there are two variations just for convenience 
     void PostSync(dtCore::Transform& trans) const;
     void PostSync(Kinematic& ko) const;

     dtAI::FSM& GetStateMachine() { return mStateMachine; }
     const dtAI::FSM& GetStateMachine() const { return mStateMachine; }

     AISteeringModel* GetSteeringModel() { return mSteeringModel.get(); }
     const AISteeringModel* GetSteeringModel() const { return mSteeringModel.get(); }

     AIPhysicsModel* GetPhysicsModel() { return mPhysicsModel.get(); }
     const AIPhysicsModel* GetPhysicsModel() const { return mPhysicsModel.get(); }

   protected:
     BaseAIHelper(const BaseAIHelper&);  //not implemented by design
     BaseAIHelper& operator=(const BaseAIHelper&);  //not implemented by design
     ~BaseAIHelper();

     /**
     * A function to add transitions to the finite state machine using the AIState
     * which avoids redundant calls to FSM::GetState.
     */
     void AddTransition(const AIEvent* eventToTriggerTransition, const AIState* fromState, const AIState* toState);

     dtCore::RefPtr<dtAI::FSM::FactoryType> mFactory;
     dtAI::FSM mStateMachine;

     dtCore::RefPtr<AISteeringModel> mSteeringModel;
     dtCore::RefPtr<AIPhysicsModel> mPhysicsModel;
   };

} //namespace NetDemo

#endif //DELTA_BASEAIHELPER_H