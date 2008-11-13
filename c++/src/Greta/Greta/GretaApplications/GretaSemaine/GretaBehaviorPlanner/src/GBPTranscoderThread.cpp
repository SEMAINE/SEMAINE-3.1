#include "GBPTranscoderThread.h"
// Include of the GBPEngine definition file to allow cross references
#include "GBPEngine.h"

#include "EngineParameterSetOfSets.h"

// default constructor
GBPTranscoderThread::GBPTranscoderThread()
{
	baselineLoaded = 0;
	bqcLoaded = 0;
	mssLoaded = 0;
	baselineBuffer="";
	baselineFilename="";
	AAE = new FMLAPML_AgentEngine();
}

// Constructor to use
GBPTranscoderThread::GBPTranscoderThread(GBPEngine *gbpe)
{
	this->gbpengine=gbpe;
	baselineLoaded = 0;
	bqcLoaded = 0;
	mssLoaded = 0;
	baselineBuffer="";
	baselineFilename="";
	AAE = new FMLAPML_AgentEngine();
}

// Destructor
GBPTranscoderThread::~GBPTranscoderThread()
{

}

// To do : compute the absolute time before and cut into different packet by using absolute time. 
int GBPTranscoderThread::Chunk(std::string buffer)
{
	XMLGenericTree *t;
	XMLGenericParser *p;
	p=new XMLGenericParser();
	p->SetValidating(true);
	char * buf = (char *)buffer.c_str();
	// load the FML buffer into an XML tree
	t=p->ParseBuffer(buf);
	std::list<GMTextElement> textlist;	
	textlist = std::list<GMTextElement>();
	std::list<GMTextElement>::iterator textElementListIter;
	std::string lastTM = "";
	std::string firstTM = "";
	std::string speech_node = "";
	GMTextElement currentTextElement;
	if(!t->child.empty())
	{
		XMLGenericTree * fmlapmltree;
		fmlapmltree = t->FindNodeCalled("fml-apml");
		std::list<XMLGenericTree*>::iterator fmlapmlchilditer;
		// Creation of the fml apml balise
		// treatment of the doc tree
		for(fmlapmlchilditer=fmlapmltree->child.begin();fmlapmlchilditer!=fmlapmltree->child.end();fmlapmlchilditer++)
		{
			XMLGenericTree * fmlapmlchildtree =(*fmlapmlchilditer) ;
			//Treatment of the FML APML tree
			if (fmlapmlchildtree->name=="bml")
			{
				std::list<XMLGenericTree*>::iterator bmlchilditer;
				for(bmlchilditer=fmlapmlchildtree->child.begin();bmlchilditer!=fmlapmlchildtree->child.end();bmlchilditer++)
				{
					XMLGenericTree * bmlchildTree =(*bmlchilditer) ;
					//Treatment of the BML tree
					if (bmlchildTree->name=="speech")
					{
						std::string speech_id = bmlchildTree->GetAttribute("id");
						std::string speech_voice =bmlchildTree->GetAttribute("voice");
						std::string speech_language =bmlchildTree->GetAttribute("language");
						std::string speech_speed_num =bmlchildTree->GetAttribute("speed_num");
						std::string speech_speed_sym =bmlchildTree->GetAttribute("speed_sym");
						std::string speech_text =bmlchildTree->GetAttribute("text");
						std::string speech_start =bmlchildTree->GetAttribute("start");
						speech_node ="<speech id = \""+ speech_id+"\" start =\""+ speech_start+"\" language= \"" + speech_language +"\" voice =\""+speech_voice+"\" speed_num=\""
			+speech_speed_num +"\" speed_sym=\""+speech_speed_sym +"\" text=\""+speech_text+"\" >\n";
						// Treatment of the speech tree
						std::list<XMLGenericTree*>::iterator speechchilditer;
						for(speechchilditer=bmlchildTree->child.begin();speechchilditer!=bmlchildTree->child.end();speechchilditer++)
						{
							XMLGenericTree * speechchildTree =(*speechchilditer) ;					
							/// description node
							if (speechchildTree->name=="description")
							{
								speech_node = speech_node + "<description level=\""+speechchildTree->GetAttribute("level")+"\" type=\""+speechchildTree->GetAttribute("type")+"\">\n";								
								// References recording
								std::list<XMLGenericTree*>::iterator descchilditer;
								for(descchilditer=speechchildTree->child.begin();descchilditer!=speechchildTree->child.end();descchilditer++)
								{
									XMLGenericTree * descchildtree =(*descchilditer) ;	
									std::string reference ="";
									if (descchildtree->name=="reference")
									{
										reference=descchildtree->FindNodeCalled("text")->value;		
										speech_node = speech_node + "<reference>" + reference +"</reference>\n";
									}
								}
								speech_node = speech_node + "</description>\n";
							}
							else 
							{
								if (speechchildTree->name=="boundary")
								{
									//Boundary node
									speechchildTree->GetAttribute("id");
									speechchildTree->GetAttribute("type");
									speechchildTree->GetAttribute("start");
									std::string pstarttm = speechchildTree->GetAttribute("start");	
									std::string test2 = pstarttm.substr(5);
									std::istringstream isstart(test2);
									int p2starttm ;
									isstart>>p2starttm;										
									for(textElementListIter=textlist.begin();textElementListIter!=textlist.end();textElementListIter++)
									{
										std::string test3 = (*textElementListIter).getStopTM().substr(2);
										std::string test4 = (*textElementListIter).getStartTM().substr(2);
										std::istringstream iststop(test3);
										std::istringstream iststart(test4);
										int ptstoptm ;
										int ptstarttm ;
										iststop>>ptstoptm;
										iststart>>ptstarttm;	
										if (ptstarttm<=p2starttm && ptstoptm>=p2starttm)
										{
											// Apply the effect
											std::string pxml = "<boundary id=\"" +speechchildTree->GetAttribute("id") + "\" type=\"" + speechchildTree->GetAttribute("type")+ "\" start=\""+pstarttm+"\"/>\n";			
											(*textElementListIter).addToSpeechContent(pxml);

										}
									}

								}
								else
								{
									if (speechchildTree->name=="pitchaccent")
									{
										// Pitaccent node									
										std::string pstoptm = speechchildTree->GetAttribute("end");
										std::string pstarttm = speechchildTree->GetAttribute("start");	
										std::string test1 = pstoptm.substr(5);
										std::string test2 = pstarttm.substr(5);
										std::istringstream isstop(test1);
										std::istringstream isstart(test2);
										int p2stoptm ;
										int p2starttm ;
										isstop>>p2stoptm;
										isstart>>p2starttm;										
										for(textElementListIter=textlist.begin();textElementListIter!=textlist.end();textElementListIter++)
										{
											std::string test3 = (*textElementListIter).getStopTM().substr(2);
											std::string test4 = (*textElementListIter).getStartTM().substr(2);
											std::istringstream iststop(test3);
											std::istringstream iststart(test4);
											int ptstoptm ;
											int ptstarttm ;
											iststop>>ptstoptm;
											iststart>>ptstarttm;	
											if (ptstarttm<=p2starttm && ptstoptm>=p2starttm || ptstarttm<=p2stoptm && ptstoptm>=p2stoptm)
											{
												// Apply the effect
												std::string pxml = "<pitchaccent id=\"" +speechchildTree->GetAttribute("id") + "\" type=\"" + speechchildTree->GetAttribute("type")+ "\" start=\""+pstarttm+"\"";
												pxml = pxml + " end =\"" +pstoptm+ "\"/>\n";					
												(*textElementListIter).addToSpeechContent(pxml);

											}
										}
									}
									else
									{
										if (speechchildTree->name=="tm")
										{
                                            // Time marker node
											std::string tm = speechchildTree->GetAttribute("id");
											if (firstTM=="")
											{
												firstTM=tm;
											}
											std::string tmxml = "<tm id=\""+tm+"\"/>\n"; 
											currentTextElement.addToSpeechContent(tmxml);
											lastTM=tm;
										}
										else
										{
											// Text content
											std::string text = " "+speechchildTree->value+"\n";
											currentTextElement.addToSpeechContent(text);
											if (text.find(".")!=std::string::npos 
												|| text.find("!")!=std::string::npos 
												|| text.find(",")!=std::string::npos
												|| text.find("?")!=std::string::npos)
											{
												currentTextElement.setStartTM(firstTM);
												currentTextElement.setStopTM(lastTM);
												textlist.push_back(currentTextElement);
												firstTM="";
												currentTextElement = GMTextElement();
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				if (fmlapmlchildtree->name=="fml")
				{
					std::list<XMLGenericTree*>::iterator fmlchilditer;
					for(fmlchilditer=fmlapmlchildtree->child.begin();fmlchilditer!=fmlapmlchildtree->child.end();fmlchilditer++)
					{
						XMLGenericTree * fmlchildTree =(*fmlchilditer) ;
						std::string fmlchild = "";
						fmlchild = "<"+ fmlchildTree->name + " id=\"" + fmlchildTree->GetAttribute("id")+"\"";
						if (fmlchildTree->GetAttribute("type")!="")
						{
							fmlchild = fmlchild + " type=\""+fmlchildTree->GetAttribute("type") +"\"";
						}
						if (fmlchildTree->GetAttribute("ref_type")!="")
						{
							fmlchild = fmlchild + " ref_type=\""+fmlchildTree->GetAttribute("ref_type") +"\"";
						}
						if (fmlchildTree->GetAttribute("ref_id")!="")
						{
							fmlchild = fmlchild + " ref_id=\""+fmlchildTree->GetAttribute("ref_id") +"\"";
						}
						if (fmlchildTree->GetAttribute("prop_type")!="")
						{
							fmlchild = fmlchild + " prop_type=\""+fmlchildTree->GetAttribute("prop_type") +"\"";
						}
						if (fmlchildTree->GetAttribute("prop_value")!="")
						{
							fmlchild = fmlchild + " prop_value=\""+fmlchildTree->GetAttribute("prop_value") +"\"";
						}
						std::string pstoptm = fmlchildTree->GetAttribute("end");
						std::string pstarttm = fmlchildTree->GetAttribute("start");	
						std::string test1 = pstoptm.substr(5);
						std::string test2 = pstarttm.substr(5);
						std::istringstream isstop(test1);
						std::istringstream isstart(test2);
						int p2stoptm ;
						int p2starttm ;
						isstop>>p2stoptm;
						isstart>>p2starttm;		
						if (fmlchildTree->GetAttribute("start")!="")
						{
							fmlchild = fmlchild + " start=\""+fmlchildTree->GetAttribute("start") +"\"";
						}
						if (fmlchildTree->GetAttribute("end")!="")
						{
							fmlchild = fmlchild + " end=\""+fmlchildTree->GetAttribute("end") +"\"";
						}
						fmlchild = fmlchild + "/>\n";
						for(textElementListIter=textlist.begin();textElementListIter!=textlist.end();textElementListIter++)
						{
							std::string test3 = (*textElementListIter).getStopTM().substr(2);
							std::string test4 = (*textElementListIter).getStartTM().substr(2);
							std::istringstream iststop(test3);
							std::istringstream iststart(test4);
							int ptstoptm ;
							int ptstarttm ;
							iststop>>ptstoptm;
							iststart>>ptstarttm;	
							if (ptstarttm<=p2starttm && ptstoptm>=p2starttm || ptstarttm<=p2stoptm && ptstoptm>=p2stoptm)
							{
								// Apply the effect
													
								(*textElementListIter).addToFMLContent(fmlchild);

							}
						}
						std::cout<<fmlchild<<std::endl;
					}
				}
			}
		}
	}	
	std::cout << "Fin de l'analyse " << std::endl;
	for(textElementListIter=textlist.begin();textElementListIter!=textlist.end();textElementListIter++)
	{
		std::string tag = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<!DOCTYPE fml-apml SYSTEM \"apml/fml-apml.dtd\" []>\n<fml-apml>";
		std::string bml = "<bml>\n"+speech_node+(*textElementListIter).getSpeechContent()+"</speech></bml>\n";
		std::string fml = "<fml>\n" +(*textElementListIter).getFMLContent()+"</fml>\n";
		tag = tag + bml + fml + "</fml-apml>" ;
		std::cout <<   tag <<std::endl;
		taglist.push_back(tag);
	}
	return 1;
}

// The baseline loading methods
int GBPTranscoderThread::loadBaseline(std::string baselinefilename,char *buffer)
{
	if (baselinefilename!="")
	{
		baselineFilename=baselinefilename.c_str();
		return 1;
	}
	else
	{
		baselineBuffer = JString(buffer);
		return 1;	
	}
}

// The behavior qualifier loading methods
int GBPTranscoderThread::loadBQC(std::string bqcfilename,char *buffer)
{
	if (bqcfilename!="")
	{
		bqcFilename=bqcfilename.c_str();
		return 1;
	}
	else
	{
		bqcBuffer = JString(buffer);	
		return 1;
	}
}

// The lexicon loading methods
int GBPTranscoderThread::loadMMS(std::string mssfilename,char *buffer)
{
	if (mssfilename!="")
	{
		mssFilename=mssfilename.c_str();
		return 1;
	}
	else
	{
		mssBuffer = JString(buffer);		
		return 1;
	}
}

// the execution methods
void GBPTranscoderThread::run()
{	
	// Begin of initialization step
	// The initialization state : 1 = System ready, initialization successful
	int initialized =0;
	// Load the baseline : comment this line to load the baseline throw the whiteboard
	this->loadBaseline("D:/Doc/Code/Greta/bin/mmsystem/baseline1.xml","");
	// Load the behavior qualifiers : comment this line to load the baseline throw the whiteboard
	this->loadBQC("mmsystem/test.xml","");
	// Load the lexicon : comment this line to load the baseline throw the whiteboard
	this->loadMMS("mmsystem/lexicon.xml","");
	// While the system isn't correctly initialized
	while (initialized!=1)
	{	
		// Check if the baseline is already loaded
		if (baselineLoaded!=1)
		{
			// Check if a baseline is in the buffer memory
			if (baselineBuffer!="")
			{
				AAE->loadBaseline("",baselineBuffer);
				baselineLoaded=1;
			}
			else
			{
				// Check if a baseline file name is in the baseline file name memory
				if (baselineFilename!="")
				{
					AAE->loadBaseline(baselineFilename.c_str(),"");
					baselineLoaded=1;
				}
			}
		}
		// Check if the behavior qualifiers is already loaded
		if (bqcLoaded!=1)
		{
			// Check if a behavior qualifiers is in the buffer memory
			if (bqcBuffer!="")
			{
				AAE->loadBQC("",bqcBuffer);
				bqcLoaded=1;
			}
			else
			{
				// Check if a behavior qualifiers file name is in the baseline file name memory
				if (bqcFilename!="")
				{
					AAE->loadBQC(bqcFilename.c_str(),"");
					bqcLoaded=1;
				}
			}
		}
		// Check if the lexicon is already loaded
		if (mssLoaded!=1)
		{
			// Check if a lexicon is in the buffer memory
			if (mssBuffer!="")
			{
				AAE->loadMMS("",mssBuffer);
				mssLoaded=1;
			}
			else
			{
				// Check if a lexicon file name is in the baseline file name memory
				if (mssFilename!="")
				{
					AAE->loadMMS(mssFilename.c_str(),"");
					mssLoaded=1;
				}
			}
		}
		if (baselineLoaded==1 && bqcLoaded== 1 && mssLoaded==1)
		{
			initialized = 1;
		}
		Sleep(5);
	}
	// End of initialization step

    std::cout << "Transcoder ready : Baseline BQC and MMS Loaded" << std::endl;

	// Begin of the transcoding step
	while (1)
	{
		if(!messageList.empty())
		{
			std::string mes = (std::string) messageList.front();
			//char * tag = (char *)(*tagiter).c_str();
			char * tag = (char *)mes.c_str();
			// Sending of the message content to the FML_APMLAgent engine
			std::string bml = AAE->Execute(tag);
			// Adding the produced BML message to the Sender buffer 
			gbpengine->getpGBPSender()->addMessage(bml);
			// Suppressing of the treated message from the FML message Buffer
			messageList.pop_front();
			
			// uncomment to enable the FML cutting 
			/*
			std::string mes = (std::string) messageList.front();	
			Chunk(mes);
			std::list<std::string>::iterator tagiter;
			for(tagiter=taglist.begin();tagiter!=taglist.end();tagiter++)
			{
				char * tag = (char *)(*tagiter).c_str();
				//char * tag = (char *)mes.c_str();
				// Sending of the message content to the FML_APMLAgent engine
				std::string bml = AAE->Execute(tag);
				// Adding the produced BML message to the Sender buffer 
				gbpengine->getpGBPSender()->addMessage(bml);
				// Suppressing of the treated message from the FML message Buffer
				messageList.pop_front();
			}
			*/
		}
		// don't overload processor
		Sleep(5);
	}
}

// Add a message to the FML message buffer 
int GBPTranscoderThread::addMessage(std::string mes)
{
	messageList.push_back(mes);
	return 1;
}