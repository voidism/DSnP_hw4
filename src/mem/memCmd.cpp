/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO.Done
      // check option
      vector<string> options;
      if (!CmdExec::lexOptions(option, options))
         return CMD_EXEC_ERROR;

      if (options.empty())
         return CmdExec::errorOption(CMD_OPT_MISSING, "");
   
      //bool doArr = false;
      int numObjects = 0;
      int arraySize = 0;
      if (options.size() == 1)
      {
        if(!myStr2Int(options[0], numObjects)) 
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
        if(numObjects<=0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
        // allocate mem
        try{
        mtest.newObjs(numObjects);
        }
        catch(bad_alloc& cat){
                #ifdef MEM_DEBUG
                cerr << "catch you!!!\n";
                #endif
        }
      }
      else{
        for (unsigned int i = 0; i < options.size(); i++){
                if(myStrNCmp("-Array", options[i], 2) == 0){
                        if ((i+1)>=options.size()){
                                return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
                        }
                        else{
                                if(!myStr2Int(options[++i], arraySize)) 
                                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                                
                                if(arraySize<=0)
                                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                                
                        }
                }
                else if(numObjects==0){
                        if(!myStr2Int(options[i], numObjects)) 
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                        if(numObjects<=0)
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                }
                else{
                        return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
                }
        }
        if(numObjects==0)
                return CmdExec::errorOption(CMD_OPT_MISSING, "");
        //allocate mem
        try{
        mtest.newArrs((size_t)numObjects, (size_t)arraySize);
        }
        catch(bad_alloc& cat){
                //#ifdef MEM_DEBUG
                //cerr << "catch you!!!\n";
                //#endif
        }
      }

   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO.Done
   vector<string> options;
   if (!CmdExec::lexOptions(option, options))
      return CMD_EXEC_ERROR;

   if (options.empty())
      return CmdExec::errorOption(CMD_OPT_MISSING, "");

   bool doArr = false;
   string raname;
   int objId = -1;
   string objIdstr;
   int numRandId = -1;
   string numRandIdstr;
   for (size_t i = 0; i < options.size(); i++)
   {
           if (myStrNCmp("-Array", options[i], 2) == 0)
           {
                if (doArr!=false) return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                doArr = true;
           }
           else if (myStrNCmp("-Index", options[i], 2) == 0)
           {
                   if (objId!=-1 || numRandId!=-1)
                           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                   if ((i + 1) >= options.size())
                   {
                           return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
                   }
                   else
                   {
                           if (!myStr2Int(options[++i], objId))
                                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                           objIdstr = options[i];
                           if (objId < 0)
                                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                   }
           }
           else if (myStrNCmp("-Random", options[i], 2) == 0)
           {
                   raname = options[i];
                   if (objId != -1 || numRandId != -1)
                   {
                           #ifdef MEM_DEBUG
                           cout << "redifine!" ; 
                           #endif
                           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                   }
                   if ((i + 1) >= options.size())
                   {
                           return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
                   }
                   else
                   {
                           i++;
                           if (!myStr2Int(options[i], numRandId)){
                                   #ifdef MEM_DEBUG
                                   cout << "mystr2int fail!"; 
                                   #endif
                                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                           }
                           numRandIdstr = options[i];
                           if (numRandId <= 0){
                                   #ifdef MEM_DEBUG
                                   cout << "numRandId <= 0"; 
                                   #endif
                                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                           }
                   }
           }
           else
           {
                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
           }
     }
             if(doArr){
                     
                     //_arrList[objId] = 0;
                     if(objId!=-1){
                        if((size_t)objId>=mtest.getArrListSize()){
                                cerr << "Size of array list ("<< mtest.getArrListSize() <<") is <= " << objIdstr <<"!!"<< endl;
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL, objIdstr);
                        }
                        else mtest.deleteArr((size_t)objId);
                     }
                     else if (numRandId!=-1){
                                if (mtest.getArrListSize()==0){
                                cerr << "Size of array list is 0!!"<< endl;
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL, raname);
                        }
                             for (int i = 0; i < numRandId;i++){
                                     mtest.deleteArr((size_t)rnGen((int)mtest.getArrListSize()));
                             }
                     }
             }
             else{
                        
                        //_objList[objId] = 0;

                        if(objId!=-1){
                                if((size_t)objId>=mtest.getObjListSize()){
                                        //#ifdef MEM_DEBUG
                                        cerr << "Size of object list ("<< mtest.getObjListSize() <<") is <= " << objIdstr <<"!!"<< endl;
                                        //#endif // MEM_DEBUG
                                        return CmdExec::errorOption(CMD_OPT_ILLEGAL, objIdstr);
                                }
                                else mtest.deleteObj((size_t)objId);
                        }
                        else if (numRandId!=-1){
                                if (mtest.getObjListSize()==0){
                                   cerr << "Size of object list is 0!!"<< endl;
                                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, raname);
                              }
                                for (int i = 0; i < numRandId;i++){
                                        mtest.deleteObj((size_t)rnGen((int)mtest.getObjListSize()));
                                }
                        }
             }
     
   
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


