/*
 * DVTE Stealth Viewer
 * Copyright (C) 2006, Alion Science and Technology.
 *
 */
#include <prefix/SimCorePrefix-src.h>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QSplashScreen>
#include <QtGui/QPixmap>

#include <StealthQt/MainWindow.h>

#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/macros.h>
#include <dtCore/globals.h>
#include <dtCore/system.h>
#include <dtAudio/audiomanager.h>
#include <dtDAL/project.h>
#include <dtDAL/librarymanager.h>
#include <dtGame/gameapplication.h>

#include <sstream>

dtCore::RefPtr<dtGame::GameApplication> InitGameApp();

int main(int argc, char *argv[])
{
   // Log to log file, and not to console
   dtUtil::Log::GetInstance().SetOutputStreamBit(dtUtil::Log::TO_FILE);

#ifdef DELTA_WIN32

   const std::string &executable = argv[0];
   dtUtil::FileInfo info = dtUtil::FileUtils::GetInstance().GetFileInfo(executable);
   if(info.fileType == dtUtil::FILE_NOT_FOUND)
   {
      LOG_ERROR(std::string("Unable to change to the directory of application \"")
         + executable + "\": file not found.");
   }
   else
   {
      std::string path = info.path;
      LOG_INFO("The path to the executable is: " + path);
      LOG_INFO("Changing to directory \"" + path + dtUtil::FileUtils::PATH_SEPARATOR + "..\".");
      try 
      {
         if(!info.path.empty())
            dtUtil::FileUtils::GetInstance().ChangeDirectory(path);

         dtUtil::FileUtils::GetInstance().ChangeDirectory("..");
      } 
      catch(const dtUtil::Exception &ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);
      }
   }

#endif

   dtAudio::AudioManager::Instantiate();
   dtAudio::AudioManager::GetInstance().Config(AudioConfigData(32));
   
   dtCore::SetDataFilePathList(".;" +
                               dtCore::GetDeltaDataPathList() + ";" +
                               "apps/StealthViewer") ;
   int result;
   QApplication app(argc, argv);
   
   try
   {
      dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_INFO);
      
      //Now that everything is initialized, show the main window.
      //Construct the application... 
      dtCore::RefPtr<dtGame::GameApplication> gameApp = InitGameApp();
      StealthQt::MainWindow mainWindow(*gameApp);
      mainWindow.show();
            
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
      dtCore::System::GetInstance().Config();
      result = app.exec();
      dtCore::System::GetInstance().Stop();
   }
   catch(const dtUtil::Exception &e)
   {
      e.LogException(dtUtil::Log::LOG_ERROR);
      std::ostringstream ss;
      ss << "Exception (" << e.TypeEnum() << "): " << e.What()
         << "\n\tLine: " << e.Line() << " File: " << e.File(); 
         
      QMessageBox::critical(NULL,"Exception",ss.str().c_str(),
                           QMessageBox::Ok,QMessageBox::NoButton);
         
      dtAudio::AudioManager::Destroy();
         
      return 1;
   }
   catch(const std::exception &e)
   {
      QString message("A std::exception has been thrown. The exception message is: ");
      message += e.what();
   
      QMessageBox::critical(NULL, "Exception", message, QMessageBox::Ok);

      dtAudio::AudioManager::Destroy();

      return 1;
   }
   catch(...)
   {
      QString message("An unknown exception has been thrown. Please try restarting the application."); 

      QMessageBox::critical(NULL, "Exception", message, QMessageBox::Ok);

      dtAudio::AudioManager::Destroy();

      return 1;
   }

    dtAudio::AudioManager::Destroy();

    return result;
}

const int appArgc = 15;
static char* appArgv[appArgc] = 
{
   "GameStart",
   "--UI", "1",
   "--statisticsInterval", "30",
   "--enableLogging", "1",
   "--enablePlayback", "1",
   "--projectPath", "DVTEProject",
   "--hasBinos", 
   "--hasCompass",  
   "--hasGPS",
   "--hasNightVis"
};


dtCore::RefPtr<dtGame::GameApplication> InitGameApp()
{
   std::string appLibName("StealthGMApp");
   dtCore::RefPtr<dtGame::GameApplication> gameApp = new dtGame::GameApplication(appArgc, appArgv);
   gameApp->SetGameLibraryName(appLibName);

   return gameApp;
}
