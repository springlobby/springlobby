#ifndef MUTEXWRAPPER_H
#define MUTEXWRAPPER_H
#include <wx/thread.h>
#include <wx/log.h>

template<class T>
class MutexWrapper;

class AbstractMutexWrapper{
  public:
  virtual ~AbstractMutexWrapper(){}
  virtual void Lock()=0;
  virtual void UnLock()=0;
};

template<class T>
class ScopedLocker
{
  private:
  MutexWrapper<T> &mw;
  ScopedLocker(const ScopedLocker<T> &/*other*/){}/// prevent copying
  ScopedLocker&  operator= (const ScopedLocker& /*other*/){}/// and assignment
  public:
  explicit ScopedLocker(MutexWrapper<T> &mw_):mw(mw_){
    mw.Lock();
  }
  ~ScopedLocker(){
    mw.UnLock();
  }
  T &Get(){
    return mw.GetData();
  }
};
/*
class ScopedLocker
{
  private:
  AbstractMutexWrapper *mw;
  ScopedLocker(const ScopedLocker<T> &other){}/// prevent copying
  ScopedLocker&  operator= (const ScopedLocker& other){}/// and assignment
  public:
  explicit ScopedLocker(AbstractMutexWrapper &mw_):mw(*mw_){
    mw.Lock();
  }
  ~ScopedLocker(){
    mw.UnLock();
  }
};*/


template<class T>
class MutexWrapper: public AbstractMutexWrapper
{
  wxCriticalSection mutex;/// critical section is same as mutex except on windows it only works within one process (i.e. program). I'm gonna call it mutex.
  T data;
  bool locked;
  public:
    MutexWrapper():locked(false){
    }
    virtual ~MutexWrapper(){
    }
    virtual void Lock(){
      mutex.Enter();
      locked=true;
    }
    virtual void UnLock(){
      locked=false;
      mutex.Leave();
    }
    protected:
    T &GetData(){
      if(!locked) {
            wxLogError(_T("serious error in MutexWrapper usage : not locked, but Get() is called!"));
      }
      return data;
    }
    friend class ScopedLocker<T>;
};

#endif // MUTEXWRAPPER_H

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2011

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    SpringLobby is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

