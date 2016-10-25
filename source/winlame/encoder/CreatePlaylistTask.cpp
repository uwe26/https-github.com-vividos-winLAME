//
// winLAME - a frontend for the LAME encoding engine
// Copyright (c) 2000-2016 Michael Fink
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file CreatePlaylistTask.cpp
/// \brief encoder task class

// includes
#include "StdAfx.h"
#include "CreatePlaylistTask.hpp"

CreatePlaylistTask::CreatePlaylistTask(const EncoderJobList& encoderjoblist)
{
}

TaskInfo CreatePlaylistTask::GetTaskInfo()
{
   TaskInfo info(Id(), TaskInfo::taskWritePlaylist);
   // TODO set name, desc, etc.
   info.Name(_T("Writing Playlist"));

   return info;
}

void CreatePlaylistTask::Run()
{
   // TODO implement
}

void CreatePlaylistTask::Stop()
{
}