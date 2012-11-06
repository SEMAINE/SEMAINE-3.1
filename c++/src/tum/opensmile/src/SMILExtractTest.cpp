/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/


#include <smileCommon.hpp>

#include <configManager.hpp>
#include <commandlineParser.hpp>
#include <componentManager.hpp>

#define MODULE "SMILExtract"

//using namespace std;

/************** Ctrl+C signal handler **/
#include  <signal.h>

cComponentManager *cmanGlob = NULL;

void INThandler(int);

void INThandler(int sig)
{
//  char  c;
  signal(sig, SIG_IGN);
  if (cmanGlob != NULL) cmanGlob->requestAbort();
  signal(SIGINT, INThandler);
}
/*******************************************/

int main(int argc, char *argv[])
{try {

  // test the smile logger
  LOGGER.setLogFile("smile.log");
  LOGGER.setLogLevel(0);
  LOGGER.enableConsoleOutput();
  SMILE_MSG(0,"openSMILE starting!");
  
  // commandline parser:
  cCommandlineParser cmdline(argc,argv);
  //cmdline.addStr( "configfile", 'C', "Path to openSMILE config file", "smile.conf" );
  cmdline.addInt( "loglevel", 'l', "Verbosity level (0-9)", 2 );
  cmdline.addInt( "nticks", 't', "Number of ticks to process (-1 = infinite)", -1 );
  #ifdef DEBUG
  cmdline.addBoolean( "debug", 'd', "Show debug messages (on/off)", 0 );
  #endif
  cmdline.addBoolean( "configHelp", 'H', "Show documentation of registered config types (on/off)", 0 );
  cmdline.addBoolean( "components", 'L', "Show component list", 0 );
  cmdline.addBoolean( "ccmdHelp", 'c', "Show custom commandline option help (those specified in config file)", 0 );

  int help = 0;
  if (cmdline.doParse() == -1) {
    LOGGER.setLogLevel(0);
    help = 1;
  }
  LOGGER.setLogLevel(cmdline.getInt("loglevel"));

  #ifdef DEBUG
  if (!cmdline.getBoolean("debug"))
    LOGGER.setLogLevel(LOG_DEBUG, 0);
  #endif
  
  //SMILE_PRINT("config file is: %s",cmdline.getStr("configfile"));

  // create configManager:
  cConfigManager *configManager = new cConfigManager(&cmdline);
  // add the file config reader:
  //configManager->addReader( new cFileConfigReader( cmdline.getStr("configfile") ) );

  // componentlist ist in componentManger.cpp definiert
  cComponentManager *cMan = new cComponentManager(configManager,componentlist);
  if (cmdline.getBoolean("configHelp")) {
    configManager->printTypeHelp(0);
    help = 1;
  }
  if (cmdline.getBoolean("components")) {
    cMan->printComponentList();
    help = 1;
  }
  if (help==1) { 
	delete configManager;
	delete cMan;
    return -1; 
  }

  // configManager->readConfig();
/*
    // read config here and print ccmdHelp...
  configManager->readConfig();
  cmdline.doParse(1,0); // warn if unknown options are detected on the commandline
  if (cmdline.getBoolean("ccmdHelp")) {
    cmdline.showUsage();
	delete configManager;
	delete cMan;
    return -1;
  }
*/

/********    
   hier kann QT run() etc. gestartet werden....
   *******/

  /*  z.B im Event-Handler für den "OK" button:

const ConfigType * typ = configManager->getTypeObj("cMfcc");
if (typ != NULL) {

 ConfigInstance * inst = new ConfigInstance("mymfcc");
 if (inst != NULL) {
   inst->setInt("firstMfcc",1);
   inst->setStr("reader.dmLevel","melspec");

   configManager->updateInstance(inst);
 }

} else {
  SMILE_IERR(1,"type not found 'cMfcc'");
}

  */

 

// Hiermit werden die Componenten erzeugt und configuriert/finalisiert:

  // try {
  cMan->createInstances(0); // 0 = do not read config (we already did that above..)
//} catch (cConfigException *ex) {
//     // GUI dialog: Fehler in Config... ungültige werte, etc...
//  }
// bei Fehler: cSmileException oder cConfigException, etc... siehe exceptions.hpp


  /*
   MAIN TICK LOOP :
  */
  cmanGlob = cMan;
//  signal(SIGINT, INThandler);  // signal handler für Ctrl+C zum abbrechen

  // hiermit wird die verarbeitung gestartet::: muss aus einem eigenen Thread aufgerufen werden um GUI nicht zu blockieren.
  long nTicks = cMan->runMultiThreaded(cmdline.getInt("nticks"));  // nticks = -1 für full input

  // nach ende des bearbeitungsthreads:::
  // (reset) funktion im ComponentManager aufrufen, die alle Componenten freigibt so dass sie erneut mit createInstances erzeugt werden können


// das hier auch erst beim beenden der applikation ausführen:
  // it is important that configManager is deleted BEFORE componentManger! (since component Manger unregisters plugin Dlls)
  delete configManager;
  delete cMan;


} catch(cSMILException) {} 

  return EXIT_SUCCESS;



  /*
  TODO::: 
     configManager, check if updateInstance will overwrite values  (OK)
	 configManager, deleteInstance function (ok)  deleteInstance(name);
	 componentManager, reset function (ok) resetInstances();

  */
  
}
