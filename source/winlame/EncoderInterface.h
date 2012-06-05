/*
   winLAME - a frontend for the LAME encoding engine
   Copyright (c) 2000-2011 Michael Fink
   Copyright (c) 2004 DeXT

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
/// \file EncoderInterface.h
/// \brief EncoderInterface is an interface for the encoder class
/// \details
/// EncoderInterface is an interface for the encoder class and lets the
/// user of the class set the encoding options, and lets the user start,
/// pause and stop the encoding process, as well as querying some infos
///
/// ModuleManager is an interface to the class that manages the used
/// and available input and output modules; it is assumed that the
/// availability of the modules don't change durin a program run

/// \defgroup encoder Encoder Backend
/// contains all classes and functions that have to do with encoding

/// \ingroup encoder
/// @{

// include guard
#pragma once

// needed includes
#include <string>
#include "resource.h"
#include "SettingsManager.h"
#include "TrackInfo.h"


// forward references
class InputModule;


/// module manager interface

class ModuleManager
{
public:
   /// returns new encoder object; destroy with delete operator
   static ModuleManager* getNewModuleManager();

   // info functions

   /// returns currently available filter string for open file dialog
   virtual void getFilterString(CString& filterstring)=0;

   /// returns infos about audio file; returns false when not supported
   virtual bool getAudioFileInfo(LPCTSTR filename,
      int& length, int& bitrate, int& samplefreq, CString& errormsg)=0;

   // module functions

   /// returns the number of available input modules
   virtual int getInputModuleCount()=0;

   /// returns the name of the input module
   virtual CString getInputModuleName(int index)=0;

   /// returns the input module ID
   virtual int getInputModuleID(int index)=0;

   /// returns the input module filter string
   virtual CString getInputModuleFilterString(int index)=0;

   /// returns the input module filter string
   virtual InputModule* getInputModuleInstance(int index)=0;

   /// returns the number of available output modules
   virtual int getOutputModuleCount()=0;

   /// returns the name of the output module
   virtual CString getOutputModuleName(int index)=0;

   /// returns the output module ID
   virtual int getOutputModuleID(int index)=0;

   /// retrieves a module version string
   virtual void getModuleVersionString(CString& version, int module_id, int special=0)=0;

   /// dtor
   virtual ~ModuleManager(){}
protected:
   /// ctor
   ModuleManager(){}
};


/// error handler interface
class EncoderErrorHandler
{
public:
   /// dtor
   virtual ~EncoderErrorHandler() throw() {}

   /// action to perform when error is handled
   enum ErrorAction { Continue=0, SkipFile, StopEncode };

   /// error handler function
   virtual ErrorAction handleError(LPCTSTR infilename,
      LPCTSTR modulename, int errnum, LPCTSTR errormsg, bool bSkipDisabled=false)=0;
};


/// encoder job
class EncoderJob
{
public:
   /// ctor
   EncoderJob(const CString& cszInputFilename) throw()
      :m_cszInputFilename(cszInputFilename)
   {
   }

   /// returns input filename
   CString InputFilename() const throw() { return m_cszInputFilename; }

   /// returns track info; const version
   const TrackInfo& GetTrackInfo() const throw() { return m_trackInfo; }

   /// returns track info
   TrackInfo& GetTrackInfo() throw() { return m_trackInfo; }

private:
   CString m_cszInputFilename;   ///< input filename
   TrackInfo m_trackInfo;      ///< track info
};


/// encoder interface

class EncoderInterface
{
public:
   /// returns new encoder object; destroy with delete operator
   static EncoderInterface* getNewEncoder();

   // encoder access functions

   /// locks access to encoder object
   virtual void lockAccess()=0;

   /// unlocks access again
   virtual void unlockAccess()=0;

   // encoder functions

   /// sets input filename
   virtual void setInputFilename(LPCTSTR infile)=0;

   /// sets output path
   virtual void setOutputPath(LPCTSTR outpath)=0;

   /// sets the settings manager to use
   virtual void setSettingsManager(SettingsManager *settings_mgr)=0;

   /// sets the module manager to use
   virtual void setModuleManager(ModuleManager *mgr)=0;

   /// sets output module to use
   virtual void setOutputModule(int module_id)=0;

   /// sets output module per index
   virtual void setOutputModulePerIndex(int idx)=0;

   /// sets error handler to use if an error occurs
   virtual void setErrorHandler(EncoderErrorHandler *handler)=0;

   /// set if files can be overwritten
   virtual void setOverwriteFiles(bool overwrite)=0;

   /// set if source file should be deleted
   virtual void setDeleteAfterEncode(bool del) = 0;

   /// set warn about lossy transcoding
   virtual void setWarnLossy(bool overwrite)=0;

   /// sets output playlist filename and enables playlist creation
   virtual void setOutputPlaylistFilename(LPCTSTR plname)=0;

   /// starts encoding thread; returns immediately
   virtual void startEncode()=0;

   /// returns if the encoder thread is running
   virtual bool isRunning()=0;

   /// pauses encoding
   virtual void pauseEncoding()=0;

   /// returns if the encoder is currently paused
   virtual bool isPaused()=0;

   /// stops encoding
   virtual void stopEncode()=0;

   /// \brief returns if there were errors during encoding
   /// \details 0 means no error, a
   /// positive int indictates an error, a negative one is a fatal error
   /// and should stop the whole encoding process
   virtual int getError()=0;

   /// returns the percent done of the encoding process
   virtual float queryPercentDone()=0;

   /// returns encoding description string
   virtual CString getEncodingDescription()=0;

   /// dtor
   virtual ~EncoderInterface(){}
protected:
   /// ctor
   EncoderInterface(){}
};

/// @}