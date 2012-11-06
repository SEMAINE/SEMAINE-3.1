
#include <smileCommon.hpp>
#include <rnn.hpp>

#define MODULE "smileRnn"

static const double expLim = log(DBL_MAX);

FLOAT_NN nnTf_logistic(FLOAT_NN x)
{
  if (x < -expLim) { return 1.0; }
  else if (x > expLim) { return 0.0; }
  return 1.0 / (1.0 + exp(-x));
}

/**************************** cNnLSTMcell *******************************************/

void cNnLSTMcell::setPeepWeights(FLOAT_NN *x, long N, int copy)
{ // function currently works for 1 cell per block only..
  if (N != 3*nCells) { // 3 is the number of gates... thus the number of peephole connections per cell
    RNN_ERR(1,"cNnLSTMcell: number of peep weights in cell %i.%i (%i) does not match expected number (%i x %i (=cellsPerBlock) = %i)!",layerIndex,cellIndex,N,3,nCells,3*nCells);
  }
  if (copy) {
    peep = (FLOAT_NN*)malloc(sizeof(FLOAT_NN)*N);
    memcpy(peep, x, N*sizeof(FLOAT_NN));
  } else {
    peep = x;
  }
  nPeeps = N;
}

// reset the cell
void cNnLSTMcell::reset() 
{
  // reset parent cell object
  cNnNNcell::reset();
  // reset cell state
  long i;
  for (i=0; i<nCells; i++) { 
    sc[i] = 0.0;
  }
  // reset outputs
  if ((cellOutputs != NULL) && (nOutputs > 1)) {
    long i; for (i=0; i<nCells; i++) { cellOutputs[i] = 0.0; }
  }
}


// feed data through the cell and compute the cell output (return value)
// note: for an lstm block with nCells > 1, the number of output values will be > 1... how to handle this??
const FLOAT_NN * cNnLSTMcell::forward(const FLOAT_NN *x, long *N) 
{
  // x[0] IG input
  // x[1] FG input
  // x[2] cell input to cell 1
  //( x[3] cell input to cell 2...)
  //( x[4] cell input to cell 3...)
  // ...
  // x[N] OG input

  FLOAT_NN actIG, actFG, actOG;

  if (nCells == 1) { // quick version for one cell per block

    // input Gate
    actIG = transferGate->f( x[0] + (*sc)*peep[0] );
    // forget Gate
    actFG = transferGate->f( x[1] + (*sc)*peep[1] );
    // cell
    *sc = actIG * *cNnNNcell::forward(x+2) + (*sc)*actFG;
    // output Gate
    actOG = transferGate->f( x[3] + (*sc)*peep[2] );
    // cell output

    cellOutput =  actOG * transferOut->f( *sc );
    //XX//fprintf(stderr,"pg: %f --> cellOutput %f   actOG %f   ag: %f\n",*sc,cellOutput,actOG,transferOut->f( *sc ));

    if (N!=NULL) *N=1;
    return &cellOutput;

  } else { // long version
    long i;
    FLOAT_NN sum=0.0;

    // input Gate
    for (i=0; i<nCells; i++) {  // TODO: check the order of the peephole weights
      sum += (sc[i])*peep[i];
    }
    actIG = transferGate->f( x[0] + sum );

    // forget Gate // TODO: add multi-dim. forget gate acts...? "num_seq_dims"
    sum = 0.0;
    for (i=0; i<nCells; i++) {  // TODO: check the order of the peephole weights
      sum += (sc[i])*peep[i+nCells];
    }
    actFG = transferGate->f( x[1] + sum );

    // cell
    for (i=0; i<nCells; i++) {  
      sc[i] = actIG * *cNnNNcell::forward(x+2+i) + (sc[i])*actFG;
      // TODO: add multi-dim. forget gate acts...? "num_seq_dims"
    }

    // output Gate
    sum = 0.0;
    for (i=0; i<nCells; i++) {  // TODO: check the order of the peephole weights
      sum += (sc[i])*peep[i+2*nCells];
    }
    actOG = transferGate->f( x[3] + sum );

    // cell output
    for (i=0; i<nCells; i++) {  
      cellOutputs[i] = actOG * transferOut->f(sc[i]);
    }

    if (N!=NULL) *N=nOutputs;
    return (const FLOAT_NN*)cellOutputs;
  }

}


/**************************** cNnLayer *******************************************/


// feed data forward, N must match the layer's input size (nInputs)
void cNnLayer::forward(FLOAT_NN *x, long N) 
{
  long i;
  FLOAT_NN * curoutp = output + curPtr * nOutputs;
  for (i=0; i<nCells; i++) {
    long n = nCellInputs;
    const FLOAT_NN *tmpout = cell[i]->forward(x,&n);
    x += nCellInputs;
    // add tmpout to our output vector
    memcpy(curoutp+i*nCellOutputs,tmpout,sizeof(FLOAT_NN)*nCellOutputs);
  }
  if (nContext > 0) {
    curPtr++; 
    if (curPtr > nContext) curPtr = 0;  /* NOTE: the buffersize of the output buffer is nContext + 1 */
    else nDelayed++;
  } /* else {
    memcpy(output+i*nCellOutputs,tmpout,sizeof(FLOAT_NN)*nCellOutputs);
    }*/

}

void cNnLayer::resetLayer() 
{
  // reset the output activation buffer
  nDelayed = 0;
  curPtr = 0;
  bzero(output , nContext*nOutputs*sizeof(FLOAT_NN));
  // reset the cells
  long i;
  for (i=0; i<nCells; i++) {
    cell[i]->reset();
  }
}


/**************************** cNnSoftmaxLayer *******************************************/

void cNnSoftmaxLayer::forward(FLOAT_NN *x, long N) 
{
  long i; double sum = 0.0;
  //TODO: handle curPtr and context buffered output..!? OR: ensure nContext always == 1
  if (N > MIN(nInputs,nOutputs)) { N = MIN(nInputs,nOutputs); }
  for (i=0; i<N; i++) {
    if (x[i] == -INFINITY) { output[i] = 0.0; }
    else if (x[i] > expLim) { output[i] = DBL_MAX; }
    else output[i] = exp(x[i]);
    sum += output[i];
  }
  if (sum != 0.0) {
    for (i=0; i<N; i++) {
      output[i] /= sum; 
    }
  }
}

/**************************************************************************************/
/**************************** cNnConnection *******************************************/
/**************************************************************************************/


void cNnConnection::initialise(long _bidir_bufsize)
{
  int i; int bidir=0;
  // get i/o size parameters from input layers:
  inputSize = 0;
  for (i=0; i<nInputs; i++) {
    inputStart[i] = inputSize;
    inputSize += input[i]->getOutputSize();
    if (input[i]->isReverse()) bidir = 1;
  }
  // get number of outputs:
  outputSize = output->getInputSize();
  // allocate weight memory for inputSize*outputSize weights:
  nWeights = inputSize*outputSize;
  //printf("iS: %i, oS: %i, nWeights:%i\n",inputSize,outputSize,nWeights); fflush(stdout);
  weight = (FLOAT_NN*)malloc(sizeof(FLOAT_NN)*nWeights);
  // allocate bias weight memory for outputSize weights:
  nBias = outputSize;
  bias = (FLOAT_NN*)malloc(sizeof(FLOAT_NN)*nBias);
  // allocate current output vector/buffer:
  /* we don't require the buffer in the connection object, it is in the layers
  if (bidir) {
  // bidirectional nets require buffered output for the "backward" pass
  //?? outputs = (FLOAT_NN*)calloc(1,sizeof(FLOAT_NN)*outputSize);
  } else {
  // caching of one output vector is sufficient for unidirectional (recurrent) nets
  */
  outputs = (FLOAT_NN*)calloc(1,sizeof(FLOAT_NN)*outputSize);
  //}
}

// print human readable connection information 
void cNnConnection::printInfo()
{
  int i;
  fprintf(stderr,"  from: ");
  for (i=0; i<nInputs; i++) {
    if (input[i] != NULL) 
      fprintf(stderr," '%s' (%i); ",input[i]->getName(),input[i]->getLayerIdx());
  }
  fprintf(stderr,"   -> to: '%s' (%i)\n",output->getName(),output->getLayerIdx());
}

// forward data (one timestep) from the input layers to the output layer
void cNnConnection::forward() 
{
  // the job of the connection class is:
  //   to read the layer outputs from the input layers (the first layer must initialised with forward() externally)
  //   apply the weights
  //   and build the input vectors for the output layer cells
  //   then call forward() on the output layer to "compute the next layer"
  int i;
  long j,n; 
  FLOAT_NN *wght = weight;
  FLOAT_NN *outp;

  for (j=0; j<outputSize; j++) {
    outputs[j] = bias[j];
  }

  /* handling of (buffered) bidirectional nets.... */
  /*

  check if output is reverse layer...
  if no:   
  forward data as usual, however only if all inputs from bidirectional input layers are available 
  if yes:
  forward data only (nContext times backward) if the input buffer is full (nDelayed == nContext)
  IMPORTANT: output only one next frame...?
  prob: the output activations get stored by the net..., so we only forward the top element of the activations...

  PROBLEM: theoretically every hidden layer in a bi-directional net introduces a delay
  */
  /****/

  // check if output is bidirectional reverse layer:
  int bidirOut = output->isReverse();

  // check for bidirectional inputs (if one is present we must delay processing)
  int bidirIn = 0;
  for (i=0; i<nInputs; i++) {
    if (input[i]->isReverse()) { bidirIn = 1; break; }
  }


  if (!bidirOut) { // "forward pass"
    //printf("fw ");
    // read input layer outputs, multiply with weights and sum to output
    for (i=0; i<nInputs; i++) {
      //printf("OAV(%i) = %i ",i,input[i]->outputAvailable());
      if (!(input[i]->outputAvailable())) { return; }

      long _N=0;
      long _idx = 0;
      if (!input[i]->isReverse()) { _idx = input[i]->getNContext(); } // pull top index from reverse layers...?

      const FLOAT_NN *tmp = input[i]->getOutput(&_N,_idx);
      long size = input[i]->getOutputSize();

      //XX//for (n=0; n<size; n++) { if (tmp[n] != 0.0) { fprintf(stderr,"input %i , tmp %i\n",i,n); } }

      // TODO: check _N == outputSize ??

      outp = outputs;
      for (j=0; j<outputSize; j++) {
        for (n=0; n<size; n++) {
          //XX//if (*wght != 0.0012) fprintf(stderr,"n  %i j %i\n",n,j);
          *outp += tmp[n]* (*(wght++));
        }
        outp++;
      }
      //XX//fprintf(stderr,"outputS   %i %i\n",outputSize,size);
    }

    // pass output to output layer
    //XX//for (n=0; n<outputSize; n++) { if (outputs[n] != 0.0) { fprintf(stderr,"FWoutput %i = %f\n",n,outputs[n]); } }
    output->forward(outputs,outputSize);

  } else { // "backward pass" over full buffer



    // we first must reset the layer, then gather the inputs at the end of the buffer (t=0)
    // and forward .. then loop through the buffer until we reach (t = -bufsize)
    // this is our first valid output of the output level 
    output->resetLayer();

    // check if input is available in all forward input layers... EXCLUDE self recurrent connections!
    for (i=0; i<nInputs; i++) {
      // we check if nContext samples are available in this input layer...
      if ((input[i]!=output)&&(!(input[i]->outputAvailable()))) { return; }
    }

    long nContext = output->getNContext();
    // Note: nContext in input an outout layers should be the same.. if not, we have to use a different value for nContext..? input context size?
    //printf("bw nCO=%i ",nContext);
    long c;
    for (c=0; c<nContext; c++) {

      wght = weight;
      // read input layer outputs, multiply with weights and sum to output
      for (i=0; i<nInputs; i++) {

        //printf("OAV(%i) = %i ",i,input[i]->outputAvailable()); fflush(stdout);
        // NOTE: backward recurrent connections must be forwarded disregarding the output available.... (we go through the data backwards anyways)


        long _N=0;
        long _idx = 0;
        if (input[i]->isReverse() && (input[i] != output)) { _idx = c; } // TODO?: pull top index from reverse layers, except the self recurrent layers...?
        else { _idx = c ; }
        if (input[i] == output) { _idx = 0; }

        const FLOAT_NN *tmp = input[i]->getOutput(&_N, _idx);
        long size = input[i]->getOutputSize();

        //XX//for (n=0; n<size; n++) { if (tmp[n] != 0.0) { fprintf(stderr,"input %i , tmp %i\n",i,n); } }

        // TODO: check _N == outputSize ??

        outp = outputs;
        //printf("size %i outputSize %i  (* = %i) nweights=%i\n",size,outputSize,size*outputSize,nWeights); fflush(stdout);
        for (j=0; j<outputSize; j++) {
          for (n=0; n<size; n++) {
            //XX//if (*wght != 0.0012) fprintf(stderr,"n  %i j %i\n",n,j);
            *outp += tmp[n]* (*(wght++));
          }
          outp++;
        }
        //XX//fprintf(stderr,"outputS   %i %i\n",outputSize,size);
      }

      // pass output to output layer
      //XX//for (n=0; n<outputSize; n++) { if (outputs[n] != 0.0) { fprintf(stderr,"FWoutput %i = %f\n",n,outputs[n]); } }
      output->forward(outputs,outputSize);

    }

  }
  //printf("\n");
}


/**************************************************************************************/
/**************************** cNnRnn **************************************************/
/**************************************************************************************/

// feed data forward through the net and compute the next output activations vector
void cNnRnn::forward(FLOAT_NN *x, long N)
{
  long i;
  // feed input to first layer (input layer)
  layer[0]->forward(x,N);
  // iterate through connections...
  for (i=1; i<=nConnections; i++) {
    if (connection[i] != NULL) connection[i]->forward();
    else { RNN_ERR(1,"connection[%i] is NULL",i); }
  }
}


// print the connections in human readable format to the log
void cNnRnn::printConnections()
{
  int i;
  fprintf(stderr,"%i net layers:\n",nLayers);
  for (i=0; i<nLayers; i++) {
    fprintf(stderr,"  %i : '%s'\n",i,layer[i]->getName());
  }
  for (i=1; i<=nConnections; i++) {
    if (connection[i] != NULL) {
      fprintf(stderr,"--CONNECTION %i--\n",i);
      connection[i]->printInfo();
    }
  }
}


/********************************************* RNN init *******************************/


unsigned long smileRnn_parseLayerNumber(char *s, unsigned long *dir)
{
  char *x = strchr(s,'_');
  if (dir != NULL) {
    *dir = LAYER_DIR_FWD;
  }
  if (x != NULL) { // two numbers separated by _, e.g. 0_1
    if ((*(x+1) == '1')&&(dir != NULL)) {
        *dir = LAYER_DIR_RWD;
    }
    *x = 0;
    return (unsigned long)strtol(s,NULL,10);
  } else { // only one number
    return (unsigned long)strtol(s,NULL,10);
  }
  return -1;
}

cRnnWeightVector *smileRnn_createWeightVectorFromLine(char *line)
{
  long i=0;
  char *lastl = line;
  char *curl=line;
  char *name = NULL;

  // get name
  while(1) {
    if ((*curl == ' ')||(*curl == 0)) { 
      if (*curl == 0) return NULL;
      *curl = 0; name = lastl; curl++; lastl = curl; break; 
    }
    curl++;
  }
  // check for "_weights"
  char *_to=NULL, *_from=NULL;
  int isPeep = 0;
  char *ww = strstr(name,"_weights");
  if (ww != NULL) {
    // truncate
    ww[0] = 0;
    ww = strstr(name,"_to_");
    if (ww != NULL) {
      ww[0] = 0;
      _to = ww+4;
      ww = strstr(_to,"_delay_1"); // remove the
      if (ww != NULL) ww[0] = 0;   // delay_1 suffix for recurrent connections...
      ww = strstr(_to,"_delay_-1"); // remove the
      if (ww != NULL) ww[0] = 0;   // delay_-1 suffix for recurrent connections...
      _from = name;
    } else { // peepholes?
      ww = strstr(name,"_peepholes");
      if (ww != NULL) { // yes, peephole weights
        ww[0] = 0;
        _from = name; // assign only from !
        isPeep = 1;
      } else { //syntax error
        return NULL;
      }
    }
  } else {
    // if not found (no weight vector!)
    return NULL;
  }

  // get number of weights
  char *numw=NULL;
  while(1) {
    if ((*curl == ' ')||(*curl == 0)) { 
      if (*curl == 0) return NULL;
      *curl = 0; numw = lastl; curl++; lastl = curl; break; 
    }
    curl++;
  }
  // convert numw to int
  long nWeights = strtol(numw,NULL,10);

  if (nWeights > 0) {
    if (isPeep) {
      SMILE_MSG(3,"%i peephole weights in layer '%s'",nWeights,_from);
    } else {
      SMILE_MSG(3,"%i weights : from '%s' to '%s'",nWeights,_from,_to);
    }
    // create weights vector
    cRnnWeightVector *l = new cRnnWeightVector(nWeights);
    // copy from and to names..
    if (_to != NULL) { strncpy(l->to,_to,100); }
    if (_from != NULL) { strncpy(l->from,_from,100); }
    // parse from and to names...?
    if (isPeep) { // peephole weights require only the "from" field (they are layer internal)
      l->F = 0; l->T=-1;
      if (!strncmp(l->from,"hidden_",7)) { l->F = LAYER_HIDDEN; }
      l->F |= LAYER_PEEPHOLES;
      // layer number and direction ...
      unsigned long dir=0;
      l->F |= smileRnn_parseLayerNumber(l->from+7,&dir);
      l->F |= dir;
    } else {
      l->F = 0; l->T=-1;
      if (!strncmp(l->from,"bias",4)) { l->F = LAYER_BIAS; }
      else if (!strncmp(l->from,"hidden_",7)) {  // || (!strncmp(l->from,"gather_",7))
        l->F = LAYER_HIDDEN; 
        // layer number and direction ...
        unsigned long dir=0;
        l->F |= smileRnn_parseLayerNumber(l->from+7,&dir);
        l->F |= dir;
      } else if (!strncmp(l->from,"gather_",7)) {  // || (!strncmp(l->from,"gather_",7))
        l->F = LAYER_HIDDEN_GATHER; 
        // layer number and direction ...
        //unsigned long dir=0;
        l->F |= smileRnn_parseLayerNumber(l->from+7,NULL);
        //l->F |= dir;
        printf("layer F: %i\n",l->F);
      }
      else if (!strncmp(l->from,"input",5)) { l->F = LAYER_INPUT; }

      if (!strncmp(l->to,"hidden_",7)) { 
        l->T = LAYER_HIDDEN;
        // layer number and direction ...
        unsigned long dir=0;
        l->T |= smileRnn_parseLayerNumber(l->to+7,&dir);
        l->T |= dir;
      }
      else if (!strncmp(l->to,"output",6)) { l->T = LAYER_OUTPUT; }
    }

    //XX//fprintf(stderr,"strL curl %i\n",strlen(curl));
    // copy weights to vector
    long curW = 0;
    char *w=NULL;
    while(1) {
      if (*curl == ' ') { 
        *curl=0; char *ep=NULL;
        FLOAT_NN wf = strtod(lastl,&ep);
        l->weights[curW++] = wf;
        if (nWeights == curW) {
          SMILE_ERR(1,"too many weights on line, expected only %i weights ('%s')",nWeights,lastl);
          break;
        }
        //XX//if (wf != 0.001) { fprintf(stderr,"curW %i of %i\n", curW,nWeights); }
        curl++; lastl = curl; 
      } else if ((*curl == '\n')||(*curl == '\r')||(*curl == 0)) { 
        *curl=0; char *ep=NULL;
        FLOAT_NN wf = strtod(lastl,&ep);
        l->weights[curW++] = wf;
        
        //curl++; lastl = curl; 
        break;
      } else {
        curl++;
      }
    }

    return l;
  }
  return NULL;
}

int smileRnn_loadNet(const char *filename, cRnnNetFile &net)
{
  int bidirectional = 1;
  net.nContext = 0;
  if (filename != NULL) {
    FILE *f = fopen(filename,"r");
    if (f != NULL) {
      char *line=NULL;
      size_t n=0;
      int ret;
      do { // read the net file line by line...
        ret = getline(&line,&n,f);
        if ((ret > 1)&&(line!=NULL)) {
          if (!strncmp(line,"weightContainer_",16)) { // weight vector line...
            cRnnWeightVector *v = smileRnn_createWeightVectorFromLine(line+16);
            if (v != NULL) { // add to a collection..
              net.wv[net.nWeightVectors++] = v;
            }
          } else {
            // net config line
            // we are interested in reading 'hiddenSize', 'hiddenType', 'task' (output layer!), and possibly (?) 'actSmoothing' and 'actDecay'
            if (!strncmp(line,"hiddenSize ",11)) {
              // parse string with sizes of hidden layer, space separated
              long i=0;
              char *n1=line+11, *n2=line+11;
              while(1) {
                if ((*n2 == ',')||(*n2=='\n')||(*n2=='\r')||(*n2==0)) {
                  char myc = *n2;
                  *n2 = 0;
                  long l = strtol(n1,NULL,10);
                  net.hiddenSize[i++] = l;
                  if ((myc=='\n')||(myc=='\r')||(myc==0)) { break; }
                  n2++; n1=n2;
                } else { n2++; }
              }
              net.nHiddenLayers = i;
            } else if (!strncmp(line,"hiddenType ",11)) {
              // parse string with types of hidden layers (space separated?)
              long i=0;
              char *n1=line+11, *n2=line+11;
              while(1) {
                if ((*n2 == ',')||(*n2=='\n')||(*n2=='\r')||(*n2==0)) {
                  char myc = *n2;
                  *n2 = 0;
                  if (!strncmp(n1,"lstm",4)) {
                    net.hiddenType[i] = NNLAYERTYPE_LSTM;
                    net.hiddenActType[i] = NNLSTMACT_TANHTANHLOGI;
                    i++;
                  } else if (!strncmp(n1,"linear_lstm",11)) {
                    net.hiddenType[i] = NNLAYERTYPE_LSTM;
                    net.hiddenActType[i] = NNLSTMACT_TANHIDENLOGI;
                    i++;
                  } else if (!strncmp(n1,"tanh",4)) {
                    net.hiddenType[i] = NNLAYERTYPE_RNN;
                    net.hiddenActType[i] = NNACT_TANH;
                    i++;
                  } else if (!strncmp(n1,"linear",6)) {
                    net.hiddenType[i] = NNLAYERTYPE_RNN;
                    net.hiddenActType[i] = NNACT_IDEN;
                    i++;
                  } else if (!strncmp(n1,"logistic",8)) {
                    net.hiddenType[i] = NNLAYERTYPE_RNN;
                    net.hiddenActType[i] = NNACT_LOGI;
                    i++;
                  } else {
                    SMILE_ERR(1,"unsupported hidden layer type '%s' ('hiddenType' option) while reading '%s'.",n1,filename);
                  }
                  if ((myc=='\n')||(myc=='\r')||(myc==0)) { break; }
                  n2++; n1=n2;
                } else { n2++; }
              }
              // duplicate hiddenType if # < nHiddenLayers:
             /* if (i<net.nHiddenLayers) {
                for(; i<net.nHiddenLayers; i++) {
                  net.hiddenActType[i] = net.hiddenActType[i-1];
                  net.hiddenType[i] = net.hiddenType[i-1];
                }
              }*/
              if (net.nHiddenLayers > i) { // if less types than layer sizes given, assume the following layers are of the same type as the last layer...?
                int tp = net.hiddenType[i-1];
                int ac = net.hiddenActType[i-1];
                for (; i<net.nHiddenLayers; i++) {
                  net.hiddenType[i] = tp;
                  net.hiddenActType[i] = ac;
                }
              }
            } else if (!strncmp(line,"task ",5)) {
              // net task
              if (!strncmp(line+5,"regression",10)) {
                net.task = NNTASK_REGRESSION;
              } else if (!strncmp(line+5,"classification",14)) {
                net.task = NNTASK_CLASSIFICATION;
              } else if (!strncmp(line+5,"transcription",13)) {
                net.task = NNTASK_TRANSCRIPTION;
                //SMILE_IERR(1,"CTC decoding (task = transcription) not yet supported!");
              }
            } else if (!strncmp(line,"bidirectional ",14)) {
              if (!strncmp(line+14,"false",5)) bidirectional = 0;
              else if (!strncmp(line+14,"true",4)) bidirectional = 1;
            } else if (!strncmp(line,"contextLength ",13)) { // <-- this option is new, it is not an rnnlib option!
              if (!strncmp(line+13,"inf",3)) net.nContext = 0;
              else {
                net.nContext = strtol(line+13,NULL,10);
              }
            }
          }
        } else { if (ret <= 0) break; }
      } while (1);
      fclose(f);
      free(line);
      //// now determine inputSize and outputSize from connection weights
      //// (the rnnlib determiens these sizes from the .nc files, which we don't have...)
      // 1. find bias_to_output to get output size...
      int i;
      for (i=0; i<net.nWeightVectors; i++) {
        if ((net.wv[i]->F == LAYER_BIAS) && (net.wv[i]->T == LAYER_OUTPUT)) { net.outputSize = net.wv[i]->nWeights; break; }
      }
      // 2. find input_to_hidden_0_x
      for (i=0; i<net.nWeightVectors; i++) {
        if ((net.wv[i]->F == LAYER_INPUT) && ( (net.wv[i]->T & LAYER_HIDDEN) == LAYER_HIDDEN)) { 
          long f = 1;
          if (net.hiddenType[0] == NNLAYERTYPE_LSTM) { f = 4; }
          net.inputSize = net.wv[i]->nWeights / (net.hiddenSize[0] * f); 
        }
      }
      net.cellsPerBlock = 1; // FIXME...!
      // test for bidirectional network topology
      net.bidirectional = bidirectional; // FIXME??
      /*for (i=0; i<net.nWeightVectors; i++) {  // do this as extra check ???
        if (net.wv[i]->F & LAYER_DIR_RWD == LAYER_DIR_RWD) net.bidirectional = 1;
      }*/

      // fix gather layers for the unidirectional case ....
/*      if (!bidirectional) {
        for (i=0; i<net.nWeightVectors; i++) {
          if (net.wv[i]->F == LAYER_HIDDEN_GATHER) net.wv[i]->F = LAYER_HIDDEN;
          // Note: the direction is FWD by default (0), so we don't need to set it here
        }
      }*/
      // TODO: gather layers for bidirectional case...??


      net.loaded = 1;
      
      return 1;
    } else {
      SMILE_ERR(1,"failed to open rnn net file '%s'.",filename);
    }
  } else {
    SMILE_ERR(1,"failed to open rnn net file, the filename is a NULL string.");
  }
  return 0;
}

cNnLSTMlayer *smileRnn_createLstmLayer(int i, int idx, int dir, cRnnNetFile &net)
{
  // create the forward layer
  cNnLSTMlayer * l = new cNnLSTMlayer(net.hiddenSize[i],idx,dir,net.nContext);
  // create cells with activation functions
  cNnTf * tfI=NULL, * tfO=NULL, *tfG=NULL;
  if (net.hiddenActType[i] == NNLSTMACT_TANHTANHLOGI) {
    tfI = new cNnTfTanh();
    tfO = new cNnTfTanh();
    tfG = new cNnTfLogistic();
  } else if (net.hiddenActType[i] == NNLSTMACT_TANHIDENLOGI) {
    tfI = new cNnTfTanh();
    tfO = new cNnTfIdentity();
    tfG = new cNnTfLogistic();
  } else {
    COMP_ERR("unknown hiddenActType[%i] %i while creating an LSTM layer!",i,net.hiddenActType[i]);
  }
  l->createCells(tfI,tfO,tfG , net.cellsPerBlock);

  return l;
}

int smileRnn_findPeepWeights(unsigned long id, cRnnNetFile &net) 
{
  int j;
  for (j=0; j<net.nWeightVectors; j++) {
    if ( net.wv[j]->F == (id|LAYER_PEEPHOLES) ) {
      // return the weightVector index..
      return j;
    }
  }
  return -1;
}

int smileRnn_findWeights(unsigned long idFrom, unsigned long idTo, cRnnNetFile &net) 
{
  int j;
  for (j=0; j<net.nWeightVectors; j++) {
    if ((net.wv[j]->F == idFrom)&&(net.wv[j]->T == idTo)) {
      // return the weightVector index..
      return j;
    }
  }
  return -1;
}

int smileRnn_getInputSelection(cRnnNetFile &net, FLOAT_NN **weights)
{
  if (weights == NULL) return 0;

  int j = smileRnn_findWeights(LAYER_INPUT, LAYER_DIR_FWD | LAYER_HIDDEN | 0, net);
  
  // get number of input units
  *weights = (FLOAT_NN *)calloc(1,sizeof(FLOAT_NN)*net.inputSize);

  long nOut = net.wv[j]->nWeights / net.inputSize;

  // sum up input weights
  FLOAT_NN *w = net.wv[j]->weights;

  long i,n;
  for (i=0; i<net.inputSize; i++) {
    for (n=0; n<nOut; n++) {
      (*weights)[i] += fabs( *(w++) );
    }
    (*weights)[i] /= (FLOAT_NN)nOut;
  }

  // output weight vector:
  return net.inputSize;
}

// create a network from a successfully loaded net config file (loadNet function)
int smileRnn_createNet(cRnnNetFile &net, cNnRnn *&rnn, int printConnections)
{
  int i,j,idx=0;
  if (net.loaded) {
    int nlayers = net.nHiddenLayers+2;
    if (net.bidirectional) nlayers += net.nHiddenLayers;
    rnn = new cNnRnn(nlayers); // +1 input, +1 ouput layer

    // input layer...
    SMILE_DBG(2,"net input size: %i",net.inputSize);
    cNnNNlayer *nl = new cNnNNlayer(net.inputSize,0,0,net.nContext);
    cNnTf * tf = new cNnTfIdentity(); // identity at the inputs
    nl->createCells(tf);
    nl->setName("input");
    rnn->addLayer(nl, idx++);


    // hidden layers...
    unsigned long dir = LAYER_DIR_FWD;
    for (i=0; i<net.nHiddenLayers; i++) {
      cNnLayer *l = NULL;
      int recurrent = 0;

      ////////////// forward layers and backward layers (depends on odd/even iteration and net config)

      if (net.hiddenType[i] == NNLAYERTYPE_LSTM) {
        cNnLSTMlayer * _l = smileRnn_createLstmLayer(i,idx,(dir==LAYER_DIR_FWD?0:1),net);
        char *_tmp = myvprint("hidden_lstm_%i_%c",i,(dir==LAYER_DIR_FWD?'f':'b'));
        _l->setName(_tmp);
        free(_tmp);
        //// peephole weights
        j = smileRnn_findPeepWeights(dir | LAYER_HIDDEN | i, net);
        if (j>=0) _l->setPeepWeights(net.wv[j]->weights, net.wv[j]->nWeights);
        l=(cNnLayer*)_l;
        recurrent = 1;
      } else if ((net.hiddenType[i] == NNLAYERTYPE_RNN)||(net.hiddenType[i] == NNLAYERTYPE_NN)) {
        l = new cNnNNlayer(net.hiddenSize[i],idx,(dir==LAYER_DIR_FWD?0:1),net.nContext);
        // TODO... weights, transfer functions, createCells, bias...

        char *_tmp;
        if (net.hiddenType[i] == NNLAYERTYPE_RNN) { 
          recurrent = 1; 
          _tmp = myvprint("hidden_rnn_%i_%c",i,(dir==LAYER_DIR_FWD?'f':'b'));
        } else {
          _tmp = myvprint("hidden_nn_%i_%c",i,(dir==LAYER_DIR_FWD?'f':'b'));
        }
        l->setName(_tmp);
        free(_tmp);
      }

      // add the new layer
      rnn->addLayer(l, idx);
      // connect hidden layer
      rnn->connectTo(idx,1+recurrent);
      // the input connection 
      if ((net.bidirectional)&&(idx>1)) {
        rnn->connectFrom(idx-2,idx); // TODO: consider gather layers...??
      } else {
        rnn->connectFrom(idx-1,idx); // previous layer only
      }
      if (recurrent) {
        // the recurrent connection
        rnn->connectFrom(idx,idx); 
      }

      // initialise this connection
      rnn->initialise(idx);


      //// set weights
      // the bias weights
      j = smileRnn_findWeights(LAYER_BIAS, dir | LAYER_HIDDEN | i, net);
      rnn->setBias(idx,net.wv[j]->weights, net.wv[j]->nWeights);


      // set connection weights
      if (net.bidirectional) {
        // bidirectional layers...
        // input: input layer? ?
        if (idx > 2) {
          // TODO: connect previous gather layer as input to both fwd and rwd layer
          j = smileRnn_findWeights(LAYER_HIDDEN_GATHER | i-1, dir | LAYER_HIDDEN | i, net);
        } else {
          // connect input layer to both fwd and rwd layers
          j = smileRnn_findWeights(LAYER_INPUT, dir | LAYER_HIDDEN | i , net);
        }
      } else if (idx > 1) {
        // previous hidden layer, non-bidirectional  (in the non-bidirectional case this is equivalent to the gather layer)
        j = smileRnn_findWeights(LAYER_HIDDEN_GATHER | i-1, dir | LAYER_HIDDEN | i, net);
      } else {
        // input layer..
        j = smileRnn_findWeights(LAYER_INPUT, dir | LAYER_HIDDEN | i, net);
      }
      rnn->setWeights(0,idx,net.wv[j]->weights, net.wv[j]->nWeights);


      if (recurrent) {
        // the recurrent connection weights
        j = smileRnn_findWeights(dir | LAYER_HIDDEN | i, dir | LAYER_HIDDEN | i, net);
        rnn->setWeights(1,idx,net.wv[j]->weights, net.wv[j]->nWeights);
      }

      idx++; // to next layer...

      if (net.bidirectional) { ///// add the backward layers in the next loop iteration ...
        if (dir == LAYER_DIR_FWD) { /* 0 = FWD */
          i--;
          dir = LAYER_DIR_RWD; /* 1 = reverse */
        } else {
          dir = LAYER_DIR_FWD;
        }
      }

    }

    // output layer... (TODO: other output layer types than regression...)
    SMILE_MSG(2,"net-task: %i",net.task);
    SMILE_DBG(2,"net output size: %i",net.outputSize);
    if (net.task == NNTASK_CLASSIFICATION) {
      cNnSoftmaxLayer * sfl = new cNnSoftmaxLayer(net.outputSize,idx);
      sfl->setName("softmax_output");
      rnn->addLayer(sfl, idx);
    } else if (net.task == NNTASK_TRANSCRIPTION) {
      cNnSoftmaxLayer * sfl = new cNnSoftmaxLayer(net.outputSize,idx);
      sfl->setName("CTCsoftmax_output");
      rnn->addLayer(sfl, idx);
    } else {
      nl = new cNnNNlayer(net.outputSize,idx);
      tf = new cNnTfIdentity(); // default sigmoid unit..? or identity at the inputs??
      nl->createCells(tf);
      nl->setName("output");
      rnn->addLayer(nl, idx);
      //rnn->addLayer(new cNnNNlayer(net.outputSize,idx), idx);
    }

    // connect output layer
    if (net.bidirectional) { // TODO.... memory!
      rnn->connectTo(idx, 2);
      rnn->connectFrom(idx-1, idx);
      rnn->connectFrom(idx-2, idx);
    } else {
      rnn->connectTo(idx, 1);
      rnn->connectFrom(idx-1, idx);
    }

    rnn->initialise(idx);

    j = smileRnn_findWeights(LAYER_BIAS, LAYER_OUTPUT, net);
    rnn->setBias(idx,net.wv[j]->weights, net.wv[j]->nWeights);

    j = smileRnn_findWeights(LAYER_DIR_FWD | LAYER_HIDDEN | (net.nHiddenLayers-1), LAYER_OUTPUT, net);
    //    fprintf(stderr,"setW j=%i  %i  %i\n",j,idx,net.wv[j]->nWeights);
    rnn->setWeights(0,idx,net.wv[j]->weights, net.wv[j]->nWeights);
    if (net.bidirectional) {
      j = smileRnn_findWeights(LAYER_DIR_RWD | LAYER_HIDDEN | (net.nHiddenLayers-1), LAYER_OUTPUT, net);
      rnn->setWeights(1,idx,net.wv[j]->weights, net.wv[j]->nWeights);
    }

    if (printConnections) rnn->printConnections();

    return 1;
  }
  return 0;
}
