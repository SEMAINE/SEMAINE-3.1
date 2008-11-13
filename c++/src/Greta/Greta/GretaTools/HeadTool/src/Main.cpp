#include "XMLGenericParser.h"
#include "Math.h"
#include "Main.h"
#include "string.h"

CMesh* m;	// The face object pointer
			// It contains all the data we are needed to draw the face

//////////MAURIZIO START

#define NUM_OF_TEXTURES 1

typedef struct
{
	char image[256];
	int alpha;
	
} Texture;

Texture images[NUM_OF_TEXTURES]=
{
	{"mario.bmp",0}
};

GLuint texture[NUM_OF_TEXTURES];

AUX_RGBImageRec *LoadBMP(char *Filename)
{
	FILE *File = NULL;

	if( !Filename )
		return NULL;

	File = fopen(Filename,"r");

	if( File )
	{
		fclose(File);
		return auxDIBImageLoad(Filename);
	}

	return NULL;
}

#define MIN_ALPHA_VALUE 128

//********************************************
// AddAlphaLayer
//********************************************
unsigned char* AddAlpha(unsigned char *src, int size) // 0 - 255
{
	// Alloc memory
	unsigned char *pData;
	int i;

	pData= (unsigned char*)malloc(size*4);
	if(pData == NULL)
	{
		return 0;
	}

	for(i=0;i<size;i++)
	{
		pData[4*i+0] = src[3*i+0];
		pData[4*i+1] = src[3*i+1];
		pData[4*i+2] = src[3*i+2];
		if (src[3*i+0]>MIN_ALPHA_VALUE&&
			src[3*i+1]>MIN_ALPHA_VALUE&&
			src[3*i+2]>MIN_ALPHA_VALUE)
				pData[4*i+3] = 0x00;
		else
				pData[4*i+3] = 0xFF;

	}
	
	return pData;
}

void LoadGLTextures()
{
	char path[256];
	int i;

	AUX_RGBImageRec *TextureImage[NUM_OF_TEXTURES];


	memset(TextureImage,0,sizeof(void *)*NUM_OF_TEXTURES);

	for (i=0;i<NUM_OF_TEXTURES;i++)
	{
		memset(path,0,256);
		strcpy(path,"");
		strcat(path,"C:\\code\\face tool\\_INPUT\\textures\\");
		strcat(path,images[i].image);
		// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
		if (TextureImage[i] = LoadBMP(path))
		{
			if (images[i].alpha)
			{
				TextureImage[i]->data=AddAlpha(TextureImage[i]->data,TextureImage[i]->sizeX*TextureImage[i]->sizeY);

				glGenTextures(1, &texture[i]);
				// Typical Texture Generation Using Data From The Bitmap
				glBindTexture(GL_TEXTURE_2D, texture[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, 4, TextureImage[i]->sizeX, TextureImage[i]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage[i]->data);
			}
			else
			{
				glGenTextures(1, &texture[i]);
				// Typical Texture Generation Using Data From The Bitmap
				glBindTexture(GL_TEXTURE_2D, texture[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[i]->data);
			}		

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

		} 

		if (TextureImage[i])
		{
			if (TextureImage[i]->data)
				free(TextureImage[i]->data);
			free(TextureImage[i]);
		}
	}
}

void InitGLTextures()
{

    glEnable(GL_DEPTH_TEST); // enable the z-buffer
    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping

}

//////////MAURIZIO END
/////////////
///////////// GL SCENE CONSTRUCTION
/////////////
int main(int argc,char** argv)
{
	init_fdps();
	glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutInitWindowPosition(100,100);

	main_window=glutCreateWindow("A tool for the face of Greta");

    // The function below are called when the respective event is triggered.
    glutReshapeFunc(myResizeFunc);		// called every time the screen is resized
    glutDisplayFunc(display);			// called when window needs to be redisplayed
    glutIdleFunc(myIdleFunc);			// called whenever the application is idle
    glutKeyboardFunc(myKeyboardFunc);	// called when the application receives a input from the keyboard
    glutMouseFunc(myMouseFunc);			// called when the application receives a input from the mouse
    glutMotionFunc(myMotionFunc);		// called when the mouse moves over the screen with one of this button pressed
	glutPassiveMotionFunc(myPassiveMotionFunc);
    glutSpecialFunc(mySpecialFunc);		// called when a special key is pressed such as SHIFT
	initPar();
	initGL();

	//MAURIZIO
	LoadGLTextures();

	// Creation of the menu
	{
		int sMenuGetInformations=glutCreateMenu(myMenuInformationsFunc);
		glutAddMenuEntry("About the face", FACE_INFORMATION);
		glutAddMenuEntry("About the region", REGION_INFORMATION);
		glutAddMenuEntry("About the triangle", TRIANGLE_INFORMATION);
		//int sMenuAssignName=glutCreateMenu(myMenuAssignNameFunc);
		//for(int i=1;i<REGION_NUMBER;i++)
		//	glutAddMenuEntry(region_names[i], region_indices[i]);
		int sMenuLoadFile=glutCreateMenu(myMenuLoadFileFunc);
		glutAddMenuEntry("Load all regions", 0);
		for(int i=1;i<REGION_NUMBER;i++)
			glutAddMenuEntry(region_names[i], region_indices[i]);
		//int sMenuChangeColor=glutCreateMenu(myMenuChangeColorFunc);
		//glutAddMenuEntry(region_names[0], SKIN);
		//for(int i=1;i<REGION_NUMBER;i++)
		//	if(i!=SYSTEM_USING_COLOR)
		//		glutAddMenuEntry(region_names[i], i);

		int menu;
		menu=glutCreateMenu(myMenuFunc);
		glutAddSubMenu("Get Informations",sMenuGetInformations);
		//glutAddSubMenu("Assign name to region", sMenuAssignName);
		glutAddSubMenu("Load File", sMenuLoadFile);
		//glutAddSubMenu("Change color to", sMenuChangeColor);
		glutAddMenuEntry("Use as normal color", NORMALCOLOR_OPTION);
		glutAddMenuEntry("Use as symetric color", SYMETRICCOLOR_OPTION);
		glutAddMenuEntry("Save this region", WRITE_OPTION);
		glutAddMenuEntry("Open regions", OPEN_OPTION);
		glutAddMenuEntry("Open FDPs", FDP_OPTION);
		glutAddMenuEntry("Save data as XML", XML_OPTION);
		glutAddMenuEntry("Exit", EXIT_OPTION);
		glutAddMenuEntry("Quit", QUIT_OPTION);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}

	/////////////
	///////////// GLUI CODE
	/////////////
	{
		// CREATE THE GLUI
		glui=GLUI_Master.create_glui_subwindow( glutGetWindow(), GLUI_SUBWINDOW_BOTTOM );
		//glui=GLUI_Master.create_glui("GLUI");

		main_rollout=glui->add_rollout("Functions");
		//glui->add_statictext("Drawing Modes");
		// PANEL: SYMETRIC MODE
		panelSym=glui->add_panel_to_panel(main_rollout, "Symetric Mode:", 1);
		// RadioGroup
		groupSymetricMode=glui->add_radiogroup_to_panel(panelSym, &SYMETRIC_MODE, 0, (GLUI_Update_CB) symetricModeFunction);
		glui->add_radiobutton_to_group(groupSymetricMode, "Off");
		glui->add_radiobutton_to_group(groupSymetricMode, "On");
		groupSymetricMode->set_int_val(0);
		glui->add_column_to_panel(panelSym, 1);
		listboxColorNormal=glui->add_listbox_to_panel(panelSym, "Selection Color:", &normalColorIndex, 0, (GLUI_Update_CB) normalColorFunction);
		listboxColorSymetric=glui->add_listbox_to_panel(panelSym, "Symetric Color:", &symetricColorIndex);
		// Adding the colors to the listbox
		// If the listbox is changed, Colors.h and CMesh::setColor(int triangle, int colorIndex) have to be updated
		for(int i=0;i<REGION_NUMBER;i++)
			if(i!=SYSTEM_USING_COLOR)
				listboxColorNormal->add_item(i, region_names[i]);
		//listboxColorNormal->set_int_val(RED);		// set RED as default color for normal selection
		// Adding the colors to the listbox
		// If the listbox is changed, Colors.h and CMesh::setColor(int triangle, int colorIndex) have to be updated
		for(i=0;i<REGION_NUMBER;i++)
			if(i!=SYSTEM_USING_COLOR)
				listboxColorSymetric->add_item(i, region_names[i]);
		//listboxColorSymetric->set_int_val(MAGENTA);	// set MAGENTA as default color for symetric selection

		// PANEL: VIEW_AND_SELECTION
		panelViewAndSelection=glui->add_panel_to_panel(main_rollout, "View:", 1);
		// PANEL: VIEW
		panelV=glui->add_panel_to_panel(panelViewAndSelection, "Type:", 0);
		// Checkbox
		checkboxVertices	=	glui->add_checkbox_to_panel(panelV,"Vertices",&VERTEX_MODE);
		checkboxTriangles	=	glui->add_checkbox_to_panel(panelV,"Triangles",&TRIANGLE_MODE);
		// RadioGroup
		groupSelectionMode=glui->add_radiogroup_to_panel(panelV, &MOUSE_MODE);
		groupSelectionMode=glui->add_radiogroup_to_panel(panelV, &MOUSE_MODE, 0, (GLUI_Update_CB) mouseModeFunction);
		glui->add_radiobutton_to_group(groupSelectionMode, "Delete");
		glui->add_radiobutton_to_group(groupSelectionMode, "Select");
		groupSelectionMode->set_int_val(1);
		glui->add_column_to_panel(panelViewAndSelection, 0);

		// PANEL: TRANSLATION
		panelTranslation	=	glui->add_panel_to_panel(panelViewAndSelection, "Translation", 0);
		// Translation
		// Integer and float area
		translationX=glui->add_translation_to_panel(panelTranslation, "Translate X:",	GLUI_TRANSLATION_X, &translateX, 0, (GLUI_Update_CB) translationXFunction);
		glui->add_column_to_panel(panelTranslation,0);
		translationY=glui->add_translation_to_panel(panelTranslation, "Translate Y:",	GLUI_TRANSLATION_Y, &translateY, 0, (GLUI_Update_CB) translationYFunction);
		glui->add_column_to_panel(panelTranslation,0);
		translationZ=glui->add_translation_to_panel(panelTranslation, "Translate Z:",	GLUI_TRANSLATION_Z, &translateZ, 0, (GLUI_Update_CB) translationZFunction);

		glui->add_column_to_panel(main_rollout, 1);
		// PANEL: VALUES
		panelValues=glui->add_panel_to_panel(main_rollout, "Values", 1);
		// ROLLOUT: TRANSLATION
		rolloutTranslation	=	glui->add_rollout_to_panel(panelValues, "Translation", 0);
		translationX_spinner=glui->add_spinner_to_panel(rolloutTranslation, "Horizontal translation:",	GLUI_SPINNER_FLOAT, &translateX, 0, (GLUI_Update_CB) translationXFunction);
		translationY_spinner=glui->add_spinner_to_panel(rolloutTranslation, "Vertical translation:",	GLUI_SPINNER_FLOAT, &translateY, 0, (GLUI_Update_CB) translationYFunction);
		translationZ_spinner=glui->add_spinner_to_panel(rolloutTranslation, "Depth translation:",		GLUI_SPINNER_FLOAT, &translateZ, 0, (GLUI_Update_CB) translationZFunction);
		perspective_spinner=glui->add_spinner_to_panel(rolloutTranslation,  "Perspective angle:",		GLUI_SPINNER_FLOAT, &perspective, 0, (GLUI_Update_CB) perspectiveFunction);
		// ROLLOUT: ROTATION
		rolloutRotation		=	glui->add_rollout_to_panel(panelValues, "Rotation:", 0);
		rotationX_spinner=glui->add_spinner_to_panel(rolloutRotation, "X-Rotation:",GLUI_SPINNER_FLOAT, &rotateX, 0, (GLUI_Update_CB) rotationXFunction);
		rotationY_spinner=glui->add_spinner_to_panel(rolloutRotation, "Y-Rotation:",GLUI_SPINNER_FLOAT, &rotateY, 0, (GLUI_Update_CB) rotationYFunction);
		rotationZ_spinner=glui->add_spinner_to_panel(rolloutRotation, "Z-Rotation:",GLUI_SPINNER_FLOAT, &rotateZ, 0, (GLUI_Update_CB) rotationZFunction);

		// BUTTON: QUIT
		glui->add_column_to_panel(main_rollout);
		quit=glui->add_button_to_panel(main_rollout, "Quit", 0, (GLUI_Update_CB) exit);
		clearColors=glui->add_button_to_panel(main_rollout, "Clear All", 0, (GLUI_Update_CB) clearColorsFunction);
		// BUTTON: HELP
		help=glui->add_button_to_panel(main_rollout, "Help", 0, (GLUI_Update_CB) helpFunction);

		// LISTBOX: ACTIVE FDPS
		listboxFDPs=glui->add_listbox_to_panel(main_rollout, "FDPs");

		// CHECKBOX:
		checkboxOverwrite=glui->add_checkbox_to_panel(main_rollout, "Overwrite mode", &OVERWRITE_MODE);
		checkboxOverwrite->set_int_val(OVERWRITE_MODE);

		// ALLOCATE THE GLUI TO THE WINDOW
		glui->set_main_gfx_window(main_window);
	}
	// END OF GLUI CODE

	glutMainLoop();
	return 0;
}

void initGL(void)
{
	// Translation
	defaultTranslation->x=-(m->max->x + m->min->x)/2;
	defaultTranslation->y=-(m->max->y + m->min->y)/2;
	defaultTranslation->z=-(m->max->z + m->min->z)/2;
	// Scale
	//defaultScale->x=AREA / abs(m->max->x - m->min->x);
	//defaultScale->y=AREA / abs(m->max->y - m->min->y);
	//defaultScale->z=AREA / abs(m->max->z - m->min->z);

	GLfloat	ambientProperties[]  = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat positionProperties[] = {0.0f, 0.0f, -10.0f, 0.0f};

	glDrawBuffer(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv(GL_LIGHT0, GL_POSITION, positionProperties);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	//glShadeModel(GL_FLAT);
	glEnable(GL_CULL_FACE);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientProperties);
}

void initPar(void)
{
	int i;
	m=new CMesh();
	undo = new Undo();
	defaultTranslation = new VECTOR3D();
	defaultScale = new VECTOR3D();

	// Initialize the ARRAYS MEMORY
	// regionToColor[FAP_NUMBER]
	if(!(regionToColor=(int*)malloc(REGION_NUMBER*sizeof(int))))
	{
		printf("ERROR: malloc failed on regionToColor\n");
		MessageBox(NULL, "ERROR: malloc failed on regionToColor", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	// colorToRegion[COLOR_NUMBER]
	if(!(colorToRegion=(int*)malloc(COLOR_NUMBER*sizeof(int))))
	{
		printf("ERROR: malloc failed on colorToRegion\n");
		MessageBox(NULL, "ERROR: malloc failed on colorToRegion", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	// trianglesOfRegion[FAP_NUMBER][m->TRIANGLE_NUMBER]
	if(!(trianglesOfRegion=(int**)malloc(REGION_NUMBER*sizeof(int*))))
	{
		printf("ERROR: malloc failed on trianglesOfRegion\n");
		MessageBox(NULL, "ERROR: malloc failed on trianglesOfRegion", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=1;i<=REGION_NUMBER;i++)
		if(!(trianglesOfRegion[i]=(int*)malloc(m->TRIANGLE_NUMBER*sizeof(int))))
		{
			printf("ERROR: malloc failed on trianglesOfRegion[%i]\n", i);
			MessageBox(NULL, "ERROR: malloc failed on trianglesOfRegion", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	// screenDesc[WIDTH][HEIGHT]
	if(!(screenDesc=(int**)malloc(WIDTH*sizeof(int*))))
	{
		printf("ERROR: malloc failed on screenDesc\n");
		MessageBox(NULL, "ERROR: malloc failed on screenDesc", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<WIDTH;i++)
		if(!(screenDesc[i]=(int*)malloc(HEIGHT*sizeof(int))))
		{
			printf("ERROR: malloc failed on screenDesc[%i]\n", i);
		MessageBox(NULL, "ERROR: malloc failed on screenDesc", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	// DEPTH[WIDTH][HEIGHT]
	if(!(DEPTH=(float**)malloc(WIDTH*sizeof(float*))))
	{
		printf("ERROR: malloc failed on DEPTH\n");
		MessageBox(NULL, "ERROR: malloc failed on DEPTH", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<WIDTH;i++)
		if(!(DEPTH[i]=(float*)malloc(HEIGHT*sizeof(float))))
		{
			printf("ERROR: malloc failed on DEPTH[%i]\n", i);
		MessageBox(NULL, "ERROR: malloc failed on DEPTH", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	// Initialize the ARRAYS DATA
	for(i=0;i<REGION_NUMBER;i++)
		regionToColor[i]=i;		// -1 is used to specified an unaffected region
	for(i=0;i<COLOR_NUMBER;i++)
		if(i>REGION_NUMBER)
			colorToRegion[i]=-1;		// -1 is used to specified an unaffected color
		else
			colorToRegion[i]=i;
	for(i=1;i<REGION_NUMBER;i++)
		trianglesOfRegion[i][0]=1;	// The size of the array
	for(i=0;i<FDP_NUMBER;i++)
		FDPs[i]=-1;					// -1 is used to specified an unaffected FDP

}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawScene();
	glFlush();
	glutSwapBuffers();
}
void drawScene(void)
{
	// Objects that are related to the face
	GLfloat** vertices=m->vertices;
	GLfloat** normals=m->normals;
	GLint** triangles=m->triangles;
	GLfloat** texture_vertices=m->texture_vertices;
	GLint** texture_triangles=m->texture_triangles;
	int* colors=m->colors;
	int TRIANGLE_NUMBER=m->TRIANGLE_NUMBER;
	int VERTEX_NUMBER=m->VERTEX_NUMBER;
	int i;

	// Load the matrix in projection view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(perspective, (GLdouble)WIDTH/(GLdouble)HEIGHT, 0.01, 10.0);
	glTranslatef(	translateX/100,
					translateY/100,
					translateZ/100 - 1
				);

	// Load the matrix in model view
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(	modelviewScaleX,
				modelviewScaleY,
				modelviewScaleZ
			);
	glRotated(rotateX, 1.0, 0.0, 0.0);
	glRotated(rotateY, 0.0, 1.0, 0.0);
	glRotated(rotateZ, 0.0, 0.0, 1.0);

	///////////// 
	///////////// IN RENDER MODE
	/////////////
	glScalef(6,6,6);
	if(RENDER_MODE && TRIANGLE_MODE)
	{
		GLfloat*	normal;
		GLint		*triangle,*texture_triangle;
		GLfloat		*vertex_0, *vertex_1, *vertex_2;
		GLfloat		*normal_0, *normal_1, *normal_2;
		GLfloat		*texture_vertex_0, *texture_vertex_1, *texture_vertex_2;
//#define TEXTURES

#ifdef TEXTURES
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
#endif
		for(i=0;i<TRIANGLE_NUMBER;i++)
		{
			normal		=	normals[i];		// in order to optimize the access to the memory
#ifdef TEXTURES
			texture_triangle  =   texture_triangles[i];
			texture_vertex_0	=	texture_vertices[texture_triangle[0]];
			texture_vertex_1	=	texture_vertices[texture_triangle[1]];
			texture_vertex_2	=	texture_vertices[texture_triangle[2]];
		/*	printf("tr %i v1 x:%f y:%f v2 x:%f y:%f v3 x:%f y:%f\n",
				i,texture_vertex_0[0],texture_vertex_0[1],
				texture_vertex_1[0],texture_vertex_1[1],
				texture_vertex_2[0],texture_vertex_2[1]);
		*/		
#endif
			triangle	=	triangles[i];	// in order to optimize the access to the memory
			vertex_0	=	vertices[triangle[0]];
			vertex_1	=	vertices[triangle[1]];
			vertex_2	=	vertices[triangle[2]];
			normal_0	=	normals[triangle[0]];
			normal_1	=	normals[triangle[1]];
			normal_2	=	normals[triangle[2]];
			glBegin(GL_TRIANGLES);
				mySetColor(colors[i]);
				//glColor3f(1,0,0);
				glNormal3f(	normal_0[0],
							normal_0[1],
							normal_0[2]
							);
#ifdef TEXTURES
				glTexCoord2f(texture_vertex_0[0],
							texture_vertex_0[1]
							);
#endif
				glVertex3d(	vertex_0[0]+defaultTranslation->x,
							vertex_0[1]+defaultTranslation->y,
							vertex_0[2]+defaultTranslation->z
							);

				glNormal3f(	normal_1[0],
							normal_1[1],
							normal_1[2]
							);
#ifdef TEXTURES
				glTexCoord2f(texture_vertex_1[0],
							texture_vertex_1[1]
							);
#endif
				glVertex3d(	vertex_1[0]+defaultTranslation->x,
							vertex_1[1]+defaultTranslation->y,
							vertex_1[2]+defaultTranslation->z
							);
				
				glNormal3f(	normal_2[0],
							normal_2[1],
							normal_2[2]
							);
#ifdef TEXTURES
				glTexCoord2f(texture_vertex_2[0],
							texture_vertex_2[1]
							);
#endif
				glVertex3d(	vertex_2[0]+defaultTranslation->x,
							vertex_2[1]+defaultTranslation->y,
							vertex_2[2]+defaultTranslation->z
							);
			glEnd();
		}
	}
	if(RENDER_MODE && VERTEX_MODE)
	{
		int color=listboxColorNormal->get_int_val();
		int region=colorToRegion[color];
		int group=groupOfRegion(region);
		int indice=listboxFDPs->get_int_val();

		GLfloat *vertex;
		for(i=0;i<VERTEX_NUMBER;i++)
		{
			glPushMatrix();
			mySetColor(RED);

			vertex=vertices[i];
			glTranslatef(	vertex[0]+defaultTranslation->x,
							vertex[1]+defaultTranslation->y,
							vertex[2]+defaultTranslation->z
						);
			glutSolidCube(m->EPSILON/2);
			glPopMatrix();
			
			// On vérifie si FDPs[i] correspond à un FDP
			if(i<FDP_NUMBER)
				if(FDPs[i]>=0)
				{
					glPushMatrix();
					//printf("bo %d %d\n",fdp_value(group,indice-1),FDPs[i]);
					if(FDPs[fdp_value(group,indice-1)]==FDPs[i])
					glColor4f(1,1,1,1);
					else
					mySetColor(GREEN);

					GLfloat* vertex2=vertices[FDPs[i]];
					glTranslatef(	vertex2[0]+defaultTranslation->x,
									vertex2[1]+defaultTranslation->y,
									vertex2[2]+defaultTranslation->z);
					if(FDPs[fdp_value(group,indice-1)]==FDPs[i])
					glutSolidCube(m->EPSILON*2);
					else
					glutSolidCube(m->EPSILON/2);
					
					glPopMatrix();
				}
		}
	}

#ifndef TEXTURES
	glDisable(GL_TEXTURE_2D);
#endif
	// END OF RENDER MODE

	///////////// 
	///////////// IN FEEDBACK MODE
	///////////// 
	if(FEEDBACK_MODE)
	{
		///
		/// Initialisation du buffer
		///
		glFeedbackBuffer(SIZEOFARRAY, GL_3D, feedbackBuffer);
		(void) glRenderMode(GL_FEEDBACK);
		for(i=0;i<TRIANGLE_NUMBER;i++)
		{
			GLfloat* normal=normals[i];		// in order to optimize the access to the memory
			GLint* triangle=triangles[i];	// in order to optimize the access to the memory
			glPassThrough((GLint)(i+PASSTHROUGH_SHIFT));	// to avoid some mistakes between the coordinates and the passThrough values :
															// sometimes, a coordinate can take the value 2.0, the same as the second passThrough
															// "PASSTHROUGH_SHIFT" is just enough to go outside of the range of possible values for coordinates
			glBegin(GL_TRIANGLES);
				//glNormal3d(normal[0],normal[1],normal[2]);
				//glNormal3f(	normal_0[0],
				//			normal_0[1],
				//			normal_0[2]
				//			);
				glVertex3d(	vertices[triangle[0]][0]+defaultTranslation->x,
							vertices[triangle[0]][1]+defaultTranslation->y,
							vertices[triangle[0]][2]+defaultTranslation->z
							);
				glVertex3d(	vertices[triangle[1]][0]+defaultTranslation->x,
							vertices[triangle[1]][1]+defaultTranslation->y,
							vertices[triangle[1]][2]+defaultTranslation->z
							);
				glVertex3d(	vertices[triangle[2]][0]+defaultTranslation->x,
							vertices[triangle[2]][1]+defaultTranslation->y,
							vertices[triangle[2]][2]+defaultTranslation->z
							);
			glEnd();
		}
		int size=glRenderMode(GL_RENDER);	// the size of GL_FEEDBACK array (feedbackBuffer)

		///
		/// Initialisation de la table DEPTH
		///
		for(i=0;i<WIDTH;i++)
			for(int j=0;j<HEIGHT;j++)
				DEPTH[i][j]=1;

		///
		/// Initialisation de l'index indexedTriangles
		///
		if(!(indexedTriangles=(int*)malloc(TRIANGLE_NUMBER*sizeof(int))))
		{
			printf("ERROR: malloc failed on indexedTriangles\n");
		MessageBox(NULL, "ERROR: malloc failed on indexedTriangles", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
		for(i=0;i<size;i++)
			if(feedbackBuffer[i]>=PASSTHROUGH_SHIFT)
				indexedTriangles[((int)feedbackBuffer[i])-PASSTHROUGH_SHIFT]=i+3;

		///
		/// Initialisation de la table screenDesc
		///
		int actualTriangle;
		for(i=0;i<size;)
		{
			if(feedbackBuffer[i]>=PASSTHROUGH_SHIFT && feedbackBuffer[i+1]==1795.0)	// See: specifications of glRender(GL_FEEDBACK) buffer
			{
				VECTOR2D p, p1, p2, p3;
				actualTriangle=feedbackBuffer[i++]-PASSTHROUGH_SHIFT;
				i++;i++;
				GLfloat x1=feedbackBuffer[i++];
				GLfloat y1=feedbackBuffer[i++];
				GLfloat z1=feedbackBuffer[i++];
				GLfloat x2=feedbackBuffer[i++];
				GLfloat y2=feedbackBuffer[i++];
				GLfloat z2=feedbackBuffer[i++];
				GLfloat x3=feedbackBuffer[i++];
				GLfloat y3=feedbackBuffer[i++];
				GLfloat z3=feedbackBuffer[i++];

				GLfloat minX=MIN3(x1, x2, x3);
				GLfloat minY=MIN3(y1, y2, y3);
				GLfloat minZ=MIN3(z1, z2, z3);
				GLfloat maxX=MAX3(x1, x2, x3);
				GLfloat maxY=MAX3(y1, y2, y3);
				GLfloat maxZ=MAX3(z1, z2, z3);

				p1.x=x1;
				p1.y=y1;
				p2.x=x2;
				p2.y=y2;
				p3.x=x3;
				p3.y=y3;

				for(int w=minX;w<maxX;w++)
				{
					p.x=w;
					for(int h=minY;h<maxY;h++)
					{
						p.y=h;
						if(isInTriangle(&p, &p1, &p2, &p3) && (minZ<(float)DEPTH[w][h]))
						{
							screenDesc[w][h]=actualTriangle;
							DEPTH[w][h]=minZ;
						}
					}
				}
			}
			else i++;
		}
		glRenderMode(GL_RENDER);
	}
	// END OF FEEDBACK MODE
}


/////////////
///////////// CALLBACKS
/////////////
// GLUI related functions
void translationXFunction()
{
	float value=translateX;
	translationX_spinner->set_float_val(value);
}
void translationYFunction()
{
	float value=translateY;
	translationY_spinner->set_float_val(value);
}
void translationZFunction()
{
	float value=translateZ;
	translationZ_spinner->set_float_val(value);
}
void perspectiveFunction()
{
	perspective_spinner->set_float_val(perspective);
}
void rotationXFunction()
{
	rotationX_spinner->set_float_val(rotateX);
}
void rotationYFunction()
{
	rotationY_spinner->set_float_val(rotateY);
}
void rotationZFunction()
{
	rotationZ_spinner->set_float_val(rotateZ);
}
void clearColorsFunction()
{
	m->init_color();

	// Updating regionToColor
	//for(int i=1;i<REGION_NUMBER;i++)	// plus besoin
	//	regionToColor[i]=-1;			// si les couleurs sont automatiquement
	// Updating colorToRegion			// affectees aux regions
	//for(int i=0;i<COLOR_NUMBER;i++)	// 
	//	colorToRegion[i]=-1;			// 
	// Updating REGION_NUMBER
	for(int i=1;i<REGION_NUMBER;i++)
		// Use the same kind of cleaning as a quick formating :
		//	it doesnt remove the data, just the index of them
		trianglesOfRegion[i][0]=1;
	for(i=0;i<FDP_NUMBER;i++)
		FDPs[i]=-1;
		
}
void viewFunction()
{
	if(TRIANGLE_MODE)
		checkboxTriangles->draw_checked();
	else
		checkboxTriangles->draw_unchecked();

	if(VERTEX_MODE)
		checkboxVertices->draw_checked();
	else
		checkboxVertices->draw_unchecked();
}
void symetricModeFunction()
{
	groupSymetricMode->set_int_val(SYMETRIC_MODE);
	if(SYMETRIC_MODE)
		listboxColorSymetric->enable();
	else
		listboxColorSymetric->disable();
}
void normalColorFunction()
{
	updateNormalColor(normalColorIndex);
}
void updateNormalColor(int color)
{
	normalColorIndex=color;
	listboxColorNormal->set_int_val(color);
	int region=colorToRegion[color];

	// Mise à jour de la liste des FDPs pour la région active
	int group=groupOfRegion(region);
	char** fdps=getFDPs(group);
	int i;
	for(i=0;i<MAX_ITEMS;i++)
        listboxFDPs->delete_item(i);
	char** names=getFDPs(group);
	for(i=1;i<=sizeOfGroup(group);i++)
		listboxFDPs->add_item(i, names[i]);
}
void symetricColorFunction(int color)
{
	symetricColorIndex=color;
	listboxColorSymetric->set_int_val(color);
	int region=colorToRegion[color];
}
void updateSelectionModeFunction()
{
	groupSelectionMode->set_int_val(MOUSE_MODE);
}
/// EVENTS
// Mouse events
int triangleClicked=-1;
int triangleClickedSymetric=-1;
int Xmove;
int Ymove;
int ACTIVE_BUTTON;
// Help events
void helpFunction()
{
	char* path=HELP_FILE;
	// Ouverture du fichier de commentaires
	FILE* f;
	if(!(f=fopen(path, "r")))
	{
		MessageBox(NULL, "ERROR: Cant read README file", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
	// Lecture du nombre de caracteres du fichier
	int count=0;
	char c;
	while((c=fgetc(f))!=EOF)
		count++;
	fclose(f);
	// Allocation memoire pour "message"
	char* message;
	message=(char*)malloc((count+1)*sizeof(char));
	// Ecriture du fichier dans la chaine "message"
	if(!(f=fopen(path, "r")))
	{
		MessageBox(NULL, "ERROR: Cant read README file", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
	for(int i=0;i<count;i++)
		message[i]=fgetc(f);
	message[count]='\0';
	fclose(f);
	// Affichage du message
	MessageBox(NULL, message, "README", MB_OK);
}
void mouseModeFunction()
{
	if(MOUSE_MODE)
		// so, we are selecting triangles
	{
		OVERWRITE_MODE = 0;
	}
	else
		// so, we are deleting triangles
	{
		OVERWRITE_MODE = 1;
	}
	checkboxOverwrite->set_int_val(OVERWRITE_MODE);
}
void myMouseFunc(int button, int state, int x, int y)
{
	ACTIVE_BUTTON = button;
	switch(state)
	{
		case GLUT_DOWN:
				// The screen descriptor is computed here
				// in order to avoid huge computation when it is not needed
				FEEDBACK_MODE=1;
				display();			// 
				FEEDBACK_MODE=0;	// Now that the screen descriptor is updated, we dont need the FEEDBACK_MODE
				break;
		case GLUT_UP:
				break;
		default:;
	}

	if(!(x && y && x<WIDTH && y<HEIGHT))
		return;
	int triangleSelected=screenDesc[x][HEIGHT-y];

	switch(button)
	{
		case GLUT_LEFT_BUTTON:
			// First case:	the user is selecting vertices (FDP)
			if(state==GLUT_DOWN && VERTEX_MODE)
			{
				int nearestVertex=getNearestVertex(x, y);
				writeSelectedFDP(nearestVertex);
				float* vertex=m->vertices[nearestVertex];
				printf("Vertex selected: %f %f %f\n", vertex[0], vertex[1], vertex[2]);
			}
			// Second case:	the user is selecting triangles (regions of influence)
			else if(state==GLUT_DOWN && TRIANGLE_MODE)
			{
				if((triangleClicked=triangleSelected)>=0)	// if triangle==-1, so we are out of the face
				{
					int colorTmp = m->colors[triangleClicked];
					int oldRegion = colorToRegion[colorTmp];
					undo->add(triangleClicked, oldRegion, normalColorIndex);
					setColor(triangleClicked, normalColorIndex);
				}
				if(SYMETRIC_MODE)
					if((triangleClickedSymetric=getNearestSymetricTriangle(triangleClicked))>=0)	// if triangle==-1, so we are out of the face
						setColor(triangleClickedSymetric, symetricColorIndex);
			}
			break;
		case GLUT_MIDDLE_BUTTON:
				Xmove = x;
				Ymove = y;
				break;
		case GLUT_RIGHT_BUTTON:
				// Already used by the CallbackMenu
				break;
		default:;
	}
	display();
}

// Keyboard events
void myKeyboardFunc(unsigned char key, int x, int y)
{
	switch(key)
	{
		case '/':
					break;
		case '*':
					break;
		case '-':	RENDER_MODE=1;
					perspective*=1+(.1*perspective/45);
					perspectiveFunction();
					break;
		case '+':	RENDER_MODE=1;
					perspective/=1+(.1*perspective/45);
					perspectiveFunction();
					break;
		case '0':	readXML();
					break;
		case '1':
					break;
		case '2':
					break;
		case '3':
					break;
		case '4':
					break;
		case '5':
					break;
		case '6':
					break;
		case '7':
					break;
		case '8':
					break;
		case '9':
					break;
		case 'a':	synthetizeDataTextures();
					break;
		case 'A':	
					break;
		case 'b':	
					break;
		case 'c':
					break;
		case 'd':	
					break;
		case 'e':	
					break;
		case 'f':	
					break;
		case 'g':	
					break;
		case 'h':	
					break;
		case 'i':	
					break;
		case 'j':	
					break;
		case 'k':	
					break;
		case 'l':	
					break;
		case 'm':
					break;
		case 'n':	
					break;
		case 'o':	
					break;
		case 'p':	
					break;
		case 'q':	exit(0);
					break;
		case 'r':	
					break;
		case 's':	SYMETRIC_MODE=!SYMETRIC_MODE;
					symetricModeFunction();
					break;
		case 't':	TRIANGLE_MODE=1;
					VERTEX_MODE=0;
					viewFunction();
					break;
		case 'T':	TRIANGLE_MODE=1;
					viewFunction();
					break;
		case 'u':	
					break;
		case 'v':	VERTEX_MODE=1;
					TRIANGLE_MODE=0;
					viewFunction();
					break;
		case 'V':	VERTEX_MODE=1;
					viewFunction();
					break;
		case 'w':	{
						char* path=(char*)malloc(256*sizeof(char));
						getPath(path, "*");
						writeFDPsToFile(path);
						break;
					}
		case 'x':
					{
						writeRegionsToFile();
						int region=colorToRegion[normalColorIndex];
						int size=trianglesOfRegion[region][0];
						VECTOR3D* v;
						if(!(v=(VECTOR3D*)malloc((size-1)*sizeof(VECTOR3D))))
						{
							printf("ERROR: malloc failed on v\n");
							MessageBox(NULL, "ERROR: malloc failed on v", "Access error", MB_OK|MB_ICONERROR);
							exit(2);
						}
						for(int i=0;i<size-1;i++)	// why "-1" -> see how trianglesOfRegion[i][0] is defined
						{
							int triangle=trianglesOfRegion[region][i+1];
							v[i]=getBarycentre3D(m, triangle);
						}

						HullPolygon* HP=new HullPolygon(v, size-1);
						for(i=0;i<m->TRIANGLE_NUMBER;i++)
							if(HP->contain(getBarycentre3D(m, i)))
								setColor(i, normalColorIndex);
					}
					break;
		case 'y':
					{	writeRegionsToFile();
						int region=colorToRegion[symetricColorIndex];
						int size=trianglesOfRegion[region][0];
						VECTOR3D* v;
						if(!(v=(VECTOR3D*)malloc((size-1)*sizeof(VECTOR3D))))
						{
							printf("ERROR: malloc failed on v\n");
							MessageBox(NULL, "ERROR: malloc failed on v", "Access error", MB_OK|MB_ICONERROR);
							exit(2);
						}
						for(int i=0;i<size-1;i++)	// why "-1" -> see how trianglesOfRegion[i][0] is defined
						{
							int triangle=trianglesOfRegion[region][i+1];
							v[i]=getBarycentre3D(m, triangle);
						}

						HullPolygon* HP=new HullPolygon(v, size-1);
						for(i=0;i<m->TRIANGLE_NUMBER;i++)
							if(HP->contain(getBarycentre3D(m, i)))
								setColor(i, symetricColorIndex);
					}
					break;
		case 'z':
					undo->undo();
					if(!undo->error())
						setColor(undo->triangle(), undo->oldRegion());
					break;
		default:;
	}
	display();
}
// Idle event
void myIdleFunc()
{
}
// Motion events
void myMotionFunc(int x, int y)
{
	if(ACTIVE_BUTTON == GLUT_LEFT_BUTTON)
	{
		if(!(x && y && x<WIDTH && y<HEIGHT))
			return;
		int triangle, color;
		{
			if(MOUSE_MODE)
				color=normalColorIndex;
			else
				color=SKIN;
			if((triangle=screenDesc[x][HEIGHT-y])>=0)	// if triangle==0, so we are out of the face
			{
				int colorTmp = m->colors[triangle];
				int oldRegion = (colorTmp>=0?colorToRegion[colorTmp]:0);
				undo->add(triangle, oldRegion, color);
				setColor(triangle, color);
			}
		}
		if(SYMETRIC_MODE && ((triangle=screenDesc[x][HEIGHT-y])>=0))
		{
			if(MOUSE_MODE)
				color=symetricColorIndex;
			else
				color=SKIN;
			if((triangle=getNearestSymetricTriangle(triangle))>=0)
				setColor(triangle, color);
		}
	}
	if(ACTIVE_BUTTON == GLUT_MIDDLE_BUTTON)
	{
		rotateX+=Xmove - x;
		rotationXFunction();
		rotateY+=Ymove - y;
		rotationYFunction();
		Xmove = x;
		Ymove = y;
	}
	display();
}
// Passive Motion events
void myPassiveMotionFunc(int x, int y)
{
	if(!(x && y && x<WIDTH && y<HEIGHT))
		return;
	currentX=x;
	currentY=y;
}
// Resizment events
void myResizeFunc(int width, int height)
{
	WIDTH=width;
	HEIGHT=height;
	// To update:
	//	screenDesc
	//	DEPTH
	free(screenDesc);
	free(DEPTH);
	if(!(screenDesc=(int**)malloc(WIDTH*sizeof(int*))))
	{
		printf("ERROR: malloc failed on screenDesc\n");
		MessageBox(NULL, "ERROR: malloc failed on screenDesc", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(int i=0;i<WIDTH;i++)
		if(!(screenDesc[i]=(int*)malloc(HEIGHT*sizeof(int))))
		{
			printf("ERROR: malloc failed on screenDesc[%i]\n", i);
		MessageBox(NULL, "ERROR: malloc failed on screenDesc", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
	if(!(DEPTH=(float**)malloc(WIDTH*sizeof(float*))))
	{
		printf("ERROR: malloc failed on DEPTH\n");
		MessageBox(NULL, "ERROR: malloc failed on DEPTH", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(i=0;i<WIDTH;i++)
		if(!(DEPTH[i]=(float*)malloc(HEIGHT*sizeof(float))))
		{
			printf("ERROR: malloc failed on DEPTH[%i]\n", i);
		MessageBox(NULL, "ERROR: malloc failed on DEPTH", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
}
// Special events
void mySpecialFunc(int value, int x, int y)
{
	switch(value)
	{
		case GLUT_KEY_LEFT:
				rotateY-=5.0;//*perspective/45;
				rotationYFunction();
				break;
		case GLUT_KEY_UP:
				rotateX-=5.0;//*perspective/45;
				rotationXFunction();
				break;
		case GLUT_KEY_RIGHT:
				rotateY+=5.0;//*perspective/45;
				rotationYFunction();
				break;
		case GLUT_KEY_DOWN:
				rotateX+=5.0;//*perspective/45;
				rotationXFunction();
				break;
		case GLUT_KEY_F1:
				MOUSE_MODE=!MOUSE_MODE;
				updateSelectionModeFunction();
				break;
		case GLUT_KEY_PAGE_DOWN:
				swapSelectedFDP();
				break;
		case GLUT_KEY_PAGE_UP:
				swapSelectedFDP();
				break;
		default:;
	}
	display();
}
// Menu events
void myMenuFunc(int index)
{
	// The screen descriptor is computed here
	// in order to avoid huge computation when it is not needed
	FEEDBACK_MODE=1;	// Udpate the screen descriptor
	display();			// 
	FEEDBACK_MODE=0;	// Now that the screen descriptor is updated, we dont need the FEEDBACK_MODE

	switch(index)
	{
		case NORMALCOLOR_OPTION:
			{
				int triangle=screenDesc[currentX][HEIGHT-currentY];
				int color=m->colors[triangle];
				updateNormalColor(color);
			}
			break;
		case SYMETRICCOLOR_OPTION:
			{
				int triangle=screenDesc[currentX][HEIGHT-currentY];
				int color=m->colors[triangle];
				symetricColorFunction(color);
			}
			break;
		case QUIT_OPTION:
			exit(1);
		case EXIT_OPTION:
			display();
			break;
		case WRITE_OPTION:
			{
				int triangle=screenDesc[currentX][HEIGHT-currentY];
				int color=m->colors[triangle];
				int region=colorToRegion[color];
				writeTrianglesToFile(region);
			}
			break;
		case OPEN_OPTION:
			openAllRegions();
			//for(int i=0;i<REGION_NUMBER;i++)
			//	loadFile(i);
			analyzeFileData();
			break;
		case XML_OPTION:
			writeXML();
			break;
		case FDP_OPTION:
			openFDPs();
			break;
		default:;
	}
}
// Menu Informations events
void myMenuInformationsFunc(int value)
{
	// The screen descriptor is computed here:
	FEEDBACK_MODE=1;	// Udpate the screen descriptor
	display();			//	by the drawing in FEEDBACK_MODE.
	FEEDBACK_MODE=0;	// Now that the screen descriptor is updated, we dont need the FEEDBACK_MODE

	switch(value)
	{
		case FACE_INFORMATION:
			printf("Face Informations:\n");
			{
				printf("   Triangle number:   %i\n", m->TRIANGLE_NUMBER);
				printf("   Vertex number:     %i\n", m->VERTEX_NUMBER);
				printf("   Number of regions: UNAVAILABLE\n");
			}
			break;
		case REGION_INFORMATION:
			printf("Region Informations:\n");
			{
				int triangle=screenDesc[currentX][HEIGHT-currentY];
				int color=colorOf(triangle);
				if(color<0)
					break;
				//int verticesInRegion;
				int region=colorToRegion[color];
				if(region<0)
					break;
				int trianglesInRegion = trianglesOfRegion[region][0];
				char* region_name=nameOfFAP(region);
				printf("   Region' value:                      %i\n", color);
				printf("   Region' name:                       %s\n", region_name);
				printf("   Number of triangles for the region: %i\n", trianglesInRegion);
				printf("   Number of vertices for the region:  UNAVAILABLE\n");
				int group=groupOfRegion(region);
				printf("   groupOfRegion:                      %i\n", group);
				for(int k=fdp_value(group-1, sizeOfGroup(group-1));k<sizeOfGroup(group);k++)
					printf("   FDPs:         %s\n", getName(fdp_value(group, k)));						// BUG ICI
			}
			break;
		case TRIANGLE_INFORMATION:
			printf("Triangle Informations:\n");
			{
				int triangle=screenDesc[currentX][HEIGHT-currentY];
				int* verticesRef=m->triangles[triangle];
				int color=m->colors[triangle];
				printf("  Triangle: %i\n", triangle);
				printf("  Color:    %i\n", color);
				printf("  Region:   %i\n", colorToRegion[color]);
				printf("  Vertices: %i %i %i\n", verticesRef[0], verticesRef[1], verticesRef[2]);
			}
			break;
		default:;
	}
}
// Menu Assign Name events
void myMenuAssignNameFunc(int region)
{
	// Ressources used:
	//		m->TRIANGLE_NUMBER
	//		m->colors
	//		screenDesc
	//		regionToColor
	//		colorToRegion
	//		trianglesOfRegion

	// get the concerned region
	int triangle=screenDesc[currentX][HEIGHT-currentY];
	int color=m->colors[triangle];

	assignColorToRegion(region, color);
}
// Assign a Color to a Region
void assignColorToRegion(int region, int color)
{
	// If there were a region that wore the same name, we remove the name of it
	for(int i=0;i<REGION_NUMBER;i++)
		if(colorToRegion[i]==region)
			colorToRegion[i]=-1;
	// Updating regionToColor
	regionToColor[region]=color;
	// Updating colorToRegion
	colorToRegion[color]=region;
	// Updating trianglesOfRegion
	int* region_pt = trianglesOfRegion[region];
	int* colors=m->colors;
	region_pt[0]=1;
	for(i=0;i<m->TRIANGLE_NUMBER;i++)
		if(colors[i]==color)
			region_pt[region_pt[0]++]=i;
}
// Assign a FDP to a region
void assignFDPToRegion(int region, int fdp_indice)
{
	//for(int i=0;i<REGION_NUMBER;i++)
	//	if(FDPs[i]==fdp_indice)
	//		FDPs[i]=-1;
	//FDPs[region]=fdp_indice;
	
}
// Menu Load File events
void myMenuLoadFileFunc(int region)
{
	int i;
	switch(region)
	{
		case 0:
			for(i=0;i<REGION_NUMBER;i++)
				loadFile(i);
			analyzeFileData();
			break;
		default:
			loadFile(region);
			break;
	}
	display();
}
// Menu Change Color events
void myMenuChangeColorFunc(int newColor)
{
}
// mySetColor
void mySetColor(int value)
{
	// Build an association between a value and a color
	switch(value)
	{
		case	WHITE:			glColor3f(WHITE_COLOR);			break;
		case	BLACK:			glColor3f(BLACK_COLOR);			break;
		case	SKIN:			glColor3f(SKIN_COLOR);			break;
		case	RED:			glColor3f(RED_COLOR);			break;
		case	GREEN:			glColor3f(GREEN_COLOR);			break;
		case	BLUE:			glColor3f(BLUE_COLOR);			break;
		case	NONAME_0:		glColor3f(NONAME_0_COLOR);		break;
		case	NONAME_1:		glColor3f(NONAME_1_COLOR);		break;
		case	NONAME_2:		glColor3f(NONAME_2_COLOR);		break;
		case	NONAME_3:		glColor3f(NONAME_3_COLOR);		break;
		case	NONAME_4:		glColor3f(NONAME_4_COLOR);		break;
		case	NONAME_5:		glColor3f(NONAME_5_COLOR);		break;
		case	NONAME_6:		glColor3f(NONAME_6_COLOR);		break;
		case	NONAME_7:		glColor3f(NONAME_7_COLOR);		break;
		case	NONAME_8:		glColor3f(NONAME_8_COLOR);		break;
		case	NONAME_9:		glColor3f(NONAME_9_COLOR);		break;
		case	NONAME_10:		glColor3f(NONAME_10_COLOR);		break;
		case	NONAME_11:		glColor3f(NONAME_11_COLOR);		break;
		case	NONAME_12:		glColor3f(NONAME_12_COLOR);		break;
		case	NONAME_13:		glColor3f(NONAME_13_COLOR);		break;
		case	NONAME_14:		glColor3f(NONAME_14_COLOR);		break;
		case	NONAME_15:		glColor3f(NONAME_15_COLOR);		break;
		case	NONAME_16:		glColor3f(NONAME_16_COLOR);		break;
		case	NONAME_17:		glColor3f(NONAME_17_COLOR);		break;
		case	NONAME_18:		glColor3f(NONAME_18_COLOR);		break;
		case	NONAME_19:		glColor3f(NONAME_19_COLOR);		break;
		case	NONAME_20:		glColor3f(NONAME_20_COLOR);		break;
		case	NONAME_21:		glColor3f(NONAME_21_COLOR);		break;
		case	NONAME_22:		glColor3f(NONAME_22_COLOR);		break;
		case	NONAME_23:		glColor3f(NONAME_23_COLOR);		break;
		case	NONAME_24:		glColor3f(NONAME_24_COLOR);		break;
		case	NONAME_25:		glColor3f(NONAME_25_COLOR);		break;
		case	NONAME_26:		glColor3f(NONAME_26_COLOR);		break;
		case	NONAME_27:		glColor3f(NONAME_27_COLOR);		break;
		case	NONAME_28:		glColor3f(NONAME_28_COLOR);		break;
		case	NONAME_29:		glColor3f(NONAME_29_COLOR);		break;
		case	NONAME_30:		glColor3f(NONAME_30_COLOR);		break;
		case	NONAME_31:		glColor3f(NONAME_31_COLOR);		break;
		case	NONAME_32:		glColor3f(NONAME_32_COLOR);		break;
		case	NONAME_33:		glColor3f(NONAME_33_COLOR);		break;
		case	NONAME_34:		glColor3f(NONAME_34_COLOR);		break;
		case	NONAME_35:		glColor3f(NONAME_35_COLOR);		break;
		case	NONAME_36:		glColor3f(NONAME_36_COLOR);		break;
		case	NONAME_37:		glColor3f(NONAME_37_COLOR);		break;
		case	NONAME_38:		glColor3f(NONAME_38_COLOR);		break;
		case	NONAME_39:		glColor3f(NONAME_39_COLOR);		break;
		case	NONAME_40:		glColor3f(NONAME_40_COLOR);		break;
		case	NONAME_41:		glColor3f(NONAME_41_COLOR);		break;
		case	NONAME_42:		glColor3f(NONAME_42_COLOR);		break;
		case	NONAME_43:		glColor3f(NONAME_43_COLOR);		break;
		case	NONAME_44:		glColor3f(NONAME_44_COLOR);		break;
		case	NONAME_45:		glColor3f(NONAME_45_COLOR);		break;
		case	NONAME_46:		glColor3f(NONAME_46_COLOR);		break;
		case	NONAME_47:		glColor3f(NONAME_47_COLOR);		break;
		case	NONAME_48:		glColor3f(NONAME_48_COLOR);		break;
		case	NONAME_49:		glColor3f(NONAME_49_COLOR);		break;
		case	NONAME_50:		glColor3f(NONAME_50_COLOR);		break;
		case	NONAME_51:		glColor3f(NONAME_51_COLOR);		break;
		case	NONAME_52:		glColor3f(NONAME_52_COLOR);		break;
		case	NONAME_53:		glColor3f(NONAME_53_COLOR);		break;
		case	NONAME_54:		glColor3f(NONAME_54_COLOR);		break;
		case	NONAME_55:		glColor3f(NONAME_55_COLOR);		break;
		case	NONAME_56:		glColor3f(NONAME_56_COLOR);		break;
		case	NONAME_57:		glColor3f(NONAME_57_COLOR);		break;
		case	NONAME_58:		glColor3f(NONAME_58_COLOR);		break;
		case	NONAME_59:		glColor3f(NONAME_59_COLOR);		break;
		case	NONAME_60:		glColor3f(NONAME_60_COLOR);		break;
		case	NONAME_61:		glColor3f(NONAME_61_COLOR);		break;
		case	NONAME_62:		glColor3f(NONAME_62_COLOR);		break;
		case	NONAME_63:		glColor3f(NONAME_63_COLOR);		break;
		case	NONAME_64:		glColor3f(NONAME_64_COLOR);		break;
		case	NONAME_65:		glColor3f(NONAME_65_COLOR);		break;
		case	NONAME_66:		glColor3f(NONAME_66_COLOR);		break;
		case	NONAME_67:		glColor3f(NONAME_67_COLOR);		break;
		case	NONAME_68:		glColor3f(NONAME_68_COLOR);		break;
		default:;
	}
}
// colorTriangle
void setColor(int triangle, int color)
{
	if(!OVERWRITE_MODE && colorOf(triangle)>0)
		return;
    removeTriangle(triangle);
	m->setColor(triangle, color);
	if(color<0)
		return;
	int region = colorToRegion[color];
	if(region<=0)
		return;											
	int* triangles = trianglesOfRegion[region];
	// Updating trianglesOfRegion
	int size = triangles[0];
	for(int i=1;i<size;i++)			// Test if the triangle is already recorded for the region
		if(triangles[i]==triangle)
			return;
	triangles[0]++;
	triangles[size]=triangle;
}
void removeTriangle(int triangle)
{
	int color=colorOf(triangle);
	if(color<0)
		return;
	int region=colorToRegion[color];
	if(region<=0)
		return;
	int trianglesInRegion = trianglesOfRegion[region][0];
	int position=0;
	for(int i=0;i<trianglesOfRegion[region][0];i++)
		if(trianglesOfRegion[region][i]==triangle)
			position=i;
	if(position==0)
	{
		printf("ERROR!\n");
		MessageBox(NULL, "ERROR: removeTriangle function", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
	for(i=position+1;i<trianglesOfRegion[region][0];i++)
		trianglesOfRegion[region][i-1]=trianglesOfRegion[region][i];
	trianglesOfRegion[region][0]--;
}
void swapSelectedFDP()
{
	if(selectedFDP==NULL)
		return;
	writeSelectedFDP(selectedFDP->next());
}
/////////////
///////////// ALGORITHMS
/////////////
void setActiveRegion(int region)
{
	int color = regionToColor[region];
	updateNormalColor(color);
}
bool analyzeCurrentData()
{
	// First, we test if each triangle is affected by a color
	int cpt=0;
	for(int i=1;i<REGION_NUMBER;i++)
	{
		cpt+=trianglesOfRegion[i][0]-1;	// "-1": because when trianglesOfRegion[i] contains no triangle, trianglesOfRegion[i][0]=1
		if(trianglesOfRegion[i][0]==1)
			printf("Warning: there is no triangle in %s\n", nameOfFAP(i));
		else
			printf("There is %i triangles      in %s\n", trianglesOfRegion[i][0], nameOfFAP(i));
	}
	printf("Found:%i triangles\n", cpt);
	if(cpt<m->TRIANGLE_NUMBER)
		return false;
	return true;
}
bool analyzeFileData()
{
	int wrongs=0;
	int* colorOfTriangle;
	int TRIANGLE_NUMBER=m->TRIANGLE_NUMBER;
	if(!(colorOfTriangle=(int*)malloc(TRIANGLE_NUMBER*sizeof(int))))
	{
		printf("ERROR: malloc failed on colorOfTriangle\n");
		MessageBox(NULL, "ERROR: malloc failed on colorOfTriangle", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(int i=0;i<TRIANGLE_NUMBER;i++)
		colorOfTriangle[i]=0;
	// A partir de la, on lit chaque fichier et mettons a jour la table "colorOfTriangle"
	//	Cependant, si on lit le triangle "j" et que colorOfTriangle[j]!=-1, alors c'est que deux regions affectent le meme triangle
	char* pathname;
	for(int j=1;j<REGION_NUMBER;j++)
	{
		/// Initialisation du fichier a lire
		FILE* f;
		char* name=nameOfFAP(j);
		pathname=(char *)malloc(256*sizeof(char));
		pathname[0]='\0';
		strcat(pathname, TMP_PATH);
		strcat(pathname, name);
		if(!(f=fopen(pathname, "r")))
		{
			printf("ERROR: cant read file \"%s\"\n", pathname);
			//MessageBox(NULL, "ERROR: cant read file", "Access error", MB_OK|MB_ICONERROR);
			continue;
		}

		/// Start
		//reads the number of verts,edges,faces
		char cbuf[1024];
		int num_v,num_e,num_f;
		int triangle;
		
		sprintf(cbuf,"");
		while(strcmp(cbuf,"VALUES\n")!=0)
			fgets( cbuf, 256, f );
		
		if(fgets(cbuf,256, f ));
		else
		{
			printf("ERROR: bad geometry file: %s\n",cbuf);
			fclose(f);
			continue;
		}
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"%d %d %d",&num_v,&num_e,&num_f);
		}

		//reads indices
		
		sprintf(cbuf,"");
		while(strcmp(cbuf,"TRIANGLES\n")!=0)
			fgets( cbuf, 256, f );

		for(int i=0; i<num_v; i++)
		{
			if(fgets( cbuf, 256, f ));
			else
			{
				printf("ERROR: bad geometry file\n");
				fclose(f);
				continue;
			}
			if(cbuf[0] == '#') while(getc(f) != '\n');
			else
			{
				sscanf(cbuf,"%i",&triangle);
				////////////////////////////
				// C'est ici qu'on a lu le triangle, et qu'on peut donc mettre a jour "colorOfTriangle"
				if(triangle<TRIANGLE_NUMBER)
				{
					if(colorOfTriangle[triangle]==1)
					{
						printf("Warning: triangle#%i is affected more than one time\n", triangle);
						setColor(triangle, SYSTEM_USING_COLOR);
					}
					else
						colorOfTriangle[triangle]=1;
				}
				else
				{
					wrongs++;
				}
			}
		}
		free(pathname);
		fclose(f);
	}
	for(i=0;i<TRIANGLE_NUMBER;i++)
		if(colorOfTriangle[i]<0)
			return false;
	printf("wrong triangles %d\n",wrongs);
	return true;
}
void writeSelectedFDP(int vertex)
{
	//int triangle=screenDesc[x][HEIGHT-y];
	//int color=m->colors[triangle];
	//int region=colorToRegion[color];

	int color=listboxColorNormal->get_int_val();
	int region=colorToRegion[color];
	int group=groupOfRegion(region);
	int indice=listboxFDPs->get_int_val();

    FDPs[fdp_value(group,indice-1)]=vertex;
	for(int i=0;i<FDP_NUMBER;i++)
		if(FDPs[i]>=0) printf("FDP[%i]:%i\n", i, FDPs[i]);
}
int writeFDPsToFile(char* path)
{
	for(int i=0; i<FDP_NUMBER;i++)
		printf("fdp[%i]: %i\n", i, FDPs[i]);
	GLfloat* vertex;
	// Ouverture du fichier
	FILE* f;
	if(!(f=fopen(path, "w")))
	{
		printf("ERROR: cant write to file \"%s\"\n", path);
		MessageBox(NULL, "ERROR: cant write to this file", "Access error", MB_OK|MB_ICONERROR);
		return false;
	}
	// Ecriture du fichier
	// tailles des groupes
	fprintf(f, "#START\n");
	for(i=2;i<GROUP_NUMBER;i++)
		fprintf(f, "GROUP %i %i\n", i, sizeOfGroup(i));
	fprintf(f, "END\n");
	// elements des groupes
	for(i=2;i<GROUP_NUMBER;i++)
	{
		fprintf(f, "#GROUP %i\n", i);
		for(int j=0;j<sizeOfGroup(i);j++)
		{
			int fdp=fdp_value(i,j);
			printf("Indice: %i\n",  fdp);
			int v=FDPs[fdp];
			printf("V --->  %i\n", v);
			if(v<0)
				fprintf(f, "%f %f %f #%i\n", 0.0, 0.0, 0.0, 0);
			else
			{
				vertex=m->vertices[v];
				fprintf(f, "%f %f %f #%i\n", vertex[0], vertex[1], vertex[2], v);
			}
		}
	}
	fclose(f);
	return true;
}
int readXML()
{
	char	*path = (char*)malloc(256*sizeof(char));
	if(!getPath(path, "XML files (*.xml)\0*.xml\0"))
	{
		printf("Unable to load file\n");
		MessageBox(NULL, "ERROR: Unable to load file", "Access error", MB_OK|MB_ICONERROR);
		return 0;
	}

	///
	XMLGenericParser *xmlparser;
	XMLGenericTree *xmltree;

	xmlparser=new XMLGenericParser();
	xmltree=xmlparser->ParseFile(path);
	delete xmltree;

	return 1;
}
int writeXML()
{
	char	*path = (char*)malloc(256*sizeof(char));
	if(!getPath(path, "XML files (*.xml)\0*.xml\0"))
	{
		printf("Unable to load file\n");
		MessageBox(NULL, "ERROR: Unable to load file", "Access error", MB_OK|MB_ICONERROR);
		return 0;
	}
	FILE	*f;
	if(!(f=fopen(path, "w")))
	{
		printf("ERROR: cant write to file\n");
		MessageBox(NULL, "ERROR: cant write to file", "Access error", MB_OK|MB_ICONERROR);
		return 0;
	}
	// triangles
	// vertices
	// regions
	// fdps
	// ...
	int TRIANGLE_NUMBER	= m->TRIANGLE_NUMBER;
	int VERTEX_NUMBER	= m->VERTEX_NUMBER;
	int		**triangles	= m->triangles;
	float	**vertices	= m->vertices;
	int		*colors		= m->colors;
	char	**names_Regions;
	char	**names_FDPs;

	names_Regions	= (char**)malloc(sizeof(char*) * REGION_NUMBER);
	for(int i=0;i<REGION_NUMBER;i++)
		names_Regions[i] = region_names[i];

	names_FDPs		= (char**)malloc(sizeof(char*) * FDP_NUMBER);
	for(i=0;i<FDP_NUMBER;i++)
		names_FDPs[i] = getName(i);

	/// -> now, we can start to write in the XML file
	fprintf(f, "<?xml version=\"1.0\"?>\n");
	fprintf(f, "<!DOCTYPE XMLMODEL SYSTEM \"data.dtd\">\n");
	fprintf(f, "<XMLMODEL>\n");
	fprintf(f, " <MESH_DATA>\n");
	fprintf(f, "  <VERTICES size=\"%i\">\n", VERTEX_NUMBER);
	for(i=0;i<VERTEX_NUMBER;i++)
	{
		fprintf(f, "   <VERTEX id=\"%i\" x=\"%f\" y=\"%f\" z=\"%f\"/>\n",
						i,
						vertices[i][0],
						vertices[i][1],
						vertices[i][2]
				);
	}
	fprintf(f, "  </VERTICES>\n");
	fprintf(f, "  <TRIANGLES id=\"%i\" size=\"%i\">\n", 0, TRIANGLE_NUMBER);
	for(i=0;i<TRIANGLE_NUMBER;i++)
	{
		int group = colorToRegion[colors[i]];
		if(group<0) group=-1;
		fprintf(f, "   <TRIANGLE id=\"%i\" idGrp=\"%i\" x=\"%i\" y=\"%i\" z=\"%i\"/>\n",
						i,
						group,	// -> Group
						triangles[i][0],
						triangles[i][1],
						triangles[i][2]
			);
	}
	fprintf(f, "  </TRIANGLES>\n");
	fprintf(f, " </MESH_DATA>\n");
	fprintf(f, " <MPEG4_DATA>\n");
	fprintf(f, "  <FDPS>\n");

	for(i=0;i<GROUP_NUMBER;i++)
	{
		fprintf(f, "   <GROUP id=\"%i\" name=\"%i\" size=\"%i\">\n",
						i+1,
						-1,	// group_name[i],
						sizeOfGroup(i)
				);
		for(int j=0;j<sizeOfGroup(i);j++)
		{
			fprintf(f, "    <FDP id=\"%i\" str_name=\"%s\" num_name=\"%i\" group=\"%i\" position=\"%i\" linkto=\"%i\"/>\n",
								j,
								"-1",	//fdp_names[i][j],
								fdp_value(i, j),
								i+1,
								j,
								FDPs[fdp_value(i, j)]
					);
		}
		fprintf(f, "   </GROUP>\n");
	}
	fprintf(f, "  </FDPS>\n");
	fprintf(f, "  <REGIONS id=\"%i\" size=\"%i\">\n", 1, REGION_NUMBER);
	for(i=1 ; i<REGION_NUMBER ; i++)
	{
		fprintf(f, "   <REGION id=\"%i\" name=\"%s\" size=\"%i\">\n",
						i,
						names_Regions[i],
						trianglesOfRegion[i][0]
				);
		for(int j=0 ; j<trianglesOfRegion[i][0] ; j++)
		{
			fprintf(f, "    <REGION_TRIANGLE id=\"%i\" ref=\"%i\"/>\n", j, trianglesOfRegion[i][j]);
		}
		fprintf(f, "   </REGION>\n");
	}
	fprintf(f, "  </REGIONS>\n");
	fprintf(f, " </MPEG4_DATA>\n");
	fprintf(f, "</XMLMODEL>\n");


	fclose(f);
	return true;
}
int writeTrianglesToFile(int region)
{
	if(region<=0) return -1;
	int color=regionToColor[region];
	if(color<=0) return -1;
	char* name=nameOfFAP(region);
	char outputname[256];
	printf("Name of region %i <%s>\n", region, name);
	// First, we write to the vertices file
	strcat(outputname, OUTPUT_PATH);
	strcat(outputname, name);
	FILE* f;
	if(!(f=fopen(outputname, "w")))
	{
		printf("ERROR: cant write to file \"%s\"\n", outputname);
		MessageBox(NULL, "ERROR: can write to this file", "Access error", MB_OK|MB_ICONERROR);
		return false;
	}
	fprintf(f, "%s\n\n", name);
	fprintf(f, "VALUES\n%i %i %i\n\n", trianglesOfRegion[region][0]-1, 0, 0);
	fprintf(f, "TRIANGLES\n");
	for(int i=0;i<m->TRIANGLE_NUMBER;i++)
		if(m->colors[i]==color)
		{
			int* vertexRef=m->triangles[i];
			fprintf(f, "%i ",  vertexRef[0]);
			fprintf(f, "%i ",  vertexRef[1]);
			fprintf(f, "%i\n", vertexRef[2]);
		}
	fclose(f);

	// Second, we write to the indices file
	char tmpname[256];
	strcat(tmpname, TMP_PATH);
	strcat(tmpname, name);
	if(!(f=fopen(tmpname, "w")))
	{
		printf("ERROR: cant write to file \"%s\"\n", tmpname);
		MessageBox(NULL, "ERROR: can write to this file", "Access error", MB_OK|MB_ICONERROR);
		return false;
	}
	fprintf(f, "%s\n\n", name);
	fprintf(f, "VALUES\n%i %i %i\n\n", trianglesOfRegion[region][0], color, 0);
	fprintf(f, "TRIANGLES\n");
	for(i=0;i<m->TRIANGLE_NUMBER;i++)
		if(m->colors[i]==color)
			fprintf(f, "%i\n", i);
	fclose(f);
	return true;
}
int writeRegionsToFile()
{
	FILE* f=fopen("debug.txt", "w");
	for(int i=1;i<REGION_NUMBER;i++)
	{
		fprintf(f, "%i\n", i);
		for(int j=0;j<trianglesOfRegion[i][0];j++)
			fprintf(f, "   %i\n", trianglesOfRegion[i][j]);
	}
	fclose(f);
	return true;
}
void synthetizeData()
{
	//char *path=(char*)malloc(256*sizeof(char));
	//getPath(path, "*");
	// On ecrit toutes les regions actuellement affichees dans le fichier correspondant
	//for(int i=1;i<REGION_NUMBER;i++)
	//	writeTrianglesToFile(i);
	//for(int i=0;i<REGION_NUMBER;i++)
	//	loadFile(i);
	FILE* f;
	f=fopen("C:/Documents and Settings/maurizio/Bureau/triangles", "w");
	if(!f)
	{
		MessageBox(NULL, "Cant open the output file: TRIANGLES_FILE", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
	for(int region=1;region<REGION_NUMBER;region++)
	{
		int color=regionToColor[region];
		int size=trianglesOfRegion[region][0]-1;

		fprintf(f, "# %i\n", region);
		fprintf(f, "SIZE %i\n", size);

		for(int i=0;i<m->TRIANGLE_NUMBER;i++)
			if(m->colors[i]==color)
			{
				int* triangleRef=m->triangles[i];
				fprintf(f, "%i %i %i\n", triangleRef[2], triangleRef[1], triangleRef[0]);
			}
	}
	fclose(f);
}

void synthetizeDataTextures()
{
	// On ecrit toutes les regions actuellement affichees dans le fichier correspondant
	//for(int i=1;i<REGION_NUMBER;i++)
	//	writeTrianglesToFile(i);
	//for(int i=0;i<REGION_NUMBER;i++)
	//	loadFile(i);
	FILE* f;
	if(!(f=fopen("texture_indices.txt", "w")))
	{
		MessageBox(NULL, "Cant open the output file: TRIANGLES_FILE", "Access error", MB_OK|MB_ICONERROR);
		return;
	}
	for(int region=1;region<REGION_NUMBER;region++)
	{
		int color=regionToColor[region];
		int size=trianglesOfRegion[region][0]-1;

		fprintf(f, "# %i\n", region);
		fprintf(f, "SIZE %i\n", size);

		for(int i=0;i<m->TRIANGLE_NUMBER;i++)
			if(m->colors[i]==color)
			{
				int* vertexRef=m->triangles[i];
				int* texturevertexRef=m->texture_triangles[i];
				fprintf(f, "%i %i %i %i %i %i\n", vertexRef[2], vertexRef[1], vertexRef[0],
					texturevertexRef[2],texturevertexRef[1],texturevertexRef[0]);
			}
	}
	fclose(f);
}

int loadFile(int region)
{
	char* filename=nameOfFAP(region);
	char name[256];
	strcat(name, TMP_PATH);
	strcat(name, filename);
	FILE* f;
	
	if(!(f=fopen(name, "r")))
	{
		printf("ERROR: cant read file \"%s\"\n", name);
		return false;
	}

	/// Start
	//reads the number of verts,edges,faces
	char cbuf[1024];
	int num_v,num_e,num_f;
	int triangle;
	
	sprintf(cbuf,"");
	while(strcmp(cbuf,"VALUES\n")!=0)
		fgets( cbuf, 256, f );
	
	if(fgets(cbuf,256, f ));
	else
	{
		printf("ERROR: bad geometry file <%s>\n",cbuf);
		return false;
	}
	if(cbuf[0] == '#') while(getc(f) != '\n');
	else
	{
		sscanf(cbuf,"%d %d %d",&num_v,&num_e,&num_f);
	}
	int color=num_e;
	assignColorToRegion(region, color);

	//reads indices
	
	sprintf(cbuf,"");
	while(strcmp(cbuf,"TRIANGLES\n")!=0)
		fgets( cbuf, 256, f );

	for(int i=0; i<num_v; i++)
	{
		if(fgets( cbuf, 256, f ));
		else
		{
			printf("ERROR: bad geometry file <%s><%s>\n",name,cbuf);
			fclose(f);
			return false;
		}
		if(cbuf[0] == '#') while(getc(f) != '\n');
		else
		{
			sscanf(cbuf,"%i",&triangle);
			setColor(triangle, color);
		}
	}
	fclose(f);

	return true;
}
int openFDPs()
{
	char	*path = (char*)malloc(256 * sizeof(char));
	getPath(path, (path, "FDPs files (*.txt)\0*.txt\0"));
	FILE	*f;
	if(!(f=fopen(path, "r")))
	{
		printf("ERROR: unable to read file %s\n", path);
		return 0;
	}

	///
	char	cbuf[256];
	int group_nbr;
	int group_sz[GROUP_NUMBER];
	float f_0, f_1, f_2;
	fgets(cbuf, 256, f);
	sscanf(cbuf, "#START\n");
	for(int i=2 ; i<GROUP_NUMBER ; i++)
	{
		fgets(cbuf, 256, f);
		sscanf(cbuf, "GROUP %i %i\n", &group_nbr, &group_sz[i]);
	}
	fgets(cbuf, 256, f);
	sscanf(cbuf, "END\n");
	for(i=2 ; i<GROUP_NUMBER ; i++)
	{
		fgets(cbuf, 256, f);
		sscanf(cbuf, "#GROUP %i\n", &group_nbr);
		for(int j=0 ; j<group_sz[i] ; j++)
		{
			fgets(cbuf, 256, f);
			sscanf(cbuf, "%f %f %f #%i\n", &f_0, &f_1, &f_2, &FDPs[fdp_value(i, j)]);
		}
	}
	return 1;
}
int openAllRegions()
{
	char	*path = (char*)malloc(256 * sizeof(char));
	getPath(path, (path, "region(indices) files (*.txt)\0*.txt\0"));
	FILE	*f;
	if(!(f=fopen(path, "r")))
	{
		printf("ERROR: unable to read file %s\n", path);
		return 0;
	}

	///
	char	cbuf[256];
	int		grp, sz;
	int		t_0, t_1, t_2;
	for(int i=0;i<REGION_NUMBER;i++)
	{
		// # grp
		fgets(cbuf, 256, f);
		sscanf(cbuf, "# %i\n", &grp);

		// SIZE sz
		fgets(cbuf, 256, f);
		sscanf(cbuf, "SIZE %i\n", &sz);

		for(int j=0;j<sz;j++)
		{
			fgets(cbuf, 256, f);
			sscanf(cbuf, "%i %i %i\n", &t_0, &t_1, &t_2);
			
			// We are now looking for this triangle in m->triangles
			//	if it exists, so we launch setColor(triangle, color) instruction
			//	if it doesnt exist, lets pray that no bug occurs
			for(int k=0;k<m->TRIANGLE_NUMBER;k++)
				if((m->triangles[k][0]==t_0 && m->triangles[k][1]==t_1 && m->triangles[k][2]==t_2) || (m->triangles[k][2]==t_0 && m->triangles[k][1]==t_1 && m->triangles[k][0]==t_2))
				{
					setColor(k, grp);
					k=m->TRIANGLE_NUMBER;
				}
		}
	}

	fclose(f);
    return 1;
}
int regionOf(int triangle)
{
	int color=colorOf(triangle);
	if(color<0)
		return -1;
	return colorToRegion[color];
}
int colorOf(int triangle)
{
	if(triangle<0)
		return -1;
	return m->colors[triangle];
}
int* getAllTrianglesInRegion(int region)
{
    if(region<0)
		return NULL;
	return trianglesOfRegion[region];
}
int isInTriangle(VECTOR2D *p, VECTOR2D *p1, VECTOR2D *p2, VECTOR2D *p3)
// Test if a vertex is contain in a triangle
// It is running using 2D coordinates
// Condition : (p1, p2, p3) are not on a same line
{
	GLfloat x1, y1;
	GLfloat x2, y2;
	GLfloat x3, y3;
	GLfloat x, y;

	x1=p1->x;
	y1=p1->y;
	x2=p2->x;
	y2=p2->y;
	x3=p3->x;
	y3=p3->y;
	x=p->x;
	y=p->y;

	// (d12): y=(a12)x + (b12)
	// (d13): y=(a13)x + (b13)
	// (d23): y=(a23)x + (b23)
	GLfloat a12, b12;
	GLfloat a13, b13;
	GLfloat a23, b23;
	int c1, c2, c3;		// The side of p1 (p2, p3) compared to (d23) (d13, d12)
	int cp1, cp2, cp3;	// The side of p compared to (d23) (d13, d12)
	bool exception1=false, exception2=false,exception3=false;	// In order to avoid (div 0) error

	if(x2!=x1)
	{
		a12=(y2-y1)/(x2-x1);
		b12=y1-(x1*a12);
	}
	else
	{
		exception3=true;
		c3=(x3<x1?1:0);
		cp3=(x<x1?1:0);
	}
	if(x3!=x1)
	{
		a13=(y3-y1)/(x3-x1);
		b13=y1-(x1*a13);
	}
	else
	{
		exception2=true;
		c2=(x2<x1?1:0);
		cp2=(x<x1?1:0);
	}
	if(x2!=x3)
	{
		a23=(y2-y3)/(x2-x3);
		b23=y3-(x3*a23);
	}
	else
	{
		exception1=true;
		c1=(x1<x2?1:0);
		cp1=(x<x2?1:0);
	}

	if(!exception1) c1=( ((y1-(a23*x1)) < b23) ? 1 : 0);	// teste si p1 est en dessous de la droite d23
	if(!exception2) c2=( ((y2-(a13*x2)) < b13) ? 1 : 0);	// teste si p2 est en dessous de la droite d13
	if(!exception3) c3=( ((y3-(a12*x3)) < b12) ? 1 : 0);	// teste si p3 est en dessous de la droite d12

	if(!exception1) cp1=( (y-(a23*x)) < b23 ? 1 : 0);	// la meme chose pour p par rapport aux 3 droites
	if(!exception2) cp2=( (y-(a13*x)) < b13 ? 1 : 0);	// la meme chose pour p par rapport aux 3 droites
	if(!exception3) cp3=( (y-(a12*x)) < b12 ? 1 : 0);	// la meme chose pour p par rapport aux 3 droites

	return ((c1==cp1)&&(c2==cp2)&&(c3==cp3));
}
int isInPolygon(int triangle, int* triangles)
{
	// The algorithm instructions used here comes from
	//	http://www.alienryderflex.com/polygon/
	
// Globals which should be set before calling this function:
//
// int    size  =  how many corners the polygon has
// float  polyX[]    =  horizontal coordinates of corners
// float  polyY[]    =  vertical coordinates of corners
// float  x, y       =  point to be tested
//
// (Globals are used in this example for purposes of speed.
// Change as desired.)
//
// The function will return TRUE if the point x,y is inside the
// polygon, or FALSE if it is not. If the point x,y is exactly on
// the edge of the polygon, then the function may return TRUE or
// FALSE.
//
// Note that division by zero is avoided because the division is
// protected by the "if" clause which surrounds it.

	GLfloat* polyX;
	GLfloat* polyY;
	VECTOR2D *p=new VECTOR2D();
	int size=triangles[0];
	int* coordinates = m->triangles[triangle];
	GLfloat* vertex0 = m->vertices[coordinates[0]];
	GLfloat* vertex1 = m->vertices[coordinates[1]];
	GLfloat* vertex2 = m->vertices[coordinates[2]];
	p->x=(vertex0[0] + vertex1[0] + vertex2[0])/3;
	p->y=(vertex0[1] + vertex1[1] + vertex2[1])/3;

	// INITIALIZATIONS
	if(!(polyX=(float*)malloc((size-1)*sizeof(int))))
	{
		printf("ERROR: malloc failed on polyX\n");
		MessageBox(NULL, "ERROR: malloc failed on polyX", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	if(!(polyY=(float*)malloc((size-1)*sizeof(int))))
	{
		printf("ERROR: malloc failed on polyY\n");
		MessageBox(NULL, "ERROR: malloc failed on polyY", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	for(int i=1;i<size;i++)	// Be careful using triangles[i] data : the first one is the triangles number, not a triangle itself
	{	
		/*
		int index=indexedTriangles[triangle];
		p->x=(feedbackBuffer[index]   + feedbackBuffer[index+3] + feedbackBuffer[index+6]) / 3.0;
		p->y=(feedbackBuffer[index+1] + feedbackBuffer[index+4] + feedbackBuffer[index+7]) / 3.0;
		index=indexedTriangles[triangles[i]];
		polyX[i-1]=(feedbackBuffer[index]   + feedbackBuffer[index+3] + feedbackBuffer[index+6]) / 3.0;
		polyY[i-1]=(feedbackBuffer[index+1] + feedbackBuffer[index+4] + feedbackBuffer[index+7]) / 3.0;
		*/
		coordinates = m->triangles[triangles[i]];
		vertex0 = m->vertices[coordinates[0]];
		vertex1 = m->vertices[coordinates[1]];
		vertex2 = m->vertices[coordinates[2]];
		polyX[i-1]=(vertex0[0] + vertex1[0] + vertex2[0])/3.0;
		polyY[i-1]=(vertex0[1] + vertex1[1] + vertex2[1])/3.0;
	}

	// ALGORITHM:
	bool oddNODES=false;
	for(i=0;i<size-1;i++)
	{
		int j=i+1;
		if(j==size) j=0;
		if( polyY[i]<p->y && polyY[j] >= p->y ||
			polyY[j]<p->y && polyY[i] >= p->y)
			if( polyX[i]+(p->y-polyY[i]) / (polyY[j]-polyY[i]) * (polyX[j]-polyX[i]) < p->x )
			oddNODES=!oddNODES;
	}
	if(oddNODES)
	{
		FILE* f=fopen("vrac.txt", "w");
		fprintf(f, "p:  %f %f\n", p->x, p->y);
		for(int i=1;i<size-1;i++)
			fprintf(f, "    %f %f\n", polyX[i-1], polyY[i-1]);
		fclose(f);
	}

	return oddNODES;
}
int* getNeighbor(int triangle)
{	// fonction ok
	// Ressources used:
	//		m->TRIANGLE_NUMBER
	//		m->triangles

	int* neighbor;
	int cpt=0;
	int TRIANGLE_NUMBER=m->TRIANGLE_NUMBER;
	GLint** triangles=m->triangles;
	int vertices[3];
	if(!(neighbor=(int*)malloc(3*sizeof(int))))
	{
		printf("ERROR: malloc failed on neighbor\n");
		MessageBox(NULL, "ERROR: malloc failed on neighbor", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	vertices[0]=triangles[triangle][0];	// The vertices of the triangle
	vertices[1]=triangles[triangle][1];	// that we want to know
	vertices[2]=triangles[triangle][2];	// the neighborhood

	for(int i=0;i<TRIANGLE_NUMBER;i++)
	{
		if(i==triangle)
			continue;

		int v0=triangles[i][0];	// The vertices of the triangle
		int v1=triangles[i][1];	// that we want to know
		int v2=triangles[i][2];	// if it is the neighbor of the parameter

		int isANeighbor=0;
		if((v0==vertices[0]) || (v0==vertices[1]) || (v0==vertices[2])) isANeighbor++;
		if((v1==vertices[0]) || (v1==vertices[1]) || (v1==vertices[2])) isANeighbor++;
		if((v2==vertices[0]) || (v2==vertices[1]) || (v2==vertices[2])) isANeighbor++;
		if(isANeighbor==2)
			neighbor[cpt++]=i;
	}
	for(i=cpt;i<3;i++)
		neighbor[i]=-1;	// if there isnt 3 neighbors

	return neighbor;
}
int* getBestWay(int start, int end)
{	// Fonction ok
	// Ressources used:
	//		m->TRIANGLE_NUMBER
	//		getNeighbor(int)
	//		Etat.h

	/////////////
	///////////// Algorithm
	/////////////
	// Step 1: get all triangles bordering the start one,
	//			and put them in an array
	// Step 2: get all triangles bordering those contain in the array,
	//			if they are not already in this array, put them in it
	// Return: a list corresponding to the shortest way between the starting and the ending triangle
	//			nb: the first data of the return list is the length of this one

	int  currentTriangle = start;
	Etat* list;
	int cpt=0;	// index the list
	if(!(list=(Etat*)malloc(m->TRIANGLE_NUMBER*sizeof(Etat))))
	{
		printf("ERROR: malloc failed on list\n");
		MessageBox(NULL, "ERROR: malloc failed on list", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	list[cpt++]=Etat(currentTriangle);

	bool found=false;
	for(int k=0;k<m->TRIANGLE_NUMBER && !found;k++)
	{
		currentTriangle=list[k].value;
		int* currentNeighbor = getNeighbor(currentTriangle);
		for(int i=0;i<3;i++)
		{
			bool insert=true;
			// First, we test if the neighbor triangle is valid
			if(currentNeighbor[i]>=0)
			{
				// Second, we test if the neighbor triangle is not already included
				// Warning! This method is time consuming
            	for(int j=0;j<cpt && insert;j++)
					if(list[j].value==currentNeighbor[i])
						insert=false;
				// If we didnt found the currentNeighbor in the list-array, we insert it in the list
				if(insert)
				{
					list[cpt++]=Etat(currentNeighbor[i], &list[k]);
					if(list[cpt-1].value==end)
					{
						found=true;
						continue;
					}
				}
			}
		}
	}

	return list[cpt-1].history;
}
int* getBestWayExtended(int start, int* end)
{
	printf("start:%i\n", start);
	int  currentTriangle = start;
	Etat* list;
	int cpt=0;	// index the list
	if(!(list=(Etat*)malloc(m->TRIANGLE_NUMBER*sizeof(Etat))))
	{
		printf("ERROR: malloc failed on list\n");
		MessageBox(NULL, "ERROR: malloc failed on list", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	list[cpt++]=Etat(currentTriangle);

	bool found=false;
	for(int k=0;k<m->TRIANGLE_NUMBER && !found;k++)
	{
		currentTriangle=list[k].value;
		int* currentNeighbor = getNeighbor(currentTriangle);
		for(int i=0;i<3;i++)
		{
			bool insert=true;
			// First, we test if the neighbor triangle is valid
			if(currentNeighbor[i]>=0)
			{
				// Second, we test if the neighbor triangle is not already included
				// Warning! This method is time consuming
            	for(int j=0;j<cpt && insert;j++)
					if(list[j].value==currentNeighbor[i])
						insert=false;
				// If we didnt found the currentNeighbor in the list-array, we insert it in the list
				if(insert)
				{
					list[cpt++]=Etat(currentNeighbor[i], &list[k]);
					// End test
					for(int t=1;t<end[0];t++)			// THE ONLY LINE THAT CHANGE WITH ORIGINAL getBestWay
						if(list[cpt-1].value==end[t])
						{
							found=true;
							continue;
						}
				}
			}
		}
	}

	return list[cpt-1].history;
}
int getNearestSymetricTriangle(int triangle)
{	// Fonction ok
	// Ressources used:
	//		m->triangles
	//		m->vertices
	//		m->TRIANGLE_NUMBER

	GLint** triangles=m->triangles;
	GLfloat** vertices=m->vertices;
	GLfloat* vertex0=vertices[triangles[triangle][0]];
	GLfloat* vertex1=vertices[triangles[triangle][1]];
	GLfloat* vertex2=vertices[triangles[triangle][2]];

	// First, we compute the exact Symetric Vertex
	//  of the triangle barycentre
	GLfloat* bar;		// the coordinates of the triangle barycentre
	GLfloat* symBar;	// the coordinates of the symetric vertex of the barycentre
	if(!(bar=(GLfloat*)malloc(3*sizeof(GLfloat))))
	{
		printf("ERROR: malloc failed on bar\n");
		MessageBox(NULL, "ERROR: malloc failed on bar", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	if(!(symBar=(GLfloat*)malloc(3*sizeof(GLfloat))))
	{
		printf("ERROR: malloc failed on symBar\n");
		MessageBox(NULL, "ERROR: malloc failed on symBar", "Access error", MB_OK|MB_ICONERROR);
		exit(2);
	}
	//
	bar[0]=(vertex0[0]+vertex1[0]+vertex2[0])/3;
	bar[1]=(vertex0[1]+vertex1[1]+vertex2[1])/3;
	bar[2]=(vertex0[2]+vertex1[2]+vertex2[2])/3;
	symBar[0]=-bar[0];
	symBar[1]=bar[1];
	symBar[2]=bar[2];

	// Now, we are looking for the nearest vertex
	//  from the symBar one
	int bestTriangle=-1;
	GLfloat bestValue=100.0;
	GLfloat tmpValue;
	for(int i=0;i<m->TRIANGLE_NUMBER;i++)
	{
		tmpValue=getDistance(i, symBar);
		if(tmpValue<bestValue)
		{
			bestTriangle=i;
			bestValue=tmpValue;
		}
	}
	return bestTriangle;
}
int getNearestVertex(int x, int y)
{	// Fonction ok
	// Ressources used:
	//		feedbackBuffer
	//		screenDesc
	//		m->triangles
	//		VECTOR3D

	int triangle;
	if((triangle=screenDesc[x][HEIGHT-y])<0)
		return -1;

	int index=indexedTriangles[triangle];
	VECTOR3D vertex0;
	VECTOR3D vertex1;
	VECTOR3D vertex2;
	vertex0.x=feedbackBuffer[index++];
	vertex0.y=feedbackBuffer[index++];
	vertex0.z=feedbackBuffer[index++];
	
	vertex1.x=feedbackBuffer[index++];
	vertex1.y=feedbackBuffer[index++];
	vertex1.z=feedbackBuffer[index++];
	
	vertex2.x=feedbackBuffer[index++];
	vertex2.y=feedbackBuffer[index++];
	vertex2.z=feedbackBuffer[index++];

	int distance0=pow((vertex0.x-x), 2) + pow((vertex0.y-y), 2);
	int distance1=pow((vertex1.x-x), 2) + pow((vertex1.y-y), 2);
	int distance2=pow((vertex2.x-x), 2) + pow((vertex2.y-y), 2);

	int* pointer=m->triangles[triangle];
	selectedFDP=new SelectedFDP(pointer[0], pointer[1], pointer[2]);

	if (distance0<distance1 && distance0<distance2)
		return pointer[0];
	if (distance1<distance0 && distance1<distance2)
		return pointer[1];
	if (distance2<distance0 && distance2<distance1)
		return pointer[2];
	return -1;
}
GLfloat getDistance(int triangle, GLfloat* vertex)
{
	// Ressources used:
	//		m->triangles
	//		m->vertices
	//		Math.h

	GLint** triangles=m->triangles;
	GLfloat** vertices=m->vertices;
	GLfloat* vertex0=vertices[triangles[triangle][0]];
	GLfloat* vertex1=vertices[triangles[triangle][1]];
	GLfloat* vertex2=vertices[triangles[triangle][2]];
	// Distance between a vertex and a plan :
	/*
	 *	PLAN:	aX + bY + cZ + d = 0
	 *	VERTEX:	(xP, yP, zP)
	 *
	 *	DISTANCE:	abs( (a*xP + b*yP + c*zP + d) / (sqrt(a*a + b*b + c*c)) )
	 */

	// So, we have to find first, the plan equation
	//	given by the 3 vertices of the triangle
	// The plan equation with 3 vertices :
	/*
	 *	VERTICES:	P1 (x1, y1, z1)
	 *				P2 (x2, y2, z2)
	 *				P3 (x3, y3, z3)
	 *
	 *	PLAN:	|	(X-x1)	(Y-y1)	(Z-z1)	 |
	 *			|	(x2-x1)	(y2-y1)	(z2-z1)	 |	=	0
	 *			|	(x3-x1)	(y3-y1)	(z3-z1)	 |
	 *
	 *		->	aX + bY + cZ - (ax1 + by1 + cz1)	=	0
	 *			-----------------------------------------
	 *	where:
	 *			a = + ( (y2-y1)(z3-z1) - (z2-z1)(y3-y1) )
	 *			b = - ( (x2-x1)(z3-z1) - (z2-z1)(x3-x1) )
	 *			c = + ( (x2-x1)(y3-y1) - (y2-y1)(x3-x1) )
	 */
	GLfloat x1=vertex0[0];
	GLfloat y1=vertex0[1];
	GLfloat z1=vertex0[2];
	GLfloat x2=vertex1[0];
	GLfloat y2=vertex1[1];
	GLfloat z2=vertex1[2];
	GLfloat x3=vertex2[0];
	GLfloat y3=vertex2[1];
	GLfloat z3=vertex2[2];

	GLfloat A=   ( (y2-y1)*(z3-z1) - (z2-z1)*(y3-y1) );
	GLfloat B= - ( (x2-x1)*(z3-z1) - (z2-z1)*(x3-x1) );
	GLfloat C=   ( (x2-x1)*(y3-y1) - (y2-y1)*(x3-x1) );
	GLfloat D= - ( A*x1 + B*y1 + C*z1 );

	GLfloat xP=vertex[0];
	GLfloat yP=vertex[1];
	GLfloat zP=vertex[2];

	return (abs((A*xP + B*yP + C*zP + D) / (sqrt(A*A + B*B + C*C))));
}
//int loadAllRegions(char* file)
//{
//}