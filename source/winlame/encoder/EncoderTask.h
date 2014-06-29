/*
   winLAME - a frontend for the LAME encoding engine
   Copyright (c) 2000-2012 Michael Fink

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
/// \file EncoderTask.h
/// \brief encoder task class
/// \ingroup encoder
/// @{

// include guard
#pragma once

// needed includes
#include "Task.h"
#include "TrackInfo.h"
#include "SettingsManager.h"
#include "EncoderImpl.h"

struct EncoderTaskSettings
{
   /// title
   CString m_cszTitle;

   /// input filename
   CString m_cszInputFilename;

   /// output filename
   CString m_cszOutputFilename;

   /// track info to store in output
   TrackInfo m_trackInfo;

   /// the settings manager to use
   SettingsManager m_settingsManager;

   /// module manager to use
   ModuleManager* m_pModuleManager;

   /// output module id
   int m_iOutputModuleId;
};

class AlwaysSkipErrorHandler: public EncoderErrorHandler
{
public:
   /// error info
   struct ErrorInfo
   {
      ErrorInfo()
         :m_iErrorNumber(0)
      {
      }

      CString m_cszInputFilename;
      CString m_cszModuleName;
      int m_iErrorNumber;
      CString m_cszErrorMessage;
   };

   /// dtor
   virtual ~AlwaysSkipErrorHandler() throw() {}

   /// error handler function
   virtual ErrorAction handleError(LPCTSTR infilename,
      LPCTSTR modulename, int errnum, LPCTSTR errormsg, bool bSkipDisabled) override
   {
      ErrorInfo errorInfo;
      errorInfo.m_cszInputFilename = infilename;
      errorInfo.m_cszModuleName = modulename;
      errorInfo.m_iErrorNumber = errnum;
      errorInfo.m_cszErrorMessage = errormsg;

      m_vecAllErrors.push_back(errorInfo);

      return bSkipDisabled ? EncoderErrorHandler::Continue : EncoderErrorHandler::SkipFile;
   }

private:
   std::vector<ErrorInfo> m_vecAllErrors;
};

class EncoderTask:
   public Task,
   private EncoderImpl
{
public:
   /// ctor
   EncoderTask(const EncoderTaskSettings& settings);
   /// dtor
   virtual ~EncoderTask() throw() {}

   /// returns current task info; must return immediately
   virtual TaskInfo GetTaskInfo();

   /// runs task; may take longer
   virtual void Run();

   /// task should be aborted, e.g. when program is closed
   virtual void Stop();

private:
   EncoderTaskSettings m_settings;

   AlwaysSkipErrorHandler m_errorHandler;
};


/// @}