#ifndef _MAIN_H
#define _MAIN_H
//#include <GL/glut.h>
#include "glui.h"

#include "Mesh.h"
#include "Etat.h"
#include "Polygon.h"
#include "tools.h"

#define OUTPUT_PATH		"_TMP/"
#define TMP_PATH		"_TMP/saves/"
#define HELP_FILE		"_HELP/README"
#define TRIANGLES_FILE	"_OUTPUT/Triangles"
#define FDP_FILE		"_OUTPUT/FDPs"
#define VERTICES_FILE	"_OUTPUT/Vertices"
// Screen dimensions
int WIDTH=900;
int HEIGHT=800;
#define SIZEOFARRAY	262144	// The number of triangles is almost 10K, so 256K is larger than 9*10K
							// The reason (of the '9') can be found in the
							// glFeedbackBuffer(SIZEOFARRAY, GL_3D, feedbackBuffer) instruction
							// This value has to be larger than the number of data we could put in feedbackBuffer:
							//	n (triangles) * (3x3 (coordinates for each triangle) + 1 (index: indexed with glPassThrough))
							// => SIZEOFARRAY 262144 is enough until 26K triangles
#define PASSTHROUGH_SHIFT	(WIDTH>HEIGHT?WIDTH*WIDTH:HEIGHT*HEIGHT)	// One way to avoid duplication of data in the feedbackBuffer
																		// Example: feedbackBuffer contains
																		//	triangle coordinates such as (10.0, 20.0, 0.0)
																		//	triangle references such as the triangle#10.0 (see specification of: glPassThrough)
																		// In both cases, we have the floating value 10.0, so the shifting value avoid mistakes, by adding this one to the references values
																		// If there are more triangles than this value, change PASSTHROUGH_SHIFT value

#define MIN2(a, b)		(a<b?a:b)
#define MIN3(a, b, c)	(a<b?(a<c?a:c):(b<c?b:c))
#define MAX2(a, b)		(a>b?a:b)
#define MAX3(a, b, c)	(a>b?(a>c?a:c):(b>c?b:c))

#define FACE_INFORMATION		10	// Gives some informations about the face
#define REGION_INFORMATION		11	// Gives some informations about a specific area
#define TRIANGLE_INFORMATION	12	// Gives some informations about a specific triangle
#define NORMALCOLOR_OPTION		1	
#define SYMETRICCOLOR_OPTION	2
#define WRITE_OPTION			3	// All the triangles of a given area are recorded in a specific file
#define OPEN_OPTION				4	// Read regions informations from input files, and initialize the face regions
#define EXIT_OPTION				5	// Just redraw the frame
#define QUIT_OPTION				6	// Exit from the application
#define XML_OPTION				7	// Used to write xml file
#define FDP_OPTION				8	// Used to open fdp file
#define SYSTEM_USING_COLOR		WHITE	// The color used for some special application
										// The user can not use this color
										// We are reserving one color for extra uses


//////////////
////////////// VARIABLES
//////////////

///// GLUI ENVIRONMENT
// These are the live variables passed into GLUI
GLUI	*glui;
// Panel
GLUI_Panel	*panelViewAndSelection;
GLUI_Panel	*panelV;	// VERTEX_MODE / TRIANGLE_MODE
GLUI_Panel	*panelS;	// SELECTION_MODE
GLUI_Panel	*panelSym;	// SYMETRIC_MODE
GLUI_Panel	*panelTranslation;
GLUI_Panel	*panelRotation;
GLUI_Panel	*panelValues;
// Rollout
GLUI_Rollout	*main_rollout;
GLUI_Rollout	*rolloutTranslation;
GLUI_Rollout	*rolloutRotation;
// Checkbox
GLUI_Checkbox	*checkboxVertices;
GLUI_Checkbox	*checkboxTriangles;
GLUI_Checkbox	*checkboxOverwrite;
// Translation
GLUI_Translation	*translationX;
GLUI_Translation	*translationY;
GLUI_Translation	*translationZ;
// Spinner
GLUI_Spinner	*translationX_spinner;
GLUI_Spinner	*translationY_spinner;
GLUI_Spinner	*translationZ_spinner;
GLUI_Spinner	*perspective_spinner;
GLUI_Spinner	*rotationX_spinner;
GLUI_Spinner	*rotationY_spinner;
GLUI_Spinner	*rotationZ_spinner;
// Button
GLUI_Button		*quit;
GLUI_Button		*help;
GLUI_Button		*clearColors;
// Radiogroup
GLUI_RadioGroup	*groupSelectionMode;
GLUI_RadioGroup	*groupSymetricMode;
// ListBox
GLUI_Listbox	*listboxColorNormal;
GLUI_Listbox	*listboxColorSymetric;
GLUI_Listbox	*listboxFDPs;

///// GL ENVIRONMENT
int	main_window;
// Drawing modes
static int RENDER_MODE		=	1;	//	RENDER_MODE==1		=>	draw the scene
static int FEEDBACK_MODE	=	0;	//	FEEDBACK_MODE==1	=>	initialize the feedbackBuffer, and the screenDesc array
static int TRIANGLE_MODE	=	1;	//	TRIANGLE_MODE==1	=>	in RENDER_MODE, draw the triangles
static int VERTEX_MODE		=	0;	//	VERTEX_MODE==1		=>	in RENDER_MODE, draw the vertices
static int MOUSE_MODE		=	1;	//	MOUSE_MODE==1		=>	select the triangles ; ==0 => unselect the triangles
static int SYMETRIC_MODE	=	1;	//	SYMETRIC_MODE==1	=>	use the symetric selection
static int OVERWRITE_MODE	=	0;	//	OVERWRITE_MODE==1	=>	if the user try to color a triangle that already have been colored, so the color of this one change
// The coordinates of "real" vertices are projected on the frame
// see also: GL_FEEDBACK rendering mode
static GLfloat feedbackBuffer		[SIZEOFARRAY];		// WARNING! The value of SIZEOFARRAY is arbitrary. It should be computed from the TRIANGLE_NUMBER value
static int* indexedTriangles;							// 
static int** screenDesc;								// We have to maintain this array in order to specify for each pixel the triangle it is refering to
static float** DEPTH;									// In FEEDBACK mode, we use this array to specify the depth of the triangles displayed
														// So, the screenDesc array only get the closest triangles (those which are visibles)
/// Informations à propos des régions
static Undo	*undo;
// Arrays to update at each changement of triangle color
static int* regionToColor;						// Assign a color to a region
												// Example: if regionToColor[2]==RED, it means that r_cheeck_indices is red colored
static int* colorToRegion;						// Assign a region to a color
												// Example: if colorToRegion[3]==2, it means that the color RED is corresponding to the region r_cheeck_indices
static int** trianglesOfRegion;					// Describe all triangles of a particular region
												// Nb: trianglesOfRegion[i][0] give the length of trianglesOfRegion[i]
/// Informations à propos des FDPs
static int FDPs[FDP_NUMBER];
//static int* FDPs[GROUP_NUMBER];		// FDPs[GROUP_NUMBER][sizeOfGroup(i)]
static SelectedFDP* selectedFDP;

// Viewing modes
// Parameters for the rotation and the scaling of : MODELVIEW & PROJECTION
#define AREA	45
static VECTOR3D* defaultTranslation;
static VECTOR3D* defaultScale;
static GLfloat rotateX=0.0,				projectionAngleX=0.0;
static GLfloat rotateY=0.0,				projectionAngleY=0.0;
static GLfloat rotateZ=0.0,				projectionAngleZ=0.0;
static GLfloat modelviewScaleX=0.01,	projectionScaleX=0.01;
static GLfloat modelviewScaleY=0.01,	projectionScaleY=0.01;
static GLfloat modelviewScaleZ=0.01,	projectionScaleZ=0.01;
// If a triangle is clicked, the following variables should be maintained
//static int selectedTriangle=0,	selectedArea=-1;
// If a particular key is typed, the following variables should specify the translation of the camera
static GLfloat translateX =	0.0;
static GLfloat translateY =	10.0;
static GLfloat translateZ =	0.0;
static GLfloat perspective	= 45.0;
static GLfloat precision	= 100.0;
static int normalColorIndex;
static int symetricColorIndex;
static int normalFileIndex;
static int symetricFileIndex;

static int currentX;	// Gives the current coordinates
static int currentY;	//  of the mouse

//////////////
////////////// FUNCTIONS
//////////////

// Construct the frame
void initPar(void);		// Initialize some parameters
void initGL(void);		// Initialize OpenGL environment
void display(void);
void drawScene(void);
// GLUI events
void helpFunction();
void mouseModeFunction();
void translationXFunction();
void translationYFunction();
void translationZFunction();
void perspectiveFunction();
void rotationXFunction();
void rotationYFunction();
void rotationZFunction();
void clearColorsFunction();
void viewFunction();		// Update the view-checkboxes
void mouseModeFunction();	// Update the mouseMode-radiogroup
void symetricModeFunction();
void normalColorFunction();
void updateNormalColor(int color);
void symetricColorFunction(int color);
void updateSelectionModeFunction();
// Mouse and other events
void myMouseFunc(int button, int state, int x, int y);
void myKeyboardFunc(unsigned char key, int x, int y);
void myResizeFunc(int width, int height);
void myIdleFunc();
void myMotionFunc(int x, int y);
void myPassiveMotionFunc(int x, int y);
void mySpecialFunc(int value, int x, int y);
void myMenuFunc(int index);
void myMenuInformationsFunc(int index);	// Display some informations about the face, a region or a triangle
void myMenuAssignNameFunc(int index);	// Assign a name to a region
void myMenuLoadFileFunc(int index);		// Load a region
void myMenuChangeColorFunc(int newColor);
void mySetColor(int value);
void setColor(int triangle, int color);
void removeTriangle(int triangle);
void assignColorToRegion(int region, int color);
void assignFDPToRegion(int region, int fdp);
void swapSelectedFDP();					// When the user select a FDP, 3 vertices are possible
										//	This function allows the user to select one of theses 3 possibilities
// Other ones
void writeSelectedFDP(int vertex);
void setActiveRegion(int region);
bool analyzeCurrentData();						// Test if all the data that the player needs are specified
bool analyzeFileData();							// Test if all the data that the player needs are specified
int readXML();
int openAllRegions();
int openFDPs();
int writeFDPsToFile(char*);
int writeXML();
int writeTrianglesToFile(int region);
int writeRegionsToFile();				// Used to debug
void synthetizeData();
void synthetizeDataTextures();
int loadFile(int fap);
int regionOf(int triangle);
int colorOf(int triangle);
int* getAllTrianglesInRegion(int region);
int isInTriangle(VECTOR2D *point, VECTOR2D *p1, VECTOR2D *p2, VECTOR2D *p3);
int isInPolygon(int triangle, int* triangles);		// Test if a point is in a polygon (the polygon is described by the triangles it contains)
int* getNeighbor(int triangle);	// Get the 3 triangles that match to the one gave in parameter
								// If there is less than 3, put (-1) to fill the array
// Algorithms
int* getBestWay(int start, int end);				// Is seeking for the best way between 2 triangles	// To be optimize
int* getBestWayExtended(int start, int* end);		// Extension: select the shortest best way from all the possible ways induced by the end vertices
int getNearestSymetricTriangle(int triangle);		// Is seeking for the nearest symetric triangle of an other one	// To be optimize
int getNearestVertex(int x, int y);					// Is seeking for the nearest symetric vertex of an other one	// To be optimize
GLfloat getDistance(int triangle, GLfloat* vertex);	// get the absolute distance between a triangle and a vertex. I mean: the computation is done from the input coordinates files
//int loadAllRegions(char* file);

#endif	// _MAIN_H