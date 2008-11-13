/*
This is an example illustrating the use of the Bayesian Network 
inference utilities found in the dlib C++ library.  In this example
we load a saved Bayesian Network from disk.  
*/

#include "dlib/bayes_utils.h"
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"

//#include "dlib4.dll"

//#include <iostream>
#include <fstream>


using namespace dlib;
using namespace std;
using namespace bayes_node_utils;

#include <stdio.h>
#include <stdlib.h>

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv)
{


	float belief_probability[64][10];
	//float belief_probability[64][10];

	int i;

	for(i=0; i<64; i++)
		for(unsigned int j=0; j<10; j++)
			belief_probability[i][j]=-1.0f;



	FILE *fp;


	try
	{
		// This statement declares a bayesian network called bn.  Note that a bayesian network
		// in the dlib world is just a directed_graph object that contains a special kind 
		// of node called a bayes_node.
		directed_graph<bayes_node>::kernel_1a_c bn;

		if (argc != 2)
		{

			//load: newgaze or newgaze2

			cout << "You must supply a file name on the command line.  The file should "
				<< "contain a serialized Bayesian Network" << endl;
			return 1;	

		}

		ifstream fin(argv[1],ios::binary);

		// Note that the saved networks produced by the bayes_net_gui_ex.cpp example can be deserialized
		// into a network.  So you can make your networks using that GUI if you like.

		cout << "Loading the network from disk..." << endl;
		deserialize(bn, fin);

		cout << "Number of nodes in the network: " << bn.number_of_nodes() << endl;

		// Lets compute some probability values using the loaded network using the join tree (aka. Junction 
		// Tree) algorithm.

		// First we need to create an undirected graph which contains set objects at each node and
		// edge.  This long declaration does the trick.

		typedef graph<set<unsigned long>::compare_1b_c, set<unsigned long>::compare_1b_c>::kernel_1a_c join_tree_type;
		join_tree_type join_tree;

		// Now we need populate the join_tree with data from our bayesian network.  The next to 
		// function calls do this.  Explaining exactly what they do is outside the scope of this
		// example.  Just think of them as filling join_tree with information that is useful 
		// later on for dealing with our bayesian network.  

		create_moral_graph(bn, join_tree);
		create_join_tree(join_tree, join_tree);

		//not used
		bayesian_network_gibbs_sampler sampler;

		// Now we have a proper join_tree we can use it to obtain a solution to our
		// bayesian network.  Doing this is as simple as declaring an instance of
		// the bayesian_network_join_tree object as follows:

		if((fp=fopen("new_gaze", "wb"))==NULL) 
		{
			printf("Cannot open file.\n");
		}

		int counter=0;
		for (unsigned int pgs=0;pgs<2;pgs++)
			for (unsigned int pgl=0;pgl<2;pgl++)
				for (unsigned int ms=0;ms<2;ms++)
					for (unsigned int ml=0;ml<2;ml++)
						for (unsigned int sd=0;sd<2;sd++)
							for (unsigned int ld=0;ld<2;ld++) 
							{
								set_node_value(bn, 0, pgs);
								set_node_as_evidence(bn, 0);

								set_node_value(bn, 1, pgl);
								set_node_as_evidence(bn, 1);							

								//node 2 is an evidence

								set_node_value(bn, 3, ms);
								set_node_as_evidence(bn, 3);

								set_node_value(bn, 4, ml);
								set_node_as_evidence(bn, 4);

								set_node_value(bn, 5, sd);
								set_node_as_evidence(bn, 5);

								set_node_value(bn, 6, ld);
								set_node_as_evidence(bn, 6);

								printf( "Using the join tree algorithm: \n");

								bayesian_network_join_tree solution_with_evidence(bn, join_tree);

								float s1l1 =solution_with_evidence.probability(2)(0);
								float s0l1 = solution_with_evidence.probability(2)(1);
								float s1l0 = solution_with_evidence.probability(2)(2);
								float s0l0 = solution_with_evidence.probability(2)(3);

								printf(" %f \n" , s0l0 );
								printf(" %f \n" , s1l0 );
								printf(" %f \n" , s0l1 );
								printf(" %f \n" , s1l1 );

								//save to table
								belief_probability[counter][0]=(float)pgs;
								belief_probability[counter][1]=(float)pgl;
								belief_probability[counter][2]=(float)ms;
								belief_probability[counter][3]=(float)ml;
								belief_probability[counter][4]=(float)sd;
								belief_probability[counter][5]=(float)ld;
								belief_probability[counter][6]=s1l1;
								belief_probability[counter][7]=s0l1;
								belief_probability[counter][8]=s1l0;
								belief_probability[counter][9]=s0l0;

								counter++;
							}


							if(fwrite(belief_probability, sizeof(float), 640, fp) != 640)
								printf("File write error.");
							fclose(fp);


							//heuristic solution works only with binary nodes
							//node two is not a binary	node

							/*

							set_node_value(bn, 0, 0);
							set_node_as_evidence(bn, 0);

							set_node_value(bn, 1, 0);
							set_node_as_evidence(bn, 1);

							set_node_value(bn, 6, 0);
							set_node_as_evidence(bn, 6);

							set_node_value(bn, 3, 0);
							set_node_as_evidence(bn, 3);

							set_node_value(bn, 4, 0);
							set_node_as_evidence(bn, 4);

							set_node_value(bn, 5, 0);
							set_node_as_evidence(bn, 5);

							// We will leave the C node with a value of 1 and keep it as an evidence node.  

							// First create an instance of the gibbs sampler object
							// The following code illustrates the general procedure.

							// The more times you let the loop run the more accurate the result will be.  Here we loop
							// 2000 times.

							printf ( "Using the approximate Gibbs Sampler algorithm:\n" );

							unsigned long C_count = 0;

							const long rounds = 50;
							for (long i = 0; i < rounds; ++i)
							{
							sampler.sample_graph(bn);

							if (node_value(bn, 2) == 1)
							++C_count;    
							}

							printf ( "Using the approximate Gibbs Sampler algorithm:\n" );
							printf (" %f", (double)C_count/(double)rounds);
							//cout << "p(A=1 | C=1) = " << (double)A_count/(double)rounds << endl;
							//cout << "p(B=1 | C=1) = " << (double)B_count/(double)rounds << endl;
							//cout << "p(C=1 | C=1) = " << (double)C_count/(double)rounds << endl;
							//cout << "p(D=1 | C=1) = " << (double)D_count/(double)rounds << endl;

							*/

	}
	catch (exception& e)
	{
		cout << "exception thrown: " << e.what() << endl;
		return 1;
	}

	//read the results from a file

	for(i=0; i<64; i++)
		for(unsigned int j=0; j<10; j++)
			belief_probability[i][j]=-1.0f;

	if((fp=fopen("new_gaze", "rb"))==NULL) 
	{
		printf("Cannot open file.\n");
	}



	if(fread(belief_probability, sizeof(float), 640, fp) != 640) 
	{
		if(feof(fp))
			printf("Premature end of file.");
		else
			printf("File read error.");
	}
	fclose(fp);


	for(i=0; i<64; i++)
		for(unsigned int j=0; j<10; j++)
			printf("%f , ", belief_probability[i][j]);
}


