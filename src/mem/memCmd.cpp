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
        mtest.newObjs(numObjects);
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
                                
                                if(arraySize<0)
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
        mtest.newArrs(numObjects, arraySize);
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
   int objId = -1;
   string objIdstr;
   int numRandId = -1;
   string numRandIdstr;
   for (size_t i = 0; i < options.size(); i++)
   {
           if (myStrNCmp("-Array", options[i], 2) == 0)
           {
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
                   if (objId!=-1 || numRandId!=-1)
                           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                   if ((i + 1) >= options.size())
                   {
                           return CmdExec::errorOption(CMD_OPT_MISSING, options[i]);
                   }
                   else
                   {
                           if (!myStr2Int(options[++i], numRandId))
                                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                           numRandIdstr = options[i];
                           if (numRandId <= 0)
                                   return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
                   }
           }
           else
           {
                   return CmdExec::errorOption(CMD_OPT_EXTRA, options[i]);
           }
     }
             if(doArr){
                     if((size_t)objId>=mtest.getArrListSize()){
                             return CmdExec::errorOption(CMD_OPT_ILLEGAL, objIdstr);
                     }
                     //_arrList[objId] = 0;
                     if(objId!=-1)
                     mtest.deleteArr(objId);
                     else if (numRandId!=-1){
                             for (int i = 0; i < numRandId;i++){
                                     mtest.deleteArr(rnGen(mtest.getArrListSize()-1));
                             }
                     }
             }
             else{
                        if((size_t)objId>=mtest.getObjListSize()){
                                return CmdExec::errorOption(CMD_OPT_ILLEGAL, objIdstr);
                        }
                        //_objList[objId] = 0;
                        if(objId!=-1)
                        mtest.deleteObj(objId);
                        else if (numRandId!=-1){
                                for (int i = 0; i < numRandId;i++){
                                        mtest.deleteObj(rnGen(mtest.getObjListSize()-1));
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


