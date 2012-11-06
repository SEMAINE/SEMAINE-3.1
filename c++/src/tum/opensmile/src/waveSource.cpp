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


/*  openSMILE component:

waveSource : reads PCM WAVE files (RIFF format)

Support for a negative start index was added by Benedikt Gollan (TUM).

*/


#include <waveSource.hpp>
#define MODULE "cWaveSource"

SMILECOMPONENT_STATICS(cWaveSource)

SMILECOMPONENT_REGCOMP(cWaveSource)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CWAVESOURCE;
  sdescription = COMPONENT_DESCRIPTION_CWAVESOURCE;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->makeMandatory(ct->setField("filename","The filename of the PCM wave file to load. Only uncompressed RIFF files are supported. Use a suitable converter (mplayer, for example) to convert other formats to wave.","input.wav"));
    ct->setField("monoMixdown","Mix down all channels to 1 mono channel (1=on, 0=off)",1);
    ct->setField("start","The read start point in seconds from the beginning of the file",0.0);
    ct->setField("end","The read end point in seconds from the beginning of file (-1 = read to EoF)",-1.0);
    ct->setField("endrel","The read end point in seconds from the END of file (only if 'end' = -1, or not set)",0.0);
    ct->setField("startSamples","The read start in samples from the beginning of the file (this overwrites 'start')",0,0,0);
    ct->setField("endSamples","The read end in samples from the beginning of the file (this overwrites 'end' and 'endrelSamples')",-1,0,0);
    ct->setField("endrelSamples","The read end in samples from the END of file (this overwrites 'endrel')",0,0,0);
    ct->setField("noHeader","1 = treat the input file as 'raw' format, i.e. don't read the RIFF header. You must specify the parameters 'sampleRate', 'channels', and possibly 'sampleSize' if the defaults don't match your file format",0);
    ct->setField("sampleRate","Set/force the sampling rate that is assigned to the input data (required for reading raw files)",16000,0,0);
    ct->setField("sampleSize","Set/force the samplesize (in bytes) (required for reading raw files)",2,0,0);
    ct->setField("channels","Set/force the number of channels (required for reading raw files)",1,0,0);
    // overwrite cDataSource's default:
    ct->setField("blocksize_sec", NULL , 1.0);
    // TODO: check if this default will be used....!!
  )

  SMILECOMPONENT_MAKEINFO(cWaveSource);
}

SMILECOMPONENT_CREATE(cWaveSource)

//-----

cWaveSource::cWaveSource(const char *_name) :
  cDataSource(_name),
  filehandle(NULL),
  filename(NULL),
  //buffersize(2000),
  pcmDataBegin(0),
  curReadPos(0),
  eof(0),
  monoMixdown(0)
{
  // ...
}

void cWaveSource::fetchConfig()
{
  cDataSource::fetchConfig();
  
  filename = getStr("filename");
  SMILE_IDBG(2,"filename = '%s'",filename);
  if (filename == NULL) COMP_ERR("fetchConfig: getStr(filename) returned NULL! missing option in config file?");

  /*
  buffersize = getInt("buffersize");
  if (buffersize < 1) buffersize=1;
  SMILE_DBG(2,"buffersize = %i",buffersize);
*/

  monoMixdown = getInt("monoMixdown");
  if (monoMixdown) { SMILE_IDBG(2,"monoMixdown enabled!"); }

  start = getDouble("start");
  endrel = getDouble("endrel");
  end = getDouble("end");
}

int cWaveSource::configureWriter(sDmLevelConfig &c)
{
  negativestart = 0;
  int ret = 0;

  int noHeader = getInt("noHeader");

  if (!noHeader) { 
    ret = readWaveHeader();
    if (ret == 0) COMP_ERR("failed reading wave header from file '%s'! Maybe this is not a WAVE file?",filename);
  } else { 
    fseek( filehandle, 0, SEEK_END );
    size_t filesize = ftell(filehandle);
    rewind(filehandle);

    pcmParam.sampleRate = getInt("sampleRate");
    pcmParam.nChan = getInt("channels");
    pcmParam.nBPS = getInt("sampleSize");
    pcmParam.nBits = pcmParam.nBPS*8;
    pcmParam.blockSize = pcmParam.nChan * pcmParam.nBPS;
    pcmParam.nBlocks = (long)(filesize / pcmParam.blockSize);
    curReadPos = 0;
    ret = 1; 
  }

  double srate = (double)(pcmParam.sampleRate);
  if (srate==0.0) srate = 1.0;
  long flen  = (long)(pcmParam.nBlocks); // file lenght in samples ///XXXX TODO!

  if (isSet("startSamples")) {
    startSamples = getInt("startSamples");
  } else {
    SMILE_DBG(2,"start = %f",start);
    startSamples = (long)floor(start * srate);
  }
  if (startSamples < 0)
  {
	  negativestart = 1;
	  negstartoffset = -startSamples;
  }
  if (startSamples > flen) startSamples = flen;///XXXX TODO!
  SMILE_DBG(2,"startSamples = %i",startSamples);

  if (isSet("endSamples")) {
    endSamples = getInt("endSamples");
  } else {
    if (end < 0.0) endSamples = -1; ///XXXX TODO!
    else endSamples = (long)ceil(end * srate); ///XXXX TODO!
  }

  if (endSamples < 0) {
    if (isSet("endrelSamples")) {
      endrelSamples = getInt("endrelSamples");
      if (endrelSamples < 0) endrelSamples = 0;
      SMILE_DBG(2,"endrelSamples = %i",endrelSamples);
      endSamples = flen-endrelSamples; ///XXXX TODO!
      if (endSamples < 0) endSamples = 0;
    } else {
      if (isSet("endrel")) {
        endSamples = flen - (long)floor(endrel * srate); ///XXXX TODO!
        if (endSamples < 0) endSamples = 0;
      } else {
        endSamples = flen; ///XXXX TODO!
      }
    }
  }
  if (endSamples > flen) endSamples = flen; ///XXXX TODO!
  SMILE_DBG(2,"endSamples = %i",endSamples);

  if (startSamples > 0) { // seek to start pos!
    curReadPos = startSamples;
    fseek( filehandle, curReadPos*pcmParam.blockSize, SEEK_SET );
  }

    // TODO:: AUTO buffersize.. maximum length of wave data to store (depends on config of windower components)
    // so.. we CAN configure windower components first, which then set a config option in the wave source config..?
    //    OR central config unit apart from the configManager? (only for internal config?)
    // the windower comps. must set the buffersize info during their fetchCOnfig phase (i.e. BEFORE configure instance phase)
  c.T = 1.0 / (double)(pcmParam.sampleRate);
  //writer->setConfig(1,2*buffersize,T, 0.0, 0.0 , 0, DMEM_FLOAT);  // lenSec = (double)(2*buffersize)*T
  //c.basePeriod = c.T; // <- this is handled by dataSource already
  // TODO : blocksize...

  return 1;
}

int cWaveSource::myConfigureInstance()
{
  // open wave file.... etc. get header, etc.
  if (filehandle == NULL) {
    filehandle = fopen(filename, "rb");
    if (filehandle == NULL) COMP_ERR("failed to open input file '%s'",filename);
  }

  int ret = cDataSource::myConfigureInstance();
  
  if (!ret) {
    fclose(filehandle); filehandle = NULL;
  }
  return ret;
}

int cWaveSource::setupNewNames(long nEl) 
{
  // configure dataMemory level, names, etc.
  if (monoMixdown) {
    writer->addField("pcm",1);
    allocMat(1, blocksizeW);
  } else {
    writer->addField("pcm",pcmParam.nChan);
    allocMat(pcmParam.nChan, blocksizeW);
  }

  namesAreSet = 1;
  return 1;
}

/*
int cWaveSource::myFinaliseInstance()
{
 
  return cDataSource::myFinaliseInstance();
}
*/

int cWaveSource::myTick(long long t)
{
  if (isEOI()) return 0; //XXX ????
  if (mat == NULL) {
      if (monoMixdown) allocMat(1, blocksizeW);
      else allocMat(pcmParam.nChan, blocksizeW);
    }
  while (negativestart == 1)
  {
  if (negativestart == 1)
  {  
	  if (negstartoffset >= blocksizeW)
	  {
		  if (writer->checkWrite(negstartoffset)) {
		  writer->setNextMatrix(mat);
		  }
		  negstartoffset = negstartoffset - blocksizeW;
		  SMILE_MSG(1,"Negative Offset: = %f",negstartoffset);
	  }
	  if (negstartoffset > 0 && negstartoffset < blocksizeW)
	  {
		  cMatrix *matout = new cMatrix(mat->N,negstartoffset,mat->type);
		  if (writer->checkWrite(negstartoffset)) {
		  writer->setNextMatrix(matout);
		  }
		  negstartoffset = negstartoffset - blocksizeW;
		  SMILE_MSG(1,"Negative Offset: = %f",negstartoffset);
		  negativestart = 0;
	  }
	  if (negstartoffset == 0) negativestart = 0;
  }
  }
  // TODO: check if there is space in dmLevel for this write...!
  if (writer->checkWrite(blocksizeW)) {
    if (readData()) { // read new data from wave file!
      
      for (long i=0; i<mat->nT; i++) { // <<-- new timestamp assignment, untested!!
        mat->tmeta[i].smileTime = (double)(curReadPos-mat->nT+i)/(double)pcmParam.sampleRate;
        mat->tmeta[i].noAutoSmileTime = 1;
      }
      if (!writer->setNextMatrix(mat)) { // save data in dataMemory buffers
        SMILE_IERR(1,"can't write, level full... (strange, level space was checked using checkWrite(bs=%i)",blocksizeW);
      } else {
        return 1;
      }
    }
  }
  return 0;
}


cWaveSource::~cWaveSource()
{
  if (filehandle != NULL) fclose(filehandle);
}

//--------------------------------------------------  wave specific

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif


// reads data into matix m, size is determined by m, also performs format conversion to float samples and matrix format
int cWaveSource::readData(cMatrix *m)
{
  if (eof) {
    SMILE_WRN(6,"not reading from file, already EOF");
    return 0;
  }
  
  // TODO: mono-mixdown...
  
  if (m==NULL) {
    if (mat == NULL) {
      if (monoMixdown) allocMat(1, blocksizeW);
      else allocMat(pcmParam.nChan, blocksizeW);
    }
    m=mat;
  }
  if ( (m->N != pcmParam.nChan)&&((!monoMixdown) && (m->N == 1)) ) {
    SMILE_ERR(1,"readData: incompatible read! nChan=%i <-> matrix N=%i (these numbers must match!)\n",pcmParam.nChan,m->N);
    return 0;
  }
  // now convert the sample types to float or int (specified by cMatrix)
    // TODO: support int, by now only float is supported!

  int bs = pcmParam.blockSize * blocksizeW;
  if (endSamples - curReadPos < blocksizeW) { bs = (endSamples - curReadPos)*pcmParam.blockSize; }
  
  uint8_t *buf = (uint8_t *)malloc(bs);
  if (buf==NULL) OUT_OF_MEMORY;
  int nRead = (int)fread(buf, 1, bs, filehandle);
  if (nRead != pcmParam.blockSize * blocksizeW) {
    // EOF (or Error..?):
    SMILE_IWRN(5,"nRead (%i) < size to read (%i) ==> assuming EOF!",nRead,bs);
    eof=1;
    // TODO: signal EOF to component manager !
    
    m->nT = nRead / pcmParam.blockSize; // int div rounds down..!? it should better do so...
    fclose(filehandle); filehandle = NULL;
  }

  if (nRead > 0) {
    curReadPos += nRead/pcmParam.blockSize;
    
    int i,c;
    int8_t *b8=(int8_t*)buf;
    uint8_t *bu8=(uint8_t*)buf;
    int16_t *b16 = (int16_t*)buf;
    int32_t *b32 = (int32_t*)buf;

    if (monoMixdown) {

      switch(pcmParam.nBPS) {
        case 1: // 8-bit int
          for (i=0; i<m->nT; i++) {
            FLOAT_DMEM tmp = 0.0;
            for (c=0; c<pcmParam.nChan; c++) {
              tmp += (FLOAT_DMEM)b8[i*pcmParam.nChan+c];
            }
            m->setF(0,i,(tmp/(FLOAT_DMEM)pcmParam.nChan)/(FLOAT_DMEM)127.0);
          }
          break;
        case 2: // 16-bit int
          for (i=0; i<m->nT; i++) {
            FLOAT_DMEM tmp = 0.0;
            for (c=0; c<pcmParam.nChan; c++) {
              tmp += (FLOAT_DMEM)b16[i*pcmParam.nChan+c];
            }
            // NOTE: HTK does monomixdown with integer arithmetic, thus to be exact you would have to use:
            // m->setF(0,i,((FLOAT_DMEM)((long)tmp/(long)pcmParam.nChan))/(FLOAT_DMEM)32767.0);
            m->setF(0,i,(tmp/(FLOAT_DMEM)pcmParam.nChan)/(FLOAT_DMEM)32767.0);
          }
          break;
        case 3: // 24-bit int
          //COMP_ERR("24-bit wave file with 3 bytes per sample encoding not yet supported!");
          for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp = 0.0;
              for (c=0; c<pcmParam.nChan; c++) {
                // the only known file with 3bytes was exported by matlab
                // a byte order conversion was necessary here.. is that always the case?? FIXME!
                uint32_t is=0;
                is |= (uint32_t)(bu8[(i*pcmParam.nChan+c)*3])<<8;
                is |= (uint32_t)(bu8[(i*pcmParam.nChan+c)*3+1])<<16;
                is |= (uint32_t)(bu8[(i*pcmParam.nChan+c)*3+2])<<24;
                int32_t * iis = (int32_t*)&is;
                tmp += (FLOAT_DMEM)(*iis >> 8);
              }
              m->setF(0,i,(tmp/(FLOAT_DMEM)pcmParam.nChan)/(FLOAT_DMEM)(32767.0*256.0));
            }
        /*
        int24_t *b24 = (int24_t*)buf;
        for (i=0; i<m->nT; i++) for (c=0; c<pcmParam.nChan; c++) {
          m->setF(c,i,((FLOAT_DMEM)b24[i*pcmParam.nChan+c])/(32767.0*256.0));
        } break;
        */
          break;
        case 4: // 32-bit int or 24-bit packed int
          if (pcmParam.nBits == 24) {
            for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp = 0.0;
              for (c=0; c<pcmParam.nChan; c++) {
                tmp += (FLOAT_DMEM)(b32[i*pcmParam.nChan+c]&0xFFFFFF);
              }
              m->setF(0,i,(tmp/(FLOAT_DMEM)pcmParam.nChan)/(FLOAT_DMEM)(32767.0*256.0));
            }
            break;
          } else if (pcmParam.nBits == 32) {
            for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp = 0.0;
              for (c=0; c<pcmParam.nChan; c++) {
                tmp += (FLOAT_DMEM)(b32[i*pcmParam.nChan+c]);
              }
              m->setF(0,i,(tmp/(FLOAT_DMEM)pcmParam.nChan)/(FLOAT_DMEM)(32767.0*32767.0*2.0));
            }
            break;

      /*      for (i=0; i<m->nT; i++) for (c=0; c<pcmParam.nChan; c++) {
              m->setF(c,i,((FLOAT_DMEM)b32[i*pcmParam.nChan+c])/(32767.0*32767.0*2.0));
            } break;*/
        }
        default:
          SMILE_ERR(1,"readData: cannot convert unknown sample format to float! (nBPS=%i, nBits=%i)",pcmParam.nBPS,pcmParam.nBits);
          nRead=0;
      }
    
    } else { // no mixdown, multi-channel matrix output

      switch(pcmParam.nBPS) {
        case 1: // 8-bit int
          for (i=0; i<m->nT; i++) for (c=0; c<pcmParam.nChan; c++) {
            m->setF(c,i,((FLOAT_DMEM)b8[i*pcmParam.nChan+c])/(FLOAT_DMEM)127.0);
          } break;
        case 2: // 16-bit int
          for (i=0; i<m->nT; i++) for (c=0; c<pcmParam.nChan; c++) {
            m->setF(c,i,((FLOAT_DMEM)b16[i*pcmParam.nChan+c])/(FLOAT_DMEM)32767.0);
          } break;
        case 3: // 24-bit int
          for (i=0; i<m->nT; i++) {
              FLOAT_DMEM tmp = 0.0;
              for (c=0; c<pcmParam.nChan; c++) {
                // the only known file with 3bytes was exported by matlab
                // a byte order conversion was necessary here.. is that always the case?? FIXME!
                uint32_t is=0;
                is |= (uint32_t)(bu8[(i*pcmParam.nChan+c)*3])<<8;
                is |= (uint32_t)(bu8[(i*pcmParam.nChan+c)*3+1])<<16;
                is |= (uint32_t)(bu8[(i*pcmParam.nChan+c)*3+2])<<24;
                int32_t * iis = (int32_t*)&is;
                tmp = (FLOAT_DMEM)(*iis >> 8);
                m->setF(c,i,(tmp)/(FLOAT_DMEM)(32767.0*256.0));
              }
            }

          //COMP_ERR("24-bit wave file with 3 bytes per sample encoding not yet supported!");
        /*
        int24_t *b24 = (int24_t*)buf;
        for (i=0; i<m->nT; i++) for (c=0; c<pcmParam.nChan; c++) {
          m->setF(c,i,((FLOAT_DMEM)b24[i*pcmParam.nChan+c])/(32767.0*256.0));
        } break;
        */
          break;
        case 4: // 32-bit int or 24-bit packed int
          if (pcmParam.nBits == 24) {
            for (i=0; i<m->nT; i++) for (c=0; c<pcmParam.nChan; c++) {
              m->setF(c,i,((FLOAT_DMEM)(b32[i*pcmParam.nChan+c]&0xFFFFFF))/(FLOAT_DMEM)(32767.0*32767.0*2.0));
            } break;
          } else if (pcmParam.nBits == 32) {
            for (i=0; i<m->nT; i++) for (c=0; c<pcmParam.nChan; c++) {
              m->setF(c,i,((FLOAT_DMEM)b32[i*pcmParam.nChan+c])/(FLOAT_DMEM)(32767.0*32767.0*2.0));
            } break;
          }

        default:
          SMILE_ERR(1,"readData: cannot convert unknown sample format to float! (nBPS=%i, nBits=%i)",pcmParam.nBPS,pcmParam.nBits);
          nRead=0;
      }


    }
    
    // TODO: copy above code and change for DMEM_INT!
    
  }

  free(buf);
  return (nRead>0);
}

int cWaveSource::readWaveHeader()
{
  return smilePcm_readWaveHeader(filehandle, &pcmParam, filename);
#if 0
  if (filehandle != NULL) {

    int nRead;
    sRiffPcmWaveHeader head;
    sRiffChunkHeader chunkhead;
    int safetytimeout = 20;  // max <safetytimeout> chunks of 99kb size before 'data' chunk

    fseek(filehandle, 0, SEEK_SET);
    nRead = (int)fread(&head, 1, sizeof(head), filehandle);
    if (nRead != sizeof(head)) {
      SMILE_ERR(1,"Error reading %i bytes (header) from beginning of wave file '%s'! File too short??",sizeof(head),filename);
      return 0;
    }
    
    /* Check for valid header , TODO: support other endianness */
	if ((head.Riff != 0x46464952) ||
		(head.Format != 0x45564157) ||
		(head.Subchunk1ID != 0x20746D66) ||
//		(head.Subchunk2ID != 0x61746164) ||
		(head.AudioFormat != 1) ||
		(head.Subchunk1Size != 16)) {
                            SMILE_ERR(1,"\n  Riff: %x\n  Format: %x\n  Subchunk1ID: %x\n  Subchunk2ID: %x\n  AudioFormat: %x\n  Subchunk1Size: %x",
                                        head.Riff, head.Format, head.Subchunk1ID, head.Subchunk2ID, head.AudioFormat, head.Subchunk1Size);
                            SMILE_ERR(1,"bogus wave/riff header or file in wrong format!");
                            return 0;
        }

    while ((head.Subchunk2ID != 0x61746164)&&(safetytimeout>0)) { // 0x61746164 = 'data'
      // keep searching for 'data' chunk:
      if (head.Subchunk2Size < 99999) {
        char * tmp = (char*)malloc(head.Subchunk2Size);
        nRead = (int)fread(tmp, 1, head.Subchunk2Size, filehandle);
        if (nRead != head.Subchunk2Size) {
          SMILE_ERR(1,"less bytes read (%i) from wave file '%s' than indicated by Subchunk2Size (%i)! File seems broken!\n",nRead,filename,head.Subchunk2Size);
          return 0;
        }
        free(tmp);
      } else {
        SMILE_ERR(1,"Subchunk2Size > 99999. This seems to be a bogus file!\n");
        return 0;
      }
      nRead = (int)fread(&chunkhead, 1, sizeof(chunkhead), filehandle);
      if (nRead != sizeof(chunkhead)) {
        SMILE_ERR(1,"less bytes read (%i) from wave file '%s' than there should be (%i) while reading sub-chunk header! File seems broken!\n",nRead,filename,sizeof(chunkhead));
        return 0;
      }
      head.Subchunk2ID = chunkhead.SubchunkID;
      head.Subchunk2Size = chunkhead.SubchunkSize;
      safetytimeout--;
    }
    if (safetytimeout <= 0) {
      SMILE_ERR(1,"No 'data' subchunk found in wave-file among the first %i chunks! corrupt file?\n",safetytimeout);
      return 0;
    }
    
    pcmParam.sampleRate = head.SampleRate;
    pcmParam.nChan = head.NumChannels;
    pcmParam.nBPS = head.BlockAlign/head.NumChannels;
    pcmParam.nBits = head.BitsPerSample;
//    p->bits = head.BitsPerSample;
//    SMILE_DBG(5,"bits per sample = %i",head.BitsPerSample);
//    pcmParam.sampleType = pcmBitsToSampleType( head.BitsPerSample, BYTEORDER_LE, 0 );
/*
    if (head.NumChannels * head.BitsPerSample / 8 != head.BlockAlign) {
      FEATUM_ERR_FATAL(0,"Error reading wave file header: head.BlockAlign != head.NumChannels * head.BitsPerSample / 8");
      return 0;
    }
    */
    pcmParam.nBlocks = head.Subchunk2Size / head.BlockAlign;
    pcmParam.blockSize = head.BlockAlign;

    // TODO: ???
    pcmParam.byteOrder = BYTEORDER_LE;
    pcmParam.memOrga = MEMORGA_INTERLV;

    pcmDataBegin = ftell(filehandle);

    return 1;
  }
  return 0;
#endif
}

